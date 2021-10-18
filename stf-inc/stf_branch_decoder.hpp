#ifndef __STF_BRANCH_DECODER_HPP__
#define __STF_BRANCH_DECODER_HPP__

#include <cstddef>
#include "stf_branch.hpp"
#include "stf_record_types.hpp"
#include "util.hpp"

namespace stf {
    /**
     * \class STFBranchDecoder
     * \brief Decodes RISCV opcodes to find branches
     */
    class STFBranchDecoder {
        private:
            /**
             * Computes the target PC from the branch PC and offset
             * \param pc Branch PC
             * \param offset Branch offset
             */
            static constexpr uint64_t getTarget_(const uint64_t pc, const int64_t offset) {
                return static_cast<uint64_t>(static_cast<int64_t>(pc) + offset);
            }

            /**
             * Sign extends a target offset to an int64_t
             * \param val Target offset to sign extend
             */
            template<size_t Width, typename T>
            static constexpr int64_t signExtendTarget_(const T val) {
                return byte_utils::signExtend<Width, int64_t, T>(val);
            }

            /**
             * Extracts the target offset from a C.J or C.JAL (RV32 only) branch
             * \param pc Branch PC
             * \param opcode Branch opcode
             */
            static constexpr uint64_t getCJTarget_(const uint64_t pc, const uint16_t opcode) {
                using extractor = byte_utils::BitExtractor<decltype(opcode)>;
                return getTarget_(pc, signExtendTarget_<12>(extractor::get<extractor::Bit<12, 11>,
                                                                           extractor::Bit< 8, 10>,
                                                                           extractor::Bit<10,  9>,
                                                                           extractor::Bit< 9,  8>,
                                                                           extractor::Bit< 6,  7>,
                                                                           extractor::Bit< 7,  6>,
                                                                           extractor::Bit< 2,  5>,
                                                                           extractor::Bit<11,  4>,
                                                                           extractor::BitRange< 5, 3, 3>>(opcode)));
            }

            /**
             * Decodes a compressed RISCV instruction, returning true if it is a branch.
             * \param[in] iem Instruction encoding mode
             * \param[in] pc Instruction PC
             * \param[in] opcode Instruction opcode
             * \param[out] target Branch target PC (only set for non-indirect branches)
             * \param[out] is_conditional Set to true if the branch is conditional
             * \param[out] is_call Set to true if the branch is a call
             * \param[out] is_return Set to true if the branch is a return
             * \param[out] is_indirect Set to true if the branch is an indirect
             */
            static constexpr bool decodeBranch16_(const INST_IEM iem,
                                                  const uint64_t pc,
                                                  const uint16_t opcode,
                                                  uint64_t& target,
                                                  bool& is_conditional,
                                                  bool& is_call,
                                                  bool& is_return,
                                                  bool& is_indirect) {
                const uint8_t opcode_top = static_cast<uint8_t>(byte_utils::getBitRange<15, 13, 2>(opcode));
                const uint8_t opcode_bottom = static_cast<uint8_t>(byte_utils::getBitRange<1, 0>(opcode));

                target = 0;
                is_conditional = false;
                is_call = false;
                is_return = false;
                is_indirect = false;

                switch(opcode_top) {
                    case 0b001:
                        // JAL if RV32
                        if(STF_EXPECT_TRUE(opcode_bottom != 0b01 || iem != INST_IEM::STF_INST_IEM_RV32)) {
                            return false;
                        }
                        target = getCJTarget_(pc, opcode);
                        is_call = true;
                        break;
                    case 0b110:
                        // BEQZ
                    case 0b111:
                        // BNEZ
                    {
                        if(STF_EXPECT_TRUE(opcode_bottom != 0b01)) {
                            return false;
                        }
                        using extractor = byte_utils::BitExtractor<decltype(opcode)>;
                        target = getTarget_(pc, signExtendTarget_<9>(extractor::get<extractor::Bit<12, 8>,
                                                                                    extractor::Bit< 6, 7>,
                                                                                    extractor::Bit< 5, 6>,
                                                                                    extractor::Bit< 2, 5>,
                                                                                    extractor::BitRange<11, 10, 4>,
                                                                                    extractor::BitRange< 4,  3, 2>>(opcode)));
                        is_conditional = true;
                        break;
                    }
                    case 0b100:
                    {
                        // Possible J[AL]R
                        const uint8_t rs1 = static_cast<uint8_t>(byte_utils::getBitRange<11, 7, 4>(opcode));
                        const uint8_t rs2 = static_cast<uint8_t>(byte_utils::getBitRange<6, 2, 4>(opcode));
                        if(STF_EXPECT_TRUE(opcode_bottom != 0b10 || rs1 == 0 || rs2 != 0)) {
                            return false;
                        }
                        is_call = byte_utils::getBit<12>(opcode);
                        is_return = !is_call && (rs1 == 1);
                        is_indirect = true;
                        break;;
                    }
                    case 0b101:
                        // J
                        if(STF_EXPECT_TRUE(opcode_bottom != 0b01)) {
                            return false;
                        }
                        target = getCJTarget_(pc, opcode);
                        break;
                    default:
                        return false;
                };

                return true;
            }

