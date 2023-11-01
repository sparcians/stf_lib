#ifndef __STF_CHANNEL_PROTOCOL_HPP__
#define __STF_CHANNEL_PROTOCOL_HPP__

#include <boost/preprocessor/cat.hpp>
#include "stf_macro_utils.hpp"
#include "stf_object.hpp"
#include "stf_protocol_fields.hpp"

/**
 * \def CHANNEL_IDS
 * Helper macro that generates an STF_ENUM of channel IDs along with the necessary typedefs to use with
 * FIELD_CHANNEL
 */
#define CHANNEL_IDS(type, ...)                                              \
    STF_ENUM(STF_ENUM_CONFIG(AUTO_PRINT), ChannelID, type, __VA_ARGS__);    \
    using ChannelProtocol = protocols::proto::ChannelProtocol<ChannelID>;   \
    using Channel = ChannelProtocol::Channel

// Qualifies a field channel class with the given namespace
// Used in the BOOST_PP_SEQ_TRANSFORM invocation in _FIELD_CHANNEL
#define _NS_FIELD(r, ns, channel) ns::STF_UNPACK_TEMPLATE(channel)

// Generates a fully specialized FieldChannel template
// __VA_ARGS__ are the fields included in the channel
#define _FIELD_CHANNEL(class_name, enum_id, field_ns, ...)  \
    ChannelProtocol::FieldChannel<                          \
        class_name,                                         \
        ChannelID::enum_id,                                 \
        BOOST_PP_SEQ_ENUM(                                  \
            BOOST_PP_SEQ_TRANSFORM(                         \
                _NS_FIELD,                                  \
                field_ns,                                   \
                BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)       \
            )                                               \
        )                                                   \
    >

// Generates a getter method for a field in a FieldChannel subclass
// Used by the BOOST_PP_SEQ_FOR_EACH invocation in FIELD_CHANNEL
// Example: for a field named MyField it will generate a method named getMyField
#define _FIELD_GETTER(r, field_ns, channel) \
    inline typename field_ns::STF_UNPACK_TEMPLATE(channel)::ReferenceType   \
    BOOST_PP_CAT(get, STF_UNPACK_TEMPLATE_CLASS(channel))() const {         \
        return get<field_ns::STF_UNPACK_TEMPLATE(channel)>();               \
    }

/**
 * \def FIELD_CHANNEL
 * Generates a FieldChannel subclass identified by enum_id with the specified fields.
 * __VA_ARGS__ are the fields included in the channel. field_ns is the namespace containing the field
 * objects.
 */
#define FIELD_CHANNEL(class_name, enum_id, field_ns, ...)                                       \
    class class_name : public _FIELD_CHANNEL(class_name, enum_id, field_ns, __VA_ARGS__)        \
    {                                                                                           \
        private:                                                                                \
            using FieldChannel = _FIELD_CHANNEL(class_name, enum_id, field_ns, __VA_ARGS__);    \
        public:                                                                                 \
            explicit class_name(STFIFstream& reader) :                                          \
                FieldChannel(reader) {}                                                         \
            template<typename ... FieldArgs>                                                    \
            explicit class_name(FieldArgs&&... args) :                                          \
                FieldChannel(std::forward<FieldArgs>(args)...) {}                               \
            BOOST_PP_SEQ_FOR_EACH(                                                              \
                _FIELD_GETTER,                                                                  \
                field_ns,                                                                       \
                BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)                                           \
            )                                                                                   \
    }

/**
 * \def REGISTER_CHANNEL
 * Registers a channel class with its corresponding STFFactory
 */
#define REGISTER_CHANNEL(ns, cls)                                           \
    NAMESPACE_WRAP(                                                         \
        protocols_##ns,                                                     \
        protocols::ns,                                                      \
        REGISTER_WITH_FACTORY(protocols::ns::Channel, protocols::ns::cls)   \
    )

namespace stf {
    namespace protocols::proto {
        /**
         * \struct ChannelProtocol
         * Associates a channel ID enum (ChannelType) with everything needed to create FieldChannels
         * mapped to those IDs.
         */
        template<typename ChannelType>
        struct ChannelProtocol {
            class Channel : public STFObject<Channel, ChannelType> {
                public:
                    /**
                     * Constructs a Channel object
                     * \param channel_type Channel ID
                     */
                    explicit Channel(const ChannelType channel_type) :
                        STFObject<Channel, ChannelType>(channel_type)
                    {
                    }
            };

            /**
             * \typedef TypeAwareChannel
             *
             * Channel class that knows its own type
             */
            template<typename T, ChannelType channel_type>
            using TypeAwareChannel = TypeAwareSTFObject<T, Channel, channel_type>;

            /**
             * \class FieldChannel
             * Channel that can contain a variable number of fields (specified in Fields parameter)
             * Automatically handles packing/unpacking all of the fields from an STF and provides a format
             * function.
             */
            template<typename T, ChannelType channel_type, typename ... Fields>
            class FieldChannel : public TypeAwareChannel<T, channel_type>
            {
                private:
                    using Tuple = std::tuple<Fields...>;
                    Tuple fields_;

                    using ParentType = TypeAwareChannel<T, channel_type>;
                    using ParentType::read_;
                    using ParentType::write_;

                    template<size_t I = 0>
                    inline void format_(std::ostream& os) const {
                        if constexpr(I < std::tuple_size_v<Tuple>) {
                            os << std::endl;
                            std::get<I>(fields_).format(os);
                            format_<I + 1>(os);
                        }
                    }

                public:
                    explicit FieldChannel(STFIFstream& reader) :
                        fields_(Fields(reader)...)
                    {
                    }

                    template<typename ... FieldArgs>
                    explicit FieldChannel(const FieldArgs&... args) :
                        fields_(args...)
                    {
                    }

