#include "stf_reg_state.hpp"

#include <utility>
#include "stf_record.hpp"
#include "stf_record_types.hpp"
#include "stf_reg_def.hpp"
#include "stf_writer.hpp"

namespace stf {
    bool STFRegState::regStateUpdate(const InstRegRecord& rec) {
        return regStateUpdate(rec.getReg(), rec.getData());
    }

    bool STFRegState::regStateUpdate(Registers::STF_REG regno, uint64_t data) {
        const auto rbit = regbank.find(regno);
        if (rbit == regbank.end()) {
            return false;
        }

        const Registers::STF_REG mapped_reg = rbit->second.getMappedReg();
        const uint64_t mask = rbit->second.getMask();
        const uint32_t shift = rbit->second.getShiftBits();

        auto it = regstate.find(mapped_reg);
        if (it == regstate.end()) {
            const auto insert_result = regstate.emplace(std::piecewise_construct,
                                                        std::forward_as_tuple(mapped_reg),
                                                        std::forward_as_tuple(mapped_reg,
                                                                              Registers::STF_REG_OPERAND_TYPE::REG_STATE,
                                                                              0));
            it = insert_result.first;
        }

        it->second.setData((it->second.getData() & ~(mask<<shift)) | ((data & mask) << shift));

        return true ;
    }

    void STFRegState::getRegValue(Registers::STF_REG regno, uint64_t &data) const {
        Registers::STF_REG mapped_reg;

        const auto rbit = regbank.find(regno);
        if (rbit == regbank.end()) {
            throw RegNotFoundException();
        }
        mapped_reg = rbit->second.getMappedReg();

        const auto it = regstate.find(mapped_reg);
        if (it == regstate.end()) {
            throw RegNotFoundException();
        }

        data = ((it->second.getData() >> rbit->second.getShiftBits()) & rbit->second.getMask());
    }

    void STFRegState::writeRegState(STFWriter& stf_writer) const {
        for (const auto& r: regstate) {
            stf_writer << r.second;
        }
    }

    void STFRegState::insertSimpleRegister_(const Registers::STF_REG reg, const uint64_t mask) {
        regbank.emplace(std::piecewise_construct,
                        std::forward_as_tuple(reg),
                        std::forward_as_tuple(reg, mask));
    }

    void STFRegState::insertMappedRegister_(const Registers::STF_REG reg,
                                            const Registers::STF_REG mapped_reg,
                                            const uint64_t mask,
                                            const uint32_t shift) {
        regbank.emplace(std::piecewise_construct,
                        std::forward_as_tuple(reg),
                        std::forward_as_tuple(reg, mapped_reg, mask, shift));
    }

    void STFRegState::initRegBank(const ISA isa, const INST_IEM iem) {
        regbank.clear();

        uint64_t machine_length_mask = RegMapInfo::MASK64;
        uint64_t fp_length_mask = RegMapInfo::MASK64;

        switch(isa) {
            case ISA::RISCV:
                switch(iem) {
                    case INST_IEM::STF_INST_IEM_RV32:
                        machine_length_mask = RegMapInfo::MASK32;
                        break;
                    case INST_IEM::STF_INST_IEM_RV64:
                        machine_length_mask = RegMapInfo::MASK64;
                        break;
                    case INST_IEM::STF_INST_IEM_INVALID:
                    case INST_IEM::STF_INST_IEM_RESERVED:
                        stf_throw("Invalid IEM specified");
                }
                // FIXME: Properly handle FP length that doesn't match machine length
                fp_length_mask = machine_length_mask;

                // init GPRs
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X0, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X1, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X2, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X3, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X4, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X5, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X6, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X7, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X8, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X9, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X10, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X11, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X12, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X13, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X14, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X15, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X16, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X17, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X18, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X19, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X20, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X21, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X22, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X23, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X24, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X25, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X26, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X27, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X28, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X29, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X30, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_X31, machine_length_mask);

                // init FPRs
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F0, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F1, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F2, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F3, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F4, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F5, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F6, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F7, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F8, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F9, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F10, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F11, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F12, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F13, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F14, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F15, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F16, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F17, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F18, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F19, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F20, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F21, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F22, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F23, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F24, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F25, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F26, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F27, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F28, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F29, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F30, fp_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_F31, fp_length_mask);

