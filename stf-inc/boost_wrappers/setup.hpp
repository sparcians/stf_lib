#ifndef __BOOST_SETUP_HPP__
#define __BOOST_SETUP_HPP__

#include <boost/version.hpp>

// This header file is used to set up various Boost #defines
// that need to be applied for any boost includes

#if BOOST_VERSION >= 108000
    // Make boost::move use std::move
    // There was a bug (fixed in version 1.80+) that caused compile errors when this was set
    #define BOOST_MOVE_USE_STANDARD_LIBRARY_MOVE 1
#endif

#endif
