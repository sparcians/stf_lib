#ifndef __STF_TILELINK_HPP__
#define __STF_TILELINK_HPP__

#include "stf_protocol_data.hpp"
#include "stf_ifstream.hpp"
#include "stf_serializable_container.hpp"
#include "format_utils.hpp"

namespace stf {
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
                    /**
                     * Constructs a tilelink::DataChannel from an STFIFstream
                     * \param reader STFIFstream to read the tilelink::DataChannel object from
                     */
                    explicit DataChannel(STFIFstream& reader) {
                        unpack_impl(reader);
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
                        os << data_ << std::endl;
                    }

                    /**
                     * Gets the code field
                     */
                    auto getCode() const {
                        return code_;
                    }

                    /**
                     * Gets the param field
                     */
                    auto getParam() const {
                        return param_;
                    }

                    /**
                     * Gets the size field
                     */
                    auto getSize() const {
                        return size_;
                    }

                    /**
                     * Gets the source id
                     */
                    auto getSource() const {
                        return source_;
                    }

                    /**
                     * Gets the data values
                     */
                    const auto& getData() const {
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
                    /**
                     * Constructs a tilelink::AddressChannel from an STFIFstream
                     * \param reader STFIFstream to read the tilelink::AddressChannel object from
                     */
                    explicit AddressChannel(STFIFstream& reader) :
                        DataChannel<T>(reader)
                    {
                        unpack_impl(reader);
                    }

                    /**
                     * Unpacks a tilelink::AddressChannel object from an STFIFstream
                     * \param reader STFIFstream to use
                     */
                    inline void unpack_impl(STFIFstream& reader) {
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
                        format_utils::formatLabel(os, "ADDRESS");
                        format_utils::formatVA(os, address_);
                        DataChannel<T>::format_impl(os);
                        os << std::endl;
                    }

                    /**
                     * Gets the address value
                     */
                    auto getAddress() const {
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
                    /**
                     * Constructs a tilelink::SinkChannel from an STFIFstream
                     * \param reader STFIFstream to read the tilelink::SinkChannel object from
                     */
                    explicit SinkChannel(STFIFstream& reader) {
                        unpack_impl(reader);
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
                        format_utils::formatLabel(os, "SINK");
                        format_utils::formatHex(os, sink_);
                        os << std::endl;
                    }

                    /**
                     * Gets the sink ID
                     */
                    auto getSink() const {
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
                    /**
                     * Constructs a tilelink::MaskedChannel from an STFIFstream
                     * \param reader STFIFstream to read the tilelink::MaskedChannel object from
                     */
                    explicit MaskedChannel(STFIFstream& reader) :
                        AddressChannel<T>(reader)
                    {
                        unpack_impl(reader);
                    }

                    /**
                     * Unpacks a tilelink::MaskedChannel object from an STFIFstream
                     * \param reader STFIFstream to use
                     */
                    inline void unpack_impl(STFIFstream& reader) {
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
                        format_utils::formatLabel(os, "MASK");
                        os << mask_ << std::endl;
                    }

                    /**
                     * Gets the mask values
                     */
                    const auto& getMask() const {
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
                    explicit ChannelA(STFIFstream& reader) :
                        MaskedChannel(reader)
                    {
                    }

                    /**
                     * Formats a tilelink::ChannelA object to an std::ostream
                     * \param os ostream to use
                     */
                    inline void format_impl(std::ostream& os) const {
                        format_utils::formatLabel(os, "CHANNEL A");
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
                    explicit ChannelB(STFIFstream& reader) :
                        MaskedChannel(reader)
                    {
                    }

                    /**
                     * Formats a tilelink::ChannelB object to an std::ostream
                     * \param os ostream to use
                     */
                    inline void format_impl(std::ostream& os) const {
                        format_utils::formatLabel(os, "CHANNEL B");
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
                    explicit ChannelC(STFIFstream& reader) :
                        AddressChannel(reader)
                    {
                    }

                    /**
                     * Formats a tilelink::ChannelC object to an std::ostream
                     * \param os ostream to use
                     */
                    inline void format_impl(std::ostream& os) const {
                        format_utils::formatLabel(os, "CHANNEL C");
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
                    explicit ChannelD(STFIFstream& reader) :
                        DataChannel(reader),
                        SinkChannel(reader)
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
                        format_utils::formatLabel(os, "CHANNEL D");
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
                    explicit ChannelE(STFIFstream& reader) :
                        SinkChannel(reader)
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
                        format_utils::formatLabel(os, "CHANNEL E");
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
                void format_impl(std::ostream& os) const {
                    format_utils::formatLabel(os, "TILELINK");
                    os << std::endl;
                    format_utils::formatLabel(os, "CHANNEL");
                    channel_data_->format(os);
                }

                /**
                 * Gets the channel type for this TileLink transaction
                 */
                auto getChannelType() const {
                    return channel_data_->getId();
                }

                /**
                 * Casts the channel data to the specified type
                 */
                template<typename T>
                const auto& getChannelAs() const {
                    return channel_data_->as<T>();
                }
        };
    }

    DECLARE_FACTORY(ChannelFactory, protocols::tilelink::Channel)
}

#endif
