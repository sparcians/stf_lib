#ifndef __STF_DESCRIPTOR_HPP__
#define __STF_DESCRIPTOR_HPP__

#include <array>
#include <cstdint>
#include <ostream>
#include <iterator>
#include <stdexcept>

#include "stf_enum_utils.hpp"
#include "stf_exception.hpp"
#include "stf_object_id.hpp"

namespace stf {
    namespace descriptors {
        namespace encoded {
            /**
             * \enum Descriptor
             *
             * The descriptor for each STF record as it is encoded in the STF format
             *
             */
            STF_ENUM(STF_ENUM_CONFIG(OVERRIDE_START),
                Descriptor,
                uint8_t,
                STF_RESERVED,
                STF_ENUM_VAL(STF_IDENTIFIER,              1),    /**< Indicates start of an STF file */
                STF_ENUM_VAL(STF_VERSION,                 2),    /**< Trace format version */
                STF_ENUM_VAL(STF_COMMENT,                 3),    /**< Comment */
                STF_ENUM_VAL(STF_ISA,                     4),    /**< ISA family */
                STF_ENUM_VAL(STF_INST_IEM,                5),    /**< Instruction encoding mode */
                STF_ENUM_VAL(STF_TRACE_INFO,              6),    /**< Trace generator */
                STF_ENUM_VAL(STF_TRACE_INFO_FEATURE,      7),    /**< Trace features */
                STF_ENUM_VAL(STF_PROCESS_ID_EXT,          8),    /**< Process ID EXTENSION */
                STF_ENUM_VAL(STF_FORCE_PC,                9),    /**< Initial PC and trace stitching */
                STF_ENUM_VAL(STF_VLEN_CONFIG,            10),    /**< Sets the vector vlen parameter */
                STF_ENUM_VAL(STF_PROTOCOL_ID,            11),    /**< Sets the protocol encoded in a transaction trace */
                STF_ENUM_VAL(STF_CLOCK_ID,               12),    /**< Adds a clock domain ID to a transaction trace */

                STF_ENUM_VAL(STF_END_HEADER,             19),    /**< End of header record */

                STF_ENUM_VAL(STF_INST_PC_TARGET,         31),    /**< Inst generated PC target address */

                STF_ENUM_VAL(STF_INST_REG,               40),    /**< Inst operand register number and content */
                STF_ENUM_VAL(STF_INST_READY_REG,         41),    /**< Mark operand as ready */

                STF_ENUM_VAL(STF_PAGE_TABLE_WALK,        50),    /**< Page table walk */

                STF_ENUM_VAL(STF_INST_MEM_ACCESS,        60),    /**< Inst generated memory access */
                STF_ENUM_VAL(STF_INST_MEM_CONTENT,       61),    /**< Inst load/store data */
                STF_ENUM_VAL(STF_BUS_MASTER_ACCESS,      62),    /**< Memory access attributes (size, source, attr) */
                STF_ENUM_VAL(STF_BUS_MASTER_CONTENT,     63),    /**< Bus master read/write data */

                STF_ENUM_VAL(STF_EVENT,                  100),   /**< Event */
                STF_ENUM_VAL(STF_EVENT_PC_TARGET,        101),   /**< Event generated PC target address */

                STF_ENUM_VAL(STF_INST_MICROOP,           230),   /**< Micro Op */

                STF_ENUM_VAL(STF_INST_OPCODE32,          240),   /**< 4 byte instruction */
                STF_ENUM_VAL(STF_INST_OPCODE16,          241),   /**< 2 byte instruction */

                STF_ENUM_VAL(STF_TRANSACTION,            250),
                STF_ENUM_VAL(STF_TRANSACTION_DEPENDENCY, 251)
            );
        } // end namespace encoded

