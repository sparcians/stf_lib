
// <STF_Inst_Reader> -*- HPP -*-
/**
 * \brief  This file defines the STF instruction trace reader classes.
 *
 * It hides the implementation details of the STF specification and
 * provides user with all relevent information of instructions.
 *
 */

#ifndef __STF_BRANCH_READER_HPP__
#define __STF_BRANCH_READER_HPP__

#include <sys/stat.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include "stf_branch.hpp"
#include "stf_branch_decoder.hpp"
#include "stf_user_mode_skipping_reader.hpp"
#include "stf_enums.hpp"
#include "stf_exception.hpp"
#include "stf_filter_types.hpp"
#include "stf_reader.hpp"
#include "stf_record.hpp"
#include "stf_record_types.hpp"

/**
 * \namespace stf
 * \brief Defines all STF related classes
 *
 */
namespace stf {

    /**
     * \class STFBranchReader
     * \brief The STF branch reader provides an iterator to the branches in the instruction stream
     */
    class STFBranchReader final : public STFUserModeSkippingReader<STFBranch, DummyFilter, STFBranchReader> {
        private:
            using ParentReader = STFUserModeSkippingReader<STFBranch, DummyFilter, STFBranchReader>;
            friend ParentReader;
            /// \cond DOXYGEN_IGNORED
            friend typename ParentReader::BufferedReader;
            /// \endcond

            using ParentReader::DEFAULT_BUFFER_SIZE_;
            using IntDescriptor = typename ParentReader::IntDescriptor;
            using ParentReader::numItemsReadFromReader_;
            using ParentReader::updateSkipping_;
            using ParentReader::checkSkipping_;
            using ParentReader::onlyUserMode_;
            using ParentReader::skippingEnabled_;

            uint64_t num_branches_read_ = 0;
            STFBranch* last_branch_ = nullptr;
            STFRecord::UniqueHandle current_record_;
            STFBranch::OperandMap src_operands_;
            STFBranch::OperandMap dest_operands_;

            __attribute__((hot, always_inline))
            inline size_t rawNumRead_() const {
                return num_branches_read_;
            }

            __attribute__((always_inline))
            inline void skippedCleanup_(const STFBranch&) {
                last_branch_ = nullptr;
            }

            template<typename InstRecordType>
            __attribute__((hot, always_inline))
            inline void updateLastBranch_(const InstRecordType& rec) {
                if(STF_EXPECT_FALSE(last_branch_ != nullptr)) {
                    stf_assert(last_branch_->getTargetPC() == rec.getPC(),
                               "Mismatch between current PC (" << std::hex << rec.getPC() <<
                               ") and expected target PC (" << last_branch_->getTargetPC() << ") of last taken branch");
                    delegates::STFBranchDelegate::setTargetOpcode_(*last_branch_, rec.getOpcode());
                    last_branch_ = nullptr;
                }
            }

            __attribute__((hot, always_inline))
            inline void resetOperandMaps_() {
                src_operands_.clear();
                src_operands_.addOperand(Registers::STF_REG::STF_REG_X0, 0);

                dest_operands_.clear();
                dest_operands_.addOperand(Registers::STF_REG::STF_REG_X0, 0);
            }

            template<typename InstRecordType>
            __attribute__((hot, always_inline))
            inline void finalizeNotABranch_(STFBranch& branch, const STFRecord* const rec) {
                updateLastBranch_(rec->as<InstRecordType>());
                updateSkipping_();
                delegates::STFBranchDelegate::reset_(branch);
                resetOperandMaps_();
            }

            template<typename InstRecordType>
            __attribute__((hot, always_inline))
            inline bool finalizeBranch_(STFBranch& branch, const STFRecord* const rec) {
                const auto& inst_rec = rec->as<InstRecordType>();

                updateLastBranch_(inst_rec);

                if(STF_EXPECT_TRUE(!STFBranchDecoder::decode(getInitialIEM(), inst_rec, branch))) {
                    stf_assert(!branch.isTaken(), "Branch was marked taken but also didn't decode as a branch");
                    delegates::STFBranchDelegate::reset_(branch);
                    resetOperandMaps_();
                    return false;
                }

                delegates::STFBranchDelegate::setOperandValues_(branch, src_operands_, dest_operands_);
                ++num_branches_read_;
                initItemIndex_(branch);
                delegates::STFBranchDelegate::setSkipped_(branch, skippingEnabled_());
                countSkipped_(branch.skipped());

                if(branch.isTaken()) {
                    last_branch_ = &branch;
                }

                return true;
            }

            __attribute__((always_inline))
            inline const STFRecord* handleNewRecord_(STFBranch& inst, STFRecord::UniqueHandle&& urec) {
                current_record_ = std::move(urec);
                return current_record_.get();
            }

