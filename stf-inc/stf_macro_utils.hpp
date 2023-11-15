#ifndef __STF_MACRO_UTILS_HPP__
#define __STF_MACRO_UTILS_HPP__

#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/punctuation/is_begin_parens.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/enum.hpp>
#include <boost/preprocessor/variadic/to_tuple.hpp>

/**
 * \def STF_IS_TUPLE
 * Returns whether the argument is a Boost::Preprocessor tuple
 */
#define STF_IS_TUPLE(val) BOOST_PP_IS_BEGIN_PARENS(val)

/**
 * \def STF_PACK_TEMPLATE
 * Passing a template specializations with multiple arguments (e.g. A<1, 2, 3, 4>) to a C macro breaks the
 * preprocessor since it interprets the template arguments as additional macro arguments. This macro
 * packs the template class and its specialization arguments into a tuple of the form
 * (class, (arg1, arg2, ...)) so that it can be unpacked as needed with STF_UNPACK_TEMPLATE
 */
#define STF_PACK_TEMPLATE(class, ...) (class, BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__))

// Gets only the class name from a template tuple
#define _STF_UNPACK_TEMPLATE_CLASS(template_tuple) BOOST_PP_TUPLE_ELEM(0, template_tuple)

// Unpacks a template tuple into a template specialization
#define _STF_UNPACK_TEMPLATE(template_tuple) \
    _STF_UNPACK_TEMPLATE_CLASS(template_tuple)<BOOST_PP_TUPLE_ENUM(BOOST_PP_TUPLE_ELEM(1, template_tuple))>

/**
 * Gets only the class name from a tuple packed with STF_PACK_TEMPLATE. If template_tuple is not a tuple,
 * it treats it as a non-template class name and returns it unchanged.
 */
#define STF_UNPACK_TEMPLATE_CLASS(template_tuple) \
    BOOST_PP_IIF(                           \
        STF_IS_TUPLE(template_tuple),       \
        _STF_UNPACK_TEMPLATE_CLASS,         \
        BOOST_PP_EXPAND                     \
    )(template_tuple)

/**
 * Gets the full template specialization from a tuple packed with STF_PACK_TEMPLATE. If template_tuple is
 * not a tuple, it treats it as a non-template class name and returns it unchanged.
 */
#define STF_UNPACK_TEMPLATE(template_tuple) \
    BOOST_PP_IIF(                           \
        STF_IS_TUPLE(template_tuple),       \
        _STF_UNPACK_TEMPLATE,               \
        BOOST_PP_EXPAND                     \
    )(template_tuple)

#endif
