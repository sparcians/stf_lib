#include <ostream>
#include <iomanip>
#include "format_utils.hpp"
#include "stf_reg_def.hpp"

namespace stf {
    void Registers::formatCSR_(std::ostream& os, const Registers::STF_REG regno) {
        switch(regno) {
            case STF_REG::STF_REG_CSR_USTATUS:
                os << "REG_CSR_USTATUS";
                break;
            case STF_REG::STF_REG_CSR_FFLAGS:
                os << "REG_CSR_FFLAGS";
                break;
            case STF_REG::STF_REG_CSR_FRM:
                os << "REG_CSR_FRM";
                break;
            case STF_REG::STF_REG_CSR_FCSR:
                os << "REG_CSR_FCSR";
                break;
            case STF_REG::STF_REG_CSR_UIE:
                os << "REG_CSR_UIE";
                break;
            case STF_REG::STF_REG_CSR_UTVEC:
                os << "REG_CSR_UTVEC";
                break;
            case STF_REG::STF_REG_CSR_UTVT:
                os << "REG_CSR_UTVT";
                break;
            case STF_REG::STF_REG_CSR_VSTART:
                os << "REG_CSR_VSTART";
                break;
            case STF_REG::STF_REG_CSR_VXSAT:
                os << "REG_CSR_VXSAT";
                break;
            case STF_REG::STF_REG_CSR_VXRM:
                os << "REG_CSR_VXRM";
                break;
            case STF_REG::STF_REG_CSR_USCRATCH:
                os << "REG_CSR_USCRATCH";
                break;
            case STF_REG::STF_REG_CSR_UEPC:
                os << "REG_CSR_UEPC";
                break;
            case STF_REG::STF_REG_CSR_UCAUSE:
                os << "REG_CSR_UCAUSE";
                break;
            case STF_REG::STF_REG_CSR_UTVAL:
                os << "REG_CSR_UTVAL";
                break;
            case STF_REG::STF_REG_CSR_UIP:
                os << "REG_CSR_UIP";
                break;
            case STF_REG::STF_REG_CSR_UNXTI:
                os << "REG_CSR_UNXTI";
                break;
            case STF_REG::STF_REG_CSR_UINTSTATUS:
                os << "REG_CSR_UINTSTATUS";
                break;
            case STF_REG::STF_REG_CSR_USCRATCHCSW:
                os << "REG_CSR_USCRATCHCSW";
                break;
            case STF_REG::STF_REG_CSR_USCRATCHCSWL:
                os << "REG_CSR_USCRATCHCSWL";
                break;
            case STF_REG::STF_REG_CSR_SSTATUS:
                os << "REG_CSR_SSTATUS";
                break;
            case STF_REG::STF_REG_CSR_SIE:
                os << "REG_CSR_SIE";
                break;
            case STF_REG::STF_REG_CSR_STVEC:
                os << "REG_CSR_STVEC";
                break;
            case STF_REG::STF_REG_CSR_SCOUNTEREN:
                os << "REG_CSR_SCOUNTEREN";
                break;
            case STF_REG::STF_REG_CSR_STVT:
                os << "REG_CSR_STVT";
                break;
            case STF_REG::STF_REG_CSR_SSCRATCH:
                os << "REG_CSR_SSCRATCH";
                break;
            case STF_REG::STF_REG_CSR_SEPC:
                os << "REG_CSR_SEPC";
                break;
            case STF_REG::STF_REG_CSR_SCAUSE:
                os << "REG_CSR_SCAUSE";
                break;
            case STF_REG::STF_REG_CSR_STVAL:
                os << "REG_CSR_STVAL";
                break;
            case STF_REG::STF_REG_CSR_SIP:
                os << "REG_CSR_SIP";
                break;
            case STF_REG::STF_REG_CSR_SNXTI:
                os << "REG_CSR_SNXTI";
                break;
            case STF_REG::STF_REG_CSR_SINTSTATUS:
                os << "REG_CSR_SINTSTATUS";
                break;
            case STF_REG::STF_REG_CSR_SSCRATCHCSW:
                os << "REG_CSR_SSCRATCHCSW";
                break;
            case STF_REG::STF_REG_CSR_SSCRATCHCSWL:
                os << "REG_CSR_SSCRATCHCSWL";
                break;
            case STF_REG::STF_REG_CSR_SATP:
                os << "REG_CSR_SATP";
                break;
            case STF_REG::STF_REG_CSR_VSSTATUS:
                os << "REG_CSR_VSSTATUS";
                break;
            case STF_REG::STF_REG_CSR_VSIE:
                os << "REG_CSR_VSIE";
                break;
            case STF_REG::STF_REG_CSR_VSTVEC:
                os << "REG_CSR_VSTVEC";
                break;
            case STF_REG::STF_REG_CSR_VSSCRATCH:
                os << "REG_CSR_VSSCRATCH";
                break;
            case STF_REG::STF_REG_CSR_VSEPC:
                os << "REG_CSR_VSEPC";
                break;
            case STF_REG::STF_REG_CSR_VSCAUSE:
                os << "REG_CSR_VSCAUSE";
                break;
            case STF_REG::STF_REG_CSR_VSTVAL:
                os << "REG_CSR_VSTVAL";
                break;
            case STF_REG::STF_REG_CSR_VSIP:
                os << "REG_CSR_VSIP";
                break;
            case STF_REG::STF_REG_CSR_VSATP:
                os << "REG_CSR_VSATP";
                break;
            case STF_REG::STF_REG_CSR_MSTATUS:
                os << "REG_CSR_MSTATUS";
                break;
            case STF_REG::STF_REG_CSR_MISA:
                os << "REG_CSR_MISA";
                break;
            case STF_REG::STF_REG_CSR_MEDELEG:
                os << "REG_CSR_MEDELEG";
                break;
            case STF_REG::STF_REG_CSR_MIDELEG:
                os << "REG_CSR_MIDELEG";
                break;
            case STF_REG::STF_REG_CSR_MIE:
                os << "REG_CSR_MIE";
                break;
            case STF_REG::STF_REG_CSR_MTVEC:
                os << "REG_CSR_MTVEC";
                break;
            case STF_REG::STF_REG_CSR_MCOUNTEREN:
                os << "REG_CSR_MCOUNTEREN";
                break;
            case STF_REG::STF_REG_CSR_MTVT:
                os << "REG_CSR_MTVT";
                break;
            case STF_REG::STF_REG_CSR_MSCRATCH:
                os << "REG_CSR_MSCRATCH";
                break;
            case STF_REG::STF_REG_CSR_MEPC:
                os << "REG_CSR_MEPC";
                break;
            case STF_REG::STF_REG_CSR_MCAUSE:
                os << "REG_CSR_MCAUSE";
                break;
            case STF_REG::STF_REG_CSR_MTVAL:
                os << "REG_CSR_MTVAL";
                break;
            case STF_REG::STF_REG_CSR_MIP:
                os << "REG_CSR_MIP";
                break;
            case STF_REG::STF_REG_CSR_MNXTI:
                os << "REG_CSR_MNXTI";
                break;
            case STF_REG::STF_REG_CSR_MINTSTATUS:
                os << "REG_CSR_MINTSTATUS";
                break;
            case STF_REG::STF_REG_CSR_MSCRATCHCSW:
                os << "REG_CSR_MSCRATCHCSW";
                break;
            case STF_REG::STF_REG_CSR_MSCRATCHCSWL:
                os << "REG_CSR_MSCRATCHCSWL";
                break;
            case STF_REG::STF_REG_CSR_PMPCFG0:
                os << "REG_CSR_PMPCFG0";
                break;
            case STF_REG::STF_REG_CSR_PMPCFG1:
                os << "REG_CSR_PMPCFG1";
                break;
            case STF_REG::STF_REG_CSR_PMPCFG2:
                os << "REG_CSR_PMPCFG2";
                break;
            case STF_REG::STF_REG_CSR_PMPCFG3:
                os << "REG_CSR_PMPCFG3";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR0:
                os << "REG_CSR_PMPADDR0";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR1:
                os << "REG_CSR_PMPADDR1";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR2:
                os << "REG_CSR_PMPADDR2";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR3:
                os << "REG_CSR_PMPADDR3";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR4:
                os << "REG_CSR_PMPADDR4";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR5:
                os << "REG_CSR_PMPADDR5";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR6:
                os << "REG_CSR_PMPADDR6";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR7:
                os << "REG_CSR_PMPADDR7";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR8:
                os << "REG_CSR_PMPADDR8";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR9:
                os << "REG_CSR_PMPADDR9";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR10:
                os << "REG_CSR_PMPADDR10";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR11:
                os << "REG_CSR_PMPADDR11";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR12:
                os << "REG_CSR_PMPADDR12";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR13:
                os << "REG_CSR_PMPADDR13";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR14:
                os << "REG_CSR_PMPADDR14";
                break;
            case STF_REG::STF_REG_CSR_PMPADDR15:
                os << "REG_CSR_PMPADDR15";
                break;
            case STF_REG::STF_REG_CSR_HSTATUS:
                os << "REG_CSR_HSTATUS";
                break;
            case STF_REG::STF_REG_CSR_HEDELEG:
                os << "REG_CSR_HEDELEG";
                break;
            case STF_REG::STF_REG_CSR_HIDELEG:
                os << "REG_CSR_HIDELEG";
                break;
            case STF_REG::STF_REG_CSR_HCOUNTEREN:
                os << "REG_CSR_HCOUNTEREN";
                break;
            case STF_REG::STF_REG_CSR_HGATP:
                os << "REG_CSR_HGATP";
                break;
            case STF_REG::STF_REG_CSR_MCYCLE:
                os << "REG_CSR_MCYCLE";
                break;
            case STF_REG::STF_REG_CSR_MINSTRET:
                os << "REG_CSR_MINSTRET";
                break;
            case STF_REG::STF_REG_CSR_MCYCLEH:
                os << "REG_CSR_MCYCLEH";
                break;
            case STF_REG::STF_REG_CSR_MINSTRETH:
                os << "REG_CSR_MINSTRETH";
                break;
            case STF_REG::STF_REG_CSR_CYCLE:
                os << "REG_CSR_CYCLE";
                break;
            case STF_REG::STF_REG_CSR_TIME:
                os << "REG_CSR_TIME";
                break;
            case STF_REG::STF_REG_CSR_INSTRET:
                os << "REG_CSR_INSTRET";
                break;
            case STF_REG::STF_REG_CSR_CYCLEH:
                os << "REG_CSR_CYCLEH";
                break;
            case STF_REG::STF_REG_CSR_TIMEH:
                os << "REG_CSR_TIMEH";
                break;
            case STF_REG::STF_REG_CSR_INSTRETH:
                os << "REG_CSR_INSTRETH";
                break;
            case STF_REG::STF_REG_CSR_MVENDORID:
                os << "REG_CSR_MVENDORID";
                break;
            case STF_REG::STF_REG_CSR_MARCHID:
                os << "REG_CSR_MARCHID";
                break;
            case STF_REG::STF_REG_CSR_MIMPID:
                os << "REG_CSR_MIMPID";
                break;
            case STF_REG::STF_REG_CSR_MHARTID:
                os << "REG_CSR_MHARTID";
                break;
            case STF_REG::STF_REG_X0:
            case STF_REG::STF_REG_X1:
            case STF_REG::STF_REG_X2:
            case STF_REG::STF_REG_X3:
            case STF_REG::STF_REG_X4:
            case STF_REG::STF_REG_X5:
            case STF_REG::STF_REG_X6:
            case STF_REG::STF_REG_X7:
            case STF_REG::STF_REG_X8:
            case STF_REG::STF_REG_X9:
            case STF_REG::STF_REG_X10:
            case STF_REG::STF_REG_X11:
            case STF_REG::STF_REG_X12:
            case STF_REG::STF_REG_X13:
            case STF_REG::STF_REG_X14:
            case STF_REG::STF_REG_X15:
            case STF_REG::STF_REG_X16:
            case STF_REG::STF_REG_X17:
            case STF_REG::STF_REG_X18:
            case STF_REG::STF_REG_X19:
            case STF_REG::STF_REG_X20:
            case STF_REG::STF_REG_X21:
            case STF_REG::STF_REG_X22:
            case STF_REG::STF_REG_X23:
            case STF_REG::STF_REG_X24:
            case STF_REG::STF_REG_X25:
            case STF_REG::STF_REG_X26:
            case STF_REG::STF_REG_X27:
            case STF_REG::STF_REG_X28:
            case STF_REG::STF_REG_X29:
            case STF_REG::STF_REG_X30:
            case STF_REG::STF_REG_X31:
            case STF_REG::STF_REG_F0:
            case STF_REG::STF_REG_F1:
            case STF_REG::STF_REG_F2:
            case STF_REG::STF_REG_F3:
            case STF_REG::STF_REG_F4:
            case STF_REG::STF_REG_F5:
            case STF_REG::STF_REG_F6:
            case STF_REG::STF_REG_F7:
            case STF_REG::STF_REG_F8:
            case STF_REG::STF_REG_F9:
            case STF_REG::STF_REG_F10:
            case STF_REG::STF_REG_F11:
            case STF_REG::STF_REG_F12:
            case STF_REG::STF_REG_F13:
            case STF_REG::STF_REG_F14:
            case STF_REG::STF_REG_F15:
            case STF_REG::STF_REG_F16:
            case STF_REG::STF_REG_F17:
            case STF_REG::STF_REG_F18:
            case STF_REG::STF_REG_F19:
            case STF_REG::STF_REG_F20:
            case STF_REG::STF_REG_F21:
            case STF_REG::STF_REG_F22:
            case STF_REG::STF_REG_F23:
            case STF_REG::STF_REG_F24:
            case STF_REG::STF_REG_F25:
            case STF_REG::STF_REG_F26:
            case STF_REG::STF_REG_F27:
            case STF_REG::STF_REG_F28:
            case STF_REG::STF_REG_F29:
            case STF_REG::STF_REG_F30:
            case STF_REG::STF_REG_F31:
            case STF_REG::STF_REG_END_DEF:
            case STF_REG::STF_REG_INVALID:
                stf_throw("Attempted to format a non-CSR register: " << std::hex << enums::to_int(regno));
                break;
            default:
                {
                    // An unknown (to us) but likely valid CSR register number
                    std::ostringstream ss;
                    ss << "REG_CSR_UNK_" << std::hex << std::left << enums::to_int(regno);
                    os << ss.str();
                }
                break;
        }
    }

