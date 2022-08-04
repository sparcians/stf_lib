#include "stf_protocol_data.hpp"
#include "protocols/tilelink.hpp"

/**
 * \def REGISTER_PROTOCOL
 *
 * Registers a new protocol data type
 */
#define REGISTER_PROTOCOL(protocol_id, cls) REGISTER_WITH_FACTORY_NS(ProtocolFactory, protocol_id, stf::protocols, cls)

/**
 * \def REGISTER_TILELINK_CHANNEL
 *
 * Registers a new TileLink channel type
 */
#define REGISTER_TILELINK_CHANNEL(channel_id, cls) REGISTER_WITH_FACTORY_NS(ChannelFactory, channel_id, stf::protocols::tilelink, cls)

namespace stf {
    REGISTER_PROTOCOL(TILELINK, TileLink)
    REGISTER_TILELINK_CHANNEL(CHANNEL_A, ChannelA)
    REGISTER_TILELINK_CHANNEL(CHANNEL_B, ChannelB)
    REGISTER_TILELINK_CHANNEL(CHANNEL_C, ChannelC)
    REGISTER_TILELINK_CHANNEL(CHANNEL_D, ChannelD)
    REGISTER_TILELINK_CHANNEL(CHANNEL_E, ChannelE)
} // end namespace stf
