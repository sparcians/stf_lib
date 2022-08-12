#ifndef __STF_TILELINK_HPP__
#define __STF_TILELINK_HPP__

#include "stf_record_types.hpp"
#include "stf_protocol_data.hpp"
#include "stf_ifstream.hpp"
#include "stf_serializable_container.hpp"
#include "format_utils.hpp"

namespace stf {
    class TransactionRecord;

    namespace protocols {
        namespace tilelink {
            enum class ChannelType : uint8_t {
                CHANNEL_A,
                CHANNEL_B,
                CHANNEL_C,
                CHANNEL_D,
                CHANNEL_E,
                RESERVED_END // Must be at end
            };

            std::ostream& operator<<(std::ostream& os, ChannelType id);

            /**
             * \class Channel
             *
             * Base class for all TileLink channels
             */
            class Channel : public STFObject<Channel, ChannelType> {
                public:
                    // Allows SinkChannel to use read_ and write_ methods from STFObject
                    friend class SinkChannel;

                    /**
                     * Constructs a Channel object
                     * \param channel_type Channel ID
                     */
                    explicit Channel(const ChannelType channel_type) :
                        STFObject(channel_type)
                    {
                    }
            };

            /**
             * \typedef TypeAwareChannel
             *
             * Channel class that knows its own type
             */
            template<typename T>
            using TypeAwareChannel = TypeAwareSTFObject<T, Channel>;

            /**
             * \class DataChannel
             *
             * Standard TileLink channel with data
             */
            template<typename T>
            class DataChannel : public TypeAwareChannel<T> {
                private:
                    uint8_t code_;
                    uint8_t param_;
                    uint8_t size_;
                    uint64_t source_;
                    SerializableVector<uint8_t, uint16_t> data_;

                public:
                    DataChannel() = default;

                    /**
                     * Constructs a tilelink::DataChannel from an STFIFstream
                     * \param reader STFIFstream to read the tilelink::DataChannel object from
                     */
                    explicit DataChannel(STFIFstream& reader) {
                        unpack_impl(reader);
                    }

                    /**
                     * Constructs a tilelink::DataChannel
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param data TileLink data field
                     */
                    DataChannel(const uint8_t code,
                                const uint8_t param,
                                const uint8_t size,
                                const uint64_t source,
                                std::vector<uint8_t>&& data) :
                        code_(code),
                        param_(param),
                        size_(size),
                        source_(source),
                        data_(std::move(data))
                    {
                    }

                    /**
                     * Constructs a tilelink::DataChannel
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param data TileLink data field
                     */
                    DataChannel(const uint8_t code,
                                const uint8_t param,
                                const uint8_t size,
                                const uint64_t source,
                                const std::vector<uint8_t>& data) :
                        code_(code),
                        param_(param),
                        size_(size),
                        source_(source),
                        data_(data)
                    {
                    }

                    /**
                     * Constructs a tilelink::DataChannel without populating the data field
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     */
                    DataChannel(const uint8_t code,
                                const uint8_t param,
                                const uint8_t size,
                                const uint64_t source) :
                        DataChannel(code, param, size, source, {})
                    {
                    }

                    /**
                     * Unpacks a tilelink::DataChannel object from an STFIFstream
                     * \param reader STFIFstream to use
                     */
                    inline void unpack_impl(STFIFstream& reader) {
                        TypeAwareChannel<T>::read_(reader, code_, param_, size_, source_, data_);
                    }

                    /**
                     * Packs a tilelink::DataChannel object to an STFOFstream
                     * \param writer STFOFstream to use
                     */
                    inline void pack_impl(STFOFstream& writer) const {
                        TypeAwareChannel<T>::write_(writer, code_, param_, size_, source_, data_);
                    }

                    /**
                     * Formats a tilelink::DataChannel object to an std::ostream
                     * \param os ostream to use
                     */
                    inline void format_impl(std::ostream& os) const {
                        os << std::endl;
                        format_utils::formatLabel(os, "CODE");
                        format_utils::formatHex(os, code_);
                        os << std::endl;
                        format_utils::formatLabel(os, "PARAM");
                        format_utils::formatHex(os, param_);
                        os << std::endl;
                        format_utils::formatLabel(os, "SIZE");
                        format_utils::formatDec(os, size_);
                        os << std::endl;
                        format_utils::formatLabel(os, "SOURCE");
                        format_utils::formatHex(os, source_);
                        os << std::endl;
                        format_utils::formatLabel(os, "DATA");
                        os << data_;
                    }

