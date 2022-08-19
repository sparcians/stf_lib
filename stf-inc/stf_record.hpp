#ifndef __STF_RECORD_HPP__
#define __STF_RECORD_HPP__

#include <memory>
#include <type_traits>

#include "stf_descriptor.hpp"
#include "stf_enums.hpp"
#include "stf_object.hpp"
#include "util.hpp"
#include "stf_pool.hpp"
#include "stf_factory.hpp"

namespace stf {
    /**
     * \class STFRecord
     *
     * Defines the main STF record data structure
     *
     */
    class STFRecord : public STFObject<STFRecord, descriptors::internal::Descriptor, descriptors::encoded::Descriptor> {
        public:
            /**
             * Constructs an STFRecord
             * \param desc Descriptor for the record
             */
            explicit STFRecord(const descriptors::internal::Descriptor desc) :
                STFObject(desc)
            {
            }

            /**
             * Returns whether this internal::Descriptor corresponds to an instruction record
             * \param desc internal::Descriptor to check
             */
            static constexpr bool isInstructionRecord(const descriptors::internal::Descriptor desc) {
                return (desc == descriptors::internal::Descriptor::STF_INST_OPCODE16) ||
                       (desc == descriptors::internal::Descriptor::STF_INST_OPCODE32);
            }

            /**
             * Returns whether this encoded::Descriptor corresponds to an instruction record
             * \param desc encoded::Descriptor to check
             */
            static constexpr bool isInstructionRecord(const descriptors::encoded::Descriptor desc) {
                return (desc == descriptors::encoded::Descriptor::STF_INST_OPCODE16) ||
                       (desc == descriptors::encoded::Descriptor::STF_INST_OPCODE32);
            }

            /**
             * Returns whether this is an instruction record
             */
            bool isInstructionRecord() const {
                return isInstructionRecord(getId());
            }
    };
} // end namespace stf

/**
 * \def REGISTER_RECORD
 *
 * Registers a new STF record type
 */
#define REGISTER_RECORD(cls) REGISTER_WITH_FACTORY(STFRecord, cls)

#endif