            /**
             * Decodes a regular RISCV instruction, returning true if it is a branch.
             * \param[in] iem Instruction encoding mode
             * \param[in] pc Instruction PC
             * \param[in] opcode Instruction opcode
             * \param[out] target Branch target PC (only set for non-indirect branches)
             * \param[out] is_conditional Set to true if the branch is conditional
             * \param[out] is_call Set to true if the branch is a call
             * \param[out] is_return Set to true if the branch is a return
             * \param[out] is_indirect Set to true if the branch is an indirect
             */
            static constexpr bool decodeBranch32_(const uint64_t pc,
                                                  const uint32_t opcode,
                                                  uint64_t& target,
                                                  bool& is_conditional,
                                                  bool& is_call,
                                                  bool& is_return,
                                                  bool& is_indirect) {
                const uint8_t opcode_top = static_cast<uint8_t>(byte_utils::getBitRange<6, 5, 1>(opcode));
                const uint8_t opcode_bottom = static_cast<uint8_t>(byte_utils::getBitRange<4, 2, 2>(opcode));

                if(STF_EXPECT_TRUE(opcode_top != 0b11)) {
                    return false;
                }

                target = 0;
                is_conditional = false;
                is_call = false;
                is_return = false;
                is_indirect = false;

                using extractor = byte_utils::BitExtractor<decltype(opcode)>;

                switch(opcode_bottom) {
                    case 0b000:
                    {
                        // conditional branch
                        target = getTarget_(pc, signExtendTarget_<13>(extractor::get<extractor::Bit<31, 12>,
                                                                                     extractor::Bit<7, 11>,
                                                                                     extractor::BitRange<30, 25, 10>,
                                                                                     extractor::BitRange<11, 8, 4>>(opcode)));
                        is_conditional = true;
                        break;
                    }
                    case 0b001:
                    {
                        // jalr
                        const uint8_t dest_reg = static_cast<uint8_t>(byte_utils::getBitRange<11, 7>(opcode));
                        is_call = dest_reg != 0; // Indirect jumps have rd == x0
                        is_return = (dest_reg == 0) && (byte_utils::getBitRange<24, 20, 4>(opcode) == 1); // Returns have rd == x0 and rs == x1
                        is_indirect = true;
                        break;
                    }
                    case 0b011:
                        // jal
                        target = getTarget_(pc, signExtendTarget_<21>(extractor::get<extractor::Bit<31, 20>,
                                                                                     extractor::BitRange<19, 12>,
                                                                                     extractor::Bit<20, 11>,
                                                                                     extractor::BitRange<30, 21, 10>>(opcode)));
                        is_call = byte_utils::getBitRange<11, 7>(opcode) != 0; // Unconditional jumps have rd == x0
                        break;
                    default:
                        return false;
                };

                return true;
            }

