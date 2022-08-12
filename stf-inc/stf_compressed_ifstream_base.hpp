#ifndef __STF_COMPRESSED_IFSTREAM_BASE_HPP__
#define __STF_COMPRESSED_IFSTREAM_BASE_HPP__

#include <future>

#include "stf_compression_buffer.hpp"
#include "stf_compressed_chunked_base.hpp"
#include "stf_ifstream.hpp"

namespace stf {
    /**
     * \class STFCompressedIFstreamBase
     *
     * Base class for common variables/methods used by STFCompressedIFstream* classes
     */
    template<typename Decompressor, typename STFCompressedIFstreamImpl>
    class STFCompressedIFstreamBase : public STFIFstream, public STFCompressedChunkedBase {
        protected:
            Decompressor decompressor_; /**< Decompressor object */
            STFCompressionBuffer in_buf_; /**< Input data buffer - holds compressed data read from the file */
            STFCompressionBuffer out_buf_; /**< Output data pointer */
            std::future<void> decompress_result_; /**< Future used to indicate when the next chunk has been decompressed */
            std::vector<ChunkOffset>::iterator next_chunk_index_it_ = chunk_indices_.end(); /**< Iterator to next chunk offset */
            off_t last_read_pos_ = 0; /**< File offset of last read */
            off_t end_of_last_chunk_; /**< File offset of the end of the last compressed chunk - needed so we can know when to stop reading */
            bool successful_read_ = false; /**< Indicates whether the last read succeeded - needed so we can know if we still have valid data in our buffers */
            size_t block_size_ = 0; /**< Block size of the filesystem containing the trace */

            /**
             * Returns true if we have read all of the compressed chunks from the file
             */
            inline bool reachedEndOfChunks_() const {
                return last_read_pos_ + static_cast<off_t>(in_buf_.getReadPos()) >= end_of_last_chunk_;
            }

            /**
             * Returns true if all data has been consumed from the file and decompression buffers
             */
            inline bool allInputConsumedImpl_() const {
                return reachedEndOfChunks_() && out_buf_.consumed();
            }

            /**
             * Returns true if all data has been consumed from the file and decompression buffers
             */
            inline bool allInputConsumed_() const {
                return static_cast<const STFCompressedIFstreamImpl*>(this)->allInputConsumedImpl_();
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
            inline std::enable_if_t<std::is_enum_v<T>, size_t>
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
            inline std::enable_if_t<std::is_arithmetic_v<T>, size_t>
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
            inline std::enable_if_t<type_utils::is_arithmetic_or_enum_v<T>, size_t>
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
            inline std::enable_if_t<std::negation_v<type_utils::is_arithmetic_or_enum<T>>, size_t>
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
            STFCompressedIFstreamBase() = default;

            /**
             * Opens a file
             * \param filename Filename to open
             */
            void open(const std::string_view filename) override {
                STFFstream::open(filename, "r");
                // Check the magic string
                std::array<char, Decompressor::getMagic().size() + 1> magic_str = {'\0'};
                direct_read_(magic_str, Decompressor::getMagic().size(), true);
                stf_assert(Decompressor::getMagic().compare(magic_str.data()) == 0, "Not a " << Decompressor::getMagic() << " file");

                // Read the number of marker records per chunk
                direct_read_(marker_record_chunk_size_, true);

                stf_assert(marker_record_chunk_size_ > 0, "Invalid chunk size. Trace file may be corrupt.");

                // Get the end of the last chunk
                direct_read_(end_of_last_chunk_, true);

                stf_assert(end_of_last_chunk_ != 0,
                           "Last chunk pointer is 0. Trace file may be corrupt.");

                const auto file_stat = getFileStat_();
                stf_assert(end_of_last_chunk_ < file_stat.st_size,
                           "Last chunk pointer (" << end_of_last_chunk_ << ") is larger than the trace file size (" << file_stat.st_size << "). Trace file may be corrupt.");

                // Save current position so we can rewind later
                const auto cur_pos = ftell(stream_);

                // Read in the chunk indices
                fseek(stream_, end_of_last_chunk_, SEEK_SET);
                direct_read_(chunk_indices_, true);

                // Rewind to beginning of first chunk
                fseek(stream_, cur_pos, SEEK_SET);
                last_read_pos_ = cur_pos; // Reset last_read_pos_ so we don't think we already read the whole file

                // Size the input buffer to match the FS block size
                block_size_ = static_cast<size_t>(file_stat.st_blksize);
                in_buf_.initSize(block_size_);
                out_buf_.initSize(block_size_);

                next_chunk_end_ = marker_record_chunk_size_;
                if(chunk_indices_.empty()) {
                    next_chunk_index_it_ = chunk_indices_.end();
                }
                else {
                    next_chunk_index_it_ = std::next(chunk_indices_.begin());
                }

                // Read the first chunk synchronously so we can actually do some work
                readChunk_(next_chunk_index_it_, chunk_indices_.front().getUncompressedChunkSize(), out_buf_);
            }

            /**
             * Gets whether the stream is still valid
             */
            explicit inline operator bool() const final {
                return !feof_() && STFFstream::operator bool();
            }

    };
} // end namespace stf

#endif