        namespace internal {
            /**
             * \enum Descriptor
             *
             * The descriptor for each STF record - this is the *internal* library encoding
             *
             */
            STF_ENUM(
                STF_ENUM_CONFIG(AUTO_PRINT, ALLOW_UNKNOWN, OVERRIDE_START),
                Descriptor,
                enums::int_t<encoded::Descriptor>,
                STF_RESERVED,
                STF_ENUM_STR(STF_INST_REG, "INST_REG"),
                STF_ENUM_STR(STF_INST_OPCODE16, "INST_OPCODE16"),
                STF_ENUM_STR(STF_INST_OPCODE32, "INST_OPCODE32"),
                STF_ENUM_STR(STF_INST_MEM_ACCESS, "INST_MEM_ACCESS"),
                STF_ENUM_STR(STF_INST_MEM_CONTENT, "INST_MEM_CONTENT"),
                STF_ENUM_STR(STF_INST_PC_TARGET, "INST_PC_TARGET"),
                STF_ENUM_STR(STF_EVENT, "EVENT"),
                STF_ENUM_STR(STF_EVENT_PC_TARGET, "EVENT_PC_TARGET"),
                STF_ENUM_STR(STF_PAGE_TABLE_WALK, "PAGE_TABLE_WALK"),
                STF_ENUM_STR(STF_BUS_MASTER_ACCESS, "BUS_MASTER_ACCESS"),
                STF_ENUM_STR(STF_BUS_MASTER_CONTENT, "BUS_MASTER_CONTENT"),
                STF_ENUM_STR(STF_COMMENT, "COMMENT"),
                STF_ENUM_STR(STF_FORCE_PC, "FORCE_PC"),
                STF_ENUM_STR(STF_INST_READY_REG, "INST_READY_REG"),
                STF_ENUM_STR(STF_PROCESS_ID_EXT, "PROCESS_ID_EXT"),
                STF_ENUM_STR(STF_INST_MICROOP, "INST_MICROOP"),
                STF_IDENTIFIER,
                STF_ENUM_STR(STF_ISA, "ISA"),
                STF_ENUM_STR(STF_INST_IEM, "INST_IEM"),
                STF_ENUM_STR(STF_TRACE_INFO, "TRACE_INFO"),
                STF_ENUM_STR(STF_TRACE_INFO_FEATURE, "TRACE_INFO_FEATURE"),
                STF_ENUM_STR(STF_VERSION, "VERSION"),
                STF_ENUM_STR(STF_VLEN_CONFIG, "VLEN_CONFIG"),
                STF_ENUM_STR(STF_PROTOCOL_ID, "PROTOCOL_ID"),
                STF_ENUM_STR(STF_CLOCK_ID, "CLOCK_ID"),
                STF_ENUM_STR(STF_END_HEADER, "END_HEADER"),
                STF_ENUM_STR(STF_TRANSACTION, "TRANSACTION"),
                STF_ENUM_STR(STF_TRANSACTION_DEPENDENCY, "TRANSACTION_DEPENDENCY")
            );
        } // end namespace internal

        namespace iterators::__sequences {
            /**
             * \typedef SortedInternalSequence
             * Array that provides a sequence of internal::Descriptors in the same order they appear in encoded::Descriptor
             */
            using SortedInternalSequence = enums::EnumArray<internal::Descriptor, internal::Descriptor>;

            static constexpr SortedInternalSequence initSortedInternalSequence();
        } // end namespace iterators::__sequences

        namespace conversion {

            /**
             * \class DescriptorConverter
             *
             * Class that can be used to convert between different Descriptor encodings
             */
            template<typename FromDescriptor, typename ToDescriptor>
            class DescriptorConverter {
                friend constexpr iterators::__sequences::SortedInternalSequence iterators::__sequences::initSortedInternalSequence();

                private:
                    static constexpr size_t NUM_DESCRIPTORS_ = enums::size<FromDescriptor>(); /**< Specifies the array size needed to hold every FromDescriptor value */

                    using ArrayType = enums::EnumArray<ToDescriptor, FromDescriptor>; /**< Array that maps FromDescriptor values to ToDescriptor values */

