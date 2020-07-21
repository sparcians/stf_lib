#include "stf_inst_reader.hpp"

#include <iostream>
#include <vector>

#include "format_utils.hpp"
#include "stf_enums.hpp"
#include "stf_exception.hpp"
#include "stf_inst.hpp"
#include "stf_record.hpp"
#include "stf_record_types.hpp"
#include "stf_serializable_container.hpp"
#include "util.hpp"

namespace stf {
    __attribute__((hot))
    void STFInstReader::readNextInst_(STFInst &inst) {
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
            static_assert(enums::to_int(INST_MEM_ACCESS::READ) == 1, "Assumed INST_MEM_ACCESS::READ value has changed");
            static_assert(enums::to_int(INST_MEM_ACCESS::WRITE) == 2, "Assumed INST_MEM_ACCESS::WRITE value has changed");

            static constexpr std::array<STFInst::INSTFLAGS, 3> MEM_ACCESS_FLAGS {STFInst::INST_INIT_FLAGS,
                                                                                 STFInst::INST_IS_LOAD,
                                                                                 STFInst::INST_IS_STORE};

            static_assert(enums::to_int(EXECUTION_MODE::USER_MODE) == 0, "Assumed EXECUTION_MODE::USER_MODE value has changed");
            static_assert(enums::to_int(EXECUTION_MODE::SUPERVISOR_MODE) == 1, "Assumed EXECUTION_MODE::SUPERVISOR_MODE value has changed");
            static_assert(enums::to_int(EXECUTION_MODE::HYPERVISOR_MODE) == 2, "Assumed EXECUTION_MODE::HYPERVISOR_MODE value has changed");
            static_assert(enums::to_int(EXECUTION_MODE::MACHINE_MODE) == 3, "Assumed EXECUTION_MODE::MACHINE_MODE value has changed");

            static constexpr std::array<STFInst::INSTFLAGS, 4> MODE_CHANGE_FLAGS {STFInst::INST_CHANGE_TO_USER,
                                                                                  STFInst::INST_CHANGE_FROM_USER,
                                                                                  STFInst::INST_CHANGE_FROM_USER,
                                                                                  STFInst::INST_CHANGE_FROM_USER};

            const auto rec = readRecord_(inst);
            const auto desc = rec->getDescriptor();

            if(!rec) {
                event_valid = false;
                continue;
            }

            stf_assert(desc != descriptors::internal::Descriptor::STF_INST_MEM_CONTENT,
                       "Saw MemContentRecord without accompanying MemAccessRecord");

            // These are the most common records - moving them outside of the switch statement
            // eliminates a hard to predict indirect branch and improves performance
            if(STF_EXPECT_TRUE(desc == descriptors::internal::Descriptor::STF_INST_REG)) {
                const auto& reg_rec = rec->as<InstRegRecord>();
                const Registers::STF_REG_OPERAND_TYPE type = reg_rec.getOperandType();
                inst.getOperandVector_(type).emplace_back(&reg_rec);
                // Set FP flag if we have an FP source or dest register
                math_utils::conditionalSet(inst.inst_flags_,
                                           STFInst::INST_IS_FP,
                                           stf::Registers::isFPR(reg_rec.getReg()));
            }
            else if(STF_EXPECT_TRUE(desc == descriptors::internal::Descriptor::STF_INST_OPCODE16)) {
                finalizeInst_<InstOpcode16Record>(inst, rec);
                break;
            }
            else if(STF_EXPECT_TRUE(desc == descriptors::internal::Descriptor::STF_INST_OPCODE32)) {
                finalizeInst_<InstOpcode32Record>(inst, rec);
                break;
            }
            else if(STF_EXPECT_TRUE(desc == descriptors::internal::Descriptor::STF_INST_MEM_ACCESS)) {
                // Assume in the trace, INST_MEM_CONTENT always appear right
                // after INST_MEM_ACCESS of the same memory access
                const auto content_rec = readRecord_(inst);
                stf_assert(content_rec->getDescriptor() == descriptors::internal::Descriptor::STF_INST_MEM_CONTENT,
                           "Invalid trace: memory access must be followed by memory content");

                const auto access_type = rec->as<InstMemAccessRecord>().getType();
                inst.setInstFlag_(MEM_ACCESS_FLAGS[enums::to_int(access_type)]);

                inst.getMemAccessVector_(access_type).emplace_back(rec, content_rec);
            }
            // These are the least common records
            else {
                switch(desc) {
                    case descriptors::internal::Descriptor::STF_INST_REG:
                    case descriptors::internal::Descriptor::STF_INST_OPCODE16:
                    case descriptors::internal::Descriptor::STF_INST_OPCODE32:
                    case descriptors::internal::Descriptor::STF_INST_MEM_ACCESS:
                    case descriptors::internal::Descriptor::STF_INST_MEM_CONTENT:
                        break;

                    case descriptors::internal::Descriptor::STF_INST_PC_TARGET:
                        inst.inst_flags_ |= STFInst::INST_TAKEN_BRANCH;
                        inst.branch_target_ = rec->as<InstPCTargetRecord>().getAddr();
                        break;

                    case descriptors::internal::Descriptor::STF_EVENT:
                        event_valid = true;
                        {
                            const auto& event = rec->as<EventRecord>();
                            const bool is_syscall = event.isSyscall();
                            bool is_mode_change = false;

                            inst.setInstFlag_(math_utils::conditionalValue(
                                is_syscall, STFInst::INST_IS_SYSCALL,
                                !is_syscall && (is_mode_change = event.isModeChange()), MODE_CHANGE_FLAGS[event.getData().front()]
                            ));

                            if(STF_EXPECT_FALSE(is_mode_change && only_user_mode_)) {
                                const bool is_change_to_user = inst.isChangeToUserMode();
                                const bool is_change_from_user = !is_change_to_user;
                                disable_skipping_on_next_inst_ |= is_change_to_user;
                                skipping_enabled_ |= is_change_from_user;
                            }

                            if(STF_EXPECT_FALSE(only_user_mode_ && is_syscall && (event.getEvent() == EventRecord::TYPE::USER_ECALL))) {
                                pending_user_syscall_ = true;
                            }

                            if(STF_EXPECT_FALSE((only_user_mode_ || filter_mode_change_events_) && is_mode_change)) {
                                // Filter out mode change events when mode skipping or if it is explicitly required
                                break;
                            }

                            inst.events_.emplace_back(rec);
                        }
                        break;

                    case descriptors::internal::Descriptor::STF_EVENT_PC_TARGET:
                        stf_assert(event_valid, "Saw EventPCTargetRecord without accompanying EventRecord");
                        inst.events_.back().setTarget(rec);
                        event_valid = false;
                        break;

                    case descriptors::internal::Descriptor::STF_FORCE_PC:
                        inst.inst_flags_ |= STFInst::INST_COF;
                        break;

                    case descriptors::internal::Descriptor::STF_PROCESS_ID_EXT:
                        {
                            const auto& process_id = rec->as<ProcessIDExtRecord>();
                            asid_ = process_id.getASID();
                            tid_ = process_id.getTID();
                            tgid_ = process_id.getTGID();
                        }
                        break;

                    case descriptors::internal::Descriptor::STF_INST_IEM:
#ifdef STF_INST_HAS_IEM
                        iem_changed = (last_iem_ != rec->as<InstIEMRecord>().getMode());
                        stf_assert(!iem_changed || iem_changes_allowed_,
                                   "IEM changed even though IEM changes are not allowed in ISA " << getISA());
                        last_iem_ = rec->as<InstIEMRecord>().getMode();
                        break;
#endif
                    case descriptors::internal::Descriptor::STF_COMMENT:
                    case descriptors::internal::Descriptor::STF_INST_MICROOP:
                    case descriptors::internal::Descriptor::STF_INST_READY_REG:
                    case descriptors::internal::Descriptor::STF_PAGE_TABLE_WALK:
                    case descriptors::internal::Descriptor::STF_TRACE_INFO:
                    case descriptors::internal::Descriptor::STF_RESERVED:
                    case descriptors::internal::Descriptor::STF_IDENTIFIER:
                    case descriptors::internal::Descriptor::STF_VERSION:
                    case descriptors::internal::Descriptor::STF_ISA:
                    case descriptors::internal::Descriptor::STF_TRACE_INFO_FEATURE:
                    case descriptors::internal::Descriptor::STF_END_HEADER:
                    case descriptors::internal::Descriptor::STF_BUS_MASTER_ACCESS:
                    case descriptors::internal::Descriptor::STF_BUS_MASTER_CONTENT:
                    case descriptors::internal::Descriptor::STF_RESERVED_END:
                        break;
                };
            }
        }
    }

    void STFInstReader::open(const std::string_view filename, const bool check_stf_pte) {
        STFReader::open(filename);
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

    int STFInstReader::close() {
        last_iem_ = INST_IEM::STF_INST_IEM_INVALID;
        inst_buf_.reset();
        inst_head_ = 0;
        inst_tail_ = 0;
        pte_reader_.close();
        return STFReader::close();
    }
} // end namespace stf
