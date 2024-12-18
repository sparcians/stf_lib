#ifndef __STF_PROTOCOL_FIELDS_HPP__
#define __STF_PROTOCOL_FIELDS_HPP__

#include <algorithm>
#include <array>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/push_back.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

#include "format_utils.hpp"
#include "stf_macro_utils.hpp"
#include "stf_ofstream.hpp"
#include "stf_serializable_container.hpp"
#include "type_utils.hpp"

// Conda adds -fvisibility-inlines-hidden to the default compiler flags, which gives a compiler error unless
// we add this visibility tag to field class definitions
#if __GNUC__ >= 4
    #define DEFAULT_VIS __attribute__((visibility("default")))
#else
    #define DEFAULT_VIS
#endif

// Some GCC versions complain about Formatter lambdas that are defined in this header. As far as I can tell,
// these warnings are spurious. This disables the warnings if we're running GCC, but only around fields with
// custom formatters
#if defined(__GNUC__) && !defined(__clang__)
    #define FORMATTER_PRAGMA_START \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wsubobject-linkage\"")
    #define FORMATTER_PRAGMA_END _Pragma("GCC diagnostic pop")
#else
    #define FORMATTER_PRAGMA_START
    #define FORMATTER_PRAGMA_END
#endif

// Defines r-value and l-value field class constructors for the given argument type
// parent_class_tuple and arg_type should either be bare type names or tuples packed with STF_PACK_TEMPLATE
#define __FIELD_CONSTRUCTOR(parent_class_tuple, field_name, arg_type)                                   \
    explicit field_name(const STF_UNPACK_TEMPLATE(arg_type)& arg) :                                     \
        STF_UNPACK_TEMPLATE(parent_class_tuple)(arg) {}                                                 \
    explicit field_name(STF_UNPACK_TEMPLATE(arg_type)&& arg) :                                          \
        STF_UNPACK_TEMPLATE(parent_class_tuple)(std::forward<STF_UNPACK_TEMPLATE(arg_type)>(arg)) {}

// Forwards from BOOST_PP_SEQ_FOR_EACH to __FIELD_CONSTRUCTOR
// data is a tuple of the form (parent_class_tuple, arg_type)
#define _FIELD_CONSTRUCTOR(r, data, elem)   \
    __FIELD_CONSTRUCTOR(                    \
        BOOST_PP_TUPLE_ELEM(0, data),       \
        BOOST_PP_TUPLE_ELEM(1, data),       \
        elem                                \
    )

// Declares a field class with the given parent type, name, and constructor argument types
#define __FIELD(parent_class_tuple, field_name, ...)                    \
    class DEFAULT_VIS field_name : public STF_UNPACK_TEMPLATE(parent_class_tuple) { \
        public:                                                         \
            field_name() = default;                                     \
            explicit field_name(STFIFstream& reader) :                  \
                STF_UNPACK_TEMPLATE(parent_class_tuple)(reader) {}      \
            BOOST_PP_SEQ_FOR_EACH(                                      \
                _FIELD_CONSTRUCTOR,                                     \
                (parent_class_tuple, field_name),                       \
                BOOST_PP_SEQ_PUSH_BACK(                                 \
                    BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__),              \
                    type_utils::none_t                                  \
                )                                                       \
            )                                                           \
            static inline const char* getName() {                       \
                return BOOST_PP_STRINGIZE(field_name);                  \
            }                                                           \
    }

// Transforms a field class name to a formatter function name
#define _FIELD_FORMATTER_NAME(field_name) BOOST_PP_CAT(field_name, _formatter)

// Declares a field class with a custom formatter function
// The __VA_ARGS__ correspond to the field class template parameters.
// The first template parameter *must* be the underlying type of the field
#define _FIELD_FORMAT_WRAPPER(fmt, field_macro, field_name, ...)            \
    FORMATTER_PRAGMA_START \
    inline static constexpr auto _FIELD_FORMATTER_NAME(field_name) = fmt;       \
    field_macro(field_name, __VA_ARGS__, _FIELD_FORMATTER_NAME(field_name)); \
    FORMATTER_PRAGMA_END

// Declares a subclass of ProtocolField
// The __VA_ARGS__ are the template parameters supplied to ProtocolField
#define _PROTOCOL_FIELD(field_name, ...)        \
    __FIELD(                                    \
        STF_PACK_TEMPLATE(                      \
            ProtocolField,                      \
            field_name,                         \
            __VA_ARGS__                         \
        ),                                      \
        field_name,                             \
        BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__)  \
    )

// Declares a subclass of ProtocolField with a custom format function
#define _FIELD_2(field_name, type, fmt) \
    _FIELD_FORMAT_WRAPPER(fmt, _PROTOCOL_FIELD, field_name, type)

