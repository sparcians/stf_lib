#ifndef __STF_PROTOCOL_ID_HPP__
#define __STF_PROTOCOL_ID_HPP__

#include <cstdint>

namespace stf {
    enum class ProtocolId : uint8_t {
        INVALID = 0,
        TILELINK = 1
    };
} // end namespace std

#endif
