
// <STF_Inst_Reader> -*- HPP -*-

/**
 * \brief  This file defines the STF instruction trace reader classes.
 *
 * It hides the implementation details of the STF specification and
 * provides user with all relevent information of instructions.
 *
 */

#ifndef __STF_INST_READER_HPP__
#define __STF_INST_READER_HPP__

#include <sys/stat.h>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include "stf_enums.hpp"
#include "stf_exception.hpp"
#include "stf_inst.hpp"
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
     * \class STFInstReaderBase
     * \brief The STF instruction reader provides an iterator to the instruction stream
     * that hides the details of the trace format
     *
     * Essentially, this reader will collect instruction record groups (IRGs) and return them in an
     * easy to use format.
     *
     */
    template<typename FilterType>
    class STFInstReaderBase: public STFReader {
        private:
            using IntDescriptor = descriptors::internal::Descriptor;

            static constexpr size_t DEFAULT_BUFFER_SIZE_ = 1024;

            const bool only_user_mode_ = false; // skips non-user-mode instructions if true
            const bool filter_mode_change_events_ = false; // Filters out all mode change events if true
            const size_t buffer_size_; // buffer size;
            const size_t buffer_mask_;

            bool last_inst_read_ = false;
            bool iem_changes_allowed_ = false;

            using BufferT = STFInst[]; // NOLINT: Use C-array here so we can use [] operator on the unique_ptr
            std::unique_ptr<BufferT> inst_buf_; // circular instruction buffer

            INST_IEM last_iem_ = INST_IEM::STF_INST_IEM_INVALID;         // the latest IEM
            uint32_t tgid_;             // current tgid
            uint32_t tid_;              // current tid;
            uint32_t asid_;             // current asid;
            uint32_t pte_asid_;         // current asid in pte file;
            bool pte_end_;              // whether get all initial PTEs
            STFReader pte_reader_;     // the stf-pte reader;

            std::string filename_; /**< filename */
            size_t inst_head_;         // index of head of current instruction in circular buffer;
            size_t inst_tail_;         // index of tail of current instruction in circular buffer;
#ifdef STF_INST_HAS_IEM
            bool initial_iem_ = true;
#endif

            bool skipping_enabled_ = false; // Marks all instructions read as skipped while true
            bool disable_skipping_on_next_inst_ = false; // If true, disables skipping when the next instruction is read
            bool pending_user_syscall_ = false; // If true, the instruction that is currently being processed is a user syscall
            size_t num_skipped_instructions_ = 0; // Counts number of skipped instructions so that instruction indices can be adjusted
            size_t num_insts_read_ = 0; // Counts number of instructions read from the buffer
            bool buffer_is_empty_ = true; // True if the buffer contains no instructions

            FilterType filter_;

            /**
             * \brief Helper function to read records in the separated PTE file.
             * The separate PTE trace file has header records - VERSION, COMMENT, TRACE_INFO;
             * and PTE_ASID and PTE records; One ASID associates with the following PTEs.
             */
            bool readPte_(ConstUniqueRecordHandle<PageTableWalkRecord> &pte) {
                if(pte_end_) {
                    return false;
                }

                STFRecord::UniqueHandle rec;
                while(pte_reader_ >> rec) {
                    if(STF_EXPECT_FALSE(rec->getDescriptor() == IntDescriptor::STF_PAGE_TABLE_WALK)) {
                        grabRecordOwnership(pte, rec);
                        break;
                    }
                }

                pte_end_ = !static_cast<bool>(pte_reader_);
                return !pte_end_;
            }

            // Initialization of circular buffer
            // Create the circular buffer and fill instructions from trace;
            bool initInstBuffer_() {
                inst_buf_ = std::make_unique<BufferT>(static_cast<size_t>(buffer_size_));

                size_t i = 0;
                while(i < buffer_size_) {
                    try {
                        readNextInst_(inst_buf_[i]);
                        if(STF_EXPECT_FALSE(inst_buf_[i].skipped_)) {
                            continue;
                        }
                    }
                    catch(const EOFException&) {
                        last_inst_read_ = true;
                        break;
                    }
                    ++inst_tail_;
                    ++i;
                }

                // no instruction in the file;
                if (STF_EXPECT_FALSE(inst_tail_ == 0)) {
                    buffer_is_empty_ = true;
                    return false;
                }

                buffer_is_empty_ = false;
                --inst_tail_; // Make inst_tail_ point to the last instruction read instead of one past the last instruction
                inst_head_ = 0;
                return true;
            }

            // Internal function to validate instruction by index;
            //  to prevent instruction iterator runs out put circular
            //  buffer range;
            inline void validateInstIndex_(const uint64_t index) {
                const auto& tail_inst = inst_buf_[inst_tail_];
                stf_assert(index >= inst_buf_[inst_head_].index() && (tail_inst.skipped_ || index <= tail_inst.index()),
                           "sliding window index out of range");
            }

            // get instruciton based on index/location
            // Since index and loc are paired; skip location calculation to speed up;
            inline STFInst* getInst_(const uint64_t index, const size_t loc) {
                validateInstIndex_(index);
                return &inst_buf_[loc];
            }

            // Check if the intruction index is the last;
            inline bool isLastInst_(const uint64_t index, const size_t loc) {
                validateInstIndex_(index);

                if(STF_EXPECT_TRUE(!last_inst_read_)) {
                    return false;
                }

                return loc == inst_tail_;
            }

            // Helper function for instr iteration;
            // The function does the following work;
            //  * refill the circular buffer if iterates to 2nd
            //    last instruction in the buffer;
            //  * handle buffer location crossing boundary;
            //  * pair the instruction index and buffer location;
            //
            inline bool moveToNextInst_(uint64_t &index, size_t &loc) {
                bool skip_instruction = false;

                do {
                    // validate the instruction index;
                    validateInstIndex_(index);

                    // if current location is the 2nd last inst in buffer;
                    // refill the half of the buffer;
                    if (STF_EXPECT_FALSE(loc == inst_tail_ -1)) {
                        fillHalfInstBuffer_();
                    }

                    // since 2nd last is used for refill;
                    // the tail is absolute the end of trace;
                    if (STF_EXPECT_FALSE(loc == inst_tail_)) {
                        return false;
                    }

                    index++;
                    loc = (loc + 1) & buffer_mask_;
                    if(only_user_mode_) {
                        const auto inst = getInst_(index, loc);
                        skip_instruction = inst->skipped_;
                        if(skip_instruction) {
                            --index;
                        }
                    }
                }
                while(skip_instruction);

                num_insts_read_ = index;
                return true;
            }

            /**
             * Returns the number of instructions read so far with filtering
             */
            inline size_t numInstsReadFromReader_() const {
                return rawNumInstsRead() - num_skipped_instructions_;
            }

            // Fill instruction into the half of the circular buffer;
            size_t fillHalfInstBuffer_() {
                size_t pos = inst_tail_;
                const size_t init_inst_cnt = numInstsReadFromReader_();
                const size_t max_inst_cnt = init_inst_cnt + (buffer_size_ / 2);
                while(numInstsReadFromReader_() < max_inst_cnt) {
                    pos = (pos + 1) & buffer_mask_;

                    try {
                        readNextInst_(inst_buf_[pos]);
                        if(STF_EXPECT_FALSE(inst_buf_[pos].skipped_)) {
                            pos = (pos - 1) & buffer_mask_;
                        }
                    }
                    catch(const EOFException&) {
                        last_inst_read_ = true;
                        break;
                    }
                }

                const size_t inst_cnt = numInstsReadFromReader_() - init_inst_cnt;
                // adjust head and tail;
                if (STF_EXPECT_TRUE(inst_cnt != 0)) {
                    inst_tail_ = (inst_tail_ + inst_cnt) & buffer_mask_;
                    inst_head_ = (inst_head_ + inst_cnt) & buffer_mask_;
                }

                return inst_cnt;
            }

            // read STF records to construction a STFInst instance
            __attribute__((hot, always_inline))
            inline void readNextInst_(STFInst &inst) {
                inst.reset_();
                bool ended = false;
#ifdef STF_INST_HAS_IEM
                bool iem_changed = initial_iem_;
                initial_iem_ = false;
#endif

                bool event_valid = false;
                if(STF_EXPECT_FALSE(disable_skipping_on_next_inst_)) {
                    skipping_enabled_ = false;
                    disable_skipping_on_next_inst_ = false;
                }

                pending_user_syscall_ = false;

                while(!ended) {
                    static_assert(enums::to_int(INST_MEM_ACCESS::READ) == 1,
                                  "Assumed INST_MEM_ACCESS::READ value has changed");
                    static_assert(enums::to_int(INST_MEM_ACCESS::WRITE) == 2,
                                  "Assumed INST_MEM_ACCESS::WRITE value has changed");

                    static constexpr std::array<STFInst::INSTFLAGS, 3> MEM_ACCESS_FLAGS {
                        STFInst::INST_INIT_FLAGS,
                        STFInst::INST_IS_LOAD,
                        STFInst::INST_IS_STORE
                    };

                    static_assert(enums::to_int(EXECUTION_MODE::USER_MODE) == 0,
                                  "Assumed EXECUTION_MODE::USER_MODE value has changed");
                    static_assert(enums::to_int(EXECUTION_MODE::SUPERVISOR_MODE) == 1,
                                  "Assumed EXECUTION_MODE::SUPERVISOR_MODE value has changed");
                    static_assert(enums::to_int(EXECUTION_MODE::HYPERVISOR_MODE) == 2,
                                  "Assumed EXECUTION_MODE::HYPERVISOR_MODE value has changed");
                    static_assert(enums::to_int(EXECUTION_MODE::MACHINE_MODE) == 3,
                                  "Assumed EXECUTION_MODE::MACHINE_MODE value has changed");

                    static constexpr std::array<STFInst::INSTFLAGS, 4> MODE_CHANGE_FLAGS {
                        STFInst::INST_CHANGE_TO_USER,
                        STFInst::INST_CHANGE_FROM_USER,
                        STFInst::INST_CHANGE_FROM_USER,
                        STFInst::INST_CHANGE_FROM_USER
                    };

                    const auto rec = readRecord_(inst);

                    if(!rec) {
                        event_valid = false;
                        continue;
                    }

                    const auto desc = rec->getDescriptor();
                    stf_assert(desc != IntDescriptor::STF_INST_MEM_CONTENT,
                               "Saw MemContentRecord without accompanying MemAccessRecord");

                    // These are the most common records - moving them outside of the switch statement
                    // eliminates a hard to predict indirect branch and improves performance
                    if(STF_EXPECT_TRUE(desc == IntDescriptor::STF_INST_REG)) {
                        const auto& reg_rec = rec->template as<InstRegRecord>();
                        const Registers::STF_REG_OPERAND_TYPE type = reg_rec.getOperandType();
                        inst.getOperandVector_(type).emplace_back(&reg_rec);
                        // Set FP flag if we have an FP source or dest register
                        // Set vector flag if we have a vector source or dest register
                        inst.setInstFlag_(math_utils::conditionalValue(reg_rec.isFP(), STFInst::INST_IS_FP,
                                                                       reg_rec.isVector(), STFInst::INST_IS_VECTOR));
                    }
                    else if(STF_EXPECT_TRUE(desc == IntDescriptor::STF_INST_OPCODE16)) {
                        finalizeInst_<InstOpcode16Record>(inst, rec);
                        break;
                    }
                    else if(STF_EXPECT_TRUE(desc == IntDescriptor::STF_INST_OPCODE32)) {
                        finalizeInst_<InstOpcode32Record>(inst, rec);
                        break;
                    }
                    else if(STF_EXPECT_TRUE(desc == IntDescriptor::STF_INST_MEM_ACCESS)) {
                        // Assume in the trace, INST_MEM_CONTENT always appears right
                        // after INST_MEM_ACCESS of the same memory access
                        const auto content_rec = readRecord_(inst);
                        if(STF_EXPECT_TRUE(content_rec != nullptr)) {
                            stf_assert(content_rec->getDescriptor() == IntDescriptor::STF_INST_MEM_CONTENT,
                                       "Invalid trace: memory access must be followed by memory content");

                            const auto access_type = rec->template as<InstMemAccessRecord>().getType();
                            inst.setInstFlag_(MEM_ACCESS_FLAGS[enums::to_int(access_type)]);

                            inst.getMemAccessVector_(access_type).emplace_back(rec, content_rec);
                        }
                    }
                    // These are the least common records
                    else {
                        switch(desc) {
                            case IntDescriptor::STF_INST_PC_TARGET:
                                inst.inst_flags_ |= STFInst::INST_TAKEN_BRANCH;
                                inst.branch_target_ = rec->template as<InstPCTargetRecord>().getAddr();
                                break;

                            case IntDescriptor::STF_EVENT:
                                event_valid = true;
                                {
                                    const auto& event = rec->template as<EventRecord>();
                                    const bool is_syscall = event.isSyscall();
                                    bool is_mode_change = false;

                                    inst.setInstFlag_(math_utils::conditionalValue(
                                        is_syscall, STFInst::INST_IS_SYSCALL,
                                        event.isFault(), STFInst::INST_IS_FAULT,
                                        !is_syscall && (is_mode_change = event.isModeChange()), MODE_CHANGE_FLAGS[event.getData().front()]
                                    ));

                                    if(STF_EXPECT_FALSE(is_mode_change && only_user_mode_)) {
                                        const bool is_change_to_user = inst.isChangeToUserMode();
                                        const bool is_change_from_user = !is_change_to_user;
                                        disable_skipping_on_next_inst_ |= is_change_to_user;
                                        skipping_enabled_ |= is_change_from_user;
                                    }

                                    if(STF_EXPECT_FALSE(only_user_mode_ &&
                                                        is_syscall &&
                                                        (event.getEvent() == EventRecord::TYPE::USER_ECALL))) {
                                        pending_user_syscall_ = true;
                                    }

                                    if(STF_EXPECT_FALSE((only_user_mode_ || filter_mode_change_events_) &&
                                                        is_mode_change)) {
                                        // Filter out mode change events when mode skipping or if it is explicitly required
                                        break;
                                    }

                                    inst.events_.emplace_back(rec);
                                }
                                break;

                            case IntDescriptor::STF_EVENT_PC_TARGET:
                                stf_assert(event_valid, "Saw EventPCTargetRecord without accompanying EventRecord");
                                inst.events_.back().setTarget(rec);
                                event_valid = false;
                                break;

                            case IntDescriptor::STF_FORCE_PC:
                                inst.inst_flags_ |= STFInst::INST_COF;
                                break;

                            case IntDescriptor::STF_PROCESS_ID_EXT:
                                {
                                    const auto& process_id = rec->template as<ProcessIDExtRecord>();
                                    asid_ = process_id.getASID();
                                    tid_ = process_id.getTID();
                                    tgid_ = process_id.getTGID();
                                }
                                break;

                            case IntDescriptor::STF_INST_IEM:
#ifdef STF_INST_HAS_IEM
                                iem_changed = (last_iem_ != rec->as<InstIEMRecord>().getMode());
                                stf_assert(!iem_changed || iem_changes_allowed_,
                                           "IEM changed even though IEM changes are not allowed in ISA " << getISA());
                                last_iem_ = rec->as<InstIEMRecord>().getMode();
                                break;
#endif

                            // These descriptors don't need any special handling
                            case IntDescriptor::STF_COMMENT:
                            case IntDescriptor::STF_INST_MICROOP:
                            case IntDescriptor::STF_INST_READY_REG:
                            case IntDescriptor::STF_PAGE_TABLE_WALK:
                            case IntDescriptor::STF_BUS_MASTER_ACCESS:
                            case IntDescriptor::STF_BUS_MASTER_CONTENT:
                                break;

                            // These descriptors *should* never be seen since they are header-only
                            case IntDescriptor::STF_TRACE_INFO:
                            case IntDescriptor::STF_IDENTIFIER:
                            case IntDescriptor::STF_VERSION:
                            case IntDescriptor::STF_ISA:
                            case IntDescriptor::STF_TRACE_INFO_FEATURE:
                            case IntDescriptor::STF_VLEN_CONFIG:
                            case IntDescriptor::STF_END_HEADER:
                                stf_throw("Saw an unexpected record outside of the header: " << desc);

                            // These descriptors *will* never be seen here
                            case IntDescriptor::STF_INST_REG: // handled earlier
                            case IntDescriptor::STF_INST_OPCODE16: // handled earlier
                            case IntDescriptor::STF_INST_OPCODE32: // handled earlier
                            case IntDescriptor::STF_INST_MEM_ACCESS: // handled earlier
                            case IntDescriptor::STF_INST_MEM_CONTENT: // handled earlier
                            case IntDescriptor::STF_RESERVED: // cannot be constructed
                            case IntDescriptor::STF_RESERVED_END: // cannot be constructed
                                __builtin_unreachable();
                        };
                    }
                }
            }

            /**
             * \brief Helper function to open the separated PTE file.
             * The trace file has file extension .stf and the separated PTE file
             * has .stf-pte as file extension.
             */
            bool openPTE_(std::string stffn, const bool force_single_threaded_stream = false) {
                static constexpr std::string_view stf_extension = ".stf";
                static constexpr std::string_view stf_compressed_extension = ".stf.?z";
                static constexpr std::string_view stf_xz_extension = ".stf.xz";
                static constexpr std::string_view stf_gz_extension = ".stf.gz";

                size_t len = stffn.length();
                if (len <= stf_extension.size()) {
                    return false;
                }

                // get pte file name;
                std::string ptefn;

                // Check if the file is compressed; 7 is length of ".stf.xz" etc;
                if (len > stf_compressed_extension.size()) {
                    if (stffn.rfind(stf_xz_extension) != std::string::npos) {
                        ptefn = stffn.replace(len - stf_xz_extension.size(), stf_xz_extension.size(), ".stf-pte.xz");
                    } else if (stffn.rfind(stf_gz_extension) != std::string::npos) {
                        ptefn = stffn.replace(len - stf_gz_extension.size(), stf_gz_extension.size(), ".stf-pte.gz");
                    }
                }

                // Check the uncompressed file with extension ".stf". 4 is length of extension
                if (ptefn.empty() && (len > stf_xz_extension.size())) {
                    if (stffn.find_last_of(".stf") == len - 1) {
                        ptefn = stffn + "-pte";
                    }
                }

                // check if stf-pte exist;
                struct stat buffer;
                if (ptefn.length() && (stat(ptefn.c_str(), &buffer) == 0)) {
                    pte_reader_.open(ptefn, force_single_threaded_stream);
                }

                return static_cast<bool>(pte_reader_);
            }

            /**
             * \brief Read the next STF record
             * \param rec Stores the next record content
             * \return true if record is valid; otherwise false
             */
            __attribute__((always_inline))
            inline const STFRecord* readRecord_(STFInst& inst) {
                STFRecord::UniqueHandle urec;
                operator>>(urec);

                if(STF_EXPECT_FALSE(filter_.isFiltered(urec->getDescriptor()))) {
                    return nullptr;
                }

                return inst.appendOrigRecord_(std::move(urec));
            }

            inline uint64_t getFirstIndex_() {
                return inst_buf_[inst_head_].index();
            }

            template<typename InstRecordType>
            inline void finalizeInst_(STFInst& inst, const STFRecord* const rec) {
                const auto& inst_rec = rec->as<InstRecordType>();
                inst.opcode_ = inst_rec.getOpcode();
                inst.pc_ = inst_rec.getPC();
                inst.opcode_size_ = inst_rec.getOpcodeSize();

#ifdef STF_INST_HAS_IEM
                inst.iem_ = last_iem_;
                inst.iem_changed_ = iem_changed_;
#endif

                // User syscalls need to be overridden instead of skipped
                inst.skipped_ = skipping_enabled_ && !pending_user_syscall_;
                num_skipped_instructions_ += inst.skipped_;

                if(std::is_same<InstRecordType, InstOpcode16Record>::value) {
                    inst.inst_flags_ |= STFInst::INST_OPCODE16;
                }

                // Override user syscall into a nop
                if(STF_EXPECT_FALSE(pending_user_syscall_)) {
                    inst.setNop_();
                    pending_user_syscall_ = false;
                }

                inst.index_ = numInstsReadFromReader_();

                inst.asid_ = asid_;
                inst.tid_ = tid_;
                inst.tgid_ = tgid_;

                inst.inst_flags_ |= STFInst::INST_VALID;
            }

        public:
            /**
             * \brief Constructor
             * \param filename The trace file name
             * \param only_user_mode If true, non-user-mode instructions will be skipped
             * \param check_stf_pte Check for a PTE STF
             * \param filter_mode_change_events If true, all mode change events will be filtered out
             * \param buffer_size The size of the instruction sliding window
             * \param force_single_threaded_stream If true, forces single threaded mode in reader
             */
            template<typename StrType>
            explicit STFInstReaderBase(const StrType& filename,
                                       const bool only_user_mode = false,
                                       const bool check_stf_pte = false,
                                       const bool filter_mode_change_events = false,
                                       const size_t buffer_size = DEFAULT_BUFFER_SIZE_,
                                       const bool force_single_threaded_stream = false) :
                only_user_mode_(only_user_mode),
                filter_mode_change_events_(filter_mode_change_events),
                buffer_size_(buffer_size),
                buffer_mask_(buffer_size_ - 1)
            {
                open(filename, check_stf_pte, force_single_threaded_stream);
            }

            /**
             * \class pte_iterator
             *
             * Class that implements an iterator for page table entries
             */
            class pte_iterator {
                private:
                    STFInstReaderBase *sir_ = nullptr;        // the instruction reader
                    bool end_ = true;                  // whether this is an end iterator
                    ConstUniqueRecordHandle<PageTableWalkRecord> pte_; // store the current PTE content
                    size_t index_ = 0;            // index to the PTEs

                public:
                    /**
                     * \typedef difference_type
                     * Type used for finding difference between two iterators
                     */
                    using difference_type = std::ptrdiff_t;

                    /**
                     * \typedef value_type
                     * Type pointed to by this iterator
                     */
                    using value_type = PageTableWalkRecord;

                    /**
                     * \typedef pointer
                     * Pointer to a value_type
                     */
                    using pointer = const PageTableWalkRecord*;

                    /**
                     * \typedef reference
                     * Reference to a value_type
                     */
                    using reference = const PageTableWalkRecord&;

                    /**
                     * \typedef iterator_category
                     * Iterator type - using forward_iterator_tag because backwards iteration is not currently supported
                     */
                    using iterator_category = std::forward_iterator_tag;

                    /**
                     * \brief Default constructor
                     *
                     */
                    pte_iterator() = default;

                    /**
                     * \brief Constructor
                     * \param sir The STF instruction reader to iterate
                     * \param end Whether this is an end iterator
                     *
                     */
                    explicit pte_iterator(STFInstReaderBase *sir, const bool end = false) :
                        sir_(sir),
                        index_(1)
                    {
                        if (!end) {
                            stf_assert(sir, "A pte_iterator must have a valid STFInstReader unless it is an end iterator");
                        }

                        end_ = end | sir_->pte_end_;
                        if (!end_) {
                            end_ = !sir->readPte_(pte_);
                        }
                    }

                    ~pte_iterator() = default;

                    /**
                     * \brief Pre-increment operator
                     */
                    pte_iterator & operator++() {
                        stf_assert(!end_, "Can't increment the end pte_iterator");
                        end_ = !sir_->readPte_(pte_);
                        index_ ++;
                        return *this;
                    }

                    /**
                     * \brief Post-increment operator
                     */
                    pte_iterator & operator++(int) {
                        return operator++();
                    }

                    /**
                     * \brief The equal operator to check ending
                     * \param rv The iterator to compare with
                     */
                    bool operator==(const pte_iterator& rv) const {
                        if (end_ || rv.end_) {
                            return end_ && rv.end_;
                        }

                        return index_ == rv.index_;
                    }

                    /**
                     * \brief The unequal operator to check ending
                     * \param rv The iterator to compare with
                     */
                    bool operator!=(const pte_iterator& rv) const {
                        return !operator==(rv);
                    }

                    /**
                     * \brief Return the STFPTE pointer the iterator points to
                     */
                    const auto& current() const {
                        static const decltype(pte_) NULL_PTR;

                        if (!end_) {
                            return pte_;
                        }

                        return NULL_PTR;
                    }

                    /**
                     * \brief Return the STFPTE pointer the iterator points to
                     */
                    const value_type& operator*() const { return *current(); }

                    /**
                     * \brief Return the STFPTE pointer the iterator points to
                     */
                    pointer operator->() const { return current().get(); }
            };

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
            class iterator {
                friend class STFInstReaderBase;

                private:
                    STFInstReaderBase *sir_ = nullptr;  // the instruction reader
                    uint64_t index_ = 1;            // index to the instruction stream
                    size_t loc_ = 0;                // location in the sliding window buffer;
                                                    // keep it to speed up casting;
                    bool end_ = true;               // whether this is an end iterator

                public:
                    /**
                     * \typedef difference_type
                     * Type used for finding difference between two iterators
                     */
                    using difference_type = std::ptrdiff_t;

                    /**
                     * \typedef value_type
                     * Type pointed to by this iterator
                     */
                    using value_type = STFInst;

                    /**
                     * \typedef pointer
                     * Pointer to a value_type
                     */
                    using pointer = const STFInst*;

                    /**
                     * \typedef reference
                     * Reference to a value_type
                     */
                    using reference = const value_type&;

                    /**
                     * \typedef iterator_category
                     * Iterator type - using forward_iterator_tag because backwards iteration is not currently supported
                     */
                     using iterator_category = std::forward_iterator_tag;

                    /**
                     * \brief Default constructor
                     *
                     */
                    iterator() = default;

                    /**
                     * \brief Constructor
                     * \param sir The STF instruction reader to iterate
                     * \param end Whether this is an end iterator
                     *
                     */
                    explicit iterator(STFInstReaderBase *sir, const bool end = false) :
                        sir_(sir),
                        end_(end)
                    {
                        if(!end) {
                            if (!sir_->inst_buf_) {
                                if(!sir_->initInstBuffer_()) {
                                    end_ = true;
                                }
                                loc_ = 0;
                            }
                            else {
                                end_ = sir_->buffer_is_empty_;
                            }

                            index_ = sir_->getFirstIndex_();
                        }
                    }

                    /**
                     * \brief Copy constructor
                     * \param rv The existing iterator to copy from
                     *
                     */
                    iterator(const iterator & rv) = default;

                    /**
                     * \brief Assignment operator
                     * \param rv The existing iterator to copy from
                     *
                     */
                    iterator & operator=(const iterator& rv) = default;

                    /**
                     * \brief Pre-increment operator
                     */
                    __attribute__((always_inline))
                    inline iterator & operator++() {
                        stf_assert(!end_, "Can't increment the end iterator");

                        // index_ and loc_ are increased in moveToNextInst;
                        if(STF_EXPECT_FALSE(!sir_->moveToNextInst_(index_, loc_))) {
                            end_ = true;
                        }

                        return *this;
                    }

                    /**
                     * \brief Post-increment operator
                     */
                    __attribute__((always_inline))
                    inline iterator operator++(int) {
                        auto temp = *this;
                        operator++();
                        return temp;
                    }

                    /**
                     * \brief Return the STFInst pointer the iterator points to
                     */
                    inline pointer current() const {
                        if (STF_EXPECT_FALSE(end_)) {
                            return nullptr;
                        }
                        return sir_->getInst_(index_, loc_);
                    }

                    /**
                     * \brief Returns whether the iterator is still valid
                     */
                    inline bool valid() const {
                        try {
                            // Try to get a valid pointer
                            return current();
                        }
                        catch(const STFException&) {
                            // The instruction is outside the current window, so it's invalid
                            return false;
                        }
                    }

                    /**
                     * \brief Return the STFInst pointer the iterator points to
                     */
                    inline const value_type& operator*() const { return *current(); }

                    /**
                     * \brief Return the STFInst pointer the iterator points to
                     */
                    inline pointer operator->() const { return current(); }

                    /**
                     * \brief The equal operator to check ending
                     * \param rv The iterator to compare with
                     */
                    inline bool operator==(const iterator& rv) const {
                        if (end_ || rv.end_) {
                            return end_ && rv.end_;
                        }

                        return index_ == rv.index_;
                    }

                    /**
                     * \brief The unequal operator to check ending
                     * \param rv The iterator to compare with
                     */
                    inline bool operator!=(const iterator& rv) const {
                        return !operator==(rv);
                    }

                    /**
                     * \brief whether pointing to the last instruction
                     * \return true if last instruction
                     */
                    inline bool isLastInst() const {
                        return sir_->isLastInst_(index_, loc_);
                    }
            };

            /**
             * \brief The beginning of the instruction stream
             *
             */
            inline iterator begin() { return iterator(this); }

            /**
             * \brief The end of the instruction stream
             *
             */
            inline iterator end() { return iterator(this, true); }

            /**
             * \brief The beginning of the PTE stream
             *
             */
            pte_iterator pteBegin() { return pte_iterator(this); }

            /**
             * \brief The end of the PTE stream
             *
             */
            pte_iterator pteEnd() { return pte_iterator(this, true); }

            /**
             * \brief Opens a file
             */
            void open(const std::string_view filename,
                      const bool check_stf_pte = false,
                      const bool force_single_threaded_stream = false) {
                STFReader::open(filename, force_single_threaded_stream);
                filename_ = filename;
                asid_ = 0;
                tid_ = 0;
                tgid_ = 0;
                pte_end_ = false;
                last_iem_ = getInitialIEM();
                iem_changes_allowed_ = (getISA() != ISA::RISCV);

                //open pte file;
                if (!openPTE_(filename.data())) {
                    pte_end_ = true;
                    stf_assert(!check_stf_pte, "Check for stf-pte file was enabled but no stf-pte was found for " << filename);
                }

                inst_buf_.reset();
                inst_head_ = 0;
                inst_tail_ = 0;
            }

            /**
             * \brief Closes the file
             */
            int close() {
                last_iem_ = INST_IEM::STF_INST_IEM_INVALID;
                inst_buf_.reset();
                inst_head_ = 0;
                inst_tail_ = 0;
                pte_reader_.close();
                return STFReader::close();
            }

            /**
             * Seeks the reader by the specified number of instructions and returns an iterator to that point
             * \note Intended for seeking the reader prior to reading any instructions. For seeking in a reader that
             * has already been iterated over, use the seek() method.
             */
            inline iterator seekFromBeginning(const size_t num_instructions) {
                auto it = begin();
                seek(it, num_instructions);
                return it;
            }

            /**
             * \brief Seeks an iterator by the given number of instructions
             * \param it Iterator to seek
             * \param num_instructions Number of instructions to seek by
             */
            inline void seek(iterator& it, const size_t num_instructions) {
                const size_t num_buffered = inst_tail_ - it.loc_ + 1;
                // If the instructions are already buffered or we skipping mode is enabled,
                // we have to seek the slow way
                if(only_user_mode_ || num_instructions <= num_buffered) {
                    const auto end_it = end();
                    for(size_t i = 0; i < num_instructions && it != end_it; ++i) {
                        ++it;
                    }
                }
                else {
                    // We don't need to seek the reader past instructions we've already read
                    const size_t num_to_skip = num_instructions - num_buffered;
                    STFReader::seek(num_to_skip);
                    inst_head_ = 0;
                    inst_tail_ = 0;
                    initInstBuffer_();
                    it = begin();
                }
            }

            /**
             * Returns the number of records read so far
             */
            inline size_t numRecordsRead() const {
                return STFReader::numRecordsRead();
            }

            /**
             * Returns the number of instructions read so far with filtering
             */
            inline size_t numInstsRead() const {
                return num_insts_read_;
            }

            /**
             * Returns the number of instructions read so far without filtering
             */
            inline size_t rawNumInstsRead() const {
                return STFReader::numInstsRead();
            }

            /**
             * Gets the filter object for this reader
             */
            FilterType& getFilter() {
                return filter_;
            }
    };

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
            std::bitset<descriptors::internal::NUM_DESCRIPTORS> ignored_records_;

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

    /**
     * \typedef STFInstReader
     * \brief Basic STFInst reader with no filtering
     */
    using STFInstReader = STFInstReaderBase<DummyFilter>;

    /**
     * \typedef FilteredInstReader
     * \brief STFInst reader with descriptor-based filtering
     */
    using FilteredInstReader = STFInstReaderBase<RecordFilter>;

} //end namespace stf

// __STF_INST_READER_HPP__
#endif