// Declares a subclass of ProtocolField with the default format function
#define _FIELD_1(field_name, type) _PROTOCOL_FIELD(field_name, type)

// Declares a subclass of ProtocolArrayField
// The __VA_ARGS__ are the template parameters supplied to ProtocolArrayField
#define _ARRAY_FIELD(field_name, ...)               \
    template<size_t Size>                           \
    __FIELD(                                        \
        STF_PACK_TEMPLATE(                          \
            ProtocolArrayField,                     \
            Size,                                   \
            field_name<Size>,                       \
            __VA_ARGS__                             \
        ),                                          \
        field_name,                                 \
        STF_PACK_TEMPLATE(                          \
            std::array,                             \
            BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__), \
            Size                                    \
        )                                           \
    )

// Declares a subclass of ProtocolArrayField with a custom format function
#define _ARRAY_FIELD_2(field_name, type, fmt) \
    _FIELD_FORMAT_WRAPPER(fmt, _ARRAY_FIELD, field_name, type)

// Declares a subclass of ProtocolArrayField with the default format function
#define _ARRAY_FIELD_1(field_name, type) _ARRAY_FIELD(field_name, type)

// Declares a subclass of ProtocolVectorField
// The __VA_ARGS__ are the template parameters supplied to ProtocolVectorField
#define _PROTOCOL_VECTOR_FIELD(field_name, type, ...)   \
    __FIELD(                                            \
        STF_PACK_TEMPLATE(                              \
            ProtocolVectorField,                        \
            field_name,                                 \
            type,                                       \
            __VA_ARGS__                                 \
        ),                                              \
        field_name,                                     \
        STF_PACK_TEMPLATE(                              \
            SerializableVector,                         \
            type,                                       \
            BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__)      \
        ),                                              \
        STF_PACK_TEMPLATE(std::vector, type)            \
    )

// Declares a subclass of ProtocolVectorField with a custom format function
#define _VECTOR_FIELD_2(field_name, type, size_type, fmt)   \
    _FIELD_FORMAT_WRAPPER(                                  \
        fmt,                                                \
        _PROTOCOL_VECTOR_FIELD,                             \
        field_name,                                         \
        type,                                               \
        size_type                                           \
    )

// Declares a subclass of ProtocolVectorField with the default format function
#define _VECTOR_FIELD_1(field_name, type, size_type)    \
    _PROTOCOL_VECTOR_FIELD(                             \
        field_name,                                     \
        type,                                           \
        size_type                                       \
    )

// Declares a subclass of ProtocolPackedBitVectorField
// The __VA_ARGS__ are the template parameters supplied to ProtocolPackedBitVectorField
#define _PACKED_BIT_VECTOR_FIELD(field_name, type, ...) \
    __FIELD(                                            \
        STF_PACK_TEMPLATE(                              \
            ProtocolPackedBitVectorField,               \
            field_name,                                 \
            type,                                       \
            __VA_ARGS__                                 \
        ),                                              \
        field_name,                                     \
        STF_PACK_TEMPLATE(                              \
            SerializablePackedBitVector,                \
            type,                                       \
            BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__)      \
        ),                                              \
        STF_PACK_TEMPLATE(std::vector, type)            \
    )

// Declares a subclass of ProtocolPackedBitVectorField with a custom format function
#define _PACKED_BIT_VECTOR_FIELD_2(field_name, type, size_type, fmt)    \
    _FIELD_FORMAT_WRAPPER(                                              \
        fmt,                                                            \
        _PACKED_BIT_VECTOR_FIELD,                                       \
        field_name,                                                     \
        type,                                                           \
        size_type                                                       \
    )

// Declares a subclass of ProtocolPackedBitVectorField with the default format function
#define _PACKED_BIT_VECTOR_FIELD_1(field_name, type, size_type) \
    _PACKED_BIT_VECTOR_FIELD(                                   \
        field_name,                                             \
        type,                                                   \
        size_type                                               \
    )

/**
 * \def FIELD
 * Defines a field with the given name, underlying type, and optional format function
 * Allowed forms:
 *   FIELD(field_name, type)
 *     Creates a field with the given name and type.
 *     Uses the defaultProtocolFieldFormatter formatter function.
 *   FIELD(field_name, type, fmt)
 *     Creates a field with the given name, type, and formatter function.
 */
#define FIELD(field_name, ...) \
    BOOST_PP_OVERLOAD(_FIELD_, __VA_ARGS__)(field_name, __VA_ARGS__)

