#ifndef __STF_ENUM_UTILS_HPP__
#define __STF_ENUM_UTILS_HPP__

#include <array>
#include <cstdint>
#include <cstddef>
#include <type_traits>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/comparison/less.hpp>
#include <boost/preprocessor/facilities/identity.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/filter.hpp>
#include <boost/preprocessor/seq/fold_left.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/push_back.hpp>
#include <boost/preprocessor/seq/push_front.hpp>
#include <boost/preprocessor/seq/reverse.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/eat.hpp>
#include <boost/preprocessor/tuple/replace.hpp>
#include <boost/preprocessor/tuple/size.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

#include "stf_exception.hpp"
#include "stf_macro_utils.hpp"
#include "type_utils.hpp"

// Index of the name in an STF_ENUM entry tuple
#define _STF_ENUM_NAME_IDX 0
// Index of the string representation in an STF_ENUM entry tuple
#define _STF_ENUM_STR_IDX 1
// Index of the enum value in an STF_ENUM entry tuple
#define _STF_ENUM_VAL_IDX 2
// Index of the print enable bit in an STF_ENUM entry tuple
#define _STF_ENUM_ENABLE_PRINT_IDX 3

/**
 * \def STF_ENUM_NO_PRINT
 * Can be passed as the 4th argument to STF_ENUM_VAL to disable printing of an STF_ENUM element.
 * Useful for enum values that alias other enum values
 */
#define STF_ENUM_NO_PRINT 0

#define _GET_STF_ENUM_ENABLE_PRINT(elem_tuple)  \
    BOOST_PP_IIF(                               \
        BOOST_PP_LESS(                          \
            _STF_ENUM_ENABLE_PRINT_IDX,         \
            BOOST_PP_TUPLE_SIZE(elem_tuple)     \
        ),                                      \
        BOOST_PP_TUPLE_ELEM,                    \
        BOOST_PP_IDENTITY_N(1, 2)               \
    )(_STF_ENUM_ENABLE_PRINT_IDX, elem_tuple)

// Gets the name from an STF_ENUM entry tuple
#define _GET_STF_ENUM_NAME(elem_tuple) BOOST_PP_TUPLE_ELEM(_STF_ENUM_NAME_IDX, elem_tuple)

// Gets the string representation from an STF_ENUM entry tuple
#define _GET_STF_ENUM_STR(elem_tuple) BOOST_PP_TUPLE_ELEM(_STF_ENUM_STR_IDX, elem_tuple)

// Defines an STF_ENUM entry tuple with name, string representation, value, and print-enable bit
#define _STF_ENUM_VAL_3(name, val, str, enable_print) (name, str, val, enable_print)

// Defines an STF_ENUM entry tuple with name, string representation, and value.
// Print-enable is automatically set to 1
#define _STF_ENUM_VAL_2(name, val, str) _STF_ENUM_VAL_3(name, val, str, 1)

// Defines an STF_ENUM entry tuple with name and value
// String representation is auto-generated from the name and print-enable is automatically set to 1.
#define _STF_ENUM_VAL_1(name, val) _STF_ENUM_VAL_2(name, val, BOOST_PP_STRINGIZE(name))

/**
 * \def STF_ENUM_VAL
 * Defines an STF_ENUM entry tuple
 * Allowed usage:
 *   STF_ENUM_VAL(name, val)
 *     Defines an STF_ENUM entry with the given name and value. String representation is auto-generated
 *     and print-enable is set to 1.
 *   STF_ENUM_VAL(name, val, str)
 *     Defines an STF_ENUM entry with the given name, value, and string representation.
 *     Print-enable is set to 1.
 *   STF_ENUM_VAL(name, val, str, enable_print)
 *     Defines an STF_ENUM entry with the given name, value, string representation, and print-enable setting.
 */
#define STF_ENUM_VAL(name, ...) BOOST_PP_OVERLOAD(_STF_ENUM_VAL_, __VA_ARGS__)(name, __VA_ARGS__)

/**
 * \def STF_ENUM_ALIAS
 * Defines an STF_ENUM entry as an alias of another entry
 */
#define STF_ENUM_ALIAS(name, other_name) STF_ENUM_VAL(name, other_name, BOOST_PP_NIL, STF_ENUM_NO_PRINT)

