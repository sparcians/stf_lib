#include "stf_enum_utils.hpp"
#include "stf_ifstream.hpp"
#include "stf_protocol_data.hpp"

namespace stf {
    namespace protocols {
        std::ostream& operator<<(std::ostream& os, const ProtocolId id) {
            switch(id) {
                case ProtocolId::__RESERVED_START:
                    os << "RESERVED_START";
                    return os;
                case ProtocolId::TILELINK:
                    os << "TILELINK";
                    return os;
                case ProtocolId::__RESERVED_END:
                    os << "RESERVED_END";
                    return os;
            };

            os << "UNKNOWN_" + std::to_string(enums::to_printable_int(id));
            return os;
        }
    }
}
