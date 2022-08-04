#ifndef __STF_PC_TRACKER_HPP__
#define __STF_PC_TRACKER_HPP__

#include <cstdint>
#include <type_traits>

#include "stf_exception.hpp"

namespace stf {
    class STFRecord;
    template<typename T> class GenericPCTargetRecord;
    template<typename ClassT, typename OpcodeT> class GenericOpcodeRecord;
    class ForcePCRecord;
    class STFInst;

    /**
     * \class PCTracker
     * Tracks the instruction PC as we iterate over STFRecords
     */
    class PCTracker {
        private:
            uint64_t pc_ = 0; /**< Current PC */
            uint64_t next_pc_ = 0; /**< Next PC */
            uint64_t target_pc_ = 0; /**< Branch/exception/interrupt target PC */
            uint64_t pc_offset_ = 0; /**< PC offset */
            bool is_branch_ = false; /**< Last instruction was a branch/exception/interrupt */

            void setTargetPC_(const uint64_t pc) {
                target_pc_ = pc + pc_offset_;
                is_branch_ = true;
            }
            /**
             * Updates the target PC for branch-like records
             */
            template<typename RecType, typename STFRecordType>
            std::enable_if_t<std::is_same_v<STFRecordType, STFRecord>>
            setTargetPC_(const STFRecordType& rec) {
                setTargetPC_(rec.template as<RecType>().getAddr());
            }

        public:
            PCTracker() = default;

            /**
             * Constructs a PCTracker
             * \param pc Initial PC
             * \param offset Offset PCs by given value
             */
            explicit PCTracker(const uint64_t pc, const uint64_t offset = 0) :
                pc_(pc + offset),
                next_pc_(pc_),
                pc_offset_(offset)
            {
            }

            /**
             * Gets the current PC
             */
            uint64_t getPC() const {
                return pc_;
            }

            /**
             * Gets the next PC
             */
            uint64_t getNextPC() const {
                return next_pc_;
            }

            /**
             * Forces the PC to the given value
             * \param pc PC to set
             */
            void forcePC(uint64_t pc) {
                pc_ = pc;
                next_pc_ = pc;
            }

            /**
             * Inspects the record and tracks the PC accordingly
             * \param rec Record to inspect
             */
            void track(const ForcePCRecord& rec);

            /**
             * Inspects the record and tracks the PC accordingly
             * \param rec Record to inspect
             */
            template<typename T>
            void track(const GenericPCTargetRecord<T>& rec) {
                setTargetPC_(rec.getAddr());
            }

            /**
             * Inspects the record and tracks the PC accordingly
             * \param rec Record to inspect
             */
            template<typename ClassT, typename OpcodeT>
            void track(const GenericOpcodeRecord<ClassT, OpcodeT>& rec) {
                pc_ = next_pc_;

                if(STF_EXPECT_FALSE(is_branch_)) {
                    next_pc_ = target_pc_;
                    is_branch_ = false;
                }
                else {
                    next_pc_ += rec.getOpcodeSize();
                }
            }

            /**
             * Inspects an STFInst and tracks the PC accordingly
             * \param inst Instruction to inspect
             */
            void track(const STFInst& inst);
    };

} // end namespace stf

#endif