            /**
             * Decodes a compressed RISCV instruction record, returning true if it is a branch.
             * \param[in] iem Instruction encoding mode
             * \param[in] rec Instruction record
             * \param[out] target Branch target PC (only set for non-indirect branches)
             * \param[out] is_conditional Set to true if the branch is conditional
             * \param[out] is_call Set to true if the branch is a call
             * \param[out] is_return Set to true if the branch is a return
             * \param[out] is_indirect Set to true if the branch is an indirect
             */
            __attribute__((hot, always_inline))
            static inline bool decodeBranch_(const INST_IEM iem,
                                             const InstOpcode16Record& rec,
                                             uint64_t& target,
                                             bool& is_conditional,
                                             bool& is_call,
                                             bool& is_return,
                                             bool& is_indirect) {
                return decodeBranch16_(iem,
                                       rec.getPC(),
                                       rec.getOpcode(),
                                       target,
                                       is_conditional,
                                       is_call,
                                       is_return,
                                       is_indirect);
            }

            /**
             * Decodes a regular RISCV instruction record, returning true if it is a branch.
             * \param[in] iem Instruction encoding mode
             * \param[in] rec Instruction record
             * \param[out] target Branch target PC (only set for non-indirect branches)
             * \param[out] is_conditional Set to true if the branch is conditional
             * \param[out] is_call Set to true if the branch is a call
             * \param[out] is_return Set to true if the branch is a return
             * \param[out] is_indirect Set to true if the branch is an indirect
             */
            __attribute__((hot, always_inline))
            static inline bool decodeBranch_(const INST_IEM,
                                             const InstOpcode32Record& rec,
                                             uint64_t& target,
                                             bool& is_conditional,
                                             bool& is_call,
                                             bool& is_return,
                                             bool& is_indirect) {
                return decodeBranch32_(rec.getPC(),
                                       rec.getOpcode(),
                                       target,
                                       is_conditional,
                                       is_call,
                                       is_return,
                                       is_indirect);
            }

        public:
            /**
             * Decodes a RISCV instruction record, returning true if it is a branch.
             * \param[in] iem Instruction encoding mode
             * \param[in] rec Instruction record
             * \param[out] target Branch target PC (only set for non-indirect branches)
             * \param[out] is_conditional Set to true if the branch is conditional
             * \param[out] is_call Set to true if the branch is a call
             * \param[out] is_return Set to true if the branch is a return
             * \param[out] is_indirect Set to true if the branch is an indirect
             */
            template<typename RecordType>
            __attribute__((hot, always_inline))
            static inline bool decode(const INST_IEM iem,
                                      const RecordType& rec,
                                      uint64_t& target,
                                      bool& is_conditional,
                                      bool& is_call,
                                      bool& is_return,
                                      bool& is_indirect) {
                return decodeBranch_(iem, rec, target, is_conditional, is_call, is_return, is_indirect);
            }

            /**
             * Decodes a regular RISCV instruction record, returning true if it is a branch.
             * This variant is used by STFBranchReader to initialize STFBranch objects.
             * \param[in] iem Instruction encoding mode
             * \param[in] rec Instruction record
             * \param[out] branch STFBranch object to be initialized if the instruction is a branch
             */
            template<typename RecordType>
            __attribute__((hot, always_inline))
            static inline bool decode(const INST_IEM iem,
                                      const RecordType& rec,
                                      STFBranch& branch) {
                uint64_t target = 0;
                bool is_conditional = false;
                bool is_call = false;
                bool is_return = false;
                bool is_indirect = false;

                const bool is_branch = decode(iem, rec, target, is_conditional, is_call, is_return, is_indirect);

                if(is_branch) {
                    branch.setInfo(rec.getPC(),
                                   target,
                                   rec.getOpcode(),
                                   is_conditional,
                                   is_call,
                                   is_return,
                                   is_indirect);
                }

                return is_branch;
            }

            /**
             * Returns whether the specified instruction record is a branch.
             * \param iem Instruction encoding mode
             * \param rec Instruction record
             */
            template<typename RecordType>
            __attribute__((hot, always_inline))
            static inline bool isBranch(const INST_IEM iem, const RecordType& rec) {
                uint64_t target = 0;
                bool is_conditional = false;
                bool is_call = false;
                bool is_return = false;
                bool is_indirect = false;

                return decode(iem, rec, target, is_conditional, is_call, is_return, is_indirect);
            }

    };
}; // end namespace stf

#endif
