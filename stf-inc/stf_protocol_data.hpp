#ifndef __STF_PROTOCOL_DATA_HPP__
#define __STF_PROTOCOL_DATA_HPP__

#include "stf_object.hpp"
#include "stf_pool.hpp"
#include "stf_factory.hpp"

namespace stf {
    namespace protocols {
        enum class ProtocolId : uint8_t {
            TILELINK,
            RESERVED_END // Must be at the end
        };

        std::ostream& operator<<(std::ostream& os, ProtocolId id);

        /**
         * \class ProtocolData
         *
         * Represents the data associated with a transaction
         */
        class ProtocolData : public STFObject<ProtocolData, ProtocolId> {
            public:
                /**
                 * Constructs a ProtocolData
                 *
                 * \param protocol_id ID of the underlying protocol type
                 */
                explicit ProtocolData(const protocols::ProtocolId protocol_id) :
                    STFObject(protocol_id)
                {
                }
        };

        /**
         * \typedef TypeAwareProtocolData
         *
         * ProtocolData class that knows its own type
         */
        template<typename T>
        using TypeAwareProtocolData = TypeAwareSTFObject<T, ProtocolData>;

    } // end namespace protocols
    DECLARE_FACTORY(ProtocolFactory, protocols::ProtocolData)
} // end namespace stf

#endif
