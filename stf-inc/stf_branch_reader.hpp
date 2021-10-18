
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
#include "stf_buffered_reader.hpp"
#include "stf_enums.hpp"
#include "stf_exception.hpp"
#include "stf_filter_types.hpp"
#include "stf_reader.hpp"
#include "stf_record.hpp"
#include "stf_record_pointers.hpp"
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
    class STFBranchReader: public STFBufferedReader<STFBranch, DummyFilter, STFBranchReader> {
        private:
            using ParentReader = STFBufferedReader<STFBranch, DummyFilter, STFBranchReader>;
            friend ParentReader;
            using ParentReader::DEFAULT_BUFFER_SIZE_;
            using IntDescriptor = descriptors::internal::Descriptor;
            uint64_t num_branches_read_ = 0;
            STFBranch* last_branch_ = nullptr;
            STFRecord::UniqueHandle current_record_;

            __attribute__((hot, always_inline))
            inline size_t rawNumRead_() const {
                return num_branches_read_;
            }

            template<typename InstRecordType>
            inline void updateLastBranch_(const InstRecordType& rec) {
                if(STF_EXPECT_FALSE(last_branch_ != nullptr)) {
                    stf_assert(last_branch_->getTargetPC() == rec.getPC(),
                               "Mismatch between current PC (" << std::hex << rec.getPC() <<
                               ") and expected target PC (" << last_branch_->getTargetPC() << ") of last taken branch");
                    last_branch_->setTargetOpcode(rec.getOpcode());
                    last_branch_ = nullptr;
                }
            }

            template<typename InstRecordType>
            __attribute__((hot, always_inline))
            inline void finalizeNotABranch_(STFBranch& branch, const STFRecord* const rec) {
                updateLastBranch_(rec->as<InstRecordType>());
                branch.reset();
            }

            template<typename InstRecordType>
            __attribute__((hot, always_inline))
            inline bool finalizeBranch_(STFBranch& branch, const STFRecord* const rec) {
                const auto& inst_rec = rec->as<InstRecordType>();

                updateLastBranch_(inst_rec);

                if(STF_EXPECT_TRUE(!STFBranchDecoder::decode(getInitialIEM(), inst_rec, branch))) {
                    stf_assert(!branch.isTaken(), "Branch was marked taken but also didn't decode as a branch");
                    return false;
                }

                branch.setIndex(++num_branches_read_);

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
                branch.reset();

                bool not_a_branch = false;

                while(true) {
                    const auto rec = readRecord_(branch);

                    if(!rec) {
                        continue;
                    }

                    const auto desc = rec->getDescriptor();

                    if(!not_a_branch) {
                        if(STF_EXPECT_TRUE(desc == IntDescriptor::STF_INST_REG)) {
                            const auto& reg_rec = rec->template as<InstRegRecord>();
                            if(STF_EXPECT_FALSE(reg_rec.getOperandType() == Registers::STF_REG_OPERAND_TYPE::REG_STATE)) {
                                continue;
                            }
                            // Branches don't access FP or vector registers
                            not_a_branch |= reg_rec.isFP() || reg_rec.isVector();
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
                        else if(desc == IntDescriptor::STF_EVENT) {
                            // Branches don't fault
                            not_a_branch = true;
                        }
                        else if(desc == IntDescriptor::STF_INST_PC_TARGET) {
                            branch.setTaken(true);
                            branch.setTarget(rec->template as<InstPCTargetRecord>().getAddr());
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
             * \param buffer_size The size of the instruction sliding window
             * \param force_single_threaded_stream If true, forces single threaded mode in reader
             */
            template<typename StrType>
            explicit STFBranchReader(const StrType& filename,
                                     const size_t buffer_size = DEFAULT_BUFFER_SIZE_,
                                     const bool force_single_threaded_stream = false) :
                ParentReader(buffer_size)
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
             * \brief Closes the file
             */
            int close() {
                return STFReader::close();
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