/**
 * \def STF_ENUM_STR
 * Defines an STF_ENUM entry with the given name and string representation. Value is auto-generated and
 * print-enable is set to 1.
 */
#define STF_ENUM_STR(name, str) (name, str)

// Defines an STF_ENUM entry tuple from a name only. Allows users to use bare names in an STF_ENUM just as
// they would with a normal enum class. String representation and value are auto-generated and print-enable
// is set to 1.
#define _DEFAULT_STF_ENUM_VAL(name) STF_ENUM_STR(name, BOOST_PP_STRINGIZE(name))

// STF_ENUM config flags that can be supplied with STF_ENUM_CONFIG when defining an STF_ENUM
/**
 * \def AUTO_PRINT
 * Generates an automatic std::ostream operator<< helper function for the STF_ENUM
 */
#define AUTO_PRINT              0

/**
 * \def ALLOW_UNKNOWN
 * Allows unknown enum values in the automatic print function (default throws an exception for unknown
 * values). Does nothing unless AUTO_PRINT is set.
 */
#define ALLOW_UNKNOWN           1

/**
 * \def OVERRIDE_START
 * If specified, the automatic __RESERVED_START element will become an alias of the first enum element
 */
#define OVERRIDE_START          2

/**
 * \def OVERRIDE_START
 * If specified, the automatic __RESERVED_END element will become an alias of the last enum element
 */
#define OVERRIDE_END            3

// Specifies how many config flags are defined. Should always equal (highest_config_flag_value + 1)
#define _STF_ENUM_CONFIG_SIZE   4

// Generates the default config flags (all 0's, i.e. all config options disabled)
#define _DEFAULT_STF_CONFIG (BOOST_PP_ENUM(_STF_ENUM_CONFIG_SIZE, BOOST_PP_IDENTITY_N(0, 3), _))

// Enables the config flag at index elem in config tuple state
// Used by the BOOST_PP_SEQ_FOLD_LEFT invocation in _FLATTEN_STF_ENUM_CONFIG
#define _STF_SET_CONFIG_FLAG(s, state, elem) BOOST_PP_TUPLE_REPLACE(state, elem, 1)

// Converts an STF_ENUM_CONFIG tuple into a tuple of binary flags indicating which config options are set
#define _FLATTEN_STF_ENUM_CONFIG(config_tuple)  \
    BOOST_PP_SEQ_FOLD_LEFT(                     \
        _STF_SET_CONFIG_FLAG,                   \
        _DEFAULT_STF_CONFIG,                    \
        BOOST_PP_TUPLE_TO_SEQ(config_tuple)     \
    )

/**
 * \def STF_ENUM_CONFIG
 * Can be specified as the first argument to STF_ENUM to change its compile-time configuration
 * Usage:
 *   STF_ENUM(STF_ENUM_CONFIG(flag1, flag2), MY_NEW_ENUM ...) will enable config flags flag1 and flag2 in
 *   the STF_ENUM named MY_NEW_ENUM.
 */
#define STF_ENUM_CONFIG(...) BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__)

// Expands to expr 1 if config_flag is set in config_tuple. Otherwise expands to expr2.
#define _IIF_CONFIG(config_tuple, config_flag, expr1, expr2)    \
    BOOST_PP_IIF(                                               \
        BOOST_PP_TUPLE_ELEM(                                    \
            config_flag,                                        \
            config_tuple                                        \
        ),                                                      \
        expr1,                                                  \
        expr2                                                   \
    )

// Expands an STF_ENUM element tuple to C++ enum class element syntax
// If there is no value defined with STF_ENUM_VAL, will expand to:
//    name
// If there is a value defined with STF_ENUM_VAL, will expand to:
//    name = value
#define _POPULATE_STF_ENUM_ELEM(r, data, elem_tuple)    \
    _GET_STF_ENUM_NAME(elem_tuple)                      \
    BOOST_PP_IIF(                                       \
        BOOST_PP_LESS(                                  \
            _STF_ENUM_VAL_IDX,                          \
            BOOST_PP_TUPLE_SIZE(elem_tuple)             \
        ),                                              \
        = BOOST_PP_TUPLE_ELEM,                          \
        BOOST_PP_TUPLE_EAT()                            \
    )(_STF_ENUM_VAL_IDX, elem_tuple)

