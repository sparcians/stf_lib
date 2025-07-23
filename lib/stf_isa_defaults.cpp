#include "stf_isa_defaults.hpp"

namespace stf {
    #define DEFAULT_RISCV_ISA_EXTENSIONS "imafdcvh_zicbom_zicbop_zicboz_zicsr_zicond_zifencei_zihintpause_zfhmin_zba_zbb_zbs_zihintntl_zvbb_zvbc_zvkg_zvkned_zvknhb_zvksed_zvksh_zvkt_smaia_smstateen_ss1p12_ssaia_sscofpmf_ssstateen_sv48_svinval_svnapot_svpbmt"
    #define GEN_DEFAULT_RISCV_ISA_STRING(bits) "rv" #bits DEFAULT_RISCV_ISA_EXTENSIONS

    const std::string ISADefaults::empty_;
    const std::string ISADefaults::rv32_isa_extended_{GEN_DEFAULT_RISCV_ISA_STRING(32)};
    const std::string ISADefaults::rv64_isa_extended_{GEN_DEFAULT_RISCV_ISA_STRING(64)};

    const std::string& ISADefaults::getISAExtendedInfo(const ISA isa, const INST_IEM iem) {
        static const ISADefaults defaults;

        switch(isa) {
            case ISA::RISCV:
                switch(iem) {
                    case INST_IEM::STF_INST_IEM_RV32:
                        return defaults.rv32_isa_extended_;

                    case INST_IEM::STF_INST_IEM_RV64:
                        return defaults.rv64_isa_extended_;

                    default:
                        stf_throw("Invalid INST_IEM for RISCV: " << iem);
                }
            case ISA::ARM:
            case ISA::X86:
            case ISA::POWER:
                break;
            case ISA::RESERVED:
            case ISA::__RESERVED_END:
                stf_throw("Invalid ISA type");
        }

        return defaults.empty_;
    }
} // end namespace stf
