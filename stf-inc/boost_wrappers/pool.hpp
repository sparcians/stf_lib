#ifndef __BOOST_POOL_HPP__
#define __BOOST_POOL_HPP__

#include "boost_wrappers/setup.hpp"

#define BOOST_POOL_NO_MT
#pragma push_macro("T0")
#undef T0
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include <boost/pool/pool_alloc.hpp>
#pragma GCC diagnostic pop
#pragma pop_macro("T0")
#undef BOOST_POOL_NO_MT

#endif
