
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
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include "stf_user_mode_skipping_reader.hpp"
#include "stf_enums.hpp"
#include "stf_exception.hpp"
#include "stf_filter_types.hpp"
#include "stf_inst.hpp"
#include "stf_record.hpp"
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
    class STFInstReaderBase: public STFUserModeSkippingReader<STFInst, FilterType, STFInstReaderBase<FilterType>> {
        private:
            using ParentReader = STFUserModeSkippingReader<STFInst, FilterType, STFInstReaderBase<FilterType>>;
            friend ParentReader;
            /// \cond DOXYGEN_IGNORED
            friend typename ParentReader::BufferedReader;
            /// \endcond

            using ParentReader::DEFAULT_BUFFER_SIZE_;
            using IntDescriptor = typename ParentReader::IntDescriptor;
            using ParentReader::getItem_;
            using ParentReader::initItemIndex_;
            using ParentReader::readRecord_;
            using ParentReader::countSkipped_;
            using ParentReader::numItemsReadFromReader_;
            using ParentReader::updateSkipping_;
            using ParentReader::checkSkipping_;
            using ParentReader::onlyUserMode_;
            using ParentReader::skippingEnabled_;

            const bool filter_mode_change_events_ = false; // Filters out all mode change events if true

            bool iem_changes_allowed_ = false;

            INST_IEM last_iem_ = INST_IEM::STF_INST_IEM_INVALID;         // the latest IEM
            uint32_t tgid_;             // current tgid
            uint32_t tid_;              // current tid;
            uint32_t asid_;             // current asid;
            uint32_t pte_asid_;         // current asid in pte file;
            bool pte_end_;              // whether get all initial PTEs
            STFReader pte_reader_;     // the stf-pte reader;

#ifdef STF_INST_HAS_IEM
            bool initial_iem_ = true;
#endif

            bool pending_user_syscall_ = false; // If true, the instruction that is currently being processed is a user syscall
            bool buffer_is_empty_ = true; // True if the buffer contains no instructions

            /**
             * \brief Helper function to read records in the separated PTE file.
             * The separate PTE trace file has header records - VERSION, COMMENT, TRACE_INFO;
             * and PTE_ASID and PTE records; One ASID associates with the following PTEs.
             */
            bool readPte_(STFRecord::ConstHandle<PageTableWalkRecord> &pte) {
                if(pte_end_) {
                    return false;
                }

                STFRecord::UniqueHandle rec;
                while(pte_reader_ >> rec) {
                    if(STF_EXPECT_FALSE(rec->getId() == IntDescriptor::STF_PAGE_TABLE_WALK)) {
                        STFRecord::grabOwnership(pte, rec);
                        break;
                    }
                }

                pte_end_ = !static_cast<bool>(pte_reader_);
                return !pte_end_;
            }

            __attribute__((hot, always_inline))
            inline size_t rawNumRead_() const {
                return rawNumInstsRead();
            }

            // read STF records to construction a STFInst instance
            __attribute__((hot, always_inline))
            inline void readNext_(STFInst& inst) {
                delegates::STFInstDelegate::reset_(inst);
#ifdef STF_INST_HAS_IEM
                bool iem_changed = initial_iem_;
                initial_iem_ = false;
#endif

                bool event_valid = false;

                updateSkipping_();

                pending_user_syscall_ = false;

                while(true) {
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

                    const auto desc = rec->getId();
                    stf_assert(desc != IntDescriptor::STF_INST_MEM_CONTENT,
                               "Saw MemContentRecord without accompanying MemAccessRecord");

                    // These are the most common records - moving them outside of the switch statement
                    // eliminates a hard to predict indirect branch and improves performance
                    if(STF_EXPECT_TRUE(desc == IntDescriptor::STF_INST_REG)) {
                        const auto& reg_rec = rec->template as<InstRegRecord>();
                        const Registers::STF_REG_OPERAND_TYPE type = reg_rec.getOperandType();
                        delegates::STFInstDelegate::appendOperand_(inst, type, reg_rec);
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
                            stf_assert(content_rec->getId() == IntDescriptor::STF_INST_MEM_CONTENT,
                                       "Invalid trace: memory access must be followed by memory content");

                            const auto access_type = rec->template as<InstMemAccessRecord>().getType();
                            delegates::STFInstDelegate::setFlag_(inst, MEM_ACCESS_FLAGS[enums::to_int(access_type)]);

                            delegates::STFInstDelegate::appendMemAccess_(inst, access_type, rec, content_rec);
                        }
                    }
                    // These are the least common records
                    else {
                        switch(desc) {
                            case IntDescriptor::STF_INST_PC_TARGET:
                                delegates::STFInstDelegate::setTakenBranch_(inst,
                                                                            rec->template as<InstPCTargetRecord>().getAddr());
                                break;

                            case IntDescriptor::STF_EVENT:
                                event_valid = true;
                                {
                                    const auto& event = rec->template as<EventRecord>();
                                    const bool is_syscall = event.isSyscall();
                                    bool is_mode_change = false;

                                    delegates::STFInstDelegate::setFlag_(inst,
                                                                         math_utils::conditionalValue(
                                                                            is_syscall, STFInst::INST_IS_SYSCALL,
                                                                            event.isFault(), STFInst::INST_IS_FAULT,
                                                                            event.isInterrupt(), STFInst::INST_IS_INTERRUPT,
                                                                            !is_syscall && (is_mode_change = event.isModeChange()), MODE_CHANGE_FLAGS[event.getData().front()]
                                    ));

                                    checkSkipping_(is_mode_change, inst.isChangeToUserMode());

                                    if(STF_EXPECT_FALSE(onlyUserMode_() &&
                                                        is_syscall &&
                                                        (event.getEvent() == EventRecord::TYPE::USER_ECALL))) {
                                        pending_user_syscall_ = true;
                                    }

                                    if(STF_EXPECT_FALSE((onlyUserMode_() || filter_mode_change_events_) &&
                                                        is_mode_change)) {
                                        // Filter out mode change events when mode skipping or if it is explicitly required
                                        break;
                                    }

                                    delegates::STFInstDelegate::appendEvent_(inst, rec);
                                }
                                break;

                            case IntDescriptor::STF_EVENT_PC_TARGET:
                                stf_assert(event_valid, "Saw EventPCTargetRecord without accompanying EventRecord");
                                delegates::STFInstDelegate::setLastEventTarget_(inst, rec);
                                event_valid = false;
                                break;

                            case IntDescriptor::STF_FORCE_PC:
                                delegates::STFInstDelegate::setFlag_(inst, STFInst::INST_COF);
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

                            // These descriptors should only be in transaction traces
                            case IntDescriptor::STF_PROTOCOL_ID: // STFRecord throws
                            case IntDescriptor::STF_TRANSACTION: // STFRecord throws
                            case IntDescriptor::STF_TRANSACTION_DEPENDENCY: // STFRecord throws
                                stf_throw("Saw a transaction record " << desc << " in an instruction trace");

                            // These descriptors *will* never be seen here
                            case IntDescriptor::STF_INST_REG: // handled earlier
                            case IntDescriptor::STF_INST_OPCODE16: // handled earlier
                            case IntDescriptor::STF_INST_OPCODE32: // handled earlier
                            case IntDescriptor::STF_INST_MEM_ACCESS: // handled earlier
                            case IntDescriptor::STF_INST_MEM_CONTENT: // handled earlier
                            case IntDescriptor::STF_RESERVED: // cannot be constructed
                            case IntDescriptor::RESERVED_END: // cannot be constructed
                                __builtin_unreachable();
                        };
                    }
                }
            }

            /**
             * \brief Helper function to open the separated PTE file.
             * The trace file has file extension .stf and the separated PTE file
             * has .stf-pte as file extension.
             * \param stffn The trace file name
             * \param force_single_threaded_stream If true, forces single threaded mode in reader
             */
            bool openPTE_(const std::string_view stffn, const bool force_single_threaded_stream = false) {
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
                        ptefn = stffn;
                        ptefn.replace(len - stf_xz_extension.size(), stf_xz_extension.size(), ".stf-pte.xz");
                    } else if (stffn.rfind(stf_gz_extension) != std::string::npos) {
                        ptefn = stffn;
                        ptefn.replace(len - stf_gz_extension.size(), stf_gz_extension.size(), ".stf-pte.gz");
                    }
                }

                // Check the uncompressed file with extension ".stf". 4 is length of extension
                if (ptefn.empty() && (len > stf_xz_extension.size())) {
                    if (stffn.find_last_of(".stf") == len - 1) {
                        ptefn = stffn;
                        ptefn += "-pte";
                    }
                }

                // check if stf-pte exist;
                struct stat buffer;
                if (!ptefn.empty() && (stat(ptefn.c_str(), &buffer) == 0)) {
                    pte_reader_.open(ptefn, force_single_threaded_stream);
                }

                return static_cast<bool>(pte_reader_);
            }

            __attribute__((always_inline))
            inline const STFRecord* handleNewRecord_(STFInst& inst, STFRecord::UniqueHandle&& urec) {
                return delegates::STFInstDelegate::appendOrigRecord_(inst, std::move(urec));
            }

            template<typename InstRecordType>
            inline void finalizeInst_(STFInst& inst, const STFRecord* const rec) {
                const auto& inst_rec = rec->as<InstRecordType>();
                delegates::STFInstDelegate::setInstInfo_(inst,
                                                         inst_rec,
                                                         last_iem_,
#ifdef STF_INST_HAS_IEM
                                                         iem_changed_,
#endif
                                                         asid_,
                                                         tid_,
                                                         tgid_,
                                                         skippingEnabled_() && !pending_user_syscall_); // User syscalls need to be overridden instead of skipped
                countSkipped_(inst.skipped());

                // Override user syscall into a nop
                if(STF_EXPECT_FALSE(pending_user_syscall_)) {
                    delegates::STFInstDelegate::setNop_(inst);
                    pending_user_syscall_ = false;
                }

                initItemIndex_(inst);
                delegates::STFInstDelegate::setFlag_(inst, STFInst::INST_VALID);
            }

        public:
            using ParentReader::getInitialIEM;
            using ParentReader::getISA;

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
                ParentReader(only_user_mode, buffer_size),
                filter_mode_change_events_(filter_mode_change_events)
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
                    STFRecord::ConstHandle<PageTableWalkRecord> pte_; // store the current PTE content
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
             * \brief Instruction stream iterator
             */
            class iterator : public ParentReader::base_iterator {
                public:
                    iterator() = default;

                    /**
                     * Iterator constructor
                     * \param sir Parent STFInstReaderBase
                     * \param end If true, this is an end iterator
                     */
                    explicit iterator(STFInstReaderBase *sir, const bool end = false) :
                        ParentReader::base_iterator(sir, end)
                    {
                    }

                    /**
                     * Returns whether this is the last instruction in the trace
                     */
                    inline bool isLastInst() const {
                        return ParentReader::base_iterator::isLastItem_();
                    }
            };

            /**
             * \brief The beginning of the PTE stream
             *
             */
            inline pte_iterator pteBegin() { return pte_iterator(this); }

            /**
             * \brief The end of the PTE stream
             *
             */
            inline const pte_iterator& pteEnd() {
                static const auto end_it = pte_iterator(this, true);
                return end_it;
            }

            /**
             * \brief Opens a file
             * \param filename The trace file name
             * \param check_stf_pte Check for a PTE STF
             * \param force_single_threaded_stream If true, forces single threaded mode in reader
             */
            void open(const std::string_view filename,
                      const bool check_stf_pte = false,
                      const bool force_single_threaded_stream = false) {
                ParentReader::open(filename, force_single_threaded_stream);
                asid_ = 0;
                tid_ = 0;
                tgid_ = 0;
                pte_end_ = false;
                last_iem_ = getInitialIEM();
                iem_changes_allowed_ = (getISA() != ISA::RISCV);

                //open pte file;
                if (!openPTE_(filename)) {
                    pte_end_ = true;
                    stf_assert(!check_stf_pte, "Check for stf-pte file was enabled but no stf-pte was found for " << filename);
                }
            }

            /**
             * \brief Closes the file
             */
            int close() final {
                last_iem_ = INST_IEM::STF_INST_IEM_INVALID;
                pte_reader_.close();
                return ParentReader::close();
            }

            /**
             * Returns the number of instructions read so far with filtering
             */
            inline size_t numInstsRead() const {
                return ParentReader::numItemsRead_();
            }

            /**
             * Returns the number of instructions read so far without filtering
             */
            __attribute__((always_inline))
            inline size_t rawNumInstsRead() const {
                return STFReader::numInstsRead();
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
