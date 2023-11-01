#ifndef __STF_PROTOCOL_ID_HPP__
#define __STF_PROTOCOL_ID_HPP__

#include <cstdint>
#include <ostream>
#include "stf_enum_utils.hpp"

namespace stf {
    namespace protocols {
        STF_ENUM(
            STF_ENUM_CONFIG(AUTO_PRINT),
            ProtocolId,
            uint8_t,
            TILELINK
        );
    } // end namespace protocols
} // end namespace stf

#endif