                    /**
                     * Gets the code field
                     */
                    inline auto getCode() const {
                        return code_;
                    }

                    /**
                     * Gets the param field
                     */
                    inline auto getParam() const {
                        return param_;
                    }

                    /**
                     * Gets the size field
                     */
                    inline auto getSize() const {
                        return size_;
                    }

                    /**
                     * Gets the source id
                     */
                    inline auto getSource() const {
                        return source_;
                    }

                    /**
                     * Gets the data values
                     */
                    inline const auto& getData() const {
                        return data_;
                    }
            };

            /**
             * \class AddressChannel
             *
             * TileLink DataChannel that has an address field
             */
            template<typename T>
            class AddressChannel : public DataChannel<T> {
                private:
                    uint64_t address_;

                public:
                    AddressChannel() = default;

                    /**
                     * Constructs a tilelink::AddressChannel from an STFIFstream
                     * \param reader STFIFstream to read the tilelink::AddressChannel object from
                     */
                    explicit AddressChannel(STFIFstream& reader) {
                        unpack_impl(reader);
                    }

                    /**
                     * Constructs a tilelink::AddressChannel
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     * \param data TileLink data field
                     */
                    AddressChannel(const uint8_t code,
                                   const uint8_t param,
                                   const uint8_t size,
                                   const uint64_t source,
                                   const uint64_t address,
                                   const std::vector<uint8_t>& data) :
                        DataChannel<T>(code, param, size, source, data),
                        address_(address)
                    {
                    }

                    /**
                     * Constructs a tilelink::AddressChannel
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     * \param data TileLink data field
                     */
                    AddressChannel(const uint8_t code,
                                   const uint8_t param,
                                   const uint8_t size,
                                   const uint64_t source,
                                   const uint64_t address,
                                   std::vector<uint8_t>&& data) :
                        DataChannel<T>(code, param, size, source, data),
                        address_(address)
                    {
                    }

                    /**
                     * Constructs a tilelink::AddressChannel without populating the data field
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     */
                    AddressChannel(const uint8_t code,
                                   const uint8_t param,
                                   const uint8_t size,
                                   const uint64_t source,
                                   const uint64_t address) :
                        AddressChannel(code, param, size, source, address, {})
                    {
                    }

                    /**
                     * Unpacks a tilelink::AddressChannel object from an STFIFstream
                     * \param reader STFIFstream to use
                     */
                    inline void unpack_impl(STFIFstream& reader) {
                        DataChannel<T>::unpack_impl(reader);
                        TypeAwareChannel<T>::read_(reader, address_);
                    }

                    /**
                     * Packs a tilelink::AddressChannel object to an STFOFstream
                     * \param writer STFOFstream to use
                     */
                    inline void pack_impl(STFOFstream& writer) const {
                        DataChannel<T>::pack_impl(writer);
                        TypeAwareChannel<T>::write_(writer, address_);
                    }

                    /**
                     * Formats a tilelink::AddressChannel object to an std::ostream
                     * \param os ostream to use
                     */
                    inline void format_impl(std::ostream& os) const {
                        os << std::endl;
                        format_utils::formatLabel(os, "ADDRESS");
                        format_utils::formatVA(os, address_);
                        DataChannel<T>::format_impl(os);
                    }

                    /**
                     * Gets the address value
                     */
                    inline auto getAddress() const {
                        return address_;
                    }
            };

            /**
             * \class SinkChannel
             *
             * Mix-in class that adds a sink field.
             * Does not inherit from Channel to avoid the diamond problem.
             */
            class SinkChannel {
                private:
                    uint64_t sink_;

                public:
                    SinkChannel() = default;

                    /**
                     * Constructs a tilelink::SinkChannel from an STFIFstream
                     * \param reader STFIFstream to read the tilelink::SinkChannel object from
                     */
                    explicit SinkChannel(STFIFstream& reader) {
                        unpack_impl(reader);
                    }

                    /**
                     * Constructs a tilelink::SinkChannel
                     * \param sink TileLink sink field
                     */
                    explicit SinkChannel(const uint64_t sink) :
                        sink_(sink)
                    {
                    }

