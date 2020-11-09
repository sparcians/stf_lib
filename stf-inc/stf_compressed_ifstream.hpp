#ifndef __STF_COMPRESSED_IFSTREAM_HPP__
#define __STF_COMPRESSED_IFSTREAM_HPP__

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <future>
#include <string_view>

#include "stf_compression_buffer.hpp"
#include "stf_compressed_chunked_base.hpp"
#include "stf_ifstream.hpp"
#include "stf_record.hpp"

namespace stf {
    /**
     * \class STFCompressedIFstream
     *
     * Provides transparent on-the-fly decompression of a compressed STF file
     */
    template<typename Decompressor>
    class STFCompressedIFstream : public STFIFstream, public STFCompressedChunkedBase {
        private:
            Decompressor decompressor_; /**< Decompressor object */
            STFCompressionBuffer in_buf_; /**< Input data buffer - holds compressed data read from the file */
            STFCompressionBuffer out_buf_; /**< Output data pointer */
            STFCompressionBuffer decompressed_buf_; /**< Decompressed data pointer */
            std::future<void> decompress_result_; /**< Future used to indicate when the next chunk has been decompressed */
            bool decompression_in_progress_ = false; /**< Flag indicating whether a chunk is currently being decompressed */
            std::vector<ChunkOffset>::iterator next_chunk_index_it_ = chunk_indices_.end(); /**< Iterator to next chunk offset */
            off_t last_read_pos_ = 0; /**< File offset of last read */
            off_t end_of_last_chunk_; /**< File offset of the end of the last compressed chunk - needed so we can know when to stop reading */
            bool successful_read_ = false; /**< Indicates whether the last read succeeded - needed so we can know if we still have valid data in our buffers */

            /**
             * Returns true if we have read all of the compressed chunks from the file
             */
            inline bool reachedEndOfChunks_() const {
                return last_read_pos_ + static_cast<off_t>(in_buf_.getReadPos()) >= end_of_last_chunk_;
            }

            /**
             * Returns true if all data has been consumed from the file and decompression buffers
             */
            inline bool allInputConsumed_() const {
                return reachedEndOfChunks_() && out_buf_.consumed() && decompressed_buf_.consumed();
            }

            /**
             * Returns true if we have read all compresssed chunks from the file, there is no pending data
             * in the decompressor buffers, and there is not a pending valid read
             */
            inline bool feof_() const final {
                return allInputConsumed_() && !successful_read_;
            }

            /**
             * Reads directly from the underlying file without decompressing the data
             * \param data Buffer to read data into
             * \param size Number of elements to read
             * \param ignore_end_of_chunks if true, will allow reading past the end of the compressed chunks
             */
            template <typename T>
            inline size_t direct_read_(T* data, size_t size, bool ignore_end_of_chunks = false) {
                last_read_pos_ = ftell(stream_);
                const off_t next_read_end = last_read_pos_ + static_cast<off_t>(size);

                const off_t next_chunk_end =
                    STF_EXPECT_TRUE(!ignore_end_of_chunks && next_chunk_index_it_ != chunk_indices_.end()) ? next_chunk_index_it_->getOffset() : 0;

                // Don't read past the end of the last chunk
                if(STF_EXPECT_FALSE(!ignore_end_of_chunks && (next_read_end > end_of_last_chunk_))) {
                    size = static_cast<size_t>(end_of_last_chunk_ - last_read_pos_) / sizeof(T);
                }
                // Don't read past the end of the current chunk
                else if(STF_EXPECT_FALSE(next_chunk_end && (next_read_end > next_chunk_end))) {
                    size = static_cast<size_t>(next_chunk_end - last_read_pos_) / sizeof(T);
                }
                STFIFstream::fread_(data, sizeof(T), size);
                return size;
            }