    void Registers::formatFPR_(std::ostream& os, const Registers::STF_REG regno) {
        stf_assert(isFPR(regno), "Attempted to format a non-FP register: " << std::hex << enums::to_int(regno));
        os << "REG_F" + std::to_string(Registers::getArchRegIndex(regno));
    }

    void Registers::formatGPR_(std::ostream& os, const Registers::STF_REG regno) {
        stf_assert(isGPR(regno), "Attempted to format a non-GP register: " << std::hex << enums::to_int(regno));
        os << "REG_" + std::to_string(Registers::getArchRegIndex(regno));
    }

    Registers::STF_REG_packed_int Registers::getArchRegIndex(const Registers::STF_REG regno) {
        return Codec::packRegNum(regno);
    }

    inline void Registers::format(std::ostream& os, const Registers::STF_REG regno) {
        format_utils::FlagSaver flags(os);

        if (Registers::isFPR(regno)) {
            Registers::formatFPR_(os, regno);
        }
        else if (Registers::isCSR(regno)) {
            Registers::formatCSR_(os, regno);
        }
        else if (Registers::isGPR(regno)) {
            Registers::formatGPR_(os, regno);
        }
        else {
            stf_throw("Invalid STF_REG_TYPE: " << Registers::Codec::getRegType(regno));
        }
    }