                    #define _INIT_DESC_ARRAY_ENTRY(r, data, elem) \
                        case FromDescriptor::elem: newarr[i] = ToDescriptor::elem; continue;

                    /**
                     * \def INIT_DESC_ARRAY
                     * Convenience macro that auto-initializes the array with the specified Descriptor values
                     */
                    #define INIT_DESC_ARRAY(...) \
                        BOOST_PP_SEQ_FOR_EACH( \
                            _INIT_DESC_ARRAY_ENTRY, \
                            _, \
                            BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__) \
                        )

                    /**
                     * Initializes the mapping array at compile time
                     */
                    static constexpr ArrayType initDescArray_() { // cppcheck-suppress unusedPrivateFunction
                        ArrayType newarr = {ToDescriptor::STF_RESERVED};

                        for(enums::int_t<FromDescriptor> i = 0; i < NUM_DESCRIPTORS_; ++i) {
                            // Using a switch statement here ensures that we get a compile error
                            // if we add a new Descriptor and forget to update this function
                            switch(FromDescriptor(i)) {
                                INIT_DESC_ARRAY(
                                    STF_RESERVED,
                                    STF_IDENTIFIER,
                                    STF_VERSION,
                                    STF_COMMENT,
                                    STF_ISA,
                                    STF_INST_IEM,
                                    STF_TRACE_INFO,
                                    STF_TRACE_INFO_FEATURE,
                                    STF_PROCESS_ID_EXT,
                                    STF_FORCE_PC,
                                    STF_VLEN_CONFIG,
                                    STF_PROTOCOL_ID,
                                    STF_CLOCK_ID,
                                    STF_END_HEADER,
                                    STF_INST_PC_TARGET,
                                    STF_INST_REG,
                                    STF_INST_READY_REG,
                                    STF_PAGE_TABLE_WALK,
                                    STF_INST_MEM_ACCESS,
                                    STF_INST_MEM_CONTENT,
                                    STF_BUS_MASTER_ACCESS,
                                    STF_BUS_MASTER_CONTENT,
                                    STF_EVENT,
                                    STF_EVENT_PC_TARGET,
                                    STF_INST_MICROOP,
                                    STF_INST_OPCODE32,
                                    STF_INST_OPCODE16,
                                    STF_TRANSACTION,
                                    STF_TRANSACTION_DEPENDENCY,
                                    __RESERVED_END
                                )
                            };

                            // Use NUM_DESCRIPTORS_ as the default value for Descriptor enums with gaps
                            // in their defined values
                            newarr[i] = static_cast<ToDescriptor>(NUM_DESCRIPTORS_);
                        }

                        return newarr;
                    }

                    static constexpr ArrayType ARRAY_ = initDescArray_(); /**< Lookup table for translating between Descriptor encodings */

                public:
                    /**
                     * Converts from a FromDescriptor to its corresponding ToDescriptor value
                     * \param d FromDescriptor value to convert
                     */
                    static constexpr ToDescriptor convert(const FromDescriptor d) {
                        return ARRAY_.at(static_cast<size_t>(d));
                    }
            };

            /**
             * Converts from an encoded::Descriptor to an internal::Descriptor
             * \param d encoded::Descriptor to convert
             */
            static constexpr auto toInternal(const encoded::Descriptor d) {
                return DescriptorConverter<encoded::Descriptor, internal::Descriptor>::convert(d);
            }

            /**
             * Converts from an internal::Descriptor to an encoded::Descriptor
             * \param d internal::Descriptor to convert
             */
            static constexpr auto toEncoded(const internal::Descriptor d) {
                return DescriptorConverter<internal::Descriptor, encoded::Descriptor>::convert(d);
            }

            /**
             * Returns whether lhs comes before rhs in encoded::Descriptor order
             * \param rhs first internal::Descriptor to compare
             * \param lhs second internal::Descriptor to compare
             */
            static constexpr bool encodedCompare(const internal::Descriptor lhs, const internal::Descriptor rhs) {
                return toEncoded(lhs) < toEncoded(rhs);
            }

