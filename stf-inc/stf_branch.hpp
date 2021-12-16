#ifndef __STF_BRANCH_HPP__
#define __STF_BRANCH_HPP__

#include <array>
#include <ostream>
#include "stf_exception.hpp"
#include "stf_item.hpp"
#include "stf_reg_def.hpp"

namespace stf {
    class STFBranchReader;
    class STFBranchDecoder;

    namespace delegates {
        class STFBranchDelegate;
    } // end namespace delegates

    /**
     * \class STFBranch
     * \brief Branch information returned by the STFBranchReader
     */
    class STFBranch : public STFSkippableItem {
        public:
            /**
             * \class OperandMap
             * \brief Maps register numbers to their values. Only accepts up to 2 register values.
             */
            class OperandMap {
                private:
                    struct RegPair {
                        Registers::STF_REG reg;
                        uint64_t val;
                    };

                    size_t wr_index_ = 0;
                    std::array<RegPair, 2> regs_ {{
                        {Registers::STF_REG::STF_REG_INVALID, 0},
                        {Registers::STF_REG::STF_REG_INVALID, 0}
                    }};

                public:
                    /**
                     * Add an operand to the map. Up to 2 operands can be stored.
                     * \param reg_num Register number of the operand
                     * \param val Operand value
                     */
                    inline void addOperand(const Registers::STF_REG reg_num, const uint64_t val) {
                        stf_assert(wr_index_ < 2, "Attempted to add a third source register!");
                        regs_[wr_index_] = {reg_num, val};
                        ++wr_index_;
                    }

                    /**
                     * Get an operand from the map.
                     * \param reg_num Register number of the operand
                     */
                    inline uint64_t getOperand(const Registers::STF_REG reg_num) const {
                        stf_assert(reg_num != Registers::STF_REG::STF_REG_INVALID,
                                   "Attempted to read an invalid register");

                        for(const auto& reg: regs_) {
                            if(reg.reg == reg_num) {
                                return reg.val;
                            }
                        }

                        stf_throw("Register " << reg_num << " not present in the map");
                    }

                    /**
                     * Clear the map.
                     */
                    inline void clear() {
                        wr_index_ = 0;
                        for(auto& reg: regs_) {
                            reg = {Registers::STF_REG::STF_REG_INVALID, 0};
                        }
                    }

                    /**
                     * Get the number of operands in the map.
                     */
                    inline size_t size() const {
                        return wr_index_;
                    }
            };

        private:
            friend class delegates::STFBranchDelegate;

            uint64_t pc_ = 0;
            uint64_t target_ = 0;
            uint32_t opcode_ = 0;
            uint32_t target_opcode_ = 0;
            Registers::STF_REG rs1_ = Registers::STF_REG::STF_REG_INVALID;
            Registers::STF_REG rs2_ = Registers::STF_REG::STF_REG_INVALID;
            uint64_t rs1_value_ = 0;
            uint64_t rs2_value_ = 0;
            bool taken_ = false;
            bool conditional_ = false;
            bool call_ = false;
            bool return_ = false;
            bool indirect_ = false;
            bool compare_eq_ = false;
            bool compare_not_eq_ = false;
            bool compare_greater_than_or_equal_ = false;
            bool compare_less_than_ = false;
            bool compare_unsigned_ = false;

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
                rs1_ = Registers::STF_REG::STF_REG_INVALID;
                rs2_ = Registers::STF_REG::STF_REG_INVALID;
                rs1_value_ = 0;
                rs2_value_ = 0;
                taken_ = false;
                conditional_ = false;
                call_ = false;
                return_ = false;
                indirect_ = false;
                compare_eq_ = false;
                compare_not_eq_ = false;
                compare_greater_than_or_equal_ = false;
                compare_less_than_ = false;
                compare_unsigned_ = false;
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
                                 const Registers::STF_REG rs1,
                                 const Registers::STF_REG rs2,
                                 const bool is_conditional,
                                 const bool is_call,
                                 const bool is_return,
                                 const bool is_indirect,
                                 const bool compare_eq,
                                 const bool compare_not_eq,
                                 const bool compare_greater_than_or_equal,
                                 const bool compare_less_than,
                                 const bool compare_unsigned) {
                stf_assert(!(is_conditional && is_indirect), "Indirect branches cannot be conditional");
                stf_assert(!(is_conditional && is_call), "Calls cannot be conditional");
                stf_assert(!(is_conditional && is_return), "Returns cannot be conditional");

                pc_ = pc;
                target_ = validateBranchTarget_(target, is_indirect);
                opcode_ = opcode;
                rs1_ = rs1;
                rs2_ = rs2;
                conditional_ = is_conditional;
                call_ = is_call;
                return_ = is_return;
                indirect_ = is_indirect;
                compare_eq_ = compare_eq;
                compare_not_eq_ = compare_not_eq;
                compare_greater_than_or_equal_ = compare_greater_than_or_equal;
                compare_less_than_ = compare_less_than;
                compare_unsigned_ = compare_unsigned;
            }