    std::ostream& operator<<(std::ostream& os, const Registers::STF_REG regno) {
        Registers::format(os, regno);
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Registers::STF_REG_TYPE type) {
        switch(type) {
            case Registers::STF_REG_TYPE::RESERVED:
                os << "RESERVED";
                return os;
            case Registers::STF_REG_TYPE::INTEGER:
                os << "INTEGER";
                return os;
            case Registers::STF_REG_TYPE::FLOATING_POINT:
                os << "FLOATING_POINT";
                return os;
            case Registers::STF_REG_TYPE::VECTOR:
                os << "VECTOR";
                return os;
            case Registers::STF_REG_TYPE::CSR:
                os << "CSR";
                return os;
        };

        os << "UNKNOWN_" << enums::to_printable_int(type);
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Registers::STF_REG_OPERAND_TYPE type) {
        switch(type) {
            case Registers::STF_REG_OPERAND_TYPE::REG_RESERVED:
                os << "RESERVED";
                return os;
            case Registers::STF_REG_OPERAND_TYPE::REG_DEST:
                os << "DEST";
                return os;
            case Registers::STF_REG_OPERAND_TYPE::REG_SOURCE:
                os << "SOURCE";
                return os;
            case Registers::STF_REG_OPERAND_TYPE::REG_STATE:
                os << "STATE";
                return os;
        };

        stf_throw("Unknown STF_REG_OPERAND_TYPE value: " << enums::to_printable_int(type));
    }
} // end namespace stf