                    /**
                     * Unpacks a tilelink::SinkChannel object from an STFIFstream
                     * \param reader STFIFstream to use
                     */
                    inline void unpack_impl(STFIFstream& reader) {
                        Channel::read_(reader, sink_);
                    }

                    /**
                     * Packs a tilelink::SinkChannel object to an STFOFstream
                     * \param writer STFOFstream to use
                     */
                    inline void pack_impl(STFOFstream& writer) const {
                        Channel::write_(writer, sink_);
                    }

                    /**
                     * Formats a tilelink::SinkChannel object to an std::ostream
                     * \param os ostream to use
                     */
                    inline void format_impl(std::ostream& os) const {
                        os << std::endl;
                        format_utils::formatLabel(os, "SINK");
                        format_utils::formatHex(os, sink_);
                    }

                    /**
                     * Gets the sink ID
                     */
                    inline auto getSink() const {
                        return sink_;
                    }
            };

            /**
             * \class MaskedChannel
             *
             * TileLink AddressChannel that has a mask field
             */
            template<typename T>
            class MaskedChannel : public AddressChannel<T> {
                private:
                    SerializableVector<uint8_t, uint16_t> mask_;

                public:
                    MaskedChannel() = default;

                    /**
                     * Constructs a tilelink::MaskedChannel from an STFIFstream
                     * \param reader STFIFstream to read the tilelink::MaskedChannel object from
                     */
                    explicit MaskedChannel(STFIFstream& reader) {
                        unpack_impl(reader);
                    }

                    /**
                     * Constructs a tilelink::MaskedChannel
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     * \param data TileLink data field
                     * \param mask TileLink mask field
                     */
                    MaskedChannel(const uint8_t code,
                                  const uint8_t param,
                                  const uint8_t size,
                                  const uint64_t source,
                                  const uint64_t address,
                                  const std::vector<uint8_t>& data,
                                  const std::vector<uint8_t>& mask) :
                        AddressChannel<T>(code, param, size, source, address, data),
                        mask_(mask)
                    {
                    }

                    /**
                     * Constructs a tilelink::MaskedChannel
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     * \param data TileLink data field
                     * \param mask TileLink mask field
                     */
                    MaskedChannel(const uint8_t code,
                                  const uint8_t param,
                                  const uint8_t size,
                                  const uint64_t source,
                                  const uint64_t address,
                                  std::vector<uint8_t>&& data,
                                  std::vector<uint8_t>&& mask) :
                        AddressChannel<T>(code, param, size, source, address, data),
                        mask_(std::move(mask))
                    {
                    }

                    /**
                     * Constructs a tilelink::MaskedChannel without populating the data or mask fields
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     */
                    MaskedChannel(const uint8_t code,
                                  const uint8_t param,
                                  const uint8_t size,
                                  const uint64_t source,
                                  const uint64_t address) :
                        MaskedChannel(code, param, size, source, address, {}, {})
                    {
                    }

                    /**
                     * Unpacks a tilelink::MaskedChannel object from an STFIFstream
                     * \param reader STFIFstream to use
                     */
                    inline void unpack_impl(STFIFstream& reader) {
                        AddressChannel<T>::unpack_impl(reader);
                        TypeAwareChannel<T>::read_(reader, mask_);
                    }

                    /**
                     * Packs a tilelink::MaskedChannel object to an STFOFstream
                     * \param writer STFOFstream to use
                     */
                    inline void pack_impl(STFOFstream& writer) const {
                        AddressChannel<T>::pack_impl(writer);
                        TypeAwareChannel<T>::write_(writer, mask_);
                    }

                    /**
                     * Formats a tilelink::MaskedChannel object to an std::ostream
                     * \param os ostream to use
                     */
                    inline void format_impl(std::ostream& os) const {
                        AddressChannel<T>::format_impl(os);
                        os << std::endl;
                        format_utils::formatLabel(os, "MASK");
                        os << mask_;
                    }

                    /**
                     * Gets the mask values
                     */
                    inline const auto& getMask() const {
                        return mask_;
                    }
            };

            /**
             * \class ChannelA
             * Represents Channel A in the TileLink protocol
             */
            class ChannelA : public MaskedChannel<ChannelA> {
                public:
                    /**
                     * Constructs a tilelink::ChannelA from an STFIFstream
                     * \param reader STFIFstream to read the tilelink::ChannelA object from
                     */
                    explicit ChannelA(STFIFstream& reader) {
                        unpack_impl(reader);
                    }

