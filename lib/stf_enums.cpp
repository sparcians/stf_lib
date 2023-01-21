#include <ostream>
#include "stf_enums.hpp"
#include "stf_exception.hpp"

namespace stf {
    std::ostream& operator<<(std::ostream& os, const INST_MEM_ACCESS isa) {
        switch(isa) {
            case INST_MEM_ACCESS::INVALID:
                os << "INVALID";
                break;
            case INST_MEM_ACCESS::READ:
                os << "READ";
                break;
            case INST_MEM_ACCESS::WRITE:
                os << "WRITE";
                break;
        };

        return os;
    }

    std::ostream& operator<<(std::ostream& os, const ISA isa) {
        switch(isa) {
            case ISA::RESERVED:
                os << "RESERVED";
                break;
            case ISA::RISCV:
                os << "RISCV";
                break;
            case ISA::ARM:
                os << "ARM";
                break;
            case ISA::X86:
                os << "X86";
                break;
            case ISA::POWER:
                os << "POWER";
                break;
        };

        return os;
    }

    std::ostream& operator<<(std::ostream& os, const INST_IEM iem) {
        switch(iem) {
            case INST_IEM::STF_INST_IEM_INVALID:
                os << "INVALID";
                return os;
            case INST_IEM::STF_INST_IEM_RESERVED:
                os << "RESERVED";
                return os;
            case INST_IEM::STF_INST_IEM_RV32:
                os << "RV32";
                return os;
            case INST_IEM::STF_INST_IEM_RV64:
                os << "RV64";
                return os;
        };

        stf_throw("Invalid INST_IEM value: " << enums::to_printable_int(iem));
    }

    std::ostream& operator<<(std::ostream& os, const EXECUTION_MODE mode) {
        switch(mode) {
            case EXECUTION_MODE::USER_MODE:
                os << "USER";
                return os;
            case EXECUTION_MODE::SUPERVISOR_MODE:
                os << "SUPERVISOR";
                return os;
            case EXECUTION_MODE::HYPERVISOR_MODE:
                os << "HYPERVISOR";
                return os;
            case EXECUTION_MODE::MACHINE_MODE:
                os << "MACHINE";
                return os;
        };

        stf_throw("Invalid EXECUTION_MODE value: " << enums::to_printable_int(mode));
    }

    std::ostream& operator<<(std::ostream& os, const BUS_MASTER master) {
        switch(master) {
            case BUS_MASTER::CORE:
                os << "CORE";
                return os;
            case BUS_MASTER::DMA:
                os << "DMA";
                return os;
            case BUS_MASTER::GPU:
                os << "GPU";
                return os;
            case BUS_MASTER::ICN:
                os << "ICN";
                return os;
            case BUS_MASTER::PCIE:
                os << "PCIE";
                return os;
            case BUS_MASTER::SRIO:
                os << "SRIO";
                return os;
        };

        stf_throw("Invalid BUS_MASTER value: " << enums::to_printable_int(master));
    }

    std::ostream& operator<<(std::ostream& os, const BUS_MEM_ACCESS access) {
        switch(access) {
            case BUS_MEM_ACCESS::INVALID:
                os << "INVALID";
                return os;
            case BUS_MEM_ACCESS::READ:
                os << "READ";
                return os;
            case BUS_MEM_ACCESS::WRITE:
                os << "WRITE";
                return os;
        };

        stf_throw("Invalid BUS_MEM_ACCESS value: " << enums::to_printable_int(access));
    }

    std::ostream& operator<<(std::ostream& os, const TRACE_FEATURES feat) {
        switch(feat) {
            case TRACE_FEATURES::STF_CONTAIN_PHYSICAL_ADDRESS:
                os << "STF_CONTAIN_PHYSICAL_ADDRESS";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_DATA_ATTRIBUTE:
                os << "STF_CONTAIN_DATA_ATTRIBUTE";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_OPERAND_VALUE:
                os << "STF_CONTAIN_OPERAND_VALUE";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_EVENT:
                os << "STF_CONTAIN_EVENT";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_SYSTEMCALL_VALUE:
                os << "STF_CONTAIN_SYSTEMCALL_VALUE";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_RV64:
                os << "STF_CONTAIN_RV64";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_INT_DIV_OPERAND_VALUE:
                os << "STF_CONTAIN_INT_DIV_OPERAND_VALUE";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_SAMPLING:
                os << "STF_CONTAIN_SAMPLING";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_PTE:
                os << "STF_CONTAIN_PTE";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_SIMPOINT:
                os << "STF_CONTAIN_SIMPOINT";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_PROCESS_ID:
                os << "STF_CONTAIN_PROCESS_ID";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_PTE_ONLY:
                os << "STF_CONTAIN_PTE_ONLY";
                return os;
            case TRACE_FEATURES::STF_NEED_POST_PROCESS:
                os << "STF_NEED_POST_PROCESS";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_REG_STATE:
                os << "STF_CONTAIN_REG_STATE";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_MICROOP:
                os << "STF_CONTAIN_MICROOP";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_MULTI_THREAD:
                os << "STF_CONTAIN_MULTI_THREAD";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_MULTI_CORE:
                os << "STF_CONTAIN_MULTI_CORE";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_PTE_HW_AD:
                os << "STF_CONTAIN_PTE_HW_AD";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_VEC:
                os << "STF_CONTAIN_VEC";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_EVENT64:
                os << "STF_CONTAIN_EVENT64";
                return os;
            case TRACE_FEATURES::STF_CONTAIN_TRANSACTIONS:
                os << "STF_CONTAIN_TRANSACTIONS";
                return os;
        };

        stf_throw("Invalid TRACE_FEATURES value: " << enums::to_printable_int(feat));
    }

} // end namespace stf
