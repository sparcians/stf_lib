#ifndef __STF_ENUM_UTILS_HPP__
#define __STF_ENUM_UTILS_HPP__

#include <cstdint>
#include <type_traits>

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
    using int_t = typename std::underlying_type<E>::type;

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
    constexpr typename std::enable_if<sizeof(E) != 1, int_t<E>>::type
    to_printable_int(E e) {
        return to_int<E>(e);
    }

    /**
     * Converts an enum class to a printable version of its underlying type
     * \param e enum to convert
     */
    template<typename E>
    constexpr typename std::enable_if<sizeof(E) == 1, uint32_t>::type
    to_printable_int(E e) {
        return static_cast<uint32_t>(e);
    }

} // end namespace stf::enums

#endif
