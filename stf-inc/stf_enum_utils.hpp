#ifndef __STF_ENUM_UTILS_HPP__
#define __STF_ENUM_UTILS_HPP__

#include <array>
#include <cstdint>
#include <cstddef>
#include <type_traits>

#include "type_utils.hpp"

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
     * Initializes an EnumArray at compile time by iterating between enum values Start and End, calling init_func to generate the value for each entry.
     * init_func should take an enum index value and an EnumArray as parameters, returning a transformed copy of the array
     * \param init_func Callback function used to generate array values
     */
    template<typename T, typename E, T default_value, E Start, E End, typename InitFunc>
    inline constexpr EnumArray<T, E> populateEnumArray(InitFunc&& init_func) {
        static_assert(Start <= End, "Start must be less than or equal to End");

        if constexpr(Start < End) {
            auto arr = populateEnumArray<T, E, default_value, static_cast<decltype(Start)>(enums::to_int(Start)+1), End, InitFunc>(std::forward<InitFunc>(init_func));
            return init_func(std::integral_constant<E, Start>(), arr);
        }

        return type_utils::initArray<EnumArray<T, E>>(default_value);
    }

    /**
     * Initializes an EnumArray at compile time by iterating over every value in the enum, calling init_func to generate the value for each entry.
     * init_func should take an enum index value and an EnumArray as parameters, returning a transformed copy of the array
     * \param init_func Callback function used to generate array values
     */
    template<typename T, typename E, T default_value, typename InitFunc>
    inline constexpr EnumArray<T, E> populateEnumArray(InitFunc&& init_func) {
        return populateEnumArray<T, E, default_value, E::__RESERVED_START, E::__RESERVED_END, InitFunc>(std::forward<InitFunc>(init_func));
    }
} // end namespace stf::enums

#endif
