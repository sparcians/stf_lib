#include "stf_enum_utils.hpp"
#include "protocols/tilelink.hpp"

namespace stf {
    namespace protocols {
        namespace tilelink {
            std::ostream& operator<<(std::ostream& os, const ChannelType id) {
                switch(id) {
                    case ChannelType::CHANNEL_A:
                        os << "CHANNEL_A";
                        return os;
                    case ChannelType::CHANNEL_B:
                        os << "CHANNEL_B";
                        return os;
                    case ChannelType::CHANNEL_C:
                        os << "CHANNEL_C";
                        return os;
                    case ChannelType::CHANNEL_D:
                        os << "CHANNEL_D";
                        return os;
                    case ChannelType::CHANNEL_E:
                        os << "CHANNEL_E";
                        return os;
                    case ChannelType::RESERVED_END:
                        os << "RESERVED_END";
                        return os;
                };

                os << "UNKNOWN_" + std::to_string(enums::to_printable_int(id));

                    return os;
            }
        }
    }
}
