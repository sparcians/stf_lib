#ifndef __STF_COMPRESSED_IFSTREAM_SINGLE_THREADED_HPP__
#define __STF_COMPRESSED_IFSTREAM_SINGLE_THREADED_HPP__

#include <cstddef>
#include <cstdint>
#include <string_view>

#include "stf_compression_buffer.hpp"
#include "stf_compressed_ifstream_base.hpp"

namespace stf {
    /**
     * \class STFCompressedIFstreamSingleThreaded
     *
     * Provides transparent on-the-fly decompression of a compressed STF file. Single-threaded implementation.
     */
    template<typename Decompressor>
    class STFCompressedIFstreamSingleThreaded final : public STFCompressedIFstreamBase<Decompressor, STFCompressedIFstreamSingleThreaded<Decompressor>> {
        private:
            using base_class = STFCompressedIFstreamBase<Decompressor, STFCompressedIFstreamSingleThreaded<Decompressor>>;

            friend base_class;

            // From STFCompressedIFstreamBase
            using base_class::decompressor_;
            using base_class::in_buf_;
            using base_class::out_buf_;
            using base_class::decompress_result_;
            using base_class::next_chunk_index_it_;
            using base_class::last_read_pos_;
            using base_class::block_size_;
            using base_class::readChunk_;
            using base_class::endChunk_;

            // From STFCompressedChunkedBase
            using base_class::marker_record_chunk_size_;
            using base_class::next_chunk_end_;
            using base_class::chunk_indices_;

            // From STFFstream
            using base_class::stream_;
            using base_class::pc_tracker_;
            using base_class::num_marker_records_;

            /**
             * Asynchronously read and decompress the next chunk in the file
             */
            void readNextChunk_() {
                // Make sure there are still chunks left to read
                if(STF_EXPECT_FALSE(next_chunk_index_it_ == chunk_indices_.end())) {
                    return;
                }
                const size_t uncompressed_chunk_size = next_chunk_index_it_->getUncompressedChunkSize();
                const auto chunk_it = ++next_chunk_index_it_;

                readChunk_(chunk_it, uncompressed_chunk_size, out_buf_);
            }

            inline void cancelCurrentChunks_() override final {
                out_buf_.consume();
                in_buf_.reset();
                endChunk_();
            }

        public:
            STFCompressedIFstreamSingleThreaded() = default;


            /**
             * Constructs an STFCompressedIFstreamSingleThreaded
             *
             * \param filename Filename to open
             */
            explicit STFCompressedIFstreamSingleThreaded(const std::string_view filename) : // cppcheck-suppress passedByValue
                STFCompressedIFstreamSingleThreaded()
            {
                base_class::open(filename);
            }

            /**
             * Callback for marker records
             */
            inline void markerRecordCallback() override final {
                STFIFstream::markerRecordCallback();

                // If we cross a chunk boundary, move on to the next chunk
                if(STF_EXPECT_FALSE(num_marker_records_ >= next_chunk_end_)) {
                    next_chunk_end_ += marker_record_chunk_size_;

                    // Start decompressing the next chunk
                    readNextChunk_();
                }
            }
    };
} // end namespace stf

#endif