                    /**
                     * Constructs a tilelink::ChannelA
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     * \param data TileLink data field
                     * \param mask TileLink mask field
                     */
                    ChannelA(const uint8_t code,
                             const uint8_t param,
                             const uint8_t size,
                             const uint64_t source,
                             const uint64_t address,
                             const std::vector<uint8_t>& data,
                             const std::vector<uint8_t>& mask) :
                        MaskedChannel<ChannelA>(code, param, size, source, address, data, mask)
                    {
                    }

                    /**
                     * Constructs a tilelink::ChannelA
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     * \param data TileLink data field
                     * \param mask TileLink mask field
                     */
                    ChannelA(const uint8_t code,
                             const uint8_t param,
                             const uint8_t size,
                             const uint64_t source,
                             const uint64_t address,
                             std::vector<uint8_t>&& data,
                             std::vector<uint8_t>&& mask) :
                        MaskedChannel<ChannelA>(code, param, size, source, address, data, mask)
                    {
                    }

                    /**
                     * Constructs a tilelink::ChannelA without populating the data or mask fields
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     */
                    ChannelA(const uint8_t code,
                             const uint8_t param,
                             const uint8_t size,
                             const uint64_t source,
                             const uint64_t address) :
                        ChannelA(code, param, size, source, address, {}, {})
                    {
                    }

                    /**
                     * Formats a tilelink::ChannelA object to an std::ostream
                     * \param os ostream to use
                     */
                    inline void format_impl(std::ostream& os) const {
                        MaskedChannel::format_impl(os);
                    }
            };

            /**
             * \class ChannelB
             * Represents Channel B in the TileLink protocol
             */
            class ChannelB : public MaskedChannel<ChannelB> {
                public:
                    /**
                     * Constructs a tilelink::ChannelB from an STFIFstream
                     * \param reader STFIFstream to read the tilelink::ChannelB object from
                     */
                    explicit ChannelB(STFIFstream& reader) {
                        unpack_impl(reader);
                    }

                    /**
                     * Constructs a tilelink::ChannelB
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     * \param data TileLink data field
                     * \param mask TileLink mask field
                     */
                    ChannelB(const uint8_t code,
                             const uint8_t param,
                             const uint8_t size,
                             const uint64_t source,
                             const uint64_t address,
                             const std::vector<uint8_t>& data,
                             const std::vector<uint8_t>& mask) :
                        MaskedChannel<ChannelB>(code, param, size, source, address, data, mask)
                    {
                    }

                    /**
                     * Constructs a tilelink::ChannelB
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     * \param data TileLink data field
                     * \param mask TileLink mask field
                     */
                    ChannelB(const uint8_t code,
                             const uint8_t param,
                             const uint8_t size,
                             const uint64_t source,
                             const uint64_t address,
                             std::vector<uint8_t>&& data,
                             std::vector<uint8_t>&& mask) :
                        MaskedChannel<ChannelB>(code, param, size, source, address, data, mask)
                    {
                    }

                    /**
                     * Constructs a tilelink::ChannelB without populating the data or mask fields
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     */
                    ChannelB(const uint8_t code,
                             const uint8_t param,
                             const uint8_t size,
                             const uint64_t source,
                             const uint64_t address) :
                        ChannelB(code, param, size, source, address, {}, {})
                    {
                    }

                    /**
                     * Formats a tilelink::ChannelB object to an std::ostream
                     * \param os ostream to use
                     */
                    inline void format_impl(std::ostream& os) const {
                        MaskedChannel::format_impl(os);
                    }
            };

            /**
             * \class ChannelC
             * Represents Channel C in the TileLink protocol
             */
            class ChannelC : public AddressChannel<ChannelC> {
                public:
                    /**
                     * Constructs a tilelink::ChannelC from an STFIFstream
                     * \param reader STFIFstream to read the tilelink::ChannelC object from
                     */
                    explicit ChannelC(STFIFstream& reader) {
                        unpack_impl(reader);
                    }

