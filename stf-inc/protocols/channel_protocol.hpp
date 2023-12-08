#ifndef __STF_CHANNEL_PROTOCOL_HPP__
#define __STF_CHANNEL_PROTOCOL_HPP__

#include <boost/preprocessor/cat.hpp>
#include "stf_macro_utils.hpp"
#include "stf_object.hpp"
#include "stf_protocol_fields.hpp"

// Defines field modifier flags
// Flags can't be combined at the moment, but an STF_ENUM_CONFIG-like mechanism could be used if
// needed in the future

// Indicates this field is a template specialization tuple
#define _FIELD_MOD_TEMPLATE_PACK 1
// Indicates this field has a name override
#define _FIELD_MOD_OVERRIDE_NAME 2

// Creates a modified field tuple
#define _FIELD_MOD(mod_type, mod_data) (mod_type, mod_data)

/**
 * \def CHANNEL_IDS
 * Helper macro that generates an STF_ENUM of channel IDs along with the necessary typedefs to use with
 * FIELD_CHANNEL
 */
#define CHANNEL_IDS(type, ...)                                              \
    STF_ENUM(STF_ENUM_CONFIG(AUTO_PRINT), ChannelID, type, __VA_ARGS__);    \
    using ChannelProtocol = protocols::proto::ChannelProtocol<ChannelID>;   \
    using Channel = ChannelProtocol::Channel

// Gets the global name from a _FIELD_MOD_TEMPLATE_PACK tuple (just unpacks the tuple into a fully
// specialized template name)
#define __GET_GLOBAL_FIELD_NAME_1(field) STF_UNPACK_TEMPLATE(field)

// Gets the global name from a _FIELD_MOD_OVERRIDE_NAME tuple
#define __GET_GLOBAL_FIELD_NAME_2(field) BOOST_PP_TUPLE_ELEM(0, field)

// Dispatches to the appropriate global name handler based on the modifier flag
#define __GET_GLOBAL_FIELD_NAME(field)  \
    BOOST_PP_CAT(                       \
        __GET_GLOBAL_FIELD_NAME_,       \
        BOOST_PP_TUPLE_ELEM(0, field)   \
    )(BOOST_PP_TUPLE_ELEM(1, field))

// Gets the "global" field name - the actual (fully specialized if necessary) name of the C++ class
// If the field isn't modified, it just returns the field name
#define _GET_GLOBAL_FIELD_NAME(field)   \
    BOOST_PP_IIF(                       \
        STF_IS_TUPLE(field),            \
        __GET_GLOBAL_FIELD_NAME(field), \
        field                           \
    )

// Gets the local name from a _FIELD_MOD_TEMPLATE_PACK tuple (just gets the class name from the tuple)
#define __GET_LOCAL_FIELD_NAME_1(field) STF_UNPACK_TEMPLATE_CLASS(field)

// Gets the local name from a _FIELD_MOD_OVERRIDE_NAME tuple
#define __GET_LOCAL_FIELD_NAME_2(field) BOOST_PP_TUPLE_ELEM(1, field)

// Dispatches to the appropriate local name handler based on the modifier flag
#define __GET_LOCAL_FIELD_NAME(field)   \
    BOOST_PP_CAT(                       \
        __GET_LOCAL_FIELD_NAME_,        \
        BOOST_PP_TUPLE_ELEM(0, field)   \
    )(BOOST_PP_TUPLE_ELEM(1, field))

// Gets the "local" field name - i.e., the name used to generate the getter method
// If the field isn't modified, it just returns the field name
#define _GET_LOCAL_FIELD_NAME(field)    \
    BOOST_PP_IIF(                       \
        STF_IS_TUPLE(field),            \
        __GET_LOCAL_FIELD_NAME(field),  \
        field                           \
    )

// Qualifies a field class with the given namespace
// Used in the BOOST_PP_SEQ_TRANSFORM invocation in _FIELD_CHANNEL
#define _NS_FIELD(r, ns, field) \
    ns::_GET_GLOBAL_FIELD_NAME(field)

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
// Fields with overridden names (using the OVERRIDE_FIELD_NAME macro) will use the overridden name instead
#define _FIELD_GETTER(r, field_ns, field)                           \
    inline typename _NS_FIELD(_, field_ns, field)::ReturnType       \
    BOOST_PP_CAT(get, _GET_LOCAL_FIELD_NAME(field))() const {       \
        return get<_NS_FIELD(_, field_ns, field)>();                \
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
 * \def FIELD_SPECIALIZATION
 * If a field is a templatized class, it needs to be wrapped along with its template arguments using
 * this macro when adding it to a channel. The __VA_ARGS__ are the template arguments that will be used
 * to specialize the class.
 */
#define FIELD_SPECIALIZATION(field_name, ...) \
    _FIELD_MOD(_FIELD_MOD_TEMPLATE_PACK, STF_PACK_TEMPLATE(field_name, __VA_ARGS__))

/**
 * \def OVERRIDE_FIELD_NAME
 * Wrapping a field with this macro changes the getter method to get{local_name}. Useful for cases where
 * a field could have multiple discrete sizes and a vector field would be inefficient.
 * Example:
 * FIELD(MyField, uint8_t)
 * FIELD(MyOtherField, uint16_t)
 * ...
 * // getter method will be MyChannel::getMyField
 * FIELD_CHANNEL(MyChannel, MY_CHANNEL_ID, MyField)
 * // getter method will be MyOtherChannel::getMyOtherField
 * FIELD_CHANNEL(MyOtherChannel, MY_OTHER_CHANNEL_ID, MyOtherField)
 * // getter method will be MyOverrideChannel::getMyField
 * FIELD_CHANNEL(MyOverrideChannel, MY_OVERRIDE_CHANNEL_ID, OVERRIDE_FIELD_NAME(MyOtherField, MyField))
 */
#define OVERRIDE_FIELD_NAME(global_name, local_name) \
    _FIELD_MOD(_FIELD_MOD_OVERRIDE_NAME, (global_name, local_name))

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
                    inline typename FieldType::ReturnType get() const {
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
