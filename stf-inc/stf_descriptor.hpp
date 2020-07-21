#ifndef __STF_DESCRIPTOR_HPP__
#define __STF_DESCRIPTOR_HPP__

#include <array>
#include <cstdint>
#include <ostream>
#include <iterator>
#include <stdexcept>

#include "stf_enum_utils.hpp"
#include "stf_exception.hpp"

namespace stf::descriptors {
    namespace encoded {
        /**
         * \enum Descriptor
         *
         * The descriptor for each STF record as it is encoded in the STF format
         *
         */
        enum class Descriptor : uint8_t {
            STF_RESERVED                    = 0,            /**< Reserved for error detection */
            STF_IDENTIFIER                  = 1,            /**< Indicates start of an STF file */
            STF_VERSION                     = 2,            /**< Trace format version */
            STF_COMMENT                     = 3,            /**< Comment */
            STF_ISA                         = 4,            /**< ISA family */
            STF_INST_IEM                    = 5,            /**< Instruction encoding mode */
            STF_TRACE_INFO                  = 6,            /**< Trace generator */
            STF_TRACE_INFO_FEATURE          = 7,            /**< Trace features */
            STF_PROCESS_ID_EXT              = 8,            /**< Process ID EXTENSION */
            STF_FORCE_PC                    = 9,            /**< Initial PC and trace stitching */

            STF_END_HEADER                  = 19,           /**< End of header record */

            STF_INST_PC_TARGET              = 31,           /**< Inst generated PC target address */

            STF_INST_REG                    = 40,           /**< Inst operand register number and content */
            STF_INST_READY_REG              = 41,           /**< Mark operand as ready */

            STF_PAGE_TABLE_WALK             = 50,           /**< Page table walk */

            STF_INST_MEM_ACCESS             = 60,           /**< Inst generated memory access */
            STF_INST_MEM_CONTENT            = 61,           /**< Inst load/store data */
            STF_BUS_MASTER_ACCESS           = 62,           /**< Memory access attributes (size, source, attr) */
            STF_BUS_MASTER_CONTENT          = 63,           /**< Bus master read/write data */

            STF_EVENT                       = 100,          /**< Event */
            STF_EVENT_PC_TARGET             = 101,          /**< Event generated PC target address */

            STF_INST_MICROOP                = 230,          /**< Micro Op */

            STF_INST_OPCODE32               = 240,          /**< 4 byte instruction */
            STF_INST_OPCODE16               = 241,          /**< 2 byte instruction */

            STF_RESERVED_END                                /**< Reserved for error detection, end of descriptor  */
        };

        static constexpr size_t NUM_DESCRIPTORS = static_cast<size_t>(Descriptor::STF_RESERVED_END) + 1; /**< Specifies the array size needed to hold every descriptor */

        /**
         * Writes a string representation of a Descriptor enum to an ostream
         */
        std::ostream& operator<<(std::ostream& os, Descriptor desc);

    } // end namespace encoded

    namespace internal {
        /**
         * \enum Descriptor
         *
         * The descriptor for each STF record - this is the *internal* library encoding
         *
         */
        enum class Descriptor : enums::int_t<encoded::Descriptor> {
            STF_INST_REG,
            STF_INST_OPCODE16,
            STF_INST_OPCODE32,
            STF_INST_MEM_ACCESS,
            STF_INST_MEM_CONTENT,
            STF_INST_PC_TARGET,
            STF_EVENT,
            STF_EVENT_PC_TARGET,
            STF_PAGE_TABLE_WALK,
            STF_BUS_MASTER_ACCESS,
            STF_BUS_MASTER_CONTENT,
            STF_COMMENT,
            STF_FORCE_PC,
            STF_INST_READY_REG,
            STF_PROCESS_ID_EXT,
            STF_INST_MICROOP,
            STF_IDENTIFIER,
            STF_ISA,
            STF_INST_IEM,
            STF_TRACE_INFO,
            STF_TRACE_INFO_FEATURE,
            STF_VERSION,
            STF_END_HEADER,
            STF_RESERVED,
            STF_RESERVED_END // MUST ALWAYS BE AT THE END
        };

        static constexpr size_t NUM_DESCRIPTORS = static_cast<size_t>(Descriptor::STF_RESERVED_END) + 1; /**< Specifies the array size needed to hold every descriptor */

