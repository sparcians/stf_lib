#ifndef __STF_PROTOCOL_DATA_HPP__
#define __STF_PROTOCOL_DATA_HPP__

#include "stf_object.hpp"
#include "stf_pool.hpp"
#include "stf_protocol_id.hpp"
#include "stf_factory.hpp"

namespace stf {
    namespace protocols {
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
         * \class TypeAwareProtocolData
         *
         * ProtocolData class that knows its own type
         */
        template<typename T>
        class TypeAwareProtocolData : public TypeAwareSTFObject<T, ProtocolData> {
            public:
                using TypeAwareSTFObject<T, ProtocolData>::getTypeId;

                /**
                 * Specialization for ProtocolData subclasses. Since the protocol ID is captured in the header,
                 * we don't need to write it for each ProtocolData record.
                 * \param writer Writer to use
                 */
                inline static void writeTraceId(STFOFstream& writer) {
                    // We don't need to write the protocol ID since it's captured in
                    // a ProtocolIdRecord in the header.
                    // Just make sure that the protocol we're writing matches the header record.
                    stf_assert(getTypeId() == writer.getProtocolId(),
                               "Attempted to write protocol " << getTypeId() <<
                               " to a trace configured for " << writer.getProtocolId());
                }
        };

    } // end namespace protocols

    class STFIFstream;
    /**
     * Specialization of STFIFstream::operator>> for ProtocolData objects. Needed since protocol ID is set globally in the trace instead of on a per-record basis.
     * \param strm STFIFstream to read from
     * \param ptr Record is read into this pointer
     */
    STFIFstream& operator>>(STFIFstream& strm, protocols::ProtocolData::UniqueHandle& ptr);

    DECLARE_FACTORY(ProtocolFactory, protocols::ProtocolData)
} // end namespace stf

#endif