            // read STF records to construction a STFInst instance
            __attribute__((hot, always_inline))
            inline void readNext_(STFBranch &branch) {
                resetOperandMaps_();
                delegates::STFBranchDelegate::reset_(branch);

                updateSkipping_();

                bool not_a_branch = false;

                while(true) {
                    const auto rec = readRecord_(branch);

                    if(!rec) {
                        continue;
                    }

                    const auto desc = rec->getId();

                    if(STF_EXPECT_FALSE(desc == IntDescriptor::STF_EVENT)) {
                        // Branches don't fault
                        not_a_branch = true;
                        const auto& event = rec->template as<EventRecord>();
                        checkSkipping_(event.isModeChange(),
                                       static_cast<EXECUTION_MODE>(event.getData().front()) == EXECUTION_MODE::USER_MODE);
                    }
                    // Most instructions aren't branches
                    else if(STF_EXPECT_FALSE(!not_a_branch)) {
                        if(STF_EXPECT_TRUE(desc == IntDescriptor::STF_INST_REG)) {
                            const auto& reg_rec = rec->template as<InstRegRecord>();
                            if(STF_EXPECT_FALSE(reg_rec.getOperandType() == Registers::STF_REG_OPERAND_TYPE::REG_STATE)) {
                                continue;
                            }
                            // Branches only access integer registers
                            if(STF_EXPECT_FALSE(!reg_rec.isInt())) {
                                not_a_branch = true;
                            }
                            else if(STF_EXPECT_TRUE(reg_rec.getOperandType() == Registers::STF_REG_OPERAND_TYPE::REG_SOURCE)) {
                                // Branches don't have more than 2 source operands
                                if(STF_EXPECT_FALSE(src_operands_.size() >= 2)) {
                                    not_a_branch = true;
                                }
                                else {
                                    src_operands_.addOperand(reg_rec.getReg(), reg_rec.getScalarData());
                                }
                            }
                            else if(STF_EXPECT_TRUE(reg_rec.getOperandType() == Registers::STF_REG_OPERAND_TYPE::REG_DEST)) {
                                // Branches don't have more than 1 dest operand
                                if(STF_EXPECT_FALSE(dest_operands_.size() > 0)) {
                                    not_a_branch = true;
                                }
                                else {
                                    dest_operands_.addOperand(reg_rec.getReg(), reg_rec.getScalarData());
                                }
                            }
                        }
                        else if(STF_EXPECT_TRUE(desc == IntDescriptor::STF_INST_OPCODE16)) {
                            if(finalizeBranch_<InstOpcode16Record>(branch, rec)) {
                                break;
                            }
                        }
                        else if(STF_EXPECT_TRUE(desc == IntDescriptor::STF_INST_OPCODE32)) {
                            if(finalizeBranch_<InstOpcode32Record>(branch, rec)) {
                                break;
                            }
                        }
                        else if(STF_EXPECT_TRUE(desc == IntDescriptor::STF_INST_MEM_ACCESS)) {
                            // Branches don't access memory
                            not_a_branch = true;
                        }
                        else if(STF_EXPECT_FALSE(desc == IntDescriptor::STF_INST_PC_TARGET)) {
                            delegates::STFBranchDelegate::setTaken_(branch,
                                                                    rec->template as<InstPCTargetRecord>().getAddr());
                        }
                    }
                    else if(STF_EXPECT_TRUE(desc == IntDescriptor::STF_INST_OPCODE16)) {
                        finalizeNotABranch_<InstOpcode16Record>(branch, rec);
                        not_a_branch = false;
                    }
                    else if(STF_EXPECT_TRUE(desc == IntDescriptor::STF_INST_OPCODE32)) {
                        finalizeNotABranch_<InstOpcode32Record>(branch, rec);
                        not_a_branch = false;
                    }
                }
            }

        public:
            using ParentReader::getInitialIEM;

            /**
             * \brief Constructor
             * \param filename The trace file name
             * \param only_user_mode If true, non-user-mode instructions will be skipped
             * \param buffer_size The size of the instruction sliding window
             * \param force_single_threaded_stream If true, forces single threaded mode in reader
             */
            template<typename StrType>
            explicit STFBranchReader(const StrType& filename,
                                     const bool only_user_mode = false,
                                     const size_t buffer_size = DEFAULT_BUFFER_SIZE_,
                                     const bool force_single_threaded_stream = false) :
                ParentReader(only_user_mode, buffer_size)
            {
                open(filename, force_single_threaded_stream);
            }

            /**
             * \class iterator
             * \brief iterator of the instruction stream that hides the sliding window.
             * Decrement is not implemented. Rewinding is done by copying or assigning
             * an existing iterator, with range limited by the sliding window size.
             *
             * Using the iterator ++ operator may advance the underlying trace stream,
             * which is un-rewindable if the trace is compressed or via STDIN
             *
             */
            class iterator : public ParentReader::base_iterator {
                public:
                    iterator() = default;

                    /**
                     * Iterator constructor
                     * \param sbr Parent STFBranchReader
                     * \param end If true, this is an end iterator
                     */
                    explicit iterator(STFBranchReader *sbr, const bool end = false) :
                        ParentReader::base_iterator(sbr, end)
                    {
                    }

                    /**
                     * Return whether this is the last branch in the trace
                     */
                    inline bool isLastBranch() const {
                        return ParentReader::base_iterator::isLastItem_();
                    }
            };


            /**
             * \brief Opens a file
             * \param filename The trace file name
             * \param force_single_threaded_stream If true, forces single threaded mode in reader
             */
            void open(const std::string_view filename,
                      const bool force_single_threaded_stream = false) {
                ParentReader::open(filename, force_single_threaded_stream);
            }

            /**
             * Returns the number of instructions read so far with filtering
             */
            inline size_t numBranchesRead() const {
                return ParentReader::numItemsRead_();
            }
    };
} //end namespace stf

// __STF_INST_READER_HPP__
#endif
