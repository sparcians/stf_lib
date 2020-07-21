#ifndef __STF_COMPRESSED_OFSTREAM_HPP__
#define __STF_COMPRESSED_OFSTREAM_HPP__

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <future>

#include "stf_compression_buffer.hpp"
#include "stf_compressed_chunked_base.hpp"
#include "stf_ofstream.hpp"
#include "stf_record.hpp"

namespace stf {
    /**
     * \class STFCompressedOFstream
     *
     * Provides transparent on-the-fly compression of an STF file
     */
    template<typename Compressor>
    class STFCompressedOFstream : public STFOFstream, public STFCompressedChunkedBase {
        private:
            Compressor compressor_; /**< Compressor object */
            STFCompressionPointerWrapper<const uint8_t*> in_buf_; /**< Input data pointer */
            STFExponentialCompressionBuffer cur_chunk_buf_;
            STFExponentialCompressionBuffer compression_chunk_buf_;
            STFCompressionBuffer out_buf_; /**< Output data buffer - holds compressed data that has not been written to the file */
            std::future<void> compression_done_;
            bool compression_in_progress_ = false;
            bool pending_chunk_ = false; /**< True if there is pending data in the output buffer */
            size_t bytes_written_ = 0; /**< Number of uncompressed bytes written so far */

            /**
             * Writes directly to the file, bypassing the compressor
             * \param data Buffer to write
             * \param size Number of elements in the buffer
             */
            template <typename T>
            void direct_write_(const T* data, size_t size) {
                STFOFstream::fwrite_(data, sizeof(T), size);
            }

            /**
             * Writes an arithmetic type directly to the file, bypassing the compressor
             * \param data Data to write
             */
            template <typename T>
            typename std::enable_if<std::is_arithmetic<T>::value>::type
            direct_write_(const T data) {
                direct_write_(&data, 1);
            }

            /**
             * Writes a non-arithmetic type directly to the file, bypassing the compressor
             * \param data Data to write
             */
            template <typename T>
            typename std::enable_if<!std::is_arithmetic<T>::value>::type
            direct_write_(const T& data) {
                direct_write_(&data, 1);
            }

            /**
             * Writes a ChunkOffset directly to the file, bypassing the compressor
             * \param data Data to write
             */
            void direct_write_(const ChunkOffset& data) {
                direct_write_(data.getOffset());
                direct_write_(data.getStartPC());
                direct_write_(data.getUncompressedChunkSize());
            }

            /**
             * Writes a vector directly to the file, bypassing the compressor
             * \param data Data to write
             * \param size Number of elements to write
             */
            template <typename T>
            typename std::enable_if<std::is_arithmetic<T>::value>::type
            direct_write_(const std::vector<T>& data, size_t size) {
                direct_write_(size);
                direct_write_(data.data(), size);
            }

            /**
             * Writes a vector directly to the file, bypassing the compressor
             * \param data Data to write
             * \param size Number of elements to write
             */
            template <typename T>
            typename std::enable_if<!std::is_arithmetic<T>::value>::type
            direct_write_(const std::vector<T>& data, size_t size) {
                direct_write_(size);
                for(size_t i = 0; i < size; ++i) {
                    direct_write_(data[i]);
                }
            }

            /**
             * Writes a vector directly to the file, bypassing the compressor
             * \param data Data to write
             */
            template <typename T>
            void direct_write_(const std::vector<T>& data) {
                direct_write_(data, data.size());
            }

            /**
             * Compresses data into the output buffer
             * \param data Buffer to write
             * \param size Size of an element
             * \param num Number of elements to read
             */
            size_t fwrite_(const void* data, size_t size, size_t num) override {
                const size_t num_bytes = size * num;
                cur_chunk_buf_.fit(num_bytes);
                const auto ptr = reinterpret_cast<const uint8_t*>(data);
                std::copy(ptr, ptr + num_bytes, cur_chunk_buf_.get() + cur_chunk_buf_.end());
                cur_chunk_buf_.advanceWritePtr(num_bytes);
                pending_chunk_ = true;
                return num;
            }

            void compressChunkAsync_() {
                const size_t num_bytes = compression_chunk_buf_.end();
                compressor_.compress(out_buf_, compression_chunk_buf_);
                bytes_written_ += num_bytes;
                endChunk_();
            }

            void compressChunk_() {
                if(STF_EXPECT_TRUE(compression_in_progress_)) {
                    compression_done_.get();
                    compression_in_progress_ = false;
                }
                if(STF_EXPECT_TRUE(pending_chunk_)) {
                    pending_chunk_ = false;
                    compression_in_progress_ = true;
                    std::swap(cur_chunk_buf_, compression_chunk_buf_);
                    compression_done_ = std::move(std::async(std::launch::async,
                                                             [this](){ this->compressChunkAsync_(); }));
                }
            }

