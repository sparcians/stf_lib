#ifndef __TYPE_UTILS_HPP__
#define __TYPE_UTILS_HPP__

#include <iterator>
#include <type_traits>

namespace stf {
    namespace type_utils {
        template <class T, class... Ts>
        struct are_same : std::conjunction<std::is_same<T, Ts>...> {};

        template <class... Ts>
        struct are_trivially_copyable : std::conjunction<std::is_trivially_copyable<std::remove_reference_t<Ts>>...> {};

        template <typename T, typename = void>
        struct is_iterable : std::false_type {};

        // this gets used only when we can call std::begin() and std::end() on that type
        template <typename T>
        struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())),
                                          decltype(std::end(std::declval<T>()))>> : std::true_type {};

    } // end namespace type_utils
} // end namespace stf

#endif