// Expands to a case statement that prints a single STF_ENUM element to an std::ostream
// Used in the BOOST_PP_SEQ_FOR_EACH invocation in _AUTO_PRINT
#define _PRINT_ENUM(r, class, val)          \
    case class::_GET_STF_ENUM_NAME(val):    \
        return os << _GET_STF_ENUM_STR(val);

// Declares an std::ostream operator<< function for an STF_ENUM
#define _OSTREAM_DECL(config, name) \
    friend std::ostream& operator<<(std::ostream& os, const name val)

// Declares an std::ostream operator<< function for an STF_ENUM *without defining it*.
// It must be separately defined somewhere else to avoid linking errors.
#define _NO_AUTO_PRINT(config, name, elem_seq) _OSTREAM_DECL(config, name);

// Gets the print-enabled flag for an STF_ENUM element tuple
// Used by the BOOST_PP_SEQ_FILTER invocation in _AUTO_PRINT
#define _PRINT_ENABLED(s, data, elem) _GET_STF_ENUM_ENABLE_PRINT(elem)

// Declares *and defines* an std::ostream operator<< function for an STF_ENUM.
// Each element will be printed using its string representation. It will be auto-generated from the
// element name unless it is manually specified with STF_ENUM_VAL or STF_ENUM_STR.
// Elements with the print-enabled flag set to 0 will be skipped.
// If the ALLOW_UNKNOWN config flag is set, unknown values will be printed as UNKNOWN_val, where val is
// the decimal representation of the enum element value.
// If ALLOW_UNKNOWN is not set, an invalid value will trigger an exception.
#define _AUTO_PRINT(config, name, elem_seq)                                                             \
    inline _OSTREAM_DECL(config, name) {                                                                \
        switch(val) {                                                                                   \
            BOOST_PP_SEQ_FOR_EACH(                                                                      \
                _PRINT_ENUM,                                                                            \
                name,                                                                                   \
                BOOST_PP_SEQ_FILTER(                                                                    \
                    _PRINT_ENABLED,                                                                     \
                    _,                                                                                  \
                    elem_seq                                                                            \
                )                                                                                       \
            )                                                                                           \
        };                                                                                              \
        _IIF_CONFIG(                                                                                    \
            config,                                                                                     \
            ALLOW_UNKNOWN,                                                                              \
            return os << "UNKNOWN_" << enums::to_printable_int(val),                                    \
            stf_throw("Invalid " BOOST_PP_STRINGIZE(name) " value: " << enums::to_printable_int(val))   \
        );                                                                                              \
    }

/**
 * \def STF_ENUM_STRUCT
 * Gets the name of the struct that encapsulates an STF_ENUM
 */
#define STF_ENUM_STRUCT(name) BOOST_PP_CAT(_STF_ENUM_, name)

// This macro does the actual work of defining an STF_ENUM, encapsulated inside of a struct so that
// STF_ENUM can be used inside a class. It also declares (and optionally defines) an std::ostream
// operator<< function. Finally, it exposes the enum to the enclosing namespace so that it can be used
// without referencing the encapsulating struct.
#define _DEFINE_STF_ENUM(config, name, type, enum_elems)    \
    struct STF_ENUM_STRUCT(name) {                          \
        enum class name : type {                            \
            BOOST_PP_SEQ_ENUM(                              \
                BOOST_PP_SEQ_TRANSFORM(                     \
                    _POPULATE_STF_ENUM_ELEM,                \
                    _,                                      \
                    enum_elems                              \
                )                                           \
            )                                               \
        };                                                  \
        _IIF_CONFIG(                                        \
            config,                                         \
            AUTO_PRINT,                                     \
            _AUTO_PRINT,                                    \
            _NO_AUTO_PRINT                                  \
        )(config, name, enum_elems)                         \
    };                                                      \
    using name = STF_ENUM_STRUCT(name)::name

// Gets the name of the first enum element in the given sequence
#define _GET_HEAD_NAME(elem_seq) _GET_STF_ENUM_NAME(BOOST_PP_SEQ_HEAD(elem_seq))