            /**
             * Returns whether lhs comes after rhs in encoded::Descriptor order
             * \param rhs first internal::Descriptor to compare
             * \param lhs second internal::Descriptor to compare
             */
            static constexpr bool reverseEncodedCompare(const internal::Descriptor lhs, const internal::Descriptor rhs) {
                return toEncoded(lhs) > toEncoded(rhs);
            }

        } // end namespace conversion

        namespace iterators {
            namespace __sequences {
                /**
                 * Generates a sequence of internal::Descriptor values in the order they appear in encoded::Descriptor
                 */
                static constexpr SortedInternalSequence initSortedInternalSequence() {
                    SortedInternalSequence seq = {internal::Descriptor::STF_RESERVED};
                    size_t i = 0;
                    for(const auto d: conversion::DescriptorConverter<encoded::Descriptor, internal::Descriptor>::ARRAY_) {
                        // Skip over gaps
                        if(static_cast<size_t>(d) == enums::size<encoded::Descriptor>()) {
                            continue;
                        }
                        seq[i++] = d;
                    }
                    return seq;
                }

                /**
                 * \typedef SortedInternalSequenceReverseMapping
                 * Maps internal::Descriptor values to their corresponding index in a SortedInternalSequence.
                 * Allows us to initialize a sorted_internal_iterator to an arbitrary value without having to
                 * iterate over a SortedInternalSequence.
                 */
                using SortedInternalSequenceReverseMapping = enums::EnumArray<size_t, internal::Descriptor>;

                /**
                 * Generates a map of internal::Descriptor values to their corresponding index in a SortedInternalSequence
                 */
                static constexpr SortedInternalSequenceReverseMapping initSortedInternalSequenceReverseMapping() {
                    constexpr SortedInternalSequence sorted_seq = initSortedInternalSequence();
                    SortedInternalSequenceReverseMapping seq = {0};
                    for(size_t i = 0; i < sorted_seq.size(); ++i) {
                        seq[static_cast<size_t>(sorted_seq[i])] = i;
                    }
                    return seq;
                }
            } // end namespace __sequences

            /**
             * \class sorted_internal_iterator
             *
             * Iterates over internal::Descriptor values in the order they appear in encoded::Descriptor
             */
            class sorted_internal_iterator : public std::random_access_iterator_tag {
                private:
                    /**
                     * \typedef sequence_t
                     * Sequence type that provides a sorted view of internal::Descriptor values
                     */
                    using sequence_t = __sequences::SortedInternalSequence;

                    /**
                     * \typedef reverse_sequence_t
                     * Type that maps internal::Descriptor values to their corresponding index in a sequence_t
                     */
                    using reverse_sequence_t = __sequences::SortedInternalSequenceReverseMapping;

                    static constexpr sequence_t sorted_sequence_ = __sequences::initSortedInternalSequence(); /**< Holds pre-sorted internal::Descriptor values */
                    // cppcheck-suppress unusedStructMember
                    static constexpr reverse_sequence_t sorted_sequence_reverse_mapping_ =
                        __sequences::initSortedInternalSequenceReverseMapping(); /**< Maps internal::Descriptor values to their position in sorted_sequence_ */

                    sequence_t::const_iterator it_ = sorted_sequence_.begin(); /**< Iterator that generates pre-sorted internal::Descriptor values */

                public:
                    /**
                     * \typedef value_type
                     * Type pointed to by this iterator
                     */
                    using value_type = sequence_t::value_type;

                    /**
                     * \typedef difference_type
                     * Type used to represent the difference between two iterators
                     */
                    using difference_type = sequence_t::difference_type;

                    /**
                     * \typedef reference
                     * Reference to a value_type
                     */
                    using reference = sequence_t::reference;

