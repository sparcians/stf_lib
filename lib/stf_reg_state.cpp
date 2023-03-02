#include "stf_reg_state.hpp"

#include <utility>
#include "stf_record.hpp"
#include "stf_record_types.hpp"
#include "stf_reg_def.hpp"
#include "stf_writer.hpp"

namespace stf {
    // cppcheck-suppress unusedFunction
    void STFRegState::writeRegState(STFWriter& stf_writer) const {
        applyRegState(
            [&stf_writer](const auto& r) {
                stf_writer << *r.second;
            }
        );
    }

    void STFRegState::initRegBank(const ISA isa, const INST_IEM iem) {
        regbank_.clear();

        uint64_t machine_length_mask = RegMapInfo::MASK64;
        uint64_t fp_length_mask = RegMapInfo::MASK64;
        static constexpr uint64_t vec_length_mask = RegMapInfo::MASK64;

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
                insertSimpleRegister_(Registers::STF_REG::STF_REG_PC, machine_length_mask);

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
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SEDELEG, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SIDELEG, machine_length_mask);

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_STVEC, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SCOUNTEREN, RegMapInfo::MASK32);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SSCRATCH, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SEPC, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SCAUSE, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_STVAL, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SIP, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SATP, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SENVCFG, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SENVCFG_COMPAT, machine_length_mask);

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_DMCONTROL, RegMapInfo::MASK32);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_DMSTATUS, RegMapInfo::MASK32);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_TSELECT, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_TDATA1, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_TDATA2, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_TDATA3, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_TINFO, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_TCONTROL, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MCONTEXT, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_SCONTEXT, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_DCSR, RegMapInfo::MASK32);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_DPC, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_DSCRATCH0, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_DSCRATCH1, machine_length_mask);

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
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG4, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG5, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG6, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG7, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG8, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG9, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG10, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG11, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG12, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG13, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG14, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPCFG15, machine_length_mask);
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
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR16, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR17, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR18, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR19, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR20, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR21, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR22, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR23, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR24, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR25, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR26, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR27, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR28, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR29, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR30, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR31, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR32, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR33, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR34, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR35, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR36, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR37, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR38, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR39, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR40, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR41, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR42, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR43, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR44, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR45, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR46, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR47, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR48, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR49, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR50, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR51, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR52, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR53, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR54, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR55, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR56, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR57, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR58, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR59, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR60, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR61, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR62, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_PMPADDR63, machine_length_mask);

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MCYCLE);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MINSTRET);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER3, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER4, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER5, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER6, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER7, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER8, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER9, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER10, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER11, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER12, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER13, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER14, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER15, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER16, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER17, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER18, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER19, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER20, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER21, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER22, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER23, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER24, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER25, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER26, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER27, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER28, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER29, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER30, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER31, machine_length_mask);

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_CYCLE);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_TIME);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_INSTRET);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER3, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER4, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER5, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER6, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER7, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER8, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER9, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER10, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER11, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER12, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER13, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER14, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER15, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER16, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER17, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER18, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER19, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER20, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER21, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER22, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER23, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER24, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER25, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER26, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER27, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER28, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER29, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER30, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HPMCOUNTER31, machine_length_mask);

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VL);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VTYPE);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VLENB);

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

                    insertMappedRegister_(Registers::STF_REG::STF_REG_CSR_MSECCFGH,
                                          Registers::STF_REG::STF_REG_CSR_MSECCFG,
                                          calcRegMask<Registers::Widths::MSECCFGH_WIDTH>(),
                                          Registers::Widths::MSECCFGH_SHIFT);

                    insertMappedRegister_(Registers::STF_REG::STF_REG_CSR_MSTATUSH,
                                          Registers::STF_REG::STF_REG_CSR_MSTATUS,
                                          calcRegMask<Registers::Widths::MSTATUSH_WIDTH>(),
                                          Registers::Widths::MSTATUSH_SHIFT);

                    insertMappedRegister_(Registers::STF_REG::STF_REG_CSR_MENVCFGH,
                                          Registers::STF_REG::STF_REG_CSR_MENVCFG,
                                          calcRegMask<Registers::Widths::MENVCFGH_WIDTH>(),
                                          Registers::Widths::MENVCFGH_SHIFT);
                }

                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MVENDORID, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MARCHID, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MIMPID, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHARTID, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MSECCFG, machine_length_mask);

                // FIXME: Guessing on these widths
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_USCRATCH, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_UEPC, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_UCAUSE, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_UTVAL, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_UTVEC, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HSTATUS, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HEDELEG, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HIDELEG, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HIE, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HTIMEDELTA, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HCOUNTEREN, RegMapInfo::MASK32);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HGEIE, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HENVCFG, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HTVAL, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HIP, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HVIP, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HTINST, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HGATP, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HCONTEXT, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_HGEIP, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MCONFIGPTR, machine_length_mask);

                // FIXME: Couldn't find definitions for these
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_UTVT);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VSTART);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VXSAT);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VXRM);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_VCSR);
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
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MENVCFG, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MCOUNTINHIBIT, RegMapInfo::MASK32);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT3, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT4, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT5, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT6, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT7, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT8, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT9, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT10, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT11, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT12, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT13, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT14, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT15, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT16, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT17, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT18, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT19, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT20, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT21, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT22, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT23, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT24, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT25, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT26, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT27, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT28, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT29, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT30, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MHPMEVENT31, machine_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MNXTI);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MINTSTATUS);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MSCRATCHCSW);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MSCRATCHCSWL);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MTINST);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_CSR_MTVAL2);

                // init vector registers
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V0, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V1, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V2, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V3, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V4, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V5, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V6, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V7, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V8, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V9, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V10, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V11, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V12, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V13, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V14, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V15, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V16, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V17, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V18, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V19, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V20, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V21, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V22, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V23, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V24, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V25, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V26, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V27, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V28, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V29, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V30, vec_length_mask);
                insertSimpleRegister_(Registers::STF_REG::STF_REG_V31, vec_length_mask);

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