            /**
             * Sets operand values
             * \param operand_map std::unordered_map mapping register numbers to values
             */
            inline void setOperandValues_(const OperandMap& operand_map) {
                if(rs1_ != Registers::STF_REG::STF_REG_INVALID) {
                    rs1_value_ = operand_map.getOperand(rs1_);

                    // Can't have an rs2 without an rs1
                    if(rs2_ != Registers::STF_REG::STF_REG_INVALID) {
                        rs2_value_ = operand_map.getOperand(rs2_);
                    }
                }
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
             * Gets whether this branch does an equality comparison
             */
            inline bool isCompareEqual() const {
                return compare_eq_;
            }

            /**
             * Gets whether this branch does an inequality comparison
             */
            inline bool isCompareNotEqual() const {
                return compare_not_eq_;
            }

            /**
             * Gets whether this branch does a greater-than or equal comparison
             */
            inline bool isCompareGreaterThanOrEqual() const {
                return compare_greater_than_or_equal_;
            }

            /**
             * Gets whether this branch does a less-than comparison
             */
            inline bool isCompareLessThan() const {
                return compare_less_than_;
            }

            /**
             * Gets whether this branch does an unsigned comparison
             */
            inline bool isCompareUnsigned() const {
                return compare_unsigned_;
            }

            /**
             * Gets the register number of RS1 (if any)
             */
            inline Registers::STF_REG getRS1() const {
                return rs1_;
            }

            /**
             * Gets the register value of RS1 (if any)
             */
            inline uint64_t getRS1Value() const {
                return rs1_value_;
            }

            /**
             * Gets the register number of RS2 (if any)
             */
            inline Registers::STF_REG getRS2() const {
                return rs2_;
            }

            /**
             * Gets the register value of RS2 (if any)
             */
            inline uint64_t getRS2Value() const {
                return rs2_value_;
            }
    };

    /**
     * Formats an STFBranch into an ostream
     */
    std::ostream& operator<<(std::ostream& os, const STFBranch& branch);

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
                                            const Registers::STF_REG rs1,
                                            const Registers::STF_REG rs2,
                                            const bool is_conditional,
                                            const bool is_call,
                                            const bool is_return,
                                            const bool is_indirect,
                                            const bool compare_eq,
                                            const bool compare_not_eq,
                                            const bool compare_greater_than_or_equal,
                                            const bool compare_less_than,
                                            const bool compare_unsigned) {
                    branch.setInfo_(pc,
                                    target,
                                    opcode,
                                    rs1,
                                    rs2,
                                    is_conditional,
                                    is_call,
                                    is_return,
                                    is_indirect,
                                    compare_eq,
                                    compare_not_eq,
                                    compare_greater_than_or_equal,
                                    compare_less_than,
                                    compare_unsigned);
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

                /**
                 * Sets the branch operand values
                 * \param branch Branch to modify
                 * \param operand_map std::unordered_map mapping register numbers to values
                 */
                __attribute__((always_inline))
                static inline void setOperandValues_(STFBranch& branch, const STFBranch::OperandMap& operand_map) {
                    branch.setOperandValues_(operand_map);
                }

                friend class stf::STFBranchReader;
                friend class stf::STFBranchDecoder;
        };
    } // end namespace delegates
} // end namespace stf

#endif
