#ifndef __STF_PROTOCOL_ID_HPP__
#define __STF_PROTOCOL_ID_HPP__

#include <cstdint>
#include <ostream>

namespace stf {
    namespace protocols {
        enum class ProtocolId : uint8_t {
            TILELINK,
            RESERVED_END // Must be at the end
        };

        std::ostream& operator<<(std::ostream& os, ProtocolId id);
    } // end namespace protocols
} // end namespace stf

#endif
