#ifndef __STF_ZSTD_DECOMPRESSOR_HPP__
#define __STF_ZSTD_DECOMPRESSOR_HPP__

#include <zstd.h>

#include "stf_compression_buffer.hpp"
#include "stf_exception.hpp"
#include "stf_zstd_interface.hpp"

namespace stf {
    /**
     * \class ZSTDDecompressor
     *
     * Wraps ZSTD stream decompression in an interface suitable for STFCompressedIFstream
     */
    class ZSTDDecompressor : public ZSTDInterface {
        private:
            ZSTD_DCtx* zstd_context_ = nullptr;
            bool leftover_data_ = false;

        public:
            ZSTDDecompressor() :
                zstd_context_(ZSTD_createDCtx())
            {
                ZSTD_initDStream(zstd_context_);
            }

            ~ZSTDDecompressor() {
                // Have to explicitly free the ZSTD context
                if(zstd_context_) {
                    ZSTD_freeDCtx(zstd_context_);
                }
            }

            /**
             * Decompresses data from in_buf into out_buf
             * \param out_buf Buffer to write decompressed data into
             * \param in_buf Buffer to read compressed data from
             * \return True if we ran out of data in in_buf before we could get the requested output size
             */
            template<typename OutPointerType, typename InPointerType>
            bool decompress(STFCompressionBufferBase<OutPointerType>& out_buf, STFCompressionBufferBase<InPointerType>& in_buf) {
                in_.pos = in_buf.getReadPos();
                in_.src = in_buf.template getPtrAs<void>();
                in_.size = in_buf.size();

                out_.pos = out_buf.end();
                out_.dst = out_buf.template getPtrAs<void>();
                out_.size = out_buf.size();

                size_t result = ZSTD_decompressStream(zstd_context_, &out_, &in_);
                stf_assert(!ZSTD_isError(result), "ZSTD error: " << ZSTD_getErrorName(result));
                in_buf.setReadPtr(in_.pos);
                out_buf.setWritePtr(out_.pos);
                leftover_data_ = result;
                return out_.pos < out_.size;
            }

            /**
             * Resets the decompression context so we can decompress a new chunk
             */
            void reset() {
#if ZSTD_VERSION_NUMBER >= 10308
                ZSTD_DCtx_reset(zstd_context_, ZSTD_reset_session_only);
#else
                ZSTD_initDStream(zstd_context_);
#endif
            }

            /**
             * Returns true if there is some data remaining in internal ZSTD buffers
             */
            bool hasPendingData() const {
                return leftover_data_;
            }
    };
} // end namespace stf

#endif