// Adds __RESERVED_START as an alias for the first enum element
// Used when the OVERRIDE_START config flag is specified
#define _ADD_RESERVED_START_OVERRIDE(enum_elems)    \
    BOOST_PP_SEQ_PUSH_BACK(                         \
        enum_elems,                                 \
        STF_ENUM_ALIAS(                             \
            __RESERVED_START,                       \
            _GET_HEAD_NAME(enum_elems)              \
        )                                           \
    )

// Adds __RESERVED_START as the first element in the enum
// Used when the OVERRIDE_START config flag is *not* specified
#define _ADD_RESERVED_START(enum_elems)         \
    BOOST_PP_SEQ_PUSH_FRONT(                    \
        enum_elems,                             \
        _DEFAULT_STF_ENUM_VAL(__RESERVED_START) \
    )

// Adds __RESERVED_START and __RESERVED_END entries to the element sequence
// If OVERRIDE_START is specified, __RESERVED_START will be an alias for the first enum element.
// If OVERRIDE_END is specified, __RESERVED_END will be an alias for the last enum element.
// Otherwise, __RESERVED_START and __RESERVED_END will be added as the first and last elements in the enum
#define _ADD_RESERVED_ENTRIES(config, enum_elems)                       \
    _IIF_CONFIG(                                                        \
        config,                                                         \
        OVERRIDE_START,                                                 \
        _ADD_RESERVED_START_OVERRIDE,                                   \
        _ADD_RESERVED_START                                             \
    )(                                                                  \
        BOOST_PP_SEQ_PUSH_BACK(                                         \
            enum_elems,                                                 \
            _IIF_CONFIG(                                                \
                config,                                                 \
                OVERRIDE_END,                                           \
                STF_ENUM_ALIAS(                                         \
                    __RESERVED_END,                                     \
                    _GET_HEAD_NAME(BOOST_PP_SEQ_REVERSE(enum_elems))    \
                ),                                                      \
                _DEFAULT_STF_ENUM_VAL(__RESERVED_END)                   \
            )                                                           \
        )                                                               \
    )

// Converts a bare enum element name to an STF_ENUM element tuple
// If the element is already a tuple, it is returned unchanged
// This allows users to mix bare names and STF_ENUM_VAL/STF_ENUM_STR/STF_ENUM_ALIAS directives in STF_ENUM
#define _PROCESS_STF_ENUM_ELEM(r, data, elem)   \
    BOOST_PP_IIF(                               \
        STF_IS_TUPLE(elem),                     \
        BOOST_PP_EXPAND,                        \
        _DEFAULT_STF_ENUM_VAL                   \
    )(elem)

// Defines an STF_ENUM with the given config, name, and underlying type.
// The __VA_ARGS__ are the enum elements, which can be either bare names or tuples generated with
// STF_ENUM_VAL/STF_ENUM_STR/STF_ENUM_ALIAS directives. These args are all processed and normalized into
// element tuples with BOOST_PP_SEQ_TRANSFORM before being passed to _ADD_RESERVED_ENTRIES.
#define __STF_ENUM_CONFIG(config, name, type, ...)      \
    _DEFINE_STF_ENUM(                                   \
        config,                                         \
        name,                                           \
        type,                                           \
        _ADD_RESERVED_ENTRIES(                          \
            config,                                     \
            BOOST_PP_SEQ_TRANSFORM(                     \
                _PROCESS_STF_ENUM_ELEM,                 \
                _,                                      \
                BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)   \
            )                                           \
        )                                               \
    )

// Defines an STF_ENUM with the given config, name, and underlying type.
// The __VA_ARGS__ are the enum elements, which can be either bare names or tuples generated with
// STF_ENUM_VAL/STF_ENUM_STR/STF_ENUM_ALIAS directives. The config is converted from an STF_ENUM_CONFIG
// tuple to a tuple of binary flags with _FLATTEN_STF_ENUM_CONFIG before passing everything to
// __STF_ENUM_CONFIG
#define _STF_ENUM_CONFIG(config, name, type, ...)   \
    __STF_ENUM_CONFIG(                              \
            _FLATTEN_STF_ENUM_CONFIG(config),       \
            name,                                   \
            type,                                   \
            __VA_ARGS__                             \
    )

