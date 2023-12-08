#include "stf_factory.hpp"
#include "stf_protocol_data.hpp"

// Every protocol header needs to be listed in this section
// BEGIN PROTOCOL HEADERS
#include "protocols/tilelink.hpp"
// END PROTOCOL HEADERS

namespace stf {
    // REQUIRED to properly instantiate ProtocolFactory and all protocols::ProtocolData types
    // Should only be specified ONCE in a .cpp file
    FINALIZE_FACTORY(protocols::ProtocolData)

    // REQUIRED to properly instantiate tilelink::Channel factory and all protocols::tilelink::Channel types
    // Should only be specified ONCE in a .cpp file
    FINALIZE_FACTORY(protocols::tilelink::Channel)
} // end namespace stf