/**
 * \def ARRAY_FIELD
 * Defines an array field with the given name, underlying type, and optional format function
 * NOTE: Does not define a size for the array. This must be specified when adding the field to a protocol
 * with the ADD_ARRAY_FIELD macro.
 * Allowed forms:
 *   ARRAY_FIELD(field_name, type)
 *     Creates an array field with the given name and type.
 *     Uses the defaultProtocolArrayFieldFormatter formatter function.
 *   ARRAY_FIELD(field_name, type, fmt)
 *     Creates a field with the given name, type, and formatter function.
 */
#define ARRAY_FIELD(field_name, ...) \
    BOOST_PP_OVERLOAD(_ARRAY_FIELD_, __VA_ARGS__)(field_name, __VA_ARGS__)

/**
 * \def VECTOR_FIELD
 * Defines a vector field with the given name, underlying type, size type, and optional format function
 * Allowed forms:
 *   VECTOR_FIELD(field_name, type, size_type)
 *     Creates a field stored in a SerializableVector with the given name, type, and size type.
 *     Uses the defaultProtocolVectorFieldFormatter formatter function
 *   VECTOR_FIELD(field_name, type, size_type, fmt)
 *     Creates a field stored in a SerializableVector with the given name, type, size type, and formatter
 *     function.
 */
#define VECTOR_FIELD(field_name, type, ...) \
    BOOST_PP_OVERLOAD(_VECTOR_FIELD_, __VA_ARGS__)(field_name, type, __VA_ARGS__)

/**
 * \def PACKED_BIT_VECTOR_FIELD
 * Defines a packed bit-vector field with the given name, underlying type, size type, and optional format
 * function
 * Allowed forms:
 *   PACKED_BIT_VECTOR_FIELD(field_name, type, size_type)
 *     Creates a field stored in a SerializablePackedBitVector with the given name, type, and size type.
 *     Uses the defaultProtocolVectorFieldFormatter formatter function
 *   PACKED_BIT_VECTOR_FIELD(field_name, type, size_type, fmt)
 *     Creates a field stored in a SerializablePackedBitVector with the given name, type, size type, and
 *     formatter function.
 */
#define PACKED_BIT_VECTOR_FIELD(field_name, type, ...) \
    BOOST_PP_OVERLOAD(_PACKED_BIT_VECTOR_FIELD_, __VA_ARGS__)(field_name, type, __VA_ARGS__)

namespace stf {
    class STFIFstream;

    /**
     * Default formatter function used for ProtocolField objects
     */
    template<typename Value>
    inline std::enable_if_t<std::negation_v<type_utils::is_array_or_serializable_vector_like<Value>>>
    defaultProtocolFieldFormatter(std::ostream& os, const char* name, const Value& data) {
        format_utils::formatLabel(os, name);
        format_utils::formatHex(os, data);
    }

    /**
     * Default formatter function used for ProtocolArrayField, ProtocolVectorField, and ProtocolPackedBitVectorField objects
     */
    template<typename Value>
    inline std::enable_if_t<type_utils::is_array_or_serializable_vector_like<Value>::value>
    defaultProtocolFieldFormatter(std::ostream& os, const char* name, const Value& data) {
        format_utils::formatLabel(os, name);
        os << '[';
        if(!data.empty()) {
            auto it = data.begin();
            format_utils::formatHex(os, *it);
            if (data.size() > 1) {
                for(++it; it != data.end(); ++it) {
                    os << ", ";
                    format_utils::formatHex(os, *it);
                }
            }
        }
        os << ']';
    }

    /**
     * \typedef ProtocolFieldFormatter
     * Expected type of a ProtocolField formatter function
     */
    template<typename Value>
    using ProtocolFieldFormatter = decltype(&defaultProtocolFieldFormatter<Value>);

    /**
     * \class ProtocolField
     * Template class that all protocol fields should inherit from. Stores protocol data in a scalar field
     * of the specified DataType. Packing/unpacking is handled automatically, and formatting is handled by
     * the Formatter function.
     */
    template<typename FieldType,
             typename DataType,
             ProtocolFieldFormatter<DataType> Formatter = defaultProtocolFieldFormatter<DataType>>
    class ProtocolField {
        public:
            /**
             * \typedef ReferenceType
             * Reference type
             */
            using ReferenceType = type_utils::optimal_const_ref_t<DataType>;

            /**
             * \typedef ReturnType
             * Return type of getValue() method
             */
            using ReturnType = type_utils::optimal_return_ref_t<DataType>;

        protected:
            friend class STFIFstream;

            DataType data_ {}; /**< The data stored in this field */

            /**
             * Gets the field name
             */
            inline const char* getName_() const {
                return static_cast<const FieldType*>(this)->getName();
            }