// Defines an STF_ENUM with the given name and underlying type.
// The __VA_ARGS__ are the enum elements, which can be either bare names or tuples generated with
// STF_ENUM_VAL/STF_ENUM_STR/STF_ENUM_ALIAS directives. Uses a default config where all flags are disabled.
#define _STF_ENUM_NO_CONFIG(name, type, ...)    \
    __STF_ENUM_CONFIG(                          \
        _DEFAULT_STF_CONFIG,                    \
        name,                                   \
        type,                                   \
        __VA_ARGS__                             \
    )

/**
 * \def STF_ENUM
 *
 * Defines a new enum class compatible with all of the helpers in stf::enums
 * Also declares an std::ostream operator<< function which may be auto-defined or can be defined manually.
 * Allowed usage:
 *   STF_ENUM(name, type, elements...)
 *     Generates an STF_ENUM with the default config, specified name, underlying type, and elements.
 *     std::ostream operator<< function is auto-declared but must be defined manually.
 *   STF_ENUM(STF_ENUM_CONFIG(flags...), name, type, elements...)
 *     Generates an STF_ENUM with the specified config, name, underlying type, and elements.
 *     If AUTO_PRINT is included in the config flags, the std::ostream operator<< function is auto-defined.
 */
#define STF_ENUM(config, name, type, ...) \
    BOOST_PP_IIF(                         \
        STF_IS_TUPLE(config),             \
        _STF_ENUM_CONFIG,                 \
        _STF_ENUM_NO_CONFIG               \
    )(config, name, type, __VA_ARGS__)

/**
 * \namespace stf::enums
 *
 * Provides convenience functions for working with enums
 */
namespace stf::enums {
    /**
     * \typedef int_t
     * Gets the underlying integer type of an enum class
     */
    template<typename E>
    using int_t = std::underlying_type_t<E>;

    /**
     * Converts an enum class to its underlying type
     * \param e enum to convert
     */
    template<typename E>
    constexpr int_t<E> to_int(E e) {
        return static_cast<int_t<E>>(e);
    }

    /**
     * Converts an enum class to a printable version of its underlying type
     * \param e enum to convert
     */
    template<typename E>
    constexpr std::enable_if_t<sizeof(E) != 1, int_t<E>>
    to_printable_int(E e) {
        return to_int<E>(e);
    }

    /**
     * Converts an enum class to a printable version of its underlying type
     * \param e enum to convert
     */
    template<typename E>
    constexpr std::enable_if_t<sizeof(E) == 1, uint32_t>
    to_printable_int(E e) {
        return static_cast<uint32_t>(e);
    }

    /**
     * Returns how many values are in an enum class
     */
    template <typename E>
    constexpr size_t size() {
        return static_cast<size_t>(E::__RESERVED_END) + 1;
    }

    /**
     * \typedef EnumArray
     *
     * std::array that gets its size from an enum class based on its __RESERVED_END member
     */
    template<typename T, typename E>
    using EnumArray = std::array<T, size<E>()>;

    /**
     * Initializes an EnumArray at compile time by iterating over an std::index_sequence, calling init_func to generate the value for each entry.
     * init_func should take an enum index value and an EnumArray as parameters, returning a transformed copy of the array
     * \param init_func Callback function used to generate array values
     */
    template<typename T, typename E, T default_value, typename InitFunc, size_t ... Is>
    inline constexpr EnumArray<T, E> populateEnumArray(InitFunc&& init_func, std::index_sequence<Is...>) {
        auto arr = type_utils::initArray<EnumArray<T, E>>(default_value);

        auto applyFunc = [&arr, &init_func](auto index){
            arr = init_func(index, arr);
        };

        (applyFunc(std::integral_constant<E, static_cast<E>(enums::to_int(E::__RESERVED_START) + Is)>()), ...);

        return arr;
    }

    /**
     * Initializes an EnumArray at compile time by iterating over every value in the enum, calling init_func to generate the value for each entry.
     * init_func should take an enum index value and an EnumArray as parameters, returning a transformed copy of the array
     * \param init_func Callback function used to generate array values
     */
    template<typename T, typename E, T default_value, typename InitFunc>
    inline constexpr EnumArray<T, E> populateEnumArray(InitFunc&& init_func) {
        return populateEnumArray<T, E, default_value, InitFunc>(std::forward<InitFunc>(init_func),
                                                                std::make_index_sequence<enums::to_int(E::__RESERVED_END) - enums::to_int(E::__RESERVED_START)>());
    }
} // end namespace stf::enums

#endif
