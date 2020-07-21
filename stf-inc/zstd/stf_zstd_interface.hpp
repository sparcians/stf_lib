#ifndef __STF_ZSTD_INTERFACE_HPP__
#define __STF_ZSTD_INTERFACE_HPP__

#include <string_view>
#include <zstd.h>

namespace stf {
    /**
     * \class ZSTDInterface
     *
     * Base class used to define common features of ZSTD compressors/decompressors
     */
    class ZSTDInterface {
        private:
            static constexpr std::string_view MAGIC_ = "ZSTF"; /**< Magic string placed at the head of the file to identify its format */
        protected:
            ZSTD_inBuffer in_ = {nullptr, 0, 0}; /**< ZSTD input buffer - holds uncompressed data */
            ZSTD_outBuffer out_ = {nullptr, 0, 0}; /**< ZSTD output buffer - holds compressed data */

            ZSTDInterface() = default;

        public:
            /**
             * Get the magic string for ZSTD
             */
            static constexpr std::string_view getMagic() {
                return MAGIC_;
            }
    };
} // end namespace stf

#endif