                    /**
                     * Constructs a tilelink::ChannelC
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     * \param data TileLink data field
                     */
                    ChannelC(const uint8_t code,
                             const uint8_t param,
                             const uint8_t size,
                             const uint64_t source,
                             const uint64_t address,
                             const std::vector<uint8_t>& data) :
                        AddressChannel<ChannelC>(code, param, size, source, address, data)
                    {
                    }

                    /**
                     * Constructs a tilelink::ChannelC
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     * \param data TileLink data field
                     */
                    ChannelC(const uint8_t code,
                             const uint8_t param,
                             const uint8_t size,
                             const uint64_t source,
                             const uint64_t address,
                             std::vector<uint8_t>&& data) :
                        AddressChannel<ChannelC>(code, param, size, source, address, data)
                    {
                    }

                    /**
                     * Constructs a tilelink::ChannelC without populating the data field
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param address TileLink address field
                     */
                    ChannelC(const uint8_t code,
                             const uint8_t param,
                             const uint8_t size,
                             const uint64_t source,
                             const uint64_t address) :
                        ChannelC(code, param, size, source, address, {})
                    {
                    }

                    /**
                     * Formats a tilelink::ChannelC object to an std::ostream
                     * \param os ostream to use
                     */
                    inline void format_impl(std::ostream& os) const {
                        AddressChannel::format_impl(os);
                    }
            };

            /**
             * \class ChannelD
             * Represents Channel D in the TileLink protocol
             */
            class ChannelD : public DataChannel<ChannelD>, public SinkChannel {
                public:
                    /**
                     * Constructs a tilelink::ChannelD from an STFIFstream
                     * \param reader STFIFstream to read the tilelink::ChannelD object from
                     */
                    explicit ChannelD(STFIFstream& reader) {
                        unpack_impl(reader);
                    }

                    /**
                     * Constructs a tilelink::ChannelD
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param sink TileLink sink field
                     * \param data TileLink data field
                     */
                    ChannelD(const uint8_t code,
                             const uint8_t param,
                             const uint8_t size,
                             const uint64_t source,
                             const uint64_t sink,
                             const std::vector<uint8_t>& data) :
                        DataChannel<ChannelD>(code, param, size, source, data),
                        SinkChannel(sink)
                    {
                    }

                    /**
                     * Constructs a tilelink::ChannelD
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param sink TileLink sink field
                     * \param data TileLink data field
                     */
                    ChannelD(const uint8_t code,
                             const uint8_t param,
                             const uint8_t size,
                             const uint64_t source,
                             const uint64_t sink,
                             std::vector<uint8_t>&& data) :
                        DataChannel<ChannelD>(code, param, size, source, data),
                        SinkChannel(sink)
                    {
                    }

                    /**
                     * Constructs a tilelink::ChannelD without populating the data field
                     * \param code TileLink code field
                     * \param param TileLink param field
                     * \param size TileLink size field
                     * \param source TileLink source field
                     * \param sink TileLink sink field
                     */
                    ChannelD(const uint8_t code,
                             const uint8_t param,
                             const uint8_t size,
                             const uint64_t source,
                             const uint64_t sink) :
                        ChannelD(code, param, size, source, sink, {})
                    {
                    }

                    /**
                     * Unpacks a tilelink::ChannelD object from an STFIFstream
                     * \param reader STFIFstream to use
                     */
                    inline void unpack_impl(STFIFstream& reader) {
                        DataChannel::unpack_impl(reader);
                        SinkChannel::unpack_impl(reader);
                    }

                    /**
                     * Packs a tilelink::ChannelD object to an STFOFstream
                     * \param writer STFOFstream to use
                     */
                    inline void pack_impl(STFOFstream& writer) const {
                        DataChannel::pack_impl(writer);
                        SinkChannel::pack_impl(writer);
                    }

                    /**
                     * Formats a tilelink::ChannelD object to an std::ostream
                     * \param os ostream to use
                     */
                    inline void format_impl(std::ostream& os) const {
                        SinkChannel::format_impl(os);
                        DataChannel::format_impl(os);
                    }
            };

            /**
             * \class ChannelE
             * Represents Channel E in the TileLink protocol
             */
            class ChannelE : public TypeAwareChannel<ChannelE>, public SinkChannel {
                public:
                    /**
                     * Constructs a tilelink::ChannelE from an STFIFstream
                     * \param reader STFIFstream to read the tilelink::ChannelE object from
                     */
                    explicit ChannelE(STFIFstream& reader) {
                        unpack_impl(reader);
                    }

