
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
#include "stf_page_table.hpp"
#include "stf_pte_reader.hpp"
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
    class STFInstReaderBase final : public STFUserModeSkippingReader<STFInst, FilterType, STFInstReaderBase<FilterType>> {
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
            bool enable_address_translation_ = false;

            bool iem_changes_allowed_ = false;

            INST_IEM last_iem_ = INST_IEM::STF_INST_IEM_INVALID;         // the latest IEM
            uint32_t hw_thread_id_ = 0;     // current hardware thread ID
            uint32_t pid_ = 0;              // current pid
            uint32_t tid_ = 0;              // current tid;
            std::unique_ptr<STFPTEReader> pte_reader_;       // the STF PTE reader;

#ifdef STF_INST_HAS_IEM
            bool initial_iem_ = true;
#endif

            bool pending_user_syscall_ = false; // If true, the instruction that is currently being processed is a user syscall
            bool buffer_is_empty_ = true; // True if the buffer contains no instructions

            std::unique_ptr<STFRegState> reg_state_; // Tracks register states when instructions are being skipped

            __attribute__((hot, always_inline))
            inline size_t rawNumRead_() const {
                return rawNumInstsRead();
            }

            // read STF records to construct an STFInst instance
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

                const bool skipping_already_enabled = skippingEnabled_(); // Check if we're already skipping instructions

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
                                    const bool is_fault = event.isFault();
                                    bool is_mode_change = false;

                                    delegates::STFInstDelegate::setFlag_(inst,
                                                                         math_utils::conditionalValue(
                                                                            is_syscall, STFInst::INST_IS_SYSCALL,
                                                                            is_fault, STFInst::INST_IS_FAULT,
                                                                            event.isInterrupt(), STFInst::INST_IS_INTERRUPT,
                                                                            !is_syscall && (is_mode_change = event.isModeChange()), MODE_CHANGE_FLAGS[event.getData().front()]
                                    ));

                                    checkSkipping_(is_mode_change, inst.isChangeToUserMode());

                                    if(STF_EXPECT_FALSE(onlyUserMode_() &&
                                                        ((is_syscall && (event.getEvent() == EventRecord::TYPE::USER_ECALL)) ||
                                                         (is_fault && !skipping_already_enabled && (event.getEvent() == EventRecord::TYPE::ILLEGAL_INST))))) {
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
                                    hw_thread_id_ = process_id.getHardwareTID();
                                    pid_ = process_id.getPID();
                                    tid_ = process_id.getTID();
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
                            case IntDescriptor::STF_INST_READY_REG:
                            case IntDescriptor::STF_PAGE_TABLE_WALK:
                            case IntDescriptor::STF_BUS_MASTER_ACCESS:
                            case IntDescriptor::STF_BUS_MASTER_CONTENT:
                            case IntDescriptor::STF_INST_MICROOP:
                                break;

                            // These descriptors *should* never be seen since they are header-only
                            case IntDescriptor::STF_IDENTIFIER:
                            case IntDescriptor::STF_ISA:
                            case IntDescriptor::STF_TRACE_INFO:
                            case IntDescriptor::STF_TRACE_INFO_FEATURE:
                            case IntDescriptor::STF_VERSION:
                            case IntDescriptor::STF_VLEN_CONFIG:
                            case IntDescriptor::STF_END_HEADER:
                                stf_throw("Saw an unexpected record outside of the header: " << desc);

                            // These descriptors should only be in transaction traces
                            case IntDescriptor::STF_PROTOCOL_ID: // STFRecord throws
                            case IntDescriptor::STF_CLOCK_ID: // STFRecord throws
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
                            case IntDescriptor::__RESERVED_END: // cannot be constructed
                                __builtin_unreachable();
                        };
                    }
                }
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
                                                         hw_thread_id_,
                                                         pid_,
                                                         tid_,
                                                         skippingEnabled_() && !pending_user_syscall_); // User syscalls need to be overridden instead of skipped
                countSkipped_(inst.skipped());

                // Override user syscall into a nop
                if(STF_EXPECT_FALSE(pending_user_syscall_)) {
                    delegates::STFInstDelegate::setNop_(inst);
                    pending_user_syscall_ = false;
                }

                initItemIndex_(inst);
                delegates::STFInstDelegate::setFlag_(inst, STFInst::INST_VALID);

                if(enable_address_translation_) {
                    delegates::STFInstDelegate::setPTEReader_(inst, pte_reader_.get());
                }
            }

            __attribute__((always_inline))
            inline void skippedCleanup_(const STFInst& inst) {
                for(const auto& op: inst.getRegisterStates()) {
                    reg_state_->regStateUpdate(op.getRecord());
                }
                for(const auto& op: inst.getSourceOperands()) {
                    reg_state_->regStateUpdate(op.getRecord());
                }
                for(const auto& op: inst.getDestOperands()) {
                    reg_state_->regStateUpdate(op.getRecord());
                }
            }

            __attribute__((always_inline))
            inline void skippingDone_(STFInst& inst) {
                delegates::STFInstDelegate::applyRegisterState_(inst, *reg_state_);
                reg_state_->stateClear();
            }

            /**
             * Disables fast seeking when non-user mode skipping is enabled
             */
            __attribute__((always_inline))
            inline bool slowSeek_() const {
                return ParentReader::slowSeek_();
            }

        public:
            using ParentReader::getInitialIEM;
            using ParentReader::getISA;

            /**
             * \brief Constructor
             * \param filename The trace file name
             * \param only_user_mode If true, non-user-mode instructions will be skipped
             * \param enable_address_translation If true, will spawn an additional thread to read page translations
             * \param filter_mode_change_events If true, all mode change events will be filtered out
             * \param buffer_size The size of the instruction sliding window
             * \param force_single_threaded_stream If true, forces single threaded mode in reader
             */
            template<typename StrType>
            explicit STFInstReaderBase(const StrType& filename,
                                       const bool only_user_mode = false,
                                       const bool enable_address_translation = false,
                                       const bool filter_mode_change_events = false,
                                       const size_t buffer_size = DEFAULT_BUFFER_SIZE_,
                                       const bool force_single_threaded_stream = false) :
                ParentReader(only_user_mode, buffer_size),
                filter_mode_change_events_(filter_mode_change_events)
            {
                open(filename, enable_address_translation, force_single_threaded_stream);
            }

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
             * \brief Opens a file
             * \param filename The trace file name
             * \param enable_address_translation If true, will spawn an additional thread to read page translations
             * \param force_single_threaded_stream If true, forces single threaded mode in reader
             */
            void open(const std::string_view filename,
                      const bool enable_address_translation = false,
                      const bool force_single_threaded_stream = false) {
                ParentReader::open(filename, force_single_threaded_stream);
                hw_thread_id_ = 0;
                pid_ = 0;
                tid_ = 0;
                last_iem_ = getInitialIEM();
                iem_changes_allowed_ = (getISA() != ISA::RISCV);
                enable_address_translation_ = enable_address_translation;

                if(enable_address_translation_) {
                    if(!pte_reader_) {
                        pte_reader_ = std::make_unique<STFPTEReader>(filename);
                    }
                    else {
                        pte_reader_->open(filename);
                    }
                }

                if(STF_EXPECT_TRUE(reg_state_)) {
                    reg_state_->initRegBank(getISA(), getInitialIEM());
                }
                else {
                    reg_state_ = std::make_unique<STFRegState>(getISA(), getInitialIEM());
                }
            }

            /**
             * \brief Closes the file
             */
            int close() final {
                last_iem_ = INST_IEM::STF_INST_IEM_INVALID;
                if(enable_address_translation_ && pte_reader_) {
                    pte_reader_->close();
                }
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