            /**
             * Reads an enum directly from the underlying file without decompressing the data
             * \param data Buffer to read data into
             * \param ignore_end_of_chunks if true, will allow reading past the end of the compressed chunks
             */
            template <typename T>
            inline typename std::enable_if<std::is_enum<T>::value, size_t>::type
            direct_read_(T& data, bool ignore_end_of_chunks = false) {
                enums::int_t<T> val;
                size_t num_bytes = direct_read_(&val, 1, ignore_end_of_chunks);
                data = static_cast<T>(val);
                return num_bytes;
            }

            /**
             * Reads an arithmetic value directly from the underlying file without decompressing the data
             * \param data Buffer to read data into
             * \param ignore_end_of_chunks if true, will allow reading past the end of the compressed chunks
             */
            template <typename T>
            inline typename std::enable_if<std::is_arithmetic<T>::value, size_t>::type
            direct_read_(T& data, bool ignore_end_of_chunks = false) {
                return direct_read_(&data, 1, ignore_end_of_chunks);
            }

            /**
             * Reads a ChunkOffset directly from the underlying file without decompressing the data
             * \param data Buffer to read data into
             * \param ignore_end_of_chunks if true, will allow reading past the end of the compressed chunks
             */
            inline size_t direct_read_(ChunkOffset& data, bool ignore_end_of_chunks = false) {
                off_t offset;
                uint64_t start_pc;
                size_t chunk_size;
                direct_read_(offset, ignore_end_of_chunks);
                direct_read_(start_pc, ignore_end_of_chunks);
                direct_read_(chunk_size, ignore_end_of_chunks);
                data.set(offset, start_pc, chunk_size);
                return 1;
            }

            /**
             * Reads an array directly from the underlying file without decompressing the data
             * \param data Buffer to read data into
             * \param ignore_end_of_chunks if true, will allow reading past the end of the compressed chunks
             */
            template <typename T, size_t N>
            inline size_t direct_read_(std::array<T, N>& data, bool ignore_end_of_chunks = false) {
                return direct_read_(data, N, ignore_end_of_chunks);
            }

            /**
             * Reads an array directly from the underlying file without decompressing the data
             * \param data Buffer to read data into
             * \param size Number of elements to read
             * \param ignore_end_of_chunks if true, will allow reading past the end of the compressed chunks
             */
            template <typename T, size_t N>
            inline size_t direct_read_(std::array<T, N>& data, size_t size, bool ignore_end_of_chunks = false) {
                return direct_read_(data.data(), size, ignore_end_of_chunks);
            }

            /**
             * Reads a vector directly from the underlying file without decompressing the data
             * \param data Buffer to read data into
             * \param ignore_end_of_chunks if true, will allow reading past the end of the compressed chunks
             */
            template <typename T>
            inline typename std::enable_if<std::is_arithmetic<T>::value || std::is_enum<T>::value, size_t>::type
            direct_read_(std::vector<T>& data, bool ignore_end_of_chunks = false) {
                size_t size;
                direct_read_(size, ignore_end_of_chunks);
                data.resize(size);
                direct_read_(data.data(), data.size(), ignore_end_of_chunks);
                return size;
            }

            /**
             * Reads a vector directly from the underlying file without decompressing the data
             * \param data Buffer to read data into
             * \param ignore_end_of_chunks if true, will allow reading past the end of the compressed chunks
             */
            template <typename T>
            inline typename std::enable_if<!(std::is_arithmetic<T>::value || std::is_enum<T>::value), size_t>::type
            direct_read_(std::vector<T>& data, bool ignore_end_of_chunks = false) {
                size_t size;
                direct_read_(size, ignore_end_of_chunks);
                data.resize(size);
                for(size_t i = 0; i < size; ++i) {
                    direct_read_(data[i], ignore_end_of_chunks);
                }
                return size;
            }