                    /**
                     * Constructs a tilelink::ChannelE without populating the data field
                     * \param sink TileLink sink field
                     */
                    explicit ChannelE(const uint64_t sink) :
                        SinkChannel(sink)
                    {
                    }

                    /**
                     * Unpacks a tilelink::ChannelE object from an STFIFstream
                     * \param reader STFIFstream to use
                     */
                    inline void unpack_impl(STFIFstream& reader) {
                        SinkChannel::unpack_impl(reader);
                    }

                    /**
                     * Packs a tilelink::ChannelE object to an STFOFstream
                     * \param writer STFOFstream to use
                     */
                    inline void pack_impl(STFOFstream& writer) const {
                        SinkChannel::pack_impl(writer);
                    }

                    /**
                     * Formats a tilelink::ChannelE object to an std::ostream
                     * \param os ostream to use
                     */
                    inline void format_impl(std::ostream& os) const {
                        SinkChannel::format_impl(os);
                    }
            };
        }

        /**
         * \class TileLink
         * Represents a TileLink protocol transaction
         */
        class TileLink : public TypeAwareProtocolData<TileLink> {
            private:
                tilelink::Channel::UniqueHandle channel_data_;

            public:
                /**
                 * Constructs a TileLink protocol from an STFIFstream
                 * \param reader STFIFstream to read the TileLink object from
                 */
                explicit TileLink(STFIFstream& reader) {
                    unpack_impl(reader);
                }

                /**
                 * Constructs a TileLink protocol with the given channel data
                 * \param channel_data Channel data to use
                 */
                explicit TileLink(tilelink::Channel::UniqueHandle&& channel_data) :
                    channel_data_(std::move(channel_data))
                {
                }

                /**
                 * Constructs a TileLink channel wrapped in a TileLink protocol object, wrapped in a TransactionRecord
                 * \param time_delta Time delta to use
                 * \param args Arguments passed to channel data constructor
                 */
                template<typename ChannelType, typename ... Args>
                inline static TransactionRecord makeTransactionWithDelta(const uint64_t time_delta, Args&&... args) {
                    return TransactionRecord::createNext(
                        time_delta,
                        TileLink::pool_type::construct<TileLink>(
                            ChannelType::pool_type::template construct<ChannelType>(std::forward<Args>(args)...)
                        )
                    );
                }

                /**
                 * Constructs a TileLink channel wrapped in a TileLink protocol object, wrapped in a TransactionRecord
                 * \param args Arguments passed to channel data constructor
                 */
                template<typename ChannelType, typename ... Args>
                inline static TransactionRecord makeTransaction(Args&&... args) {
                    return TransactionRecord::createNext(
                        TileLink::pool_type::construct<TileLink>(
                            ChannelType::pool_type::template construct<ChannelType>(std::forward<Args>(args)...)
                        )
                    );
                }

                /**
                 * TileLink copy constructor
                 * \param rhs TileLink object to copy
                 */
                TileLink(const TileLink& rhs) :
                    channel_data_(rhs.channel_data_->clone())
                {
                }

                /**
                 * TileLink move constructor
                 */
                TileLink(TileLink&&) = default;

                /**
                 * Packs a TileLink object to an STFOFstream
                 * \param writer STFOFstream to use
                 */
                inline void pack_impl(STFOFstream& writer) const {
                    write_(writer, channel_data_);
                }

                /**
                 * Unpacks a TileLink object from an STFIFstream
                 * \param reader STFIFstream to use
                 */
                inline void unpack_impl(STFIFstream& reader) {
                    reader >> channel_data_;
                }

                /**
                 * Formats a TileLink object to an std::ostream
                 * \param os ostream to use
                 */
                inline void format_impl(std::ostream& os) const {
                    os << std::endl;
                    format_utils::formatLabel(os, "CHANNEL");
                    channel_data_->format(os);
                }

                /**
                 * Gets the channel type for this TileLink transaction
                 */
                inline auto getChannelType() const {
                    return channel_data_->getId();
                }

                /**
                 * Casts the channel data to the specified type
                 */
                template<typename T>
                inline const auto& getChannelAs() const {
                    return channel_data_->as<T>();
                }
        };
    }

    DECLARE_FACTORY(ChannelFactory, protocols::tilelink::Channel)
}

#endif