        /**
         * Writes a string representation of a Descriptor enum to an ostream
         */
        std::ostream& operator<<(std::ostream& os, Descriptor desc);

    } // end namespace internal

    namespace iterators::__sequences {
        /**
         * \typedef SortedInternalSequence
         * Array that provides a sequence of internal::Descriptors in the same order they appear in encoded::Descriptor
         */
        using SortedInternalSequence = std::array<internal::Descriptor, internal::NUM_DESCRIPTORS>;

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
                static constexpr size_t NUM_DESCRIPTORS_ = static_cast<size_t>(FromDescriptor::STF_RESERVED_END) + 1; /**< Specifies the array size needed to hold every FromDescriptor value */

                using ArrayType = std::array<ToDescriptor, NUM_DESCRIPTORS_>; /**< Array that maps FromDescriptor values to ToDescriptor values */

                /**
                 * \def INIT_DESC_ARRAY
                 * Convenience macro that adds the specified Descriptor value to the array
                 */
                #define INIT_DESC_ARRAY(x) case FromDescriptor::x: newarr[i] = ToDescriptor::x; continue;
                /**
                 * Initializes the mapping array at compile time
                 */
                static constexpr ArrayType initDescArray_() {
                    ArrayType newarr = {ToDescriptor::STF_RESERVED};

                    for(enums::int_t<FromDescriptor> i = 0; i < NUM_DESCRIPTORS_; ++i) {
                        // Using a switch statement here ensures that we get a compile error
                        // if we add a new Descriptor and forget to update this function
                        switch(FromDescriptor(i)) {
                            INIT_DESC_ARRAY(STF_RESERVED)
                            INIT_DESC_ARRAY(STF_IDENTIFIER)
                            INIT_DESC_ARRAY(STF_VERSION)
                            INIT_DESC_ARRAY(STF_COMMENT)
                            INIT_DESC_ARRAY(STF_ISA)
                            INIT_DESC_ARRAY(STF_INST_IEM)
                            INIT_DESC_ARRAY(STF_TRACE_INFO)
                            INIT_DESC_ARRAY(STF_TRACE_INFO_FEATURE)
                            INIT_DESC_ARRAY(STF_PROCESS_ID_EXT)
                            INIT_DESC_ARRAY(STF_FORCE_PC)
                            INIT_DESC_ARRAY(STF_END_HEADER)
                            INIT_DESC_ARRAY(STF_INST_PC_TARGET)
                            INIT_DESC_ARRAY(STF_INST_REG)
                            INIT_DESC_ARRAY(STF_INST_READY_REG)
                            INIT_DESC_ARRAY(STF_PAGE_TABLE_WALK)
                            INIT_DESC_ARRAY(STF_INST_MEM_ACCESS)
                            INIT_DESC_ARRAY(STF_INST_MEM_CONTENT)
                            INIT_DESC_ARRAY(STF_BUS_MASTER_ACCESS)
                            INIT_DESC_ARRAY(STF_BUS_MASTER_CONTENT)
                            INIT_DESC_ARRAY(STF_EVENT)
                            INIT_DESC_ARRAY(STF_EVENT_PC_TARGET)
                            INIT_DESC_ARRAY(STF_INST_MICROOP)
                            INIT_DESC_ARRAY(STF_INST_OPCODE32)
                            INIT_DESC_ARRAY(STF_INST_OPCODE16)
                            INIT_DESC_ARRAY(STF_RESERVED_END)
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
                    if(static_cast<size_t>(d) == encoded::NUM_DESCRIPTORS) {
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
            using SortedInternalSequenceReverseMapping = std::array<size_t, internal::NUM_DESCRIPTORS>;

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
                explicit constexpr sorted_internal_iterator(const size_t idx) :
                    sorted_internal_iterator(static_cast<difference_type>(idx))
                {
                }

                /**
                 * Constructs a sorted_internal_iterator
                 * \param start New iterator will start at the specified index
                 */
                explicit constexpr sorted_internal_iterator(const difference_type idx) :
                    it_(std::next(sorted_sequence_.begin(), idx))
                {
                }

            public:
                /**
                 * Constructs a sorted_internal_iterator
                 * \param is_end If true, constructs an end iterator. Otherwise, constructs a begin iterator.
                 */
                explicit constexpr sorted_internal_iterator(const bool is_end = false) :
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
} // end namespace stf::descriptors

#endif
