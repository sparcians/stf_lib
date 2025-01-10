#ifndef __STF_BRANCH_HPP__
#define __STF_BRANCH_HPP__

#include <algorithm>
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
                        uint64_t val; // cppcheck-suppress unusedStructMember
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
                            // cppcheck-suppress useStlAlgorithm
                            if(reg.reg == reg_num) {
                                return reg.val;
                            }
                        }

                        return 0;
                    }

                    /**
                     * Check if an operand exists in the map.
                     * \param reg_num Register number of the operand
                     */
                    inline bool hasOperand(const Registers::STF_REG reg_num) const {
                        return std::any_of(
                            regs_.begin(),
                            regs_.end(),
                            [&reg_num](const auto& reg) {
                                return reg.reg == reg_num;
                            }
                        );
                    }

                    /**
                     * Clear the map.
                     */
                    inline void clear() {
                        wr_index_ = 0;
                        std::fill(regs_.begin(), regs_.end(), RegPair{Registers::STF_REG::STF_REG_INVALID, 0});
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

            uint64_t inst_index_ = 0;
            uint64_t unskipped_inst_index_ = 0;
            uint64_t pc_ = 0;
            uint64_t target_ = 0;
            uint32_t opcode_ = 0;
            uint32_t target_opcode_ = 0;
            Registers::STF_REG rs1_ = Registers::STF_REG::STF_REG_INVALID;
            Registers::STF_REG rs2_ = Registers::STF_REG::STF_REG_INVALID;
            Registers::STF_REG rd_ = Registers::STF_REG::STF_REG_INVALID;
            uint64_t rs1_value_ = 0;
            uint64_t rs2_value_ = 0;
            uint64_t rd_value_ = 0;
            bool compressed_ = false;
            bool taken_ = false;
            bool conditional_ = false;
            bool call_ = false;
            bool return_ = false;
            bool millicall_ = false;
            bool millireturn_ = false;
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
            // cppcheck-suppress duplInheritedMember
            inline void reset_() {
                STFSkippableItem::reset_();
                pc_ = 0;
                target_ = 0;
                opcode_ = 0;
                target_opcode_ = 0;
                rs1_ = Registers::STF_REG::STF_REG_INVALID;
                rs2_ = Registers::STF_REG::STF_REG_INVALID;
                rd_ = Registers::STF_REG::STF_REG_INVALID;
                rs1_value_ = 0;
                rs2_value_ = 0;
                rd_value_ = 0;
                compressed_ = false;
                taken_ = false;
                conditional_ = false;
                call_ = false;
                return_ = false;
                millicall_ = false;
                millireturn_ = false;
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
             * \param rs1 Number of first source register (if any)
             * \param rs2 Number of second source register (if any)
             * \param rd Number of dest register (if any)
             * \param is_conditional Set to true if the branch is conditional
             * \param is_call Set to true if the branch is a call
             * \param is_return Set to true if the branch is a return
             * \param is_millicall Set to true if the branch is a millicall
             * \param is_millireturn Set to true if the branch is a millireturn
             * \param is_indirect Set to true if the branch is an indirect
             * \param compare_eq Set to true if the branch is comparing equality
             * \param compare_not_eq Set to true if the branch is comparing inequality
             * \param compare_greater_than_or_equal Set to true if the branch is comparing greater-than or equal
             * \param compare_less_than Set to true if the branch is comparing less-than
             * \param compare_unsigned Set to true if the branch is comparing unsigned
             */
            __attribute__((always_inline))
            inline void setInfo_(const uint64_t pc,
                                 const uint64_t target,
                                 const uint32_t opcode,
                                 const Registers::STF_REG rs1,
                                 const Registers::STF_REG rs2,
                                 const Registers::STF_REG rd,
                                 const bool compressed,
                                 const bool is_conditional,
                                 const bool is_call,
                                 const bool is_return,
                                 const bool is_millicall,
                                 const bool is_millireturn,
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
                rd_ = rd;
                compressed_ = compressed;
                conditional_ = is_conditional;
                call_ = is_call;
                return_ = is_return;
                millicall_ = is_millicall;
                millireturn_ = is_millireturn;
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
            __attribute__((always_inline))
            inline void setOperandValues_(const OperandMap& src_operand_map, const OperandMap& dest_operand_map) {
                if(rs1_ != Registers::STF_REG::STF_REG_INVALID) {
                    rs1_value_ = src_operand_map.getOperand(rs1_);

                    // Can't have an rs2 without an rs1
                    if(rs2_ != Registers::STF_REG::STF_REG_INVALID) {
                        rs2_value_ = src_operand_map.getOperand(rs2_);
                    }
                }
                if(rd_ != Registers::STF_REG::STF_REG_INVALID) {
                    rd_value_ = dest_operand_map.getOperand(rd_);
                }
            }

            /**
             * Sets instruction index
             * \param index Index value to set
             * \param unskipped_index Actual index within the trace (i.e., the index the item would
             * have if there was no skipping)
             * \param inst_index Instruction index to set
             * \param unskipped_inst_index Instruction index without skipping enabled
             */
            __attribute__((always_inline))
            inline void setIndex_(const uint64_t index,
                                  const uint64_t unskipped_index,
                                  const uint64_t inst_index,
                                  const uint64_t unskipped_inst_index) {
                STFSkippableItem::setIndex_(index, unskipped_index);
                inst_index_ = inst_index;
                unskipped_inst_index_ = unskipped_inst_index;
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
             * Gets whether the branch is compressed
             */
            inline bool isCompressed() const {
                return compressed_;
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
             * Gets whether the branch is a millicode call
             */
            inline bool isMillicall() const {
                return millicall_;
            }

            /**
             * Gets whether the branch is a millicode return
             */
            inline bool isMillireturn() const {
                return millireturn_;
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

            /**
             * Gets the register number of RD (if any)
             */
            inline Registers::STF_REG getRD() const {
                return rd_;
            }

            /**
             * Gets the register value of RD (if any)
             */
            inline uint64_t getRDValue() const {
                return rd_value_;
            }

            /**
             * Gets the instruction index of this branch
             */
            inline uint64_t instIndex() const {
                return inst_index_;
            }

            /**
             * Gets the instruction index of this branch
             */
            inline uint64_t unskippedInstIndex() const {
                return unskipped_inst_index_;
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
                 * \param rs1 Number of first source register (if any)
                 * \param rs2 Number of second source register (if any)
                 * \param rd Number of dest register (if any)
                 * \param is_conditional Set to true if the branch is conditional
                 * \param is_call Set to true if the branch is a call
                 * \param is_return Set to true if the branch is a return
                 * \param is_millicall Set to true if the branch is a millicall
                 * \param is_millireturn Set to true if the branch is a millireturn
                 * \param is_indirect Set to true if the branch is an indirect
                 * \param compare_eq Set to true if the branch is comparing equality
                 * \param compare_not_eq Set to true if the branch is comparing inequality
                 * \param compare_greater_than_or_equal Set to true if the branch is comparing greater-than or equal
                 * \param compare_less_than Set to true if the branch is comparing less-than
                 * \param compare_unsigned Set to true if the branch is comparing unsigned
                 */
                __attribute__((always_inline))
                static inline void setInfo_(STFBranch& branch,
                                            const uint64_t pc,
                                            const uint64_t target,
                                            const uint32_t opcode,
                                            const Registers::STF_REG rs1,
                                            const Registers::STF_REG rs2,
                                            const Registers::STF_REG rd,
                                            const bool compressed,
                                            const bool is_conditional,
                                            const bool is_call,
                                            const bool is_return,
                                            const bool is_millicall,
                                            const bool is_millireturn,
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
                                    rd,
                                    compressed,
                                    is_conditional,
                                    is_call,
                                    is_return,
                                    is_millicall,
                                    is_millireturn,
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
                 * \param src_operand_map std::unordered_map mapping source register numbers to values
                 * \param dest_operand_map std::unordered_map mapping dest register numbers to values
                 */
                __attribute__((always_inline))
                static inline void setOperandValues_(STFBranch& branch,
                                                     const STFBranch::OperandMap& src_operand_map,
                                                     const STFBranch::OperandMap& dest_operand_map) {
                    branch.setOperandValues_(src_operand_map, dest_operand_map);
                }

                /**
                 * Sets the index
                 * \param branch Branch to modify
                 * \param index Index value to set
                 * \param unskipped_index Actual index within the trace (i.e., the index the item would
                 * have if there was no skipping)
                 * \param inst_index Instruction index to set
                 * \param unskipped_inst_index Instruction index without skipping enabled
                 */
                __attribute__((always_inline))
                static inline void setIndex_(STFBranch& branch,
                                             const uint64_t index,
                                             const uint64_t unskipped_index,
                                             const uint64_t inst_index,
                                             const uint64_t unskipped_inst_index) {
                    branch.setIndex_(index, unskipped_index, inst_index, unskipped_inst_index);
                }

                friend class stf::STFBranchReader;
                friend class stf::STFBranchDecoder;
        };
    } // end namespace delegates
} // end namespace stf

#endif
