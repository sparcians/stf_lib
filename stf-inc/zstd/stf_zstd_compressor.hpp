#ifndef __STF_ZSTD_COMPRESSOR_HPP__
#define __STF_ZSTD_COMPRESSOR_HPP__

#include <algorithm>
#include <zstd.h>

#include "stf_compression_buffer.hpp"
#include "stf_exception.hpp"
#include "stf_zstd_interface.hpp"

#if ZSTD_VERSION_NUMBER < 10305
#define ZSTD_CLEVEL_DEFAULT 3
#endif

namespace stf {
    /**
     * \class ZSTDCompressor
     *
     * Wraps ZSTD stream compression in an interface suitable for STFCompressedOFstream
     */
    class ZSTDCompressor : public ZSTDInterface {
        private:
#if ZSTD_VERSION_NUMBER < 10308
            int compression_level_;
#endif
            ZSTD_CCtx* zstd_context_ = nullptr; /**< ZSTD compression context */

        public:
            static constexpr int DEFAULT_COMPRESSION_LEVEL = ZSTD_CLEVEL_DEFAULT; /**< Default compression level to use */

            /**
             * Constructs a ZSTDCompressor with the given compression level and strategy
             * \param compression_level Compression level
             */
            explicit ZSTDCompressor(const int compression_level = DEFAULT_COMPRESSION_LEVEL) :
#if ZSTD_VERSION_NUMBER < 10308
                compression_level_(compression_level),
#endif
                zstd_context_(ZSTD_createCCtx())
            {
#if ZSTD_VERSION_NUMBER >= 10308
                // set compression level and strategy
                const auto result = ZSTD_CCtx_setParameter(zstd_context_, ZSTD_c_compressionLevel, compression_level);
#else
                const auto result = ZSTD_initCStream(zstd_context_, compression_level_);
#endif
                stf_assert(!ZSTD_isError(result), "ZSTD Error: " << ZSTD_getErrorName(result));
            }

            ~ZSTDCompressor() {
                // Have to explicitly free the ZSTD context
                if(zstd_context_) {
                    ZSTD_freeCCtx(zstd_context_);
                }
            }

            /**
             * Compresses the data in in_buf into out_buf
             * \param in_buf input data to compress
             * \param out_buf buffer to place compressed data into
             */
            template<typename InPointerType>
            void compress(STFCompressionBuffer& out_buf, STFCompressionBufferBase<InPointerType>& in_buf) {
                // Ensure that the compressed version of in_buf will always fit in out_buf so we can
                // do the compression in one shot
                out_buf.fit(getBoundedSize(in_buf));

                in_.pos = 0;
                in_.src = in_buf.template getPtrAs<void>();
                in_.size = in_buf.size();
                out_.dst = out_buf.getPtrAs<void>();
                out_.size = out_buf.size();
#if ZSTD_VERSION_NUMBER >= 10308
                const size_t result = ZSTD_compressStream2(zstd_context_, &out_, &in_, ZSTD_e_continue);
#else
                const size_t result = ZSTD_compressStream(zstd_context_, &out_, &in_);
#endif
                stf_assert(!ZSTD_isError(result), "ZSTD Error: " << ZSTD_getErrorName(result));
                // Should never trigger these two assertions since out_buf is sized to always fit in_buf
                stf_assert(in_.pos == in_.size, "ZSTD couldn't fit input into output");
                stf_assert(!result, "Remaining data in ZSTD context");
                // Advance write pointer
                out_buf.setWritePtr(out_.pos);
                // We read all of in_buf_, so reset it
                in_buf.reset();
            }

            /**
             * Gets an initial bound for the output buffer based on the size of a ZSTD block
             */
            static size_t getInitialBoundedSize() {
                return ZSTD_CStreamOutSize();
            }

            /**
             * Gets the bounded compressed size of the input buffer
             * \param in_buf Input data to analyze
             */
            template<typename InPointerType>
            static size_t getBoundedSize(const STFCompressionBufferBase<InPointerType>& in_buf) {
                return std::max(getInitialBoundedSize(), ZSTD_compressBound(in_buf.end()));
            }

            /**
             * Flush any data that might be remaining in internal compressor buffers
             */
            void flush(STFCompressionBuffer& out_buf) {
                // Output buffer should at least be able to fit an entire block
                out_buf.fit(getInitialBoundedSize());

                out_.dst = out_buf.getPtrAs<void>();
                out_.size = out_buf.size();

                size_t result = ZSTD_endStream(zstd_context_, &out_);
                stf_assert(!ZSTD_isError(result), "ZSTD Error: " << ZSTD_getErrorName(result));

                // The flush should always succeed since we sized the buffer accordingly
                stf_assert(!result, "ZSTD failed to flush entire block.");
                out_buf.setWritePtr(out_.pos);

                // Reset the compressor state for the next chunk
#if ZSTD_VERSION_NUMBER >= 10308
                result = ZSTD_CCtx_reset(zstd_context_, ZSTD_reset_session_only);
#else
                result = ZSTD_initCStream(zstd_context_, compression_level_);
#endif
                stf_assert(!ZSTD_isError(result), "ZSTD Error: " << ZSTD_getErrorName(result));

                // We can reset the internal output ptr since we flushed everything
                out_.pos = 0;
            }

    };
} // end namespace stf

#endif
