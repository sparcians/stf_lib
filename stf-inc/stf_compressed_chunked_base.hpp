#ifndef __STF_COMPRESSED_CHUNKED_BASE_HPP__
#define __STF_COMPRESSED_CHUNKED_BASE_HPP__

#include <cstddef>
#include <cstdint>
#include <vector>
#include <fcntl.h>

#include "stf_pc_tracker.hpp"

namespace stf {
    /**
     * \class STFCompressedChunkedBase
     *
     * Holds some member variables common to chunked compressor/decompressor interfaces
     */
    class STFCompressedChunkedBase {
        public:
            static constexpr size_t DEFAULT_CHUNK_SIZE = 10000000; /**< Default chunk size, in number of instructions */

        protected:
            /**
             * \class ChunkOffset
             * Tracks the file offset and starting PC of a compressed chunk
             */
            class ChunkOffset {
                private:
                    off_t offset_; /**< File offset */
                    uint64_t start_pc_; /**< Start PC */
                    size_t uncompressed_chunk_size_; /**< Uncompressed chunk size */

                public:
                    ChunkOffset() = default;

                    /**
                     * Constructs a ChunkOffset
                     * \param offset Offset
                     * \param start_pc Start PC
                     * \param uncompressed_chunk_size Uncompressed size of the chunk
                     */
                    ChunkOffset(const off_t offset, const uint64_t start_pc, const size_t uncompressed_chunk_size) :
                        offset_(offset),
                        start_pc_(start_pc),
                        uncompressed_chunk_size_(uncompressed_chunk_size)
                    {
                    }

                    /**
                     * Gets the file offset
                     */
                    off_t getOffset() const {
                        return offset_;
                    }

                    /**
                     * Gets the start PC
                     */
                    uint64_t getStartPC() const {
                        return start_pc_;
                    }

                    /**
                     * Gets the uncompressed size of the chunk
                     */
                    size_t getUncompressedChunkSize() const {
                        return uncompressed_chunk_size_;
                    }

                    /**
                     * Sets the file offset, start PC, and uncompressed chunk size
                     * \param offset Offset
                     * \param start_pc Start PC
                     * \param uncompressed_chunk_size Uncompressed size of the chunk
                     */
                    void set(const off_t offset, const uint64_t start_pc, const size_t uncompressed_chunk_size) {
                        offset_ = offset;
                        start_pc_ = start_pc;
                        uncompressed_chunk_size_ = uncompressed_chunk_size;
                    }

                    /**
                     * Sets the uncompressed size of the chunk
                     */
                    void setUncompressedChunkSize(const size_t uncompressed_chunk_size) {
                        uncompressed_chunk_size_ = uncompressed_chunk_size;
                    }
            };

            size_t inst_chunk_size_ = DEFAULT_CHUNK_SIZE; /**< Number of instructions per compressed chunk */
            size_t next_chunk_end_ = 0; /**< The number of instructions that will have been seen when this chunk ends */
            std::vector<ChunkOffset> chunk_indices_; /**< Holds file offsets for each compressed chunk */

            STFCompressedChunkedBase() = default;

    };
} // end namespace stf

#endif