            /**
             * Gets the data
             */
            DataType& getData_() {
                return data_;
            }

        public:
            ProtocolField() = default;

            /**
             * Constructs a ProtocolField from an STFIFstream
             */
            explicit ProtocolField(STFIFstream& reader) {
                reader >> data_;
            }

            /**
             * Constructs a ProtocolField from a data value
             */
            explicit ProtocolField(const DataType& data) :
                data_(data)
            {
            }

            /**
             * Move-constructs a ProtocolField from a data value
             */
            template<typename T = DataType>
            explicit ProtocolField(DataType&& data) :
                data_(std::move(data))
            {
            }

            /**
             * Constructs an empty ProtocolField
             */
            explicit ProtocolField(const type_utils::none_t&) :
                ProtocolField()
            {
            }

            /**
             * Move-constructs an empty ProtocolField
             */
            explicit ProtocolField(type_utils::none_t&&) :
                ProtocolField()
            {
            }

            /**
             * Writes the field to an STFOFstream
             */
            friend inline STFOFstream& operator<<(STFOFstream& writer, const ProtocolField& data) {
                writer << data.data_;
                return writer;
            }

            /**
             * Formats the field to an std::ostream
             */
            inline void format(std::ostream& os) const {
                Formatter(os, getName_(), value());
            }

            /**
             * Gets the actual value stored in the field
             */
            inline ReturnType value() const {
                return data_;
            }
    };

    /**
     * \typedef ProtocolArrayField
     * Partial specialization of ProtocolField that stores data in an std::array
     */
    template<size_t Size,
             typename FieldType,
             typename DataType,
             ProtocolFieldFormatter<std::array<DataType, Size>> Formatter = defaultProtocolFieldFormatter<std::array<DataType, Size>>>
    using ProtocolArrayField = ProtocolField<FieldType, std::array<DataType, Size>, Formatter>;

    /**
     * \class ProtocolVectorField
     * Subclass of ProtocolField that stores data in a serializable vector-like object, defaulting to
     * SerializableVector
     */
    template<typename FieldType,
             typename DataType,
             typename SizeType,
             typename VectorType = SerializableVector<DataType, SizeType>,
             ProtocolFieldFormatter<VectorType> Formatter = defaultProtocolFieldFormatter<VectorType>>
    class ProtocolVectorField : public ProtocolField<FieldType,
                                                     VectorType,
                                                     Formatter>
    {
        private:
            using ParentType = ProtocolField<FieldType,
                                             VectorType,
                                             Formatter>;
        public:
            using typename ParentType::ReferenceType;

            ProtocolVectorField() = default;

            /**
             * Constructs a ProtocolVectorField from an STFIFstream
             */
            explicit ProtocolVectorField(STFIFstream& reader) :
                ParentType(reader)
            {
            }

            /**
             * Constructs a ProtocolVectorField from a single data value
             */
            explicit ProtocolVectorField(ReferenceType data) :
                ParentType(data)
            {
            }

            /**
             * Move-constructs a ProtocolVectorField from a serializable vector of data values
             */
            explicit ProtocolVectorField(VectorType&& data) :
                ParentType(std::forward<VectorType>(data))
            {
            }

            /**
             * Constructs a ProtocolVectorField from a vector of data values
             */
            explicit ProtocolVectorField(const std::vector<DataType>& data) :
                ParentType(VectorType(data))
            {
            }

            /**
             * Move-constructs a ProtocolVectorField from a vector of data values
             */
            explicit ProtocolVectorField(std::vector<DataType>&& data) :
                ParentType(VectorType(std::forward<std::vector<DataType>>(data)))
            {
            }

            /**
             * Constructs an empty ProtocolVectorField
             */
            explicit ProtocolVectorField(const type_utils::none_t&) :
                ParentType(type_utils::none)
            {
            }

            /**
             * Move-constructs an empty ProtocolVectorField
             */
            explicit ProtocolVectorField(type_utils::none_t&&) :
                ParentType(type_utils::none)
            {
            }
    };

    /**
     * \typedef ProtocolPackedBitVectorField
     * Partial specialization of ProtocolVectorField that stores data in a SerializablePackedBitVector
     */
    template<typename FieldType,
             typename DataType,
             typename SizeType,
             ProtocolFieldFormatter<SerializablePackedBitVector<DataType, SizeType>> Formatter = defaultProtocolFieldFormatter<SerializablePackedBitVector<DataType, SizeType>>>
    using ProtocolPackedBitVectorField = ProtocolVectorField<FieldType, DataType, SizeType, SerializablePackedBitVector<DataType, SizeType>, Formatter>;
}

#endif