            /**
             * Reads and decompresses an entire chunk from the file
             * \param chunk_it Iterator pointing to metadata of the next chunk
             * \param uncompressed_chunk_size Size of the decompressed chunk
             * \param out_buf Buffer to store decompressed chunk in
             */
            void readChunk_(const std::vector<ChunkOffset>::iterator& chunk_it,
                            const size_t uncompressed_chunk_size,
                            STFCompressionBuffer& out_buf) {
                // Calculate the number of bytes to read from the file
                const auto num_bytes =
                    static_cast<size_t>((STF_EXPECT_FALSE(chunk_it == chunk_indices_.end()) ? end_of_last_chunk_ : chunk_it->getOffset()) - ftell(stream_));

                // Reset and resize buffers
                in_buf_.reset();
                in_buf_.fit(num_bytes);
                out_buf.reset();
                out_buf.fit(uncompressed_chunk_size);

                // Read entire compressed chunk
                const size_t num_bytes_read = direct_read_(in_buf_.get(), num_bytes);
                in_buf_.advanceWritePtr(num_bytes);

                // Read should complete successfully
                stf_assert(num_bytes_read == num_bytes,
                           "Failed to read entire chunk from file: requested " << num_bytes << " but read " << num_bytes_read);

                // Decompress chunk
                const bool keep_reading = decompressor_.decompress(out_buf, in_buf_);

                // Since we allocated enough room for the entire uncompressed chunk, this should complete in a single shot
                stf_assert(!keep_reading, "Failed to decompress entire chunk");

                // Reset the decompressor
                endChunk_();
            }

            /**
             * Asynchronously read and decompress the next chunk in the file
             */
            void readNextChunk_() {
                // Make sure there are still chunks left to read
                if(STF_EXPECT_FALSE(next_chunk_index_it_ == chunk_indices_.end())) {
                    decompressed_buf_.consume();
                    return;
                }
                const size_t uncompressed_chunk_size = next_chunk_index_it_->getUncompressedChunkSize();
                const auto chunk_it = ++next_chunk_index_it_;

                // Decompress the chunk in a separate thread
                decompression_in_progress_ = true;
                decompress_result_ = std::move(std::async(std::launch::async,
                                                          [this, chunk_it, uncompressed_chunk_size]() {
                                                            this->readChunk_(chunk_it,
                                                                             uncompressed_chunk_size,
                                                                             this->decompressed_buf_);
                                                          }));
            }

            /**
             * Checks whether we can read more data from the file or any buffers
             */
            bool checkIfReadIsPossible_() {
                if(STF_EXPECT_FALSE(allInputConsumed_())) {
                    successful_read_ = false;
                    return false;
                }

                return true;
            }

            /**
             * Reads the specified number of T objects from the decompression buffer.
             *
             * \param data Buffer to read data into
             * \param num Number of elements to read
             */
            template<typename T>
            inline size_t fread_multiple_(void* data, const size_t num) {
                if(STF_EXPECT_FALSE(!checkIfReadIsPossible_())) {
                    return 0;
                }

                const auto ptr = reinterpret_cast<T*>(data);
                const auto read_ptr = reinterpret_cast<T*>(out_buf_.get() + out_buf_.getReadPos());
                std::copy(read_ptr, read_ptr + num, ptr);
                out_buf_.advanceReadPtr(num * sizeof(T));
                successful_read_ = true;

                return num;
            }

            /**
             * Reads a single T object from the decompression buffer.
             *
             * \param data Buffer to read data into
             */
            template<typename T>
            inline size_t fread_single_(void* data) {
                if(STF_EXPECT_FALSE(!checkIfReadIsPossible_())) {
                    return 0;
                }

                const auto ptr = reinterpret_cast<T*>(data);
                const auto read_ptr = reinterpret_cast<T*>(out_buf_.get() + out_buf_.getReadPos());
                std::copy(read_ptr, read_ptr + 1, ptr);
                out_buf_.advanceReadPtr(sizeof(T));
                successful_read_ = true;

                return 1;
            }

            /**
             * Reads a single uint8_t from the decompression buffer.
             *
             * \param data Buffer to read data into
             */
            inline size_t fread_u8_(void* data) final {
                return fread_single_<uint8_t>(data);
            }