                // init CSRs
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_USTATUS, machine_length_mask);
                insertMappedRegister_(Registers::STF_REG::STF_REG_CSR_UIE,
                                      Registers::STF_REG::STF_REG_CSR_USTATUS,
                                      calcRegMask<Registers::Widths::UIE_WIDTH>());

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_FCSR, RegMapInfo::MASK32);
                insertMappedRegister_(Registers::STF_REG::STF_REG_CSR_FFLAGS,
                                      Registers::STF_REG::STF_REG_CSR_FCSR,
                                      calcRegMask<Registers::Widths::FFLAGS_WIDTH>());
                insertMappedRegister_(Registers::STF_REG::STF_REG_CSR_FRM,
                                      Registers::STF_REG::STF_REG_CSR_FCSR,
                                      calcRegMask<Registers::Widths::FRM_WIDTH>(),
                                      Registers::Widths::FRM_SHIFT);

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_UIP, machine_length_mask);

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SSTATUS, machine_length_mask);
                insertMappedRegister_(Registers::STF_REG::STF_REG_CSR_SIE,
                                      Registers::STF_REG::STF_REG_CSR_SSTATUS,
                                      calcRegMask<Registers::Widths::SIE_WIDTH>(),
                                      Registers::Widths::SIE_SHIFT);

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_STVEC, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SCOUNTEREN, RegMapInfo::MASK32);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SSCRATCH, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SEPC, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SCAUSE, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_STVAL, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SIP, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SATP, machine_length_mask);

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MSTATUS, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MISA, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MEDELEG, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MIDELEG, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MIE, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MTVEC, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MCOUNTEREN, RegMapInfo::MASK32);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MSCRATCH, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MEPC, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MCAUSE, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MTVAL, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MIP, machine_length_mask);

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG0, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG1, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG2, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG3, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR0, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR1, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR2, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR3, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR4, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR5, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR6, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR7, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR8, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR9, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR10, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR11, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR12, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR13, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR14, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR15, machine_length_mask);

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MCYCLE);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MINSTRET);

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_CYCLE);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_TIME);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_INSTRET);

                if(iem == INST_IEM::STF_INST_IEM_RV32) {
                    insertMappedRegister_(Registers::STF_REG::STF_REG_CSR_MCYCLEH,
                                          Registers::STF_REG::STF_REG_CSR_MCYCLE,
                                          calcRegMask<Registers::Widths::MCYCLEH_WIDTH>(),
                                          Registers::Widths::MCYCLEH_SHIFT);

                    insertMappedRegister_(Registers::STF_REG::STF_REG_CSR_MINSTRETH,
                                          Registers::STF_REG::STF_REG_CSR_MINSTRET,
                                          calcRegMask<Registers::Widths::MINSTRETH_WIDTH>(),
                                          Registers::Widths::MINSTRETH_SHIFT);

                    insertMappedRegister_(Registers::STF_REG::STF_REG_CSR_CYCLEH,
                                          Registers::STF_REG::STF_REG_CSR_CYCLE,
                                          calcRegMask<Registers::Widths::CYCLEH_WIDTH>(),
                                          Registers::Widths::CYCLEH_SHIFT);

                    insertMappedRegister_(Registers::STF_REG::STF_REG_CSR_TIMEH,
                                          Registers::STF_REG::STF_REG_CSR_TIME,
                                          calcRegMask<Registers::Widths::TIMEH_WIDTH>(),
                                          Registers::Widths::TIMEH_SHIFT);

                    insertMappedRegister_(Registers::STF_REG::STF_REG_CSR_INSTRETH,
                                          Registers::STF_REG::STF_REG_CSR_INSTRET,
                                          calcRegMask<Registers::Widths::INSTRETH_WIDTH>(),
                                          Registers::Widths::INSTRETH_SHIFT);
                }

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MVENDORID, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MARCHID, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MIMPID, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHARTID, machine_length_mask);

                // FIXME: Guessing on these widths
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_USCRATCH, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_UEPC, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_UCAUSE, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_UTVAL, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_UTVEC, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HSTATUS, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HEDELEG, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HIDELEG, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HCOUNTEREN, RegMapInfo::MASK32);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HGATP, machine_length_mask);

                // FIXME: Couldn't find definitions for these
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_UTVT);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VSTART);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VXSAT);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VXRM);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_UNXTI);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_UINTSTATUS);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_USCRATCHCSW);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_USCRATCHCSWL);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_STVT);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SNXTI);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SINTSTATUS);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SSCRATCHCSW);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SSCRATCHCSWL);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VSSTATUS);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VSIE);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VSTVEC);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VSSCRATCH);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VSEPC);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VSCAUSE);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VSTVAL);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VSIP);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VSATP);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MTVT);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MNXTI);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MINTSTATUS);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MSCRATCHCSW);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MSCRATCHCSWL);
                break;
            case ISA::ARM:
            case ISA::POWER:
            case ISA::X86:
                stf_throw("ISA " << isa << " not yet supported");
            case ISA::RESERVED:
                stf_throw("Invalid ISA specified");
        };
    }
} // end namespace stf