                    template<typename ... FieldArgs>
                    explicit FieldChannel(FieldArgs&&... args) :
                        fields_(std::forward<FieldArgs>(args)...)
                    {
                    }

                    /**
                     * Packs the entire channel into an STFOFstream
                     */
                    inline void pack_impl(STFOFstream& writer) const {
                        write_(writer, fields_);
                    }

                    /**
                     * Unpacks the entire channel from an STFIFstream
                     */
                    inline void unpack_impl(STFIFstream& reader) {
                        read_(reader, fields_);
                    }

                    /**
                     * Formats the channel to an std::ostream
                     */
                    inline void format_impl(std::ostream& os) const {
                        format_(os);
                    }

                    /**
                     * Gets the value of the specified field
                     */
                    template<typename FieldType>
                    inline typename FieldType::ReferenceType get() const {
                        return std::get<FieldType>(fields_).value();
                    }
            };
        };

        /**
         * \class TypeAwareChannelProtocol
         * A channel-based protocol that knows its own type
         */
        template<ProtocolId protocol_id, typename BaseChannelType>
        class TypeAwareChannelProtocol : public TypeAwareProtocolData<TypeAwareChannelProtocol<protocol_id, BaseChannelType>, protocol_id> {
            private:
                using ProtocolType = TypeAwareChannelProtocol<protocol_id, BaseChannelType>;
                using TypeAwareProtocolData<ProtocolType, protocol_id>::write_;
                typename BaseChannelType::UniqueHandle channel_data_;

            public:
                /**
                 * Constructs a channel protocol from an STFIFstream
                 * \param reader STFIFstream to read the channel object from
                 */
                explicit TypeAwareChannelProtocol(STFIFstream& reader) {
                    unpack_impl(reader);
                }

                /**
                 * Constructs a channel protocol with the given channel data
                 * \param channel_data Channel data to use
                 */
                explicit TypeAwareChannelProtocol(typename BaseChannelType::UniqueHandle&& channel_data) :
                    channel_data_(std::move(channel_data))
                {
                }

                /**
                 * Constructs a channel wrapped in a channel protocol object, wrapped in a TransactionRecord
                 * \param id_manager IdManager that tracks the ID values for the current transaction stream
                 * \param clock_id ID of the clock domain corresponding to this channel
                 * \param time_delta Time delta to use
                 * \param args Arguments passed to channel data constructor
                 */
                template<typename ChannelType, typename ... Args>
                static inline TransactionRecord makeTransactionWithDelta(RecordIdManager& id_manager,
                                                                         const ClockId clock_id,
                                                                         const uint64_t time_delta,
                                                                         Args&&... args) {
                    return TransactionRecord(
                        id_manager,
                        clock_id,
                        time_delta,
                        ProtocolType::pool_type::template construct<ProtocolType>(
                            ChannelType::pool_type::template construct<ChannelType>(std::forward<Args>(args)...)
                        )
                    );
                }

                /**
                 * Constructs a channel wrapped in a channel protocol object, wrapped in a TransactionRecord
                 * \param id_manager IdManager that tracks the ID values for the current transaction stream
                 * \param time_delta Time delta to use
                 * \param args Arguments passed to channel data constructor
                 */
                template<typename ChannelType, typename ... Args>
                static inline TransactionRecord makeTransactionWithDelta(RecordIdManager& id_manager,
                                                                         const uint64_t time_delta,
                                                                         Args&&... args) {
                    return TransactionRecord(
                        id_manager,
                        time_delta,
                        ProtocolType::pool_type::template construct<ProtocolType>(
                            ChannelType::pool_type::template construct<ChannelType>(std::forward<Args>(args)...)
                        )
                    );
                }

                /**
                 * Constructs a channel wrapped in a channel protocol object, wrapped in a TransactionRecord
                 * \param id_manager IdManager that tracks the ID values for the current transaction stream
                 * \param args Arguments passed to channel data constructor
                 */
                template<typename ChannelType, typename ... Args>
                static inline TransactionRecord makeTransaction(RecordIdManager& id_manager,
                                                                Args&&... args) {
                    return TransactionRecord(
                        id_manager,
                        ProtocolType::pool_type::template construct<ProtocolType>(
                            ChannelType::pool_type::template construct<ChannelType>(std::forward<Args>(args)...)
                        )
                    );
                }

                /**
                 * Copy constructor
                 * \param rhs channel object to copy
                 */
                TypeAwareChannelProtocol(const TypeAwareChannelProtocol& rhs) :
                    channel_data_(rhs.channel_data_->clone())
                {
                }

                /**
                 * Move constructor
                 */
                TypeAwareChannelProtocol(TypeAwareChannelProtocol&&) = default;

                /**
                 * Packs a channel object to an STFOFstream
                 * \param writer STFOFstream to use
                 */
                inline void pack_impl(STFOFstream& writer) const {
                    write_(writer, channel_data_);
                }

                /**
                 * Unpacks a channel object from an STFIFstream
                 * \param reader STFIFstream to use
                 */
                inline void unpack_impl(STFIFstream& reader) {
                    reader >> channel_data_;
                }

                /**
                 * Formats a channel object to an std::ostream
                 * \param os ostream to use
                 */
                inline void format_impl(std::ostream& os) const {
                    format_utils::formatLabel(os, "CHANNEL");
                    channel_data_->format(os);
                }

                /**
                 * Gets the channel type for this transaction
                 */
                inline auto getChannelType() const {
                    return channel_data_->getId();
                }

                /**
                 * Casts the channel data to the specified type
                 */
                template<typename T>
                inline const auto& getChannelAs() const {
                    return channel_data_->template as<T>();
                }
        };
    }
}

#endif
