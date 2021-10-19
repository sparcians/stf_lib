#ifndef __STF_BRANCH_HPP__
#define __STF_BRANCH_HPP__

#include "stf_exception.hpp"
#include "stf_item.hpp"

namespace stf {
    class STFBranchReader;
    class STFBranchDecoder;

    namespace delegates {
        class STFBranchDelegate;
    };

    /**
     * \class STFBranch
     * \brief Branch information returned by the STFBranchReader
     */
    class STFBranch : public STFSkippableItem {
        private:
            friend class delegates::STFBranchDelegate;

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
            __attribute__((always_inline))
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

            /**
             * Resets branch info to initial state (used by STFBranchReader)
             */
            __attribute__((always_inline))
            inline void reset_() {
                STFSkippableItem::reset_();
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

            /**
             * Sets the branch to taken
             * \param target Target address
             */
            __attribute__((always_inline))
            inline void setTaken_(const uint64_t target) {
                taken_ = true;
                target_ = target;
            }

            /**
             * Sets the target opcode
             * \param target_opcode Target opcode value
             */
            __attribute__((always_inline))
            inline void setTargetOpcode_(const uint32_t target_opcode) {
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
            __attribute__((always_inline))
            inline void setInfo_(const uint64_t pc,
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
    };

    namespace delegates {
        /**
         * \class STFBranchDelegate
         * Delegate class used to hide any non-const methods from non-reader classes
         */
        class STFBranchDelegate : public STFSkippableItemDelegate {
            private:
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
                __attribute__((always_inline))
                static inline void setInfo_(STFBranch& branch,
                                            const uint64_t pc,
                                            const uint64_t target,
                                            const uint32_t opcode,
                                            const bool is_conditional,
                                            const bool is_call,
                                            const bool is_return,
                                            const bool is_indirect) {
                    branch.setInfo_(pc,
                                    target,
                                    opcode,
                                    is_conditional,
                                    is_call,
                                    is_return,
                                    is_indirect);
                }

                /**
                 * Resets branch info to initial state
                 * \param branch Branch to reset
                 */
                __attribute__((always_inline))
                static inline void reset_(STFBranch& branch) {
                    branch.reset_();
                }

                /**
                 * Sets the branch to taken
                 * \param branch Branch to modify
                 * \param target Target address
                 */
                __attribute__((always_inline))
                static inline void setTaken_(STFBranch& branch, const uint64_t target) {
                    branch.setTaken_(target);
                }

                /**
                 * Sets the target opcode
                 * \param branch Branch to modify
                 * \param target_opcode Target opcode value
                 */
                __attribute__((always_inline))
                static inline void setTargetOpcode_(STFBranch& branch, const uint32_t opcode) {
                    branch.setTargetOpcode_(opcode);
                }

                friend class stf::STFBranchReader;
                friend class stf::STFBranchDecoder;
        };
    } // end namespace delegates
} // end namespace stf

#endif
