#ifndef __STF_COMPRESSED_IFSTREAM_HPP__
#define __STF_COMPRESSED_IFSTREAM_HPP__

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <future>
#include <string_view>

#include "stf_compression_buffer.hpp"
#include "stf_compressed_ifstream_base.hpp"

namespace stf {
    /**
     * \class STFCompressedIFstream
     *
     * Provides transparent on-the-fly decompression of a compressed STF file
     */
    template<typename Decompressor>
    class STFCompressedIFstream final : public STFCompressedIFstreamBase<Decompressor, STFCompressedIFstream<Decompressor>> {
        private:
            using base_class = STFCompressedIFstreamBase<Decompressor, STFCompressedIFstream<Decompressor>>;

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

            bool decompression_in_progress_ = false; /**< Flag indicating whether a chunk is currently being decompressed */
            STFCompressionBuffer decompressed_buf_; /**< Decompressed data pointer */

            /**
             * Returns true if all data has been consumed from the file and decompression buffers
             */
            inline bool allInputConsumedImpl_() const {
                return base_class::allInputConsumedImpl_() && decompressed_buf_.consumed();
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
             * Closes the file
             */
            int close_() override {
                if(!stream_) {
                    return 0;
                }

                if(decompression_in_progress_) {
                    decompress_result_.get();
                }

                return STFIFstream::close_();
            }

            inline void finishDecompression_() {
                if(STF_EXPECT_TRUE(decompression_in_progress_)) {
                    // Wait for decompressor to finish
                    decompress_result_.get();
                    decompression_in_progress_ = false;
                    std::swap(out_buf_, decompressed_buf_);
                }
            }

            inline void cancelCurrentChunks_() override final {
                out_buf_.consume();
                finishDecompression_();
                in_buf_.reset();
                endChunk_();
            }

            inline void seekToChunk_(const size_t chunk_idx) override final {
                base_class::seekToChunk_(chunk_idx);
                readNextChunk_();
            }

        public:
            STFCompressedIFstream() = default;

            /**
             * Constructs an STFCompressedIFstream
             *
             * \param filename Filename to open
             */
            explicit STFCompressedIFstream(const std::string_view filename) : // cppcheck-suppress passedByValue
                STFCompressedIFstream()
            {
                STFCompressedIFstream::open(filename);
            }

            // Have to override the base class destructor to ensure that *our* close method gets called before destruction
            inline ~STFCompressedIFstream() override {
                STF_FSTREAM_ACQUIRE_OPEN_CLOSE_LOCK();
                STFCompressedIFstream::close_();
            }

            /**
             * Opens a file
             * \param filename Filename to open
             */
            void open(const std::string_view filename) override final { // cppcheck-suppress passedByValue
                base_class::open(filename);
                decompressed_buf_.initSize(block_size_);
                // Read the next chunk
                readNextChunk_();
            }

            /**
             * Callback for marker records
             */
            inline void markerRecordCallback() override final {
                STFIFstream::markerRecordCallback();

                // If we cross a chunk boundary, move on to the next chunk
                if(STF_EXPECT_FALSE(num_marker_records_ >= next_chunk_end_)) {
                    next_chunk_end_ += marker_record_chunk_size_;

                    // There should be a decompressed chunk ready to go
                    finishDecompression_();

                    // Start decompressing the next chunk
                    readNextChunk_();
                }
            }
    };
} // end namespace stf

#endif
