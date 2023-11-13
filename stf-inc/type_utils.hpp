#ifndef __TYPE_UTILS_HPP__
#define __TYPE_UTILS_HPP__

#include <iterator>
#include <memory>
#include <type_traits>

#include <boost/none.hpp>

namespace stf {
    namespace type_utils {
        template <typename T, typename... Ts>
        struct are_same : std::conjunction<std::is_same<T, Ts>...> {};

        template<typename T, typename... Ts>
        inline constexpr bool are_same_v = are_same<T, Ts...>::value;

        template <typename... Ts>
        struct are_trivially_copyable : std::conjunction<std::is_trivially_copyable<std::remove_reference_t<Ts>>...> {};

        template<typename... Ts>
        inline constexpr bool are_trivially_copyable_v = are_trivially_copyable<Ts...>::value;

        template <typename T, typename = void>
        struct is_iterable : std::false_type {};

        // this gets used only when we can call std::begin() and std::end() on that type
        template <typename T>
        struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())),
                                          decltype(std::end(std::declval<T>()))>> : std::true_type {};

        template<typename T>
        struct is_arithmetic_or_enum : std::disjunction<std::is_arithmetic<T>, std::is_enum<T>> {};

        template<typename T>
        inline constexpr bool is_arithmetic_or_enum_v = is_arithmetic_or_enum<T>::value;

        template<typename T>
        struct is_integral_or_enum : std::disjunction<std::is_integral<T>, std::is_enum<T>> {};

        template<typename T>
        inline constexpr bool is_integral_or_enum_v = is_integral_or_enum<T>::value;

        template<typename T, typename U>
        struct fits_in : std::bool_constant<sizeof(T) <= sizeof(U)> {};

        template<size_t Size, typename... Ts>
        struct is_pack_size : std::bool_constant<sizeof...(Ts) == Size> {};

        template<typename... Ts>
        struct is_pack_empty : is_pack_size<0, Ts...> {};

        template<typename ArrayType, typename Value>
        inline constexpr ArrayType initArray(const Value val) {
            ArrayType new_arr{};
            for(size_t i = 0; i < new_arr.size(); ++i) {
                new_arr[i] = val;
            }
            return new_arr;
        }

        template<typename T>
        class optimal_const_ref {
            private:
                using const_type = std::add_const_t<T>;
            public:
                using type = std::conditional_t<std::is_fundamental_v<const_type>,
                                                std::remove_reference_t<const_type>,
                                                std::add_lvalue_reference_t<const_type>>;
        };

        template<typename T>
        using optimal_const_ref_t = typename optimal_const_ref<T>::type;

        using none_t = boost::none_t;
        static inline constexpr auto none = boost::none;
    } // end namespace type_utils
} // end namespace stf

#endif
