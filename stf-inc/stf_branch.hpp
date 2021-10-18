#ifndef __STF_BRANCH_HPP__
#define __STF_BRANCH_HPP__

#include "stf_exception.hpp"
#include "stf_item.hpp"

namespace stf {
    /**
     * \class STFBranch
     * \brief Branch information returned by the STFBranchReader
     */
    class STFBranch : public STFItem {
        private:
            uint64_t pc_ = 0;
            uint64_t target_ = 0;
            uint32_t opcode_ = 0;
            uint32_t target_opcode_ = 0;
            bool taken_ = false;
            bool conditional_ = false;
            bool call_ = false;
            bool return_ = false;
            bool indirect_ = false;

            /**
             * Sanity checks branch target addresses
             * \param target Target address computed by STFBranchDecoder
             * \param is_indirect If true, branch is an indirect
             */
            inline uint64_t validateBranchTarget_(const uint64_t target, const bool is_indirect) const {
                if(is_indirect) {
                    // Indirect branches must get their targets from the trace
                    stf_assert(target_ != 0, "Indirect branch with invalid target: pc = " << std::hex << pc_);
                    return target_;
                }
                // Otherwise confirm that the computed target is the same as the trace-provided target
                stf_assert(!taken_ || target_ == target,
                           "Branch target mismatch: pc = " << std::hex << pc_ <<
                           " trace = " << target_ << " calculated = " << target);

                return target;
            }

        public:
            /**
             * Gets whether a branch is taken
             */
            inline bool isTaken() const {
                return taken_;
            }

            /**
             * Gets branch PC
             */
            inline uint64_t getPC() const {
                return pc_;
            }

            /**
             * Gets branch target PC
             */
            inline uint64_t getTargetPC() const {
                return target_;
            }

            /**
             * Gets branch opcode
             */
            inline uint32_t getOpcode() const {
                return opcode_;
            }

            /**
             * Gets branch target opcode
             */
            inline uint32_t getTargetOpcode() const {
                return target_opcode_;
            }

            /**
             * Gets whether the branch is an indirect
             */
            inline bool isIndirect() const {
                return indirect_;
            }

            /**
             * Gets whether the branch is a call
             */
            inline bool isCall() const {
                return call_;
            }

            /**
             * Gets whether the branch is a return
             */
            inline bool isReturn() const {
                return return_;
            }

            /**
             * Gets whether the branch is conditional
             */
            inline bool isConditional() const {
                return conditional_;
            }

            /**
             * Gets whether the branch target PC is lower than the branch PC
             * Useful for detecting loop branches
             */
            inline bool isBackwards() const {
                return target_ <= pc_;
            }

            /**
             * Sets the index
             * \param index Index value to set
             */
            inline void setIndex(const uint64_t index) {
                index_ = index;
            }

            /**
             * Sets the taken value
             * \param taken If true, the branch is taken
             */
            inline void setTaken(const bool taken) {
                taken_ = taken;
            }

            /**
             * Sets the target PC
             * \param target Target PC value
             */
            inline void setTarget(const uint64_t target) {
                target_ = target;
            }

            /**
             * Sets the target opcode
             * \param target_opcode Target opcode value
             */
            inline void setTargetOpcode(const uint32_t target_opcode) {
                target_opcode_ = target_opcode;
            }

            /**
             * Sets branch info (used by STFBranchReader)
             * \param pc Branch PC
             * \param target Branch target PC
             * \param opcode Branch opcode
             * \param is_conditional If true, branch is conditional
             * \param is_call If true, branch is a call
             * \param is_return If true, branch is a return
             * \param is_indirect If true, branch is indirect
             */
            inline void setInfo(const uint64_t pc,
                                const uint64_t target,
                                const uint32_t opcode,
                                const bool is_conditional,
                                const bool is_call,
                                const bool is_return,
                                const bool is_indirect) {
                stf_assert(!(is_conditional && is_indirect), "Indirect branches cannot be conditional");
                stf_assert(!(is_conditional && is_call), "Calls cannot be conditional");
                stf_assert(!(is_conditional && is_return), "Returns cannot be conditional");

                pc_ = pc;
                target_ = validateBranchTarget_(target, is_indirect);
                opcode_ = opcode;
                conditional_ = is_conditional;
                call_ = is_call;
                return_ = is_return;
                indirect_ = is_indirect;
            }

            /**
             * Resets branch info to initial state (used by STFBranchReader)
             */
            inline void reset() {
                STFItem::reset_();
                pc_ = 0;
                target_ = 0;
                opcode_ = 0;
                target_opcode_ = 0;
                taken_ = false;
                conditional_ = false;
                call_ = false;
                return_ = false;
                indirect_ = false;
            }
    };
} // end namespace stf

#endif
