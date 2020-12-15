
// <Util> -*- HPP -*-

/**
 * \brief  This file defines a few simple utility functions and values
 */

#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <cstdint>
#include <memory>
#include <string_view>
#include <sstream>
#include <type_traits>
#include <limits>

#include "type_utils.hpp"

namespace stf {
    namespace byte_utils {
        /**
         * Converts bytes to bits
         */
        template<typename T>
        static constexpr T toBits(T bytes) {
            constexpr T BYTES_TO_BITS = 8;
            return BYTES_TO_BITS * bytes;
        }

        /**
         * Converts bytes to kilobytes
         */
        template<typename T>
        static constexpr T toKB(T bytes) {
            constexpr T KB_SHIFT = 10;
            return bytes >> KB_SHIFT;
        }

        template<typename T>
        static constexpr size_t bitSize() {
            return toBits(sizeof(T));
        }

        template<typename T, size_t num_bits>
        static constexpr T bitMask() {
            constexpr size_t MAX_BITS = bitSize<T>();

            static_assert(num_bits > 0 && num_bits <= MAX_BITS, "Mask must be >= 1 bit and <= sizeof(T) bits");

            if(num_bits == MAX_BITS) {
                return std::numeric_limits<T>::max();
            }
            else { //NOLINT: readability-else-after-return
                return (1ULL << num_bits) - 1;
            }
        }

    } // end namespace byte_utils

    namespace page_utils {
        static constexpr uint64_t INVALID_PHYS_ADDR = 0xFFFFFFFFFFFFFFFF; /**< constant used to denote an invalid physical address */
        static constexpr uint64_t INVALID_PAGE_SIZE = 0xFFFFFFFFFFFFFFF0; /**< constant used to denote an invalid page size */

        /**
         * Returns whether a physical address is valid
         */
        static constexpr bool isValidPhysAddr(uint64_t addr) {
            return addr != INVALID_PHYS_ADDR;
        }
    } // end namespace page_utils

    namespace math_utils {
        /**
         * \struct constexpr_log
         *
         * Struct that provides a constexpr integer log implementation
         */
        struct constexpr_log {
            /**
             * constexpr log2 implementation
             * \param n value to get log2 from
             */
            template<typename T>
            static constexpr T log2(T n) {
                return (n < 2) ? 0 : 1 + log2(n/2);
            }

            /**
             * constexpr floor(log2) implementation
             * \param n value to get log2 from
             */
            template<typename T>
            static constexpr T floor_log2(const T n) {
                return static_cast<T>(byte_utils::bitSize<T>()) - static_cast<T>(__builtin_clz(n)) - 1;
            }
        };

        /**
         * Sets mask bits in dest if cond is true
         */
        template<typename T>
        inline T conditionalValue() {
            return static_cast<T>(0);
        }

        /**
         * Sets mask bits in dest if cond is true
         */
        template<typename T>
        inline T conditionalValue(const bool enable, const T val) {
            return static_cast<T>(-enable & val);
        }

        template<typename T, typename ... Ts>
        inline typename std::enable_if<sizeof...(Ts) == 0, T>::type
        conditionalValue(const bool enable1, const T val1, const Ts ... args) {
            return conditionalValue(enable1, val1);
        }

        template<typename T, typename ... Ts>
        inline typename std::enable_if<sizeof...(Ts) != 0, T>::type
        conditionalValue(const bool enable1, const T val1, const Ts ... args) {
            return static_cast<T>(conditionalValue(enable1, val1) | conditionalValue(args...));
        }

        /**
         * Sets mask bits in dest if cond is true
         */
        template<typename T, typename U>
        inline void conditionalSet(T& dest, const U mask, const bool cond) {
            dest = static_cast<std::remove_reference_t<decltype(dest)>>(dest | conditionalValue(cond, mask));
        }
    } // end namespace math_utils

    namespace pointer_utils {
        /**
         * \struct is_unique_ptr
         * Determines whether a type is an std::unique_ptr
         */
        template <typename T, typename Deleter = std::default_delete<T>> struct is_unique_ptr : std::false_type {};
        template <typename T> struct is_unique_ptr<std::unique_ptr<T>> : std::true_type {};
        template <typename T, typename Deleter> struct is_unique_ptr<std::unique_ptr<T, Deleter>> : std::true_type {};

        /**
         * \struct is_shared_ptr
         * Determines whether a type is an std::shared_ptr
         */
        template <typename T> struct is_shared_ptr : std::false_type {};
        template <typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

        /**
         * \struct is_smart_ptr
         * Determines whether a type is a smart pointer
         */
        template <typename T>
        struct is_smart_ptr : std::bool_constant<is_unique_ptr<T>::value || is_shared_ptr<T>::value> {};

        /**
         * \struct is_ptr
         * Determines whether a type is a pointer
         */
        template <typename T>
        struct is_ptr : std::bool_constant<is_smart_ptr<T>::value || std::is_pointer<T>::value> {};

    } // end namespace pointer_utils
} // end namespace stf

#endif
