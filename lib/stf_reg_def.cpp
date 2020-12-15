#include <ostream>
#include <iomanip>
#include "format_utils.hpp"
#include "stf_reg_def.hpp"

namespace stf {
    void Registers::formatCSR_(std::ostream& os, const Registers::STF_REG regno) {
        switch(regno) {
            case STF_REG::STF_REG_CSR_USTATUS:
                os << "REG_CSR_USTATUS";
                return;
            case STF_REG::STF_REG_CSR_FFLAGS:
                os << "REG_CSR_FFLAGS";
                return;
            case STF_REG::STF_REG_CSR_FRM:
                os << "REG_CSR_FRM";
                return;
            case STF_REG::STF_REG_CSR_FCSR:
                os << "REG_CSR_FCSR";
                return;
            case STF_REG::STF_REG_CSR_UIE:
                os << "REG_CSR_UIE";
                return;
            case STF_REG::STF_REG_CSR_UTVEC:
                os << "REG_CSR_UTVEC";
                return;
            case STF_REG::STF_REG_CSR_UTVT:
                os << "REG_CSR_UTVT";
                return;
            case STF_REG::STF_REG_CSR_VSTART:
                os << "REG_CSR_VSTART";
                return;
            case STF_REG::STF_REG_CSR_VXSAT:
                os << "REG_CSR_VXSAT";
                return;
            case STF_REG::STF_REG_CSR_VXRM:
                os << "REG_CSR_VXRM";
                return;
            case STF_REG::STF_REG_CSR_VCSR:
                os << "REG_CSR_VCSR";
                return;
            case STF_REG::STF_REG_CSR_USCRATCH:
                os << "REG_CSR_USCRATCH";
                return;
            case STF_REG::STF_REG_CSR_UEPC:
                os << "REG_CSR_UEPC";
                return;
            case STF_REG::STF_REG_CSR_UCAUSE:
                os << "REG_CSR_UCAUSE";
                return;
            case STF_REG::STF_REG_CSR_UTVAL:
                os << "REG_CSR_UTVAL";
                return;
            case STF_REG::STF_REG_CSR_UIP:
                os << "REG_CSR_UIP";
                return;
            case STF_REG::STF_REG_CSR_UNXTI:
                os << "REG_CSR_UNXTI";
                return;
            case STF_REG::STF_REG_CSR_UINTSTATUS:
                os << "REG_CSR_UINTSTATUS";
                return;
            case STF_REG::STF_REG_CSR_USCRATCHCSW:
                os << "REG_CSR_USCRATCHCSW";
                return;
            case STF_REG::STF_REG_CSR_USCRATCHCSWL:
                os << "REG_CSR_USCRATCHCSWL";
                return;
            case STF_REG::STF_REG_CSR_SSTATUS:
                os << "REG_CSR_SSTATUS";
                return;
            case STF_REG::STF_REG_CSR_SIE:
                os << "REG_CSR_SIE";
                return;
            case STF_REG::STF_REG_CSR_STVEC:
                os << "REG_CSR_STVEC";
                return;
            case STF_REG::STF_REG_CSR_SCOUNTEREN:
                os << "REG_CSR_SCOUNTEREN";
                return;
            case STF_REG::STF_REG_CSR_STVT:
                os << "REG_CSR_STVT";
                return;
            case STF_REG::STF_REG_CSR_SSCRATCH:
                os << "REG_CSR_SSCRATCH";
                return;
            case STF_REG::STF_REG_CSR_SEPC:
                os << "REG_CSR_SEPC";
                return;
            case STF_REG::STF_REG_CSR_SCAUSE:
                os << "REG_CSR_SCAUSE";
                return;
            case STF_REG::STF_REG_CSR_STVAL:
                os << "REG_CSR_STVAL";
                return;
            case STF_REG::STF_REG_CSR_SIP:
                os << "REG_CSR_SIP";
                return;
            case STF_REG::STF_REG_CSR_SNXTI:
                os << "REG_CSR_SNXTI";
                return;
            case STF_REG::STF_REG_CSR_SINTSTATUS:
                os << "REG_CSR_SINTSTATUS";
                return;
            case STF_REG::STF_REG_CSR_SSCRATCHCSW:
                os << "REG_CSR_SSCRATCHCSW";
                return;
            case STF_REG::STF_REG_CSR_SSCRATCHCSWL:
                os << "REG_CSR_SSCRATCHCSWL";
                return;
            case STF_REG::STF_REG_CSR_SATP:
                os << "REG_CSR_SATP";
                return;
            case STF_REG::STF_REG_CSR_VSSTATUS:
                os << "REG_CSR_VSSTATUS";
                return;
            case STF_REG::STF_REG_CSR_VSIE:
                os << "REG_CSR_VSIE";
                return;
            case STF_REG::STF_REG_CSR_VSTVEC:
                os << "REG_CSR_VSTVEC";
                return;
            case STF_REG::STF_REG_CSR_VSSCRATCH:
                os << "REG_CSR_VSSCRATCH";
                return;
            case STF_REG::STF_REG_CSR_VSEPC:
                os << "REG_CSR_VSEPC";
                return;
            case STF_REG::STF_REG_CSR_VSCAUSE:
                os << "REG_CSR_VSCAUSE";
                return;
            case STF_REG::STF_REG_CSR_VSTVAL:
                os << "REG_CSR_VSTVAL";
                return;
            case STF_REG::STF_REG_CSR_VSIP:
                os << "REG_CSR_VSIP";
                return;
            case STF_REG::STF_REG_CSR_VSATP:
                os << "REG_CSR_VSATP";
                return;
            case STF_REG::STF_REG_CSR_MSTATUS:
                os << "REG_CSR_MSTATUS";
                return;
            case STF_REG::STF_REG_CSR_MISA:
                os << "REG_CSR_MISA";
                return;
            case STF_REG::STF_REG_CSR_MEDELEG:
                os << "REG_CSR_MEDELEG";
                return;
            case STF_REG::STF_REG_CSR_MIDELEG:
                os << "REG_CSR_MIDELEG";
                return;
            case STF_REG::STF_REG_CSR_MIE:
                os << "REG_CSR_MIE";
                return;
            case STF_REG::STF_REG_CSR_MTVEC:
                os << "REG_CSR_MTVEC";
                return;
            case STF_REG::STF_REG_CSR_MCOUNTEREN:
                os << "REG_CSR_MCOUNTEREN";
                return;
            case STF_REG::STF_REG_CSR_MTVT:
                os << "REG_CSR_MTVT";
                return;
            case STF_REG::STF_REG_CSR_MSCRATCH:
                os << "REG_CSR_MSCRATCH";
                return;
            case STF_REG::STF_REG_CSR_MEPC:
                os << "REG_CSR_MEPC";
                return;
            case STF_REG::STF_REG_CSR_MCAUSE:
                os << "REG_CSR_MCAUSE";
                return;
            case STF_REG::STF_REG_CSR_MTVAL:
                os << "REG_CSR_MTVAL";
                return;
            case STF_REG::STF_REG_CSR_MIP:
                os << "REG_CSR_MIP";
                return;
            case STF_REG::STF_REG_CSR_MNXTI:
                os << "REG_CSR_MNXTI";
                return;
            case STF_REG::STF_REG_CSR_MINTSTATUS:
                os << "REG_CSR_MINTSTATUS";
                return;
            case STF_REG::STF_REG_CSR_MSCRATCHCSW:
                os << "REG_CSR_MSCRATCHCSW";
                return;
            case STF_REG::STF_REG_CSR_MSCRATCHCSWL:
                os << "REG_CSR_MSCRATCHCSWL";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG0:
                os << "REG_CSR_PMPCFG0";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG1:
                os << "REG_CSR_PMPCFG1";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG2:
                os << "REG_CSR_PMPCFG2";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG3:
                os << "REG_CSR_PMPCFG3";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR0:
                os << "REG_CSR_PMPADDR0";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR1:
                os << "REG_CSR_PMPADDR1";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR2:
                os << "REG_CSR_PMPADDR2";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR3:
                os << "REG_CSR_PMPADDR3";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR4:
                os << "REG_CSR_PMPADDR4";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR5:
                os << "REG_CSR_PMPADDR5";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR6:
                os << "REG_CSR_PMPADDR6";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR7:
                os << "REG_CSR_PMPADDR7";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR8:
                os << "REG_CSR_PMPADDR8";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR9:
                os << "REG_CSR_PMPADDR9";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR10:
                os << "REG_CSR_PMPADDR10";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR11:
                os << "REG_CSR_PMPADDR11";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR12:
                os << "REG_CSR_PMPADDR12";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR13:
                os << "REG_CSR_PMPADDR13";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR14:
                os << "REG_CSR_PMPADDR14";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR15:
                os << "REG_CSR_PMPADDR15";
                return;
            case STF_REG::STF_REG_CSR_HSTATUS:
                os << "REG_CSR_HSTATUS";
                return;
            case STF_REG::STF_REG_CSR_HEDELEG:
                os << "REG_CSR_HEDELEG";
                return;
            case STF_REG::STF_REG_CSR_HIDELEG:
                os << "REG_CSR_HIDELEG";
                return;
            case STF_REG::STF_REG_CSR_HCOUNTEREN:
                os << "REG_CSR_HCOUNTEREN";
                return;
            case STF_REG::STF_REG_CSR_HGATP:
                os << "REG_CSR_HGATP";
                return;
            case STF_REG::STF_REG_CSR_MCYCLE:
                os << "REG_CSR_MCYCLE";
                return;
            case STF_REG::STF_REG_CSR_MINSTRET:
                os << "REG_CSR_MINSTRET";
                return;
            case STF_REG::STF_REG_CSR_MCYCLEH:
                os << "REG_CSR_MCYCLEH";
                return;
            case STF_REG::STF_REG_CSR_MINSTRETH:
                os << "REG_CSR_MINSTRETH";
                return;
            case STF_REG::STF_REG_CSR_CYCLE:
                os << "REG_CSR_CYCLE";
                return;
            case STF_REG::STF_REG_CSR_TIME:
                os << "REG_CSR_TIME";
                return;
            case STF_REG::STF_REG_CSR_INSTRET:
                os << "REG_CSR_INSTRET";
                return;
            case STF_REG::STF_REG_CSR_VL:
                os << "REG_CSR_VL";
                return;
            case STF_REG::STF_REG_CSR_VTYPE:
                os << "REG_CSR_VTYPE";
                return;
            case STF_REG::STF_REG_CSR_VLENB:
                os << "REG_CSR_VLENB";
                return;
            case STF_REG::STF_REG_CSR_CYCLEH:
                os << "REG_CSR_CYCLEH";
                return;
            case STF_REG::STF_REG_CSR_TIMEH:
                os << "REG_CSR_TIMEH";
                return;
            case STF_REG::STF_REG_CSR_INSTRETH:
                os << "REG_CSR_INSTRETH";
                return;
            case STF_REG::STF_REG_CSR_MVENDORID:
                os << "REG_CSR_MVENDORID";
                return;
            case STF_REG::STF_REG_CSR_MARCHID:
                os << "REG_CSR_MARCHID";
                return;
            case STF_REG::STF_REG_CSR_MIMPID:
                os << "REG_CSR_MIMPID";
                return;
            case STF_REG::STF_REG_CSR_MHARTID:
                os << "REG_CSR_MHARTID";
                return;
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
            case STF_REG::STF_REG_V0:
            case STF_REG::STF_REG_V1:
            case STF_REG::STF_REG_V2:
            case STF_REG::STF_REG_V3:
            case STF_REG::STF_REG_V4:
            case STF_REG::STF_REG_V5:
            case STF_REG::STF_REG_V6:
            case STF_REG::STF_REG_V7:
            case STF_REG::STF_REG_V8:
            case STF_REG::STF_REG_V9:
            case STF_REG::STF_REG_V10:
            case STF_REG::STF_REG_V11:
            case STF_REG::STF_REG_V12:
            case STF_REG::STF_REG_V13:
            case STF_REG::STF_REG_V14:
            case STF_REG::STF_REG_V15:
            case STF_REG::STF_REG_V16:
            case STF_REG::STF_REG_V17:
            case STF_REG::STF_REG_V18:
            case STF_REG::STF_REG_V19:
            case STF_REG::STF_REG_V20:
            case STF_REG::STF_REG_V21:
            case STF_REG::STF_REG_V22:
            case STF_REG::STF_REG_V23:
            case STF_REG::STF_REG_V24:
            case STF_REG::STF_REG_V25:
            case STF_REG::STF_REG_V26:
            case STF_REG::STF_REG_V27:
            case STF_REG::STF_REG_V28:
            case STF_REG::STF_REG_V29:
            case STF_REG::STF_REG_V30:
            case STF_REG::STF_REG_V31:
            case STF_REG::STF_REG_END_DEF:
            case STF_REG::STF_REG_INVALID:
                stf_throw("Attempted to format a non-CSR register: " << std::hex << enums::to_int(regno));
        }

        // An unknown (to us) but likely valid CSR register number
        std::ostringstream ss;
        ss << "REG_CSR_UNK_" << std::hex << std::left << enums::to_int(regno);
        os << ss.str();
    }

    void Registers::formatFPR_(std::ostream& os, const Registers::STF_REG regno) {
        stf_assert(isFPR(regno), "Attempted to format a non-FP register: " << std::hex << enums::to_int(regno));
        os << "REG_F" + std::to_string(Registers::getArchRegIndex(regno));
    }

    void Registers::formatGPR_(std::ostream& os, const Registers::STF_REG regno) {
        stf_assert(isGPR(regno), "Attempted to format a non-GP register: " << std::hex << enums::to_int(regno));
        os << "REG_" + std::to_string(Registers::getArchRegIndex(regno));
    }

    void Registers::formatVector_(std::ostream& os, const Registers::STF_REG regno) {
        stf_assert(isVector(regno), "Attempted to format a non-vector register: " << std::hex << enums::to_int(regno));
        os << "REG_V" + std::to_string(Registers::getArchRegIndex(regno));
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
        else if (Registers::isVector(regno)) {
            Registers::formatVector_(os, regno);
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
