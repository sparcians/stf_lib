#ifndef __STF_MACRO_UTILS_HPP__
#define __STF_MACRO_UTILS_HPP__

#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/punctuation/is_begin_parens.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/enum.hpp>
#include <boost/preprocessor/variadic/to_tuple.hpp>

#define STF_IS_TUPLE(val) BOOST_PP_IS_BEGIN_PARENS(val)

#define STF_PACK_TEMPLATE(class, ...) (class, BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__))

#define _STF_UNPACK_TEMPLATE_CLASS(template_tuple) BOOST_PP_TUPLE_ELEM(0, template_tuple)

#define _STF_UNPACK_TEMPLATE(template_tuple) \
    _STF_UNPACK_TEMPLATE_CLASS(template_tuple)<BOOST_PP_TUPLE_ENUM(BOOST_PP_TUPLE_ELEM(1, template_tuple))>

#define STF_UNPACK_TEMPLATE_CLASS(template_tuple) \
    BOOST_PP_IIF(                           \
        STF_IS_TUPLE(template_tuple),       \
        _STF_UNPACK_TEMPLATE_CLASS,         \
        BOOST_PP_EXPAND                     \
    )(template_tuple)

#define STF_UNPACK_TEMPLATE(template_tuple) \
    BOOST_PP_IIF(                           \
        STF_IS_TUPLE(template_tuple),       \
        _STF_UNPACK_TEMPLATE,               \
        BOOST_PP_EXPAND                     \
    )(template_tuple)

#endif
