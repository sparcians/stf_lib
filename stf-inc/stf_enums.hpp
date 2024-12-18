#ifndef __STF_ENUMS_HPP__
#define __STF_ENUMS_HPP__

#include <cstdint>
#include <type_traits>
#include <ostream>

#include "stf_enum_utils.hpp"

namespace stf {
    /**
     * \enum STF_FILE_TYPE
     * \brief Supported STF file types
     */
    enum class STF_FILE_TYPE {
        UNKNOWN,
        STF,
        ZSTF,
        STF_XZ,
        STF_GZ,
        STF_SH,
        STDIO
    };

    /**
     * \enum INST_IEM
     *
     * The instruction encoding modes
     *
     */
    STF_ENUM(
        STF_ENUM_CONFIG(OVERRIDE_START, OVERRIDE_END, AUTO_PRINT),
        INST_IEM,
        uint16_t,
        STF_ENUM_STR(STF_INST_IEM_INVALID, "INVALID"),           /**< Invalid */
        STF_ENUM_VAL(STF_INST_IEM_RV32, 1, "RV32"),
        STF_ENUM_VAL(STF_INST_IEM_RV64, 2, "RV64"),
        STF_ENUM_VAL(STF_INST_IEM_RESERVED, 0xFFFF, "RESERVED")
    );

    /**
     * \enum BUS_MASTER
     *
     * The memory source types
     *
     */
    STF_ENUM(
        STF_ENUM_CONFIG(AUTO_PRINT, OVERRIDE_START),
        BUS_MASTER,
        uint8_t,
        INVALID,
        CORE,                 /**< Core access */
        GPU,                  /**< GPU access */
        DMA,                  /**< DMA access */
        PCIE,                 /**< PCIe access */
        SRIO,                 /**< sRIO access */
        ICN                   /**< Inter Connection Network */
    );

    /**
     * \enum BUS_MEM_ACCESS
     *
     * The bus memory access types
     *
     */
    STF_ENUM(
        STF_ENUM_CONFIG(AUTO_PRINT, OVERRIDE_START),
        BUS_MEM_ACCESS,
        uint8_t,
        INVALID,
        READ,              /**< Data read */
        WRITE             /**< Data write */
    );

    /**
     * \enum INST_MEM_ACCESS
     *
     * The instruction memory access types
     *
     */
    STF_ENUM(
        STF_ENUM_CONFIG(AUTO_PRINT, OVERRIDE_START),
        INST_MEM_ACCESS,
        uint8_t,
        INVALID,
        STF_ENUM_VAL(READ, 1),
        STF_ENUM_VAL(WRITE, 2)
    );

    /**
     * Writes a string representation of an INST_MEM_ACCESS enum to an ostream
     */
    //std::ostream& operator<<(std::ostream& os, INST_MEM_ACCESS isa);

    /**
     * \enum ISA
     *
     * ISA families
     *
     */
    STF_ENUM(
        STF_ENUM_CONFIG(AUTO_PRINT, OVERRIDE_START),
        ISA,
        uint16_t,
        RESERVED,
        STF_ENUM_VAL(RISCV, 1),
        STF_ENUM_VAL(ARM, 2),
        STF_ENUM_VAL(X86, 3),
        STF_ENUM_VAL(POWER, 4)
    );

    /**
     * \enum TRACE_FEATURES
     *
     * Defines trace features
     */
    STF_ENUM(
        STF_ENUM_CONFIG(AUTO_PRINT),
        TRACE_FEATURES,
        uint64_t,
        STF_ENUM_VAL(STF_CONTAIN_PHYSICAL_ADDRESS,      0x00000001), /**< Trace contains PAs */
        STF_ENUM_VAL(STF_CONTAIN_DATA_ATTRIBUTE,        0x00000002), /**< Trace contains data attributes */
        STF_ENUM_VAL(STF_CONTAIN_OPERAND_VALUE,         0x00000004), /**< Trace contains operand values */
        STF_ENUM_VAL(STF_CONTAIN_EVENT,                 0x00000008), /**< Trace contains events */
        STF_ENUM_VAL(STF_CONTAIN_SYSTEMCALL_VALUE,      0x00000010), /**< Trace contains syscall values */
        STF_ENUM_VAL(STF_CONTAIN_RV64,                  0x00000020), /**< Trace contains RV64 instructions */
        STF_ENUM_VAL(STF_CONTAIN_INT_DIV_OPERAND_VALUE, 0x00000040), /**< Trace contains integer divide operand values */
        STF_ENUM_VAL(STF_CONTAIN_SAMPLING,              0x00000080), /**< Trace contains sampling */
        STF_ENUM_VAL(STF_CONTAIN_PTE,                   0x00000100), /**< Trace contains embedded PTEs */
        STF_ENUM_VAL(STF_CONTAIN_SIMPOINT,              0x00000200), /**< Trace contains simpoints */
        STF_ENUM_VAL(STF_CONTAIN_PROCESS_ID,            0x00000400), /**< Trace contains PID */
        STF_ENUM_VAL(STF_CONTAIN_PTE_ONLY,              0x00000800), /**< Trace only contains PTEs */
        STF_ENUM_VAL(STF_NEED_POST_PROCESS,             0x00001000), /**< Trace needs post-processing */
        STF_ENUM_VAL(STF_CONTAIN_REG_STATE,             0x00002000), /**< Trace contains register state */
        STF_ENUM_VAL(STF_CONTAIN_MICROOP,               0x00004000), /**< Trace contains micro-ops */
        STF_ENUM_VAL(STF_CONTAIN_MULTI_THREAD,          0x00008000), /**< Trace contains multiple threads */
        STF_ENUM_VAL(STF_CONTAIN_MULTI_CORE,            0x00010000), /**< Trace contains multiple cores */
        STF_ENUM_VAL(STF_CONTAIN_PTE_HW_AD,             0x00020000), /**< Trace contains PTE hardware addresses */
        STF_ENUM_VAL(STF_CONTAIN_VEC,                   0x00040000), /**< Trace contains vector instructions */
        STF_ENUM_VAL(STF_CONTAIN_EVENT64,               0x00080000), /**< Trace contains 64 bit event records */
        STF_ENUM_VAL(STF_CONTAIN_TRANSACTIONS,          0x00100000) /**< Trace contains timed transactions */
    );

    /**
     * \enum EXECUTION_MODE
     *
     * Defines execution modes for use with MODE_CHANGE events
     */
    enum class EXECUTION_MODE : uint64_t {
        USER_MODE = 0,
        SUPERVISOR_MODE = 1,
        HYPERVISOR_MODE = 2,
        MACHINE_MODE = 3
    };

    /**
     * Writes a string representation of an EXECUTION_MODE enum to an ostream
     */
    inline std::ostream& operator<<(std::ostream& os, const EXECUTION_MODE mode) {
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

} // end namespace stf

#endif