            /**
             * Reads a single uint16_t from the decompression buffer.
             *
             * \param data Buffer to read data into
             */
            inline size_t fread_u16_(void* data) final {
                return fread_single_<uint16_t>(data);
            }

            /**
             * Reads multiple uint16_t's from the decompression buffer.
             *
             * \param data Buffer to read data into
             * \param num Number of elements to read
             */
            inline size_t fread_u16_(void* data, const size_t num) final {
                return fread_multiple_<uint16_t>(data, num);
            }

            /**
             * Reads a single uint32_t from the decompression buffer.
             *
             * \param data Buffer to read data into
             */
            inline size_t fread_u32_(void* data) final {
                return fread_single_<uint32_t>(data);
            }

            /**
             * Reads multiple uint32_t's from the decompression buffer.
             *
             * \param data Buffer to read data into
             * \param num Number of elements to read
             */
            inline size_t fread_u32_(void* data, const size_t num) final {
                return fread_multiple_<uint32_t>(data, num);
            }

            /**
             * Reads a single uint64_t from the decompression buffer.
             *
             * \param data Buffer to read data into
             */
            inline size_t fread_u64_(void* data) final {
                return fread_single_<uint64_t>(data);
            }

            /**
             * Reads multiple uint64_t's from the decompression buffer.
             *
             * \param data Buffer to read data into
             * \param num Number of elements to read
             */
            inline size_t fread_u64_(void* data, const size_t num) final {
                return fread_multiple_<uint64_t>(data, num);
            }

            /**
             * "Reads" a PackedContainerView by setting the underlying pointer to the decompression buffer.
             *
             * It is expected that the caller will call unpack() on the PackedContainer to actually retrieve the data.
             *
             * \param data PackedContainerView to read data into
             * \param size Size of the PackedContainer pointed to by data
             */
            inline size_t freadPackedContainer_(PackedContainerViewBase& data, const size_t size) final {
                if(STF_EXPECT_FALSE(!checkIfReadIsPossible_())) {
                    return 0;
                }

                const auto read_ptr = out_buf_.get() + out_buf_.getReadPos();
                data.setView(read_ptr);
                out_buf_.advanceReadPtr(size);
                successful_read_ = true;

                return 1;
            }

            /**
             * Reads data from the decompression buffer
             * \param data Buffer to read data into
             * \param size Size of an element
             * \param num Number of elements to read
             */
            inline size_t fread_(void* data, const size_t size, const size_t num) final {
                return fread_multiple_<uint8_t>(data, size * num);
            }

            /**
             * Ends the current compressed chunk and resets the decompressor for the next chunk
             */
            inline void endChunk_() {
                decompressor_.reset();
            }

        public:
            STFCompressedIFstream() = default;

            /**
             * Constructs an STFCompressedIFstream
             *
             * \param filename Filename to open
             */
            explicit STFCompressedIFstream(const std::string_view filename) :
                STFCompressedIFstream()
            {
                open(filename);
            }

