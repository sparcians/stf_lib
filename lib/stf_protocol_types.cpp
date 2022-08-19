#include "stf_factory.hpp"
#include "stf_protocol_data.hpp"
#include "protocols/tilelink.hpp"

namespace stf {
    // REQUIRED to properly instantiate ProtocolFactory and all protocols::ProtocolData types
    // Should only be specified ONCE in a .cpp file
    FINALIZE_FACTORY(protocols::ProtocolData)

    // REQUIRED to properly instantiate TLChannelFactory and all protocols::tilelink::Channel types
    // Should only be specified ONCE in a .cpp file
    FINALIZE_FACTORY(protocols::tilelink::Channel)
} // end namespace stf