                private:
                    /**
                     * Constructs a sorted_internal_iterator
                     * \param start New iterator will start at the specified index
                     */
                    constexpr explicit sorted_internal_iterator(const size_t idx) :
                        sorted_internal_iterator(static_cast<difference_type>(idx))
                    {
                    }

                    /**
                     * Constructs a sorted_internal_iterator
                     * \param start New iterator will start at the specified index
                     */
                    constexpr explicit sorted_internal_iterator(const difference_type idx) :
                        it_(std::next(sorted_sequence_.begin(), idx))
                    {
                    }

                public:
                    /**
                     * Constructs a sorted_internal_iterator
                     * \param is_end If true, constructs an end iterator. Otherwise, constructs a begin iterator.
                     */
                    constexpr explicit sorted_internal_iterator(const bool is_end = false) :
                        it_(is_end ? sorted_sequence_.end() : sorted_sequence_.begin())
                    {
                    }

                    /**
                     * Prefix increment operator
                     */
                    constexpr auto& operator++() {
                        ++it_;
                        return *this;
                    }

                    /**
                     * Postfix increment operator
                     */
                    constexpr auto operator++(int) {
                        auto copy = *this;
                        ++(*this);
                        return copy;
                    }

                    /**
                     * Prefix decrement operator
                     */
                    constexpr auto& operator--() {
                        --it_;
                        return *this;
                    }

                    /**
                     * Postfix decrement operator
                     */
                    constexpr auto operator--(int) {
                        auto copy = *this;
                        --(*this);
                        return copy;
                    }

                    /**
                     * Dereference operator
                     */
                    constexpr auto operator*() const {
                        return *it_;
                    }

                    /**
                     * Index operator
                     */
                    constexpr auto operator[](const size_t idx) {
                        return it_[idx];
                    }

                    /**
                     * Dereference operator
                     */
                    constexpr auto& operator->() const {
                        return it_;
                    }

                    /**
                     * Equality operator
                     */
                    constexpr bool operator==(const sorted_internal_iterator& rhs) {
                        return it_ == rhs.it_;
                    }

                    /**
                     * Inequality operator
                     */
                    constexpr bool operator!=(const sorted_internal_iterator& rhs) {
                        return it_ != rhs.it_;
                    }

                    /**
                     * Less-than operator
                     */
                    constexpr bool operator<(const sorted_internal_iterator& rhs) {
                        return it_ < rhs.it_;
                    }

                    /**
                     * Less-than-or-equal operator
                     */
                    constexpr bool operator<=(const sorted_internal_iterator& rhs) {
                        return it_ <= rhs.it_;
                    }

                    /**
                     * Greater-than operator
                     */
                    constexpr bool operator>(const sorted_internal_iterator& rhs) {
                        return it_ > rhs.it_;
                    }

                    /**
                     * Greater-than-or-equal operator
                     */
                    constexpr bool operator>=(const sorted_internal_iterator& rhs) {
                        return it_ >= rhs.it_;
                    }
            };

            static constexpr sorted_internal_iterator SORTED_INTERNAL_ITERATOR_END = sorted_internal_iterator(true);

        } // end namespace iterators

        namespace encoded {
            inline std::ostream& operator<<(std::ostream& os, const Descriptor desc) {
                return os << conversion::toInternal(desc);
            }
        } // end namespace encoded
    } // end namespace descriptors

    /**
     * Converts a descriptors::encoded::Descriptor value to a descriptors::internal::Descriptor value
     *
     * \param val Value to convert
     */
    template<>
    inline constexpr auto ObjectIdConverter::fromTrace(const descriptors::encoded::Descriptor val) {
        return descriptors::conversion::toInternal(val);
    }

    /**
     * Converts a descriptors::internal::Descriptor value to a descriptors::encoded::Descriptor value
     *
     * \param val Value to convert
     */
    template<>
    inline constexpr auto ObjectIdConverter::toTrace(const descriptors::internal::Descriptor val) {
        return descriptors::conversion::toEncoded(val);
    }
} // end namespace stf

#endif
