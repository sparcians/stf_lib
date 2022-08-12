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
    class STFCompressedIFstreamSingleThreaded : public STFCompressedIFstreamBase<Decompressor, STFCompressedIFstreamSingleThreaded<Decompressor>> {
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

        public:
            STFCompressedIFstreamSingleThreaded() = default;


            /**
             * Constructs an STFCompressedIFstreamSingleThreaded
             *
             * \param filename Filename to open
             */
            explicit STFCompressedIFstreamSingleThreaded(const std::string_view filename) :
                STFCompressedIFstreamSingleThreaded()
            {
                base_class::open(filename);
            }

            /**
             * Seeks by the specified number of marker records
             * \param num_markers Number of marker records to seek by
             */
            inline void seek(size_t num_markers) final {
                // If the seek point comes before the next chunk boundary, just seek normally within the chunk
                if(num_marker_records_ + num_markers >= next_chunk_end_) {
                    // Throw away what's currently in the buffer since we're moving to a new chunk
                    out_buf_.consume();

                    in_buf_.reset();
                    endChunk_();

                    const auto chunk_idx = (num_marker_records_ + num_markers) / marker_record_chunk_size_;
                    if(STF_EXPECT_FALSE(chunk_idx >= chunk_indices_.size())) {
                        stf_throw("Attempted to seek past the end of the trace");
                    }

                    num_markers = (num_marker_records_ + num_markers) % marker_record_chunk_size_;
                    next_chunk_index_it_ = std::next(chunk_indices_.begin(), static_cast<ssize_t>(chunk_idx));
                    num_marker_records_ = chunk_idx * marker_record_chunk_size_;
                    next_chunk_end_ = num_marker_records_ + marker_record_chunk_size_;

                    fseek(stream_, next_chunk_index_it_->getOffset(), SEEK_SET);
                    last_read_pos_ = next_chunk_index_it_->getOffset();
                    pc_tracker_.forcePC(next_chunk_index_it_->getStartPC());
                    const auto current_chunk = next_chunk_index_it_;
                    if(STF_EXPECT_TRUE(next_chunk_index_it_ != chunk_indices_.end())) {
                        ++next_chunk_index_it_;
                    }

                    readChunk_(next_chunk_index_it_, current_chunk->getUncompressedChunkSize(), out_buf_);
                }

                STFIFstream::seek(num_markers);
            }

            /**
             * Callback for marker records
             */
            inline void markerRecordCallback() final {
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