            /**
             * Writes compressed data out to the file
             */
            void writeChunk_() {
                // Only bother trying to write if there's some data in the buffer
                if(out_buf_.end()) {
                    direct_write_(out_buf_.get(), out_buf_.end());
                    out_buf_.reset();
                }
            }

            /**
             * Ends the current compressed chunk and flushes it to the file
             */
            void endChunk_() {
                // Empty the buffer if it's full
                if(out_buf_.full()) {
                    writeChunk_();
                }

                // Flush any data remaining in the internal compressor buffers
                compressor_.flush(out_buf_);
                // Write any remaining data that was flushed
                writeChunk_();
                chunk_indices_.back().setUncompressedChunkSize(bytes_written_);
                bytes_written_ = 0;
                // Start a new chunk
                chunk_indices_.emplace_back(ftell(stream_), pc_tracker_.getNextPC(), 0);
            }

        public:
            /**
             * Constructs an STFCompressedOFstream
             * \param args Optional arguments that will be passed to the underlying compressor
             */
            template<typename ... CompressorArgs>
            explicit STFCompressedOFstream(CompressorArgs... args) :
                compressor_(args...)
            {
            }

            /**
             * Constructs an STFCompressedOFstream and opens it
             * \param filename Filename to open
             * \param args Optional arguments that will be passed to the underlying compressor
             */
            template<typename ... CompressorArgs>
            explicit STFCompressedOFstream(const std::string_view filename, CompressorArgs... args) :
                STFCompressedOFstream(args...)
            {
                open(filename);
            }

            // Have to override the base class destructor to ensure that *our* close method gets called before destruction
            ~STFCompressedOFstream() override {
                if(stream_) {
                    close();
                }
            }

            /**
             * Sets the chunk size
             * \note Once the stream is open this cannot be changed
             * \param chunk_size Number of instructions per chunk
             */
            void setChunkSize(const size_t chunk_size) {
                stf_assert(!stream_, "Must set chunk size before opening file.");
                inst_chunk_size_ = chunk_size;
            }

            /**
             * Opens a file using the specified chunk size
             * \param filename Filename to open
             * \param chunk_size Number of instructions per chunk
             */
            void open(const std::string_view filename, const size_t chunk_size) {
                setChunkSize(chunk_size);
                open(filename);
            }

            /**
             * Opens a file
             * \param filename Filename to open
             */
            void open(const std::string_view filename) override {
                static constexpr off_t ZERO = 0;

                // Open the file
                STFFstream::open(filename, "w");

                // Write the magic string for the compressor
                direct_write_(Compressor::getMagic().data(), Compressor::getMagic().size());

                // Write the number of instructions per chunk
                direct_write_(inst_chunk_size_);

                // Write a placeholder value for the end of the last chunk (since we don't know how much data we're going to have)
                direct_write_(ZERO);

                // The first chunk starts here
                chunk_indices_.emplace_back(ftell(stream_), 0, 0);

                // Size the output buffer to match the filesystem block size
                const size_t block_size = getFSBlockSize_();
                out_buf_.initSize(block_size);
                cur_chunk_buf_.initSize(block_size, true);
                compression_chunk_buf_.initSize(block_size, true);

                // ...but also make sure it can fit a reasonable amount of compressed data
                out_buf_.fit(Compressor::getInitialBoundedSize());

                next_chunk_end_ = inst_chunk_size_;
            }

            /**
             * Closes the file
             */
            int close() override {
                if(!stream_) {
                    return 0;
                }

                // Finish any pending chunk
                if(pending_chunk_) {
                    compressChunk_();
                    if(compression_in_progress_) {
                        compression_done_.get();
                        compression_in_progress_ = false;
                    }
                }
                // Get the current file offset so we can write it back at the beginning
                const off_t end = ftell(stream_);
                // Write the chunk offsets to the end of the file
                direct_write_(chunk_indices_, chunk_indices_.size() - 1);
                // Write the end of the last chunk into the spot we reserved when we opened the file
                fseek(stream_, Compressor::getMagic().size() + sizeof(inst_chunk_size_), SEEK_SET);
                direct_write_(end);
                return STFOFstream::close();
            }

            void instructionRecordCallback() override {
                STFOFstream::instructionRecordCallback();

                // If we've crossed the chunk boundary, close the current chunk and start a new one
                if(STF_EXPECT_FALSE(num_insts_ >= next_chunk_end_)) {
                    //endChunk_();
                    compressChunk_();
                    next_chunk_end_ += inst_chunk_size_;
                }
            }
            /**
             * Writes an STFRecord
             * \param rec Record to write
             */
            STFCompressedOFstream& operator<<(const STFRecord& rec) override {
                // Compress the record
                STFOFstream::operator<<(rec);
                return *this;
            }
    };
} // end namespace stf

#endif
