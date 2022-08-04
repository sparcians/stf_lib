// <STF> -*- HPP -*-

/**
 * \brief  This file defines the Simple Trace Format (STF) data structures and functions
 *
 */

/* works only on little endian host */
#if __BYTE_ORDER__ == __BIG_ENDIAN__
// cppcheck-suppress preprocessorErrorDirective
#error "STF library does not run on big endian computer"
#endif

#ifndef __STF_HPP__
#define __STF_HPP__

#include <cstdint>
#include <iostream>

#include "format_utils.hpp"
#include "stf_exception.hpp"
#include "stf_git_version.hpp"

namespace stf {
    /// Current STF library major version
    static constexpr uint32_t STF_CUR_VERSION_MAJOR = 1;
    /// Current STF library minor version
    static constexpr uint32_t STF_CUR_VERSION_MINOR = 5;

    /// Current STF library major version
    static constexpr uint32_t STF_OLDEST_SUPPORTED_VERSION_MAJOR = 0;
    /// Current STF library minor version
    static constexpr uint32_t STF_OLDEST_SUPPORTED_VERSION_MINOR = 8;

    /**
     * \brief Format the STF library version as a string
     * \param os ostream to format into
     */
    static inline void formatVersion(std::ostream& os) {
        os << "Built with STF version ";
        format_utils::formatDec(os, STF_CUR_VERSION_MAJOR);
        os << '.';
        format_utils::formatDec(os, STF_CUR_VERSION_MINOR);
        os << ", stf_lib commit SHA " << STF_GIT_VERSION << std::endl;
    }

    /**
     * \brief Print the STF library version used
     */
    static inline void printVersion() {
        formatVersion(std::cout);
    }

    static inline void checkVersion(uint32_t major, uint32_t minor) {
        // the STF library provides backward compatibility for v0.5 and later version
        stf_assert(((major <= STF_CUR_VERSION_MAJOR) ||
                    ((major == STF_CUR_VERSION_MAJOR) && (minor <= STF_CUR_VERSION_MINOR))) &&
                   (major > STF_OLDEST_SUPPORTED_VERSION_MAJOR || minor >= STF_OLDEST_SUPPORTED_VERSION_MINOR),
                   "ERROR: trace STF version " << std::dec << major << '.' << minor << " is incompatible with the tool's version " << STF_CUR_VERSION_MAJOR << '.' << STF_CUR_VERSION_MINOR << '!');
    }
} // end namespace stf
// __STF_HPP__
#endif
