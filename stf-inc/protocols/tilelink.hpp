#ifndef __STF_TILELINK_HPP__
#define __STF_TILELINK_HPP__

#include "stf_record_types.hpp"
#include "stf_protocol_data.hpp"
#include "protocols/channel_protocol.hpp"
#include "stf_ifstream.hpp"
#include "stf_serializable_container.hpp"
#include "format_utils.hpp"

/**
 * \def TL_CHANNEL
 *
 * Declares a TL channel with the specified name, enum value, and fields
 */
#define TL_CHANNEL(class_name, enum_val, ...) \
    FIELD_CHANNEL(class_name, enum_val, fields, __VA_ARGS__); \
    REGISTER_CHANNEL(tilelink, class_name)

namespace stf {
    namespace protocols::tilelink {
        namespace fields {
            FIELD(Code, uint8_t);
            FIELD(Param, uint8_t);
            FIELD(Size, uint8_t);
            FIELD(Source, uint64_t);
            VECTOR_FIELD(Data, uint8_t, uint16_t);
            PACKED_BIT_VECTOR_FIELD(Mask, uint8_t, uint16_t);
            FIELD(Address, uint64_t);
            FIELD(Sink, uint64_t);
        }

        CHANNEL_IDS(uint8_t,
                    CHANNEL_A,
                    CHANNEL_B,
                    CHANNEL_C,
                    CHANNEL_D,
                    CHANNEL_E
        );

        TL_CHANNEL(ChannelA, CHANNEL_A, Code, Param, Size, Source, Data, Address, Mask)
        TL_CHANNEL(ChannelB, CHANNEL_B, Code, Param, Size, Source, Data, Address, Mask)
        TL_CHANNEL(ChannelC, CHANNEL_C, Code, Param, Size, Source, Data, Address)
        TL_CHANNEL(ChannelD, CHANNEL_D, Code, Param, Size, Source, Data, Sink)
        TL_CHANNEL(ChannelE, CHANNEL_E, Sink)
    }

    namespace protocols {
        /**
         * \class TileLink
         * Represents a TileLink protocol transaction
         */
        using TileLink = proto::TypeAwareChannelProtocol<ProtocolId::TILELINK, tilelink::Channel>;
        REGISTER_PROTOCOL(TileLink)
    }
}

#undef TL_CHANNEL

#endif
