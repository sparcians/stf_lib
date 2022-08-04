#ifndef __STF_FILTER_TYPES_HPP__
#define __STF_FILTER_TYPES_HPP__

#include <bitset>
#include "stf_descriptor.hpp"
#include "stf_exception.hpp"

namespace stf {
    /**
     * \class DummyFilter
     * \brief Filter that doesn't filter anything - used to implement the basic STFInstReader
     */
    class DummyFilter {
        private:
            using IntDescriptor = descriptors::internal::Descriptor;

        public:
            /**
             * Always returns false so that no records are filtered - ensures that the filter code
             * is optimized out in the implementation of STFInstReader
             * \param descriptor Descriptor type to check
             */
            __attribute__((always_inline))
            static inline bool isFiltered(const IntDescriptor descriptor) {
                (void)descriptor;
                return false;
            }
    };

    /**
     * \class RecordFilter
     * \brief Filter that allows exclusion based on descriptor value
     */
    class RecordFilter {
        private:
            using IntDescriptor = descriptors::internal::Descriptor;
            std::bitset<enums::size<IntDescriptor>()> ignored_records_;

        public:
            /**
             * Returns whether the descriptor should be excluded filtered
             * \param descriptor Descriptor type to check
             */
            __attribute__((always_inline))
            inline bool isFiltered(const IntDescriptor descriptor) {
                return ignored_records_.test(enums::to_int(descriptor));
            }

            /**
             * Sets all record types to be ignored, except for instruction opcode records
             */
            void ignoreAllRecords() {
                ignored_records_.set();
                keepRecordType(IntDescriptor::STF_INST_OPCODE16);
                keepRecordType(IntDescriptor::STF_INST_OPCODE32);
            }

            /**
             * Sets the specified record type to be ignored
             * \param type Descriptor type to ignore
             */
            void ignoreRecordType(const IntDescriptor type) {
                stf_assert(type != IntDescriptor::STF_INST_OPCODE16 &&
                           type != IntDescriptor::STF_INST_OPCODE32,
                           "STFInstReader can't ignore instruction opcode records");
                ignored_records_.set(enums::to_int(type));
            }

            /**
             * Sets the specified record type to be kept
             * \param type Descriptor type to keep
             */
            void keepRecordType(const IntDescriptor type) {
                ignored_records_.reset(enums::to_int(type));
            }
    };
} // end namespace stf

#endif
