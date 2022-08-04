#ifndef __STF_ENUM_UTILS_HPP__
#define __STF_ENUM_UTILS_HPP__

#include <array>
#include <cstdint>
#include <cstddef>
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
        return static_cast<size_t>(E::RESERVED_END) + 1;
    }

    /**
     * \typedef EnumArray
     *
     * std::array that gets its size from an enum class based on its RESERVED_END member
     */
    template<typename T, typename E>
    using EnumArray = std::array<T, size<E>()>;

} // end namespace stf::enums

#endif
