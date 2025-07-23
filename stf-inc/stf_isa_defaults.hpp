#ifndef __STF_ISA_DEFAULTS_HPP__
#define __STF_ISA_DEFAULTS_HPP__

#include <string>

#include "stf_enums.hpp"

namespace stf {
    /**
     * \class ISADefaults
     * Provides sensible default values for ISA-specific data
     */
    class ISADefaults {
        private:
            static const std::string empty_;
            static const std::string rv32_isa_extended_;
            static const std::string rv64_isa_extended_;

            ISADefaults() = default;

        public:
            /**
             * Returns a default value for traces that do not contain an ISAExtendedInfo record
             */
            static const std::string& getISAExtendedInfo(const ISA isa, const INST_IEM iem);
    };
} // end namespace stf

#endif
