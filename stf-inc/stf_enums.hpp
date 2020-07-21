#ifndef __STF_ENUMS_HPP__
#define __STF_ENUMS_HPP__

#include <cstdint>
#include <type_traits>
#include <ostream>

#include "stf_enum_utils.hpp"

namespace stf {
    class STFWriter;
    class STFReader;

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
     * \enum STF_GEN
     *
     * The trace generator
     *
     */
    enum class STF_GEN : uint8_t {
        STF_GEN_RESERVED,                   /**< reserved */
        STF_GEN_QEMU,                       /**< QEMU */
        STF_GEN_ANDROID_EMULATOR,           /**< Android emulator */
        STF_GEN_GEM5,                       /**< GEM5 */
        STF_GEN_PTE_GEN,                    /**< PTE generator  */
        STF_GEN_IMPERAS,                    /**< Imperas */
        STF_GEN_SPIKE,                      /**< Spike */
        STF_GEN_STF_EXTRACT,                /**< stf_extract */
        STF_GEN_STF_MERGE,                  /**< stf_merge */
        STF_GEN_STF_FILTER_EVT,             /**< stf_filter_evt */
        /*
         * Note that if you add anything to this list, you need to
         * update all the users, too. For instance, stf_check will reject
         * your newly-generated stf if you don't add your new generator.
         */
        STF_GEN_RESERVED_END = 0xFF         /**< reserved end of STF_GEN*/
    };

    /**
     * Writes a string representation of an STF_GEN enum to an ostream
     */
    std::ostream& operator<<(std::ostream& os, STF_GEN generator);

    /**
     * \enum INST_IEM
     *
     * The instruction encoding modes
     *
     */
    enum class INST_IEM : uint16_t {
        STF_INST_IEM_INVALID    = 0,           /**< Invalid */
        STF_INST_IEM_RV32       = 1,
        STF_INST_IEM_RV64       = 2,
        STF_INST_IEM_RESERVED   = 0xFFFF
    };

    /**
     * Writes a string representation of an INST_IEM enum to an ostream
     */
    std::ostream& operator<<(std::ostream& os, INST_IEM iem);

    /**
     * \enum BUS_MASTER
     *
     * The memory source types
     *
     */
    enum class BUS_MASTER : uint8_t {
        CORE,                 /**< Core access */
        GPU,                  /**< GPU access */
        DMA,                  /**< DMA access */
        PCIE,                 /**< PCIe access */
        SRIO,                 /**< sRIO access */
        ICN                   /**< Inter Connection Network */
    };

    /**
     * Writes a string representation of a BUS_MASTER enum to an ostream
     */
    std::ostream& operator<<(std::ostream& os, BUS_MASTER master);

    /**
     * \enum BUS_MEM_ACCESS
     *
     * The bus memory access types
     *
     */
    enum class BUS_MEM_ACCESS : uint8_t {
        INVALID,
        READ,              /**< Data read */
        WRITE,             /**< Data write */
    };

    /**
     * Writes a string representation of a BUS_MEM_ACCESS enum to an ostream
     */
    std::ostream& operator<<(std::ostream& os, BUS_MEM_ACCESS access);

    /**
     * \enum INST_MEM_ACCESS
     *
     * The instruction memory access types
     *
     */
    enum class INST_MEM_ACCESS : uint8_t {
        INVALID = 0,
        READ = 1,
        WRITE = 2
    };

    /**
     * Writes a string representation of an INST_MEM_ACCESS enum to an ostream
     */
    std::ostream& operator<<(std::ostream& os, INST_MEM_ACCESS isa);

    /**
     * \enum ISA
     *
     * ISA families
     *
     */
    enum class ISA : uint16_t {
        RESERVED    = 0,
        RISCV       = 1,
        ARM         = 2,
        X86         = 3,
        POWER       = 4
    };

    /**
     * Writes a string representation of an ISA enum to an ostream
     */
    std::ostream& operator<<(std::ostream& os, ISA isa);

    /**
     * \enum TRACE_FEATURES
     *
     * Defines trace features
     */
    enum class TRACE_FEATURES : uint64_t {
        STF_CONTAIN_PHYSICAL_ADDRESS        = 0x00000001, /**< Trace contains PAs */
        STF_CONTAIN_DATA_ATTRIBUTE          = 0x00000002, /**< Trace contains data attributes */
        STF_CONTAIN_OPERAND_VALUE           = 0x00000004, /**< Trace contains operand values */
        STF_CONTAIN_EVENT                   = 0x00000008, /**< Trace contains events */
        STF_CONTAIN_SYSTEMCALL_VALUE        = 0x00000010, /**< Trace contains syscall values */
        STF_CONTAIN_RV64                    = 0x00000020, /**< Trace contains RV64 instructions */
        STF_CONTAIN_INT_DIV_OPERAND_VALUE   = 0x00000040, /**< Trace contains integer divide operand values */
        STF_CONTAIN_SAMPLING                = 0x00000080, /**< Trace contains sampling */
        STF_CONTAIN_PTE                     = 0x00000100, /**< Trace contains embedded PTEs */
        STF_CONTAIN_SIMPOINT                = 0x00000200, /**< Trace contains simpoints */
        STF_CONTAIN_PROCESS_ID              = 0x00000400, /**< Trace contains PID */
        STF_CONTAIN_PTE_ONLY                = 0x00000800, /**< Trace only contains PTEs */
        STF_NEED_POST_PROCESS               = 0x00001000, /**< Trace needs post-processing */
        STF_CONTAIN_REG_STATE               = 0x00002000, /**< Trace contains register state */
        STF_CONTAIN_MICROOP                 = 0x00004000, /**< Trace contains micro-ops */
        STF_CONTAIN_MULTI_THREAD            = 0x00008000, /**< Trace contains multiple threads */
        STF_CONTAIN_MULTI_CORE              = 0x00010000, /**< Trace contains multiple cores */
        STF_CONTAIN_PTE_HW_AD               = 0x00020000, /**< Trace contains PTE hardware addresses */
        STF_CONTAIN_VEC                     = 0x00040000  /**< Trace contains vector instructions */
    };

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
    std::ostream& operator<<(std::ostream& os, EXECUTION_MODE mode);

} // end namespace stf

#endif