            /**
             * Opens a file
             * \param filename Filename to open
             */
            void open(const std::string_view filename) final {
                STFFstream::open(filename, "r");
                // Check the magic string
                std::array<char, Decompressor::getMagic().size() + 1> magic_str = {'\0'};
                direct_read_(magic_str, Decompressor::getMagic().size(), true);
                stf_assert(Decompressor::getMagic().compare(magic_str.data()) == 0, "Not a " << Decompressor::getMagic() << " file");

                // Read the number of instructions per chunk
                direct_read_(inst_chunk_size_, true);

                // Get the end of the last chunk
                direct_read_(end_of_last_chunk_, true);

                // Save current position so we can rewind later
                const auto cur_pos = ftell(stream_);

                // Read in the chunk indices
                fseek(stream_, end_of_last_chunk_, SEEK_SET);
                direct_read_(chunk_indices_, true);

                // Rewind to beginning of first chunk
                fseek(stream_, cur_pos, SEEK_SET);
                last_read_pos_ = cur_pos; // Reset last_read_pos_ so we don't think we already read the whole file

                // Size the input buffer to match the FS block size
                const size_t block_size = getFSBlockSize_();
                in_buf_.initSize(block_size);
                out_buf_.initSize(block_size);
                decompressed_buf_.initSize(block_size);

                next_chunk_end_ = inst_chunk_size_;
                if(chunk_indices_.empty()) {
                    next_chunk_index_it_ = chunk_indices_.end();
                }
                else {
                    next_chunk_index_it_ = std::next(chunk_indices_.begin());
                }

                // Read the first chunk synchronously so we can actually do some work
                readChunk_(next_chunk_index_it_, chunk_indices_.front().getUncompressedChunkSize(), out_buf_);

                // Read the next chunk
                readNextChunk_();
            }

            /**
             * Seeks by the specified number of instructions
             * \param num_instructions Number of instructions to seek by
             */
            inline void seek(size_t num_instructions) final {
                // If the seek point comes before the next chunk boundary, just seek normally within the chunk
                if(num_insts_ + num_instructions >= next_chunk_end_) {
                    // Throw away what's currently in the buffer since we're moving to a new chunk
                    out_buf_.consume();

                    // Otherwise, seek to the next chunk
                    if(STF_EXPECT_TRUE(decompression_in_progress_)) {
                        // Wait for decompressor to finish
                        decompress_result_.get();
                        decompression_in_progress_ = false;
                        std::swap(out_buf_, decompressed_buf_);
                    }

                    in_buf_.reset();
                    endChunk_();

                    const auto chunk_idx = (num_insts_ + num_instructions) / inst_chunk_size_;
                    if(STF_EXPECT_FALSE(chunk_idx >= chunk_indices_.size())) {
                        stf_throw("Attempted to seek past the end of the trace");
                    }

                    num_instructions = (num_insts_ + num_instructions) % inst_chunk_size_;
                    next_chunk_index_it_ = std::next(chunk_indices_.begin(), static_cast<ssize_t>(chunk_idx));
                    num_insts_ = chunk_idx * inst_chunk_size_;
                    next_chunk_end_ = num_insts_ + inst_chunk_size_;

                    fseek(stream_, next_chunk_index_it_->getOffset(), SEEK_SET);
                    last_read_pos_ = next_chunk_index_it_->getOffset();
                    pc_tracker_.forcePC(next_chunk_index_it_->getStartPC());
                    const auto current_chunk = next_chunk_index_it_;
                    if(STF_EXPECT_TRUE(next_chunk_index_it_ != chunk_indices_.end())) {
                        ++next_chunk_index_it_;
                    }

                    readChunk_(next_chunk_index_it_, current_chunk->getUncompressedChunkSize(), out_buf_);
                    readNextChunk_();
                }

                STFIFstream::seek(num_instructions);
            }

            /**
             * Gets whether the stream is still valid
             */
            explicit inline operator bool() const final {
                return !feof_() && STFFstream::operator bool();
            }

            /**
             * Callback for instruction opcode records
             */
            inline void instructionRecordCallback() final {
                STFIFstream::instructionRecordCallback();

                // If we cross a chunk boundary, move on to the next chunk
                if(STF_EXPECT_FALSE(num_insts_ >= next_chunk_end_)) {
                    next_chunk_end_ += inst_chunk_size_;

                    // There should be a decompressed chunk ready to go
                    if(STF_EXPECT_TRUE(decompression_in_progress_)) {
                        // Wait for decompressor to finish
                        decompress_result_.get();
                        decompression_in_progress_ = false;
                        std::swap(out_buf_, decompressed_buf_);
                    }

                    // Start decompressing the next chunk
                    readNextChunk_();
                }
            }
    };
} // end namespace stf

#endif
