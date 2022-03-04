
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

        /**
         * Gets size of a type in bits
         */
        template<typename T>
        static constexpr size_t bitSize() {
            return toBits(sizeof(T));
        }

        /**
         * Gets a bitmask of a specified number of bits
         */
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

        /**
         * Gets a bitmask of a specified number of bits
         */
        template<typename T>
        inline T bitMask(const size_t num_bits) {
            constexpr size_t MAX_BITS = bitSize<T>();

            stf_assert(num_bits > 0 && num_bits <= MAX_BITS, "Mask must be >= 1 bit and <= sizeof(T) bits");

            if(num_bits == MAX_BITS) {
                return std::numeric_limits<T>::max();
            }
            else { //NOLINT: readability-else-after-return
                return (1ULL << num_bits) - 1;
            }
        }

        /**
         * \class BitExtractor
         * \brief Class that knows how to extract single bits and contiguous regions of bits from integer types
         */
        template<typename T>
        class BitExtractor {
            private:
                /**
                 * \typedef U
                 * \brief Non-const version of T that ensures this class works on const and non-const types
                 */
                using U = std::remove_const_t<T>;

                /**
                 * \class BitParameterBase
                 * \brief Dummy base class for Bit and BitRange that prevents the extraction methods from being used
                 * with other types
                 */
                struct BitParameterBase {
                };

            public:
                /**
                 * \struct Bit
                 * \brief Extracts a single bit at bit_idx, optionally shifting it to dest_bit_idx
                 */
                template<size_t bit_idx, size_t dest_bit_idx = bit_idx>
                struct Bit : public BitParameterBase {
                    static_assert(bit_idx < bitSize<U>(), "Bit index must fit inside specified type");
                    static_assert(dest_bit_idx < bitSize<U>(), "Destination bit index must fit inside specified type");

                    /**
                     * Gets the bit at bit_idx from the specified value, optionally shifting it to dest_bit_idx
                     * \param val Value to extract the bit from
                     */
                    static constexpr U get(const U val) {
                        U result = val & (U(1) << bit_idx);

// Older (<10.0) GCC versions complain about shift assignment operators with -Wconversion
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=40752
#if defined(__GNUC__) && (__GNUC__ < 10)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
                        if(dest_bit_idx > bit_idx) {
                            result <<= (dest_bit_idx - bit_idx);
                        }
                        else if(dest_bit_idx < bit_idx) {
                            result >>= (bit_idx - dest_bit_idx);
                        }
#if defined(__GNUC__) && (__GNUC__ < 10)
#pragma GCC diagnostic pop
#endif
                        return result;
                    }
                };

                /**
                 * \struct BitRange
                 * \brief Extracts the bits from the range [start_idx:end_idx], optionally shifting it to dest_start_idx
                 */
                template<size_t start_idx, size_t end_idx, size_t dest_start_idx = start_idx>
                struct BitRange : public BitParameterBase {
                    static_assert(start_idx < bitSize<U>(), "Start index must fit inside specified type");
                    static_assert(end_idx < bitSize<U>(), "End index must fit inside specified type");
                    static_assert(dest_start_idx < bitSize<U>(),
                                  "Destination start index must fit inside specified type");
                    static_assert(start_idx >= end_idx, "Bit range should be specified from largest to smallest index");
                    static_assert(dest_start_idx >= (start_idx - end_idx),
                                  "Destination bit range must fit original bit range");

                    /**
                     * Gets the bits from [start_idx:end_idx] from the specified value, optionally shifting them to dest_start_idx
                     * \param val Value to extract the bits from
                     */
                    static constexpr U get(const U val) {
                        constexpr U mask = bitMask<U, start_idx - end_idx + 1>() << end_idx;
                        U result = val & mask;

// Older (<10.0) GCC versions complain about shift assignment operators with -Wconversion
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=40752
#if defined(__GNUC__) && (__GNUC__ < 10)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
                        if(dest_start_idx > start_idx) {
                            result <<= (dest_start_idx - start_idx);
                        }
                        else if(dest_start_idx < start_idx) {
                            result >>= (start_idx - dest_start_idx);
                        }
#if defined(__GNUC__) && (__GNUC__ < 10)
#pragma GCC diagnostic pop
#endif
                        return result;
                    }
                };

                /**
                 * Extracts multiple bits from a value, specified using Bit and/or BitRange classes as the template parameters. Each extracted Bit/BitRange is bitwise-ORed together to form the final value
                 * \param val Value to extract the bits from
                 */
                template<typename ... BitArgs>
                static constexpr typename std::enable_if<(std::is_base_of_v<BitParameterBase, BitArgs> && ...), U>::type
                get(const U val) {
                    return (BitArgs::get(val) | ...);
                }
        };

        /**
         * Helper function that extracts a single bit range [start_idx:end_idx] from the specified value
         * \param val Value to extract bit range from
         */
        template<size_t start_idx, size_t end_idx, typename T>
        static constexpr T getBitRange(const T val) {
            return BitExtractor<T>::template BitRange<start_idx, end_idx>::get(val);
        }

        /**
         * Helper function that extracts a single bit range [start_idx:end_idx] from the specified value, shifting it to dest_start_idx
         * \param val Value to extract bit range from
         */
        template<size_t start_idx, size_t end_idx, size_t dest_start_idx, typename T>
        static constexpr T getBitRange(const T val) {
            return BitExtractor<T>::template BitRange<start_idx, end_idx, dest_start_idx>::get(val);
        }

        /**
         * Helper function that extracts a single bit at bit_idx from the specified value
         * \param val Value to extract bit from
         */
        template<size_t bit_idx, typename T>
        static constexpr T getBit(const T val) {
            return BitExtractor<T>::template Bit<bit_idx>::get(val);
        }

        /**
         * Helper function that extracts a single bit at bit_idx from the specified value, shifting it to dest_bit_idx
         * \param val Value to extract bit range from
         */
        template<size_t bit_idx, size_t dest_bit_idx, typename T>
        static constexpr T getBit(const T val) {
            return BitExtractor<T>::template Bit<bit_idx, dest_bit_idx>::get(val);
        }

        /**
         * Sign extends a value of Width bits to a DestT type
         * \param val Value to sign extend
         */
        template<size_t Width, typename DestT, typename T>
        static constexpr typename std::enable_if<sizeof(T) <= sizeof(DestT), DestT>::type
        signExtend(const T val) {
            struct Converter {
                DestT to_extend:Width;

                // Need to define a default constructor to silence a warning in older GCC versions
                // about an uninitialized member
                Converter() :
                    to_extend(0)
                {
                }
            } converter;
// There's an outstanding issue in GCC re: bitfields and -Wconversion
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=39170
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
            converter.to_extend = val;
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
            return converter.to_extend;
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
