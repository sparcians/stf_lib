
// <STF_Reg_Def> -*- HPP -*-

#ifndef __STF_REG_DEF_HPP__
#define __STF_REG_DEF_HPP__

#include <cstdint>
#include <ostream>
#include <type_traits>

#include "stf_exception.hpp"
#include "stf_enum_utils.hpp"
#include "util.hpp"

namespace stf {

    /**
     * \class Registers
     *
     * Class encapsulating register definitions and utility functions
     */
    class Registers {
        public:
            /**
             * \typedef STF_REG_int
             * Specifies the underlying integer type used by STF_REG
             */
            using STF_REG_int = uint32_t;

            /**
             * \typedef STF_REG_packed_int
             * Specifies the underlying integer type used by STF_REG when packed into an STF file
             */
            using STF_REG_packed_int = uint16_t;

            /**
             * \typedef STF_REG_metadata_int
             * Specifies the underlying integer type used by STF_REG_TYPE and STF_REG_OPERAND_TYPE
             */
            using STF_REG_metadata_int = uint8_t;

        private:
            static constexpr STF_REG_packed_int REG_MASK_ = std::numeric_limits<STF_REG_packed_int>::max();
            static constexpr size_t TYPE_SHIFT_AMT_ = byte_utils::bitSize<STF_REG_packed_int>();
            static constexpr size_t OPERAND_TYPE_SHIFT_AMT_ = 4;
            static constexpr STF_REG_int TYPE_MASK_ = 0xF;

            static_assert(sizeof(STF_REG_int) >= sizeof(STF_REG_packed_int) + sizeof(STF_REG_metadata_int),
                          "STF_REG_int must be large enough to hold an STF_REG_packed_int and an STF_REG_metadata_int");

        public:
            /**
              * \enum STF_REG_TYPE
              *
              * Defines the different types of registers: integer, float, vector, CSR, etc.
              */
            STF_ENUM(
                STF_ENUM_CONFIG(AUTO_PRINT, ALLOW_UNKNOWN, OVERRIDE_START),
                STF_REG_TYPE,
                STF_REG_metadata_int,
                RESERVED,
                STF_ENUM_VAL(INTEGER, 1),
                STF_ENUM_VAL(FLOATING_POINT, 2),
                STF_ENUM_VAL(VECTOR, 3),
                STF_ENUM_VAL(CSR, 4)
            );

            /**
              * \enum STF_REG
              *
              * Generic regsiter definitions
              */
            enum class STF_REG : STF_REG_int;

            /**
             * \struct Widths
             *
             * Contains widths for mapped registers
             */
            struct Widths {
                static constexpr size_t FFLAGS_WIDTH = 5; /**< Width of FFLAGS register */ // cppcheck-suppress unusedStructMember
                static constexpr size_t FRM_WIDTH = 3; /**< Width of FRM register */ // cppcheck-suppress unusedStructMember
                static constexpr size_t FRM_SHIFT = FFLAGS_WIDTH; /**< FRM shift amount */ // cppcheck-suppress unusedStructMember
                static constexpr size_t VXSAT_WIDTH = 1; /**< Width of VXSAT register */ // cppcheck-suppress unusedStructMember
                static constexpr size_t VXRM_WIDTH = 2; /**< Width of VXRM register */ // cppcheck-suppress unusedStructMember
                static constexpr size_t VXRM_SHIFT = VXSAT_WIDTH; /**< VXRM shift amount */ // cppcheck-suppress unusedStructMember
            };

            /**
             * \enum STF_REG_OPERAND_TYPE
             * Defines the different types of register records
             */
            STF_ENUM(
                STF_ENUM_CONFIG(AUTO_PRINT, OVERRIDE_START),
                STF_REG_OPERAND_TYPE,
                STF_REG_metadata_int,
                STF_ENUM_VAL(REG_RESERVED, 0, "RESERVED"),
                STF_ENUM_VAL(REG_STATE, 1, "STATE"),
                STF_ENUM_VAL(REG_SOURCE, 2, "SOURCE"),
                STF_ENUM_VAL(REG_DEST, 3, "DEST")
            );

            /**
             * Converts an STF_REG to its corresponding index
             * \param regno STF_REG value to convert
             */
            static STF_REG_packed_int getArchRegIndex(STF_REG regno);

            /**
             * Checks whether a register is a CSR
             * \param regno STF_REG value to check
             */
            static inline constexpr bool isCSR(const STF_REG regno) {
                return Registers::Codec::getRegType(regno) == Registers::STF_REG_TYPE::CSR;
            }

            /**
             * Checks whether a register is an FPR
             * \param regno STF_REG value to check
             */
            static inline constexpr bool isFPR(const STF_REG regno) {
                return Registers::Codec::getRegType(regno) == Registers::STF_REG_TYPE::FLOATING_POINT;
            }

            /**
             * Checks whether a register is a GPR
             * \param regno STF_REG value to check
             */
            static inline constexpr bool isGPR(const STF_REG regno) {
                return Registers::Codec::getRegType(regno) == Registers::STF_REG_TYPE::INTEGER;
            }

            /**
             * Checks whether a register is a vector register
             * \param regno STF_REG value to check
             */
            static inline constexpr bool isVector(const STF_REG regno) {
                return Registers::Codec::getRegType(regno) == Registers::STF_REG_TYPE::VECTOR;
            }

            /**
             * Formats an STF_REG value into a stream
             * \param os stream to format into
             * \param regno STF_REG value to format
             */
            static void format(std::ostream& os, const STF_REG regno);

            /**
             * \class Codec
             *
             * Encodes and decodes the type information into a register number
             *
             */
            class Codec {
                private:
                    template<STF_REG_int Start, STF_REG_int End>
                    struct RegRange
                    {
                        static inline constexpr bool in_range(const STF_REG_int reg_num)
                        {
                            return (Start <= reg_num) && (reg_num <= End);
                        }
                    };

                public:
                    /**
                     * Combines a raw register number and the specified STF_REG_TYPE to generate a valid STF_REG_int
                     */
                    static inline constexpr STF_REG_int combineRegType(const STF_REG_packed_int reg_num, const STF_REG_TYPE type) {
                        return static_cast<STF_REG_int>(reg_num) | static_cast<STF_REG_int>(enums::to_int(type) << TYPE_SHIFT_AMT_);
                    }

                    /**
                     * Creates a GPR STF_REG value
                     */
                    static inline constexpr STF_REG_int toGPR(const STF_REG_packed_int reg_num) {
                        return combineRegType(reg_num, STF_REG_TYPE::INTEGER);
                    }

                    /**
                     * Creates an FPR STF_REG value
                     */
                    static inline constexpr STF_REG_int toFPR(const STF_REG_packed_int reg_num) {
                        return combineRegType(reg_num, STF_REG_TYPE::FLOATING_POINT);
                    }

                    /**
                     * Creates a vector STF_REG value
                     */
                    static inline constexpr STF_REG_int toVector(const STF_REG_packed_int reg_num) {
                        return combineRegType(reg_num, STF_REG_TYPE::VECTOR);
                    }

                    /**
                     * Creates a CSR STF_REG value
                     */
                    static inline constexpr STF_REG_int toCSR(const STF_REG_packed_int reg_num) {
                        return combineRegType(reg_num, STF_REG_TYPE::CSR);
                    }

                    /**
                     * Packs an STF_REG into an STF_REG_packed_int
                     * \param reg STF_REG to pack
                     */
                    static inline STF_REG_packed_int packRegNum(const STF_REG reg) {
                        return enums::to_int(reg) & REG_MASK_;
                    }

                    /**
                     * Checks if a register number belongs to a nonstandard (e.g. vendor-defined) range
                     * \param reg Register number to check
                     */
                    static inline constexpr bool isNonstandardCSR(const STF_REG reg) {
                        using USER_NONSTANDARD_RW = RegRange<toCSR(0x800), toCSR(0x8ff)>;
                        using USER_NONSTANDARD_RO = RegRange<toCSR(0xcc0), toCSR(0xcff)>;

                        using SUPERVISOR_NONSTANDARD_RW = RegRange<toCSR(0x5c0), toCSR(0x5ff)>;
                        using SUPERVISOR_NONSTANDARD_RW2 = RegRange<toCSR(0x9c0), toCSR(0x9ff)>;
                        using SUPERVISOR_NONSTANDARD_RO = RegRange<toCSR(0xdc0), toCSR(0xdff)>;

                        using HYPERVISOR_NONSTANDARD_RW = RegRange<toCSR(0x6c0), toCSR(0x6ff)>;
                        using HYPERVISOR_NONSTANDARD_RW2 = RegRange<toCSR(0xac0), toCSR(0xaff)>;
                        using HYPERVISOR_NONSTANDARD_RO = RegRange<toCSR(0xec0), toCSR(0xeff)>;

                        using MACHINE_NONSTANDARD_RW = RegRange<toCSR(0x7c0), toCSR(0x7ff)>;
                        using MACHINE_NONSTANDARD_RW2 = RegRange<toCSR(0xbc0), toCSR(0xbff)>;
                        using MACHINE_NONSTANDARD_RO = RegRange<toCSR(0xfc0), toCSR(0xfff)>;

                        const STF_REG_int reg_num = enums::to_int(reg);

                        return USER_NONSTANDARD_RW::in_range(reg_num) ||
                               USER_NONSTANDARD_RO::in_range(reg_num) ||
                               SUPERVISOR_NONSTANDARD_RW::in_range(reg_num) ||
                               SUPERVISOR_NONSTANDARD_RW2::in_range(reg_num) ||
                               SUPERVISOR_NONSTANDARD_RO::in_range(reg_num) ||
                               HYPERVISOR_NONSTANDARD_RW::in_range(reg_num) ||
                               HYPERVISOR_NONSTANDARD_RW2::in_range(reg_num) ||
                               HYPERVISOR_NONSTANDARD_RO::in_range(reg_num) ||
                               MACHINE_NONSTANDARD_RW::in_range(reg_num) ||
                               MACHINE_NONSTANDARD_RW2::in_range(reg_num) ||
                               MACHINE_NONSTANDARD_RO::in_range(reg_num);
                    }

                    /**
                     * Packs the register type information from an STF_REG and an STF_REG_OPERAND_TYPE into an STF_REG_metadata_int
                     * \param reg STF_REG to pack
                     * \param record_type STF_REG_OPERAND_TYPE to pack
                     */
                    static inline STF_REG_metadata_int packRegMetadata(const STF_REG reg, const STF_REG_OPERAND_TYPE record_type) {
                        return static_cast<STF_REG_metadata_int>(static_cast<STF_REG_metadata_int>(enums::to_int(record_type) << OPERAND_TYPE_SHIFT_AMT_) | (enums::to_int(reg) >> TYPE_SHIFT_AMT_));
                    }

                    /**
                     * Decodes the STF_REG and operand type information from an STF_REG_packed_int and STF_REG_metadata_int
                     * \param reg_no STF_REG_int to decode
                     * \param reg_metadata STF_REG_metadata_int to decode
                     * \param decoded_reg_no decoded STF_REG
                     * \param operand_type decoded operand type
                     */
                    static inline void decode(const STF_REG_packed_int reg_no, const STF_REG_metadata_int reg_metadata, STF_REG& decoded_reg_no, STF_REG_OPERAND_TYPE& operand_type) {
                        const auto reg_type = static_cast<STF_REG_TYPE>(reg_metadata & TYPE_MASK_);
                        operand_type = static_cast<STF_REG_OPERAND_TYPE>(reg_metadata >> OPERAND_TYPE_SHIFT_AMT_);
                        decoded_reg_no = static_cast<STF_REG>(combineRegType(reg_no, reg_type));
                    }

                    /**
                     * Extracts the register type from an STF_REG
                     */
                    static inline constexpr STF_REG_TYPE getRegType(const STF_REG reg) {
                        return static_cast<STF_REG_TYPE>(enums::to_int(reg) >> TYPE_SHIFT_AMT_);
                    }
            };

        private:
            /**
             * Converts a GPR into its corresponding string representation
             */
            static void formatGPR_(std::ostream& os, Registers::STF_REG regno);

            /**
             * Converts an FPR into its corresponding string representation
             */
            static void formatFPR_(std::ostream& os, Registers::STF_REG regno);

            /**
             * Converts a vector register into its corresponding string representation
             */
            static void formatVector_(std::ostream& os, Registers::STF_REG regno);

            /**
             * Converts a CSR into its corresponding string representation
             */
            static void formatCSR_(std::ostream& os, Registers::STF_REG regno);

    };

    // Unfortunately we can't use STF_ENUM here since Boost::Preprocessor allows a maximum of
    // 256 variadic parameters
    // This isn't a huge loss since the format function for STF_REG is complicated
    enum class Registers::STF_REG : Registers::STF_REG_int {
        // 32 int registers
        STF_REG_X0                 = Codec::toGPR(0x0000),
        STF_REG_X1                 = Codec::toGPR(0x0001),
        STF_REG_X2                 = Codec::toGPR(0x0002),
        STF_REG_X3                 = Codec::toGPR(0x0003),
        STF_REG_X4                 = Codec::toGPR(0x0004),
        STF_REG_X5                 = Codec::toGPR(0x0005),
        STF_REG_X6                 = Codec::toGPR(0x0006),
        STF_REG_X7                 = Codec::toGPR(0x0007),
        STF_REG_X8                 = Codec::toGPR(0x0008),
        STF_REG_X9                 = Codec::toGPR(0x0009),
        STF_REG_X10                = Codec::toGPR(0x000a),
        STF_REG_X11                = Codec::toGPR(0x000b),
        STF_REG_X12                = Codec::toGPR(0x000c),
        STF_REG_X13                = Codec::toGPR(0x000d),
        STF_REG_X14                = Codec::toGPR(0x000e),
        STF_REG_X15                = Codec::toGPR(0x000f),
        STF_REG_X16                = Codec::toGPR(0x0010),
        STF_REG_X17                = Codec::toGPR(0x0011),
        STF_REG_X18                = Codec::toGPR(0x0012),
        STF_REG_X19                = Codec::toGPR(0x0013),
        STF_REG_X20                = Codec::toGPR(0x0014),
        STF_REG_X21                = Codec::toGPR(0x0015),
        STF_REG_X22                = Codec::toGPR(0x0016),
        STF_REG_X23                = Codec::toGPR(0x0017),
        STF_REG_X24                = Codec::toGPR(0x0018),
        STF_REG_X25                = Codec::toGPR(0x0019),
        STF_REG_X26                = Codec::toGPR(0x001a),
        STF_REG_X27                = Codec::toGPR(0x001b),
        STF_REG_X28                = Codec::toGPR(0x001c),
        STF_REG_X29                = Codec::toGPR(0x001d),
        STF_REG_X30                = Codec::toGPR(0x001e),
        STF_REG_X31                = Codec::toGPR(0x001f),
        STF_REG_PC                 = Codec::toGPR(0x0020),

        // 32 floating point registers
        STF_REG_F0                 = Codec::toFPR(0x0000),
        STF_REG_F1                 = Codec::toFPR(0x0001),
        STF_REG_F2                 = Codec::toFPR(0x0002),
        STF_REG_F3                 = Codec::toFPR(0x0003),
        STF_REG_F4                 = Codec::toFPR(0x0004),
        STF_REG_F5                 = Codec::toFPR(0x0005),
        STF_REG_F6                 = Codec::toFPR(0x0006),
        STF_REG_F7                 = Codec::toFPR(0x0007),
        STF_REG_F8                 = Codec::toFPR(0x0008),
        STF_REG_F9                 = Codec::toFPR(0x0009),
        STF_REG_F10                = Codec::toFPR(0x000a),
        STF_REG_F11                = Codec::toFPR(0x000b),
        STF_REG_F12                = Codec::toFPR(0x000c),
        STF_REG_F13                = Codec::toFPR(0x000d),
        STF_REG_F14                = Codec::toFPR(0x000e),
        STF_REG_F15                = Codec::toFPR(0x000f),
        STF_REG_F16                = Codec::toFPR(0x0010),
        STF_REG_F17                = Codec::toFPR(0x0011),
        STF_REG_F18                = Codec::toFPR(0x0012),
        STF_REG_F19                = Codec::toFPR(0x0013),
        STF_REG_F20                = Codec::toFPR(0x0014),
        STF_REG_F21                = Codec::toFPR(0x0015),
        STF_REG_F22                = Codec::toFPR(0x0016),
        STF_REG_F23                = Codec::toFPR(0x0017),
        STF_REG_F24                = Codec::toFPR(0x0018),
        STF_REG_F25                = Codec::toFPR(0x0019),
        STF_REG_F26                = Codec::toFPR(0x001a),
        STF_REG_F27                = Codec::toFPR(0x001b),
        STF_REG_F28                = Codec::toFPR(0x001c),
        STF_REG_F29                = Codec::toFPR(0x001d),
        STF_REG_F30                = Codec::toFPR(0x001e),
        STF_REG_F31                = Codec::toFPR(0x001f),

        // 32 vector registers
        STF_REG_V0                 = Codec::toVector(0x0000),
        STF_REG_V1                 = Codec::toVector(0x0001),
        STF_REG_V2                 = Codec::toVector(0x0002),
        STF_REG_V3                 = Codec::toVector(0x0003),
        STF_REG_V4                 = Codec::toVector(0x0004),
        STF_REG_V5                 = Codec::toVector(0x0005),
        STF_REG_V6                 = Codec::toVector(0x0006),
        STF_REG_V7                 = Codec::toVector(0x0007),
        STF_REG_V8                 = Codec::toVector(0x0008),
        STF_REG_V9                 = Codec::toVector(0x0009),
        STF_REG_V10                = Codec::toVector(0x000a),
        STF_REG_V11                = Codec::toVector(0x000b),
        STF_REG_V12                = Codec::toVector(0x000c),
        STF_REG_V13                = Codec::toVector(0x000d),
        STF_REG_V14                = Codec::toVector(0x000e),
        STF_REG_V15                = Codec::toVector(0x000f),
        STF_REG_V16                = Codec::toVector(0x0010),
        STF_REG_V17                = Codec::toVector(0x0011),
        STF_REG_V18                = Codec::toVector(0x0012),
        STF_REG_V19                = Codec::toVector(0x0013),
        STF_REG_V20                = Codec::toVector(0x0014),
        STF_REG_V21                = Codec::toVector(0x0015),
        STF_REG_V22                = Codec::toVector(0x0016),
        STF_REG_V23                = Codec::toVector(0x0017),
        STF_REG_V24                = Codec::toVector(0x0018),
        STF_REG_V25                = Codec::toVector(0x0019),
        STF_REG_V26                = Codec::toVector(0x001a),
        STF_REG_V27                = Codec::toVector(0x001b),
        STF_REG_V28                = Codec::toVector(0x001c),
        STF_REG_V29                = Codec::toVector(0x001d),
        STF_REG_V30                = Codec::toVector(0x001e),
        STF_REG_V31                = Codec::toVector(0x001f),

        // Control and status registers
        // User
        STF_REG_CSR_FFLAGS         = Codec::toCSR(0x001),
        STF_REG_CSR_FRM            = Codec::toCSR(0x002),
        STF_REG_CSR_FCSR           = Codec::toCSR(0x003),
        STF_REG_CSR_VSTART         = Codec::toCSR(0x008),
        STF_REG_CSR_VXSAT          = Codec::toCSR(0x009),
        STF_REG_CSR_VXRM           = Codec::toCSR(0x00a),
        STF_REG_CSR_VCSR           = Codec::toCSR(0x00f),
        STF_REG_CSR_SSP            = Codec::toCSR(0x011),
        STF_REG_CSR_SEED           = Codec::toCSR(0x015),
        STF_REG_CSR_JVT            = Codec::toCSR(0x017),

        // Supervisor
        STF_REG_CSR_SSTATUS        = Codec::toCSR(0x100),
        STF_REG_CSR_SEDELEG        = Codec::toCSR(0x102),
        STF_REG_CSR_SIDELEG        = Codec::toCSR(0x103),
        STF_REG_CSR_SIE            = Codec::toCSR(0x104),
        STF_REG_CSR_STVEC          = Codec::toCSR(0x105),
        STF_REG_CSR_SCOUNTEREN     = Codec::toCSR(0x106),
        STF_REG_CSR_SENVCFG        = Codec::toCSR(0x10a),
        STF_REG_CSR_SSTATEEN0      = Codec::toCSR(0x10c),
        STF_REG_CSR_SSTATEEN1      = Codec::toCSR(0x10d),
        STF_REG_CSR_SSTATEEN2      = Codec::toCSR(0x10e),
        STF_REG_CSR_SSTATEEN3      = Codec::toCSR(0x10f),
        STF_REG_CSR_SIEH           = Codec::toCSR(0x114),
        STF_REG_CSR_SCOUNTINHIBIT  = Codec::toCSR(0x120),
        STF_REG_CSR_SSCRATCH       = Codec::toCSR(0x140),
        STF_REG_CSR_SEPC           = Codec::toCSR(0x141),
        STF_REG_CSR_SCAUSE         = Codec::toCSR(0x142),
        STF_REG_CSR_STVAL          = Codec::toCSR(0x143),
        STF_REG_CSR_SIP            = Codec::toCSR(0x144),
        STF_REG_CSR_STIMECMP       = Codec::toCSR(0x14d),
        STF_REG_CSR_SCTRCTL        = Codec::toCSR(0x14e),
        STF_REG_CSR_SCTRSTATUS     = Codec::toCSR(0x14f),
        STF_REG_CSR_SISELECT       = Codec::toCSR(0x150),
        STF_REG_CSR_SIREG          = Codec::toCSR(0x151),
        STF_REG_CSR_SIREG2         = Codec::toCSR(0x152),
        STF_REG_CSR_SIREG3         = Codec::toCSR(0x153),
        STF_REG_CSR_SIPH           = Codec::toCSR(0x154),
        STF_REG_CSR_SIREG4         = Codec::toCSR(0x155),
        STF_REG_CSR_SIREG5         = Codec::toCSR(0x156),
        STF_REG_CSR_SIREG6         = Codec::toCSR(0x157),
        STF_REG_CSR_STOPEI         = Codec::toCSR(0x15c),
        STF_REG_CSR_STIMECMPH      = Codec::toCSR(0x15d),
        STF_REG_CSR_SCTRDEPTH      = Codec::toCSR(0x15f),
        STF_REG_CSR_SATP           = Codec::toCSR(0x180),
        STF_REG_CSR_SRMCFG         = Codec::toCSR(0x181),
        // SENVCFG was (erroneously) defined as 0x19a originally. We need to keep this definition around until there
        // is an official 0x19a CSR to prevent breakage with existing traces
        STF_REG_CSR_SENVCFG_COMPAT = Codec::toCSR(0x19a),

        // External debug
        STF_REG_CSR_TSELECT        = Codec::toCSR(0x7a0),
        STF_REG_CSR_TDATA1         = Codec::toCSR(0x7a1),
        STF_REG_CSR_TDATA2         = Codec::toCSR(0x7a2),
        STF_REG_CSR_TDATA3         = Codec::toCSR(0x7a3),
        STF_REG_CSR_TINFO          = Codec::toCSR(0x7a4),
        STF_REG_CSR_TCONTROL       = Codec::toCSR(0x7a5),
        STF_REG_CSR_MCONTEXT       = Codec::toCSR(0x7a8),
        STF_REG_CSR_MSCONTEXT      = Codec::toCSR(0x7aa),
        STF_REG_CSR_DCSR           = Codec::toCSR(0x7b0),
        STF_REG_CSR_DPC            = Codec::toCSR(0x7b1),
        STF_REG_CSR_DSCRATCH0      = Codec::toCSR(0x7b2),
        STF_REG_CSR_DSCRATCH1      = Codec::toCSR(0x7b3),

        // Virtual supervisor
        STF_REG_CSR_VSSTATUS       = Codec::toCSR(0x200),
        STF_REG_CSR_VSIE           = Codec::toCSR(0x204),
        STF_REG_CSR_VSTVEC         = Codec::toCSR(0x205),
        STF_REG_CSR_VSIEH          = Codec::toCSR(0x214),
        STF_REG_CSR_VSSCRATCH      = Codec::toCSR(0x240),
        STF_REG_CSR_VSEPC          = Codec::toCSR(0x241),
        STF_REG_CSR_VSCAUSE        = Codec::toCSR(0x242),
        STF_REG_CSR_VSTVAL         = Codec::toCSR(0x243),
        STF_REG_CSR_VSIP           = Codec::toCSR(0x244),
        STF_REG_CSR_VSTIMECMP      = Codec::toCSR(0x24d),
        STF_REG_CSR_VSCTRCTL       = Codec::toCSR(0x24e),
        STF_REG_CSR_VSISELECT      = Codec::toCSR(0x250),
        STF_REG_CSR_VSIREG         = Codec::toCSR(0x251),
        STF_REG_CSR_VSIREG2        = Codec::toCSR(0x252),
        STF_REG_CSR_VSIREG3        = Codec::toCSR(0x253),
        STF_REG_CSR_VSIPH          = Codec::toCSR(0x254),
        STF_REG_CSR_VSIREG4        = Codec::toCSR(0x255),
        STF_REG_CSR_VSIREG5        = Codec::toCSR(0x256),
        STF_REG_CSR_VSIREG6        = Codec::toCSR(0x257),
        STF_REG_CSR_VSTOPEI        = Codec::toCSR(0x25c),
        STF_REG_CSR_VSTIMECMPH     = Codec::toCSR(0x25d),
        STF_REG_CSR_VSATP          = Codec::toCSR(0x280),

        // Machine
        STF_REG_CSR_MSTATUS        = Codec::toCSR(0x300),
        STF_REG_CSR_MISA           = Codec::toCSR(0x301),
        STF_REG_CSR_MEDELEG        = Codec::toCSR(0x302),
        STF_REG_CSR_MIDELEG        = Codec::toCSR(0x303),
        STF_REG_CSR_MIE            = Codec::toCSR(0x304),
        STF_REG_CSR_MTVEC          = Codec::toCSR(0x305),
        STF_REG_CSR_MCOUNTEREN     = Codec::toCSR(0x306),
        STF_REG_CSR_MVIEN          = Codec::toCSR(0x308),
        STF_REG_CSR_MVIP           = Codec::toCSR(0x309),
        STF_REG_CSR_MENVCFG        = Codec::toCSR(0x30a),
        STF_REG_CSR_MSTATEEN0      = Codec::toCSR(0x30c),
        STF_REG_CSR_MSTATEEN1      = Codec::toCSR(0x30d),
        STF_REG_CSR_MSTATEEN2      = Codec::toCSR(0x30e),
        STF_REG_CSR_MSTATEEN3      = Codec::toCSR(0x30f),
        STF_REG_CSR_MSTATUSH       = Codec::toCSR(0x310),
        STF_REG_CSR_MEDELEGH       = Codec::toCSR(0x312),
        STF_REG_CSR_MIDELEGH       = Codec::toCSR(0x313),
        STF_REG_CSR_MIEH           = Codec::toCSR(0x314),
        STF_REG_CSR_MVIENH         = Codec::toCSR(0x318),
        STF_REG_CSR_MVIPH          = Codec::toCSR(0x319),
        STF_REG_CSR_MENVCFGH       = Codec::toCSR(0x31a),
        STF_REG_CSR_MSTATEEN0H     = Codec::toCSR(0x31c),
        STF_REG_CSR_MSTATEEN1H     = Codec::toCSR(0x31d),
        STF_REG_CSR_MSTATEEN2H     = Codec::toCSR(0x31e),
        STF_REG_CSR_MSTATEEN3H     = Codec::toCSR(0x31f),
        STF_REG_CSR_MCOUNTINHIBIT  = Codec::toCSR(0x320),
        STF_REG_CSR_MCYCLECFG      = Codec::toCSR(0x321),
        STF_REG_CSR_MINSTRETCFG    = Codec::toCSR(0x322),
        STF_REG_CSR_MHPMEVENT3     = Codec::toCSR(0x323),
        STF_REG_CSR_MHPMEVENT4     = Codec::toCSR(0x324),
        STF_REG_CSR_MHPMEVENT5     = Codec::toCSR(0x325),
        STF_REG_CSR_MHPMEVENT6     = Codec::toCSR(0x326),
        STF_REG_CSR_MHPMEVENT7     = Codec::toCSR(0x327),
        STF_REG_CSR_MHPMEVENT8     = Codec::toCSR(0x328),
        STF_REG_CSR_MHPMEVENT9     = Codec::toCSR(0x329),
        STF_REG_CSR_MHPMEVENT10    = Codec::toCSR(0x32a),
        STF_REG_CSR_MHPMEVENT11    = Codec::toCSR(0x32b),
        STF_REG_CSR_MHPMEVENT12    = Codec::toCSR(0x32c),
        STF_REG_CSR_MHPMEVENT13    = Codec::toCSR(0x32d),
        STF_REG_CSR_MHPMEVENT14    = Codec::toCSR(0x32e),
        STF_REG_CSR_MHPMEVENT15    = Codec::toCSR(0x32f),
        STF_REG_CSR_MHPMEVENT16    = Codec::toCSR(0x330),
        STF_REG_CSR_MHPMEVENT17    = Codec::toCSR(0x331),
        STF_REG_CSR_MHPMEVENT18    = Codec::toCSR(0x332),
        STF_REG_CSR_MHPMEVENT19    = Codec::toCSR(0x333),
        STF_REG_CSR_MHPMEVENT20    = Codec::toCSR(0x334),
        STF_REG_CSR_MHPMEVENT21    = Codec::toCSR(0x335),
        STF_REG_CSR_MHPMEVENT22    = Codec::toCSR(0x336),
        STF_REG_CSR_MHPMEVENT23    = Codec::toCSR(0x337),
        STF_REG_CSR_MHPMEVENT24    = Codec::toCSR(0x338),
        STF_REG_CSR_MHPMEVENT25    = Codec::toCSR(0x339),
        STF_REG_CSR_MHPMEVENT26    = Codec::toCSR(0x33a),
        STF_REG_CSR_MHPMEVENT27    = Codec::toCSR(0x33b),
        STF_REG_CSR_MHPMEVENT28    = Codec::toCSR(0x33c),
        STF_REG_CSR_MHPMEVENT29    = Codec::toCSR(0x33d),
        STF_REG_CSR_MHPMEVENT30    = Codec::toCSR(0x33e),
        STF_REG_CSR_MHPMEVENT31    = Codec::toCSR(0x33f),
        STF_REG_CSR_MSCRATCH       = Codec::toCSR(0x340),
        STF_REG_CSR_MEPC           = Codec::toCSR(0x341),
        STF_REG_CSR_MCAUSE         = Codec::toCSR(0x342),
        STF_REG_CSR_MTVAL          = Codec::toCSR(0x343),
        STF_REG_CSR_MIP            = Codec::toCSR(0x344),
        STF_REG_CSR_MTINST         = Codec::toCSR(0x34a),
        STF_REG_CSR_MTVAL2         = Codec::toCSR(0x34b),
        STF_REG_CSR_MCTRCTL        = Codec::toCSR(0x34e),
        STF_REG_CSR_MISELECT       = Codec::toCSR(0x350),
        STF_REG_CSR_MIREG          = Codec::toCSR(0x351),
        STF_REG_CSR_MIREG2         = Codec::toCSR(0x352),
        STF_REG_CSR_MIREG3         = Codec::toCSR(0x353),
        STF_REG_CSR_MIPH           = Codec::toCSR(0x354),
        STF_REG_CSR_MIREG4         = Codec::toCSR(0x355),
        STF_REG_CSR_MIREG5         = Codec::toCSR(0x356),
        STF_REG_CSR_MIREG6         = Codec::toCSR(0x357),
        STF_REG_CSR_MTOPEI         = Codec::toCSR(0x35c),

        // Machine Memory Protection
        STF_REG_CSR_PMPCFG0        = Codec::toCSR(0x3a0),
        STF_REG_CSR_PMPCFG1        = Codec::toCSR(0x3a1),
        STF_REG_CSR_PMPCFG2        = Codec::toCSR(0x3a2),
        STF_REG_CSR_PMPCFG3        = Codec::toCSR(0x3a3),
        STF_REG_CSR_PMPCFG4        = Codec::toCSR(0x3a4),
        STF_REG_CSR_PMPCFG5        = Codec::toCSR(0x3a5),
        STF_REG_CSR_PMPCFG6        = Codec::toCSR(0x3a6),
        STF_REG_CSR_PMPCFG7        = Codec::toCSR(0x3a7),
        STF_REG_CSR_PMPCFG8        = Codec::toCSR(0x3a8),
        STF_REG_CSR_PMPCFG9        = Codec::toCSR(0x3a9),
        STF_REG_CSR_PMPCFG10       = Codec::toCSR(0x3aa),
        STF_REG_CSR_PMPCFG11       = Codec::toCSR(0x3ab),
        STF_REG_CSR_PMPCFG12       = Codec::toCSR(0x3ac),
        STF_REG_CSR_PMPCFG13       = Codec::toCSR(0x3ad),
        STF_REG_CSR_PMPCFG14       = Codec::toCSR(0x3ae),
        STF_REG_CSR_PMPCFG15       = Codec::toCSR(0x3af),
        STF_REG_CSR_PMPADDR0       = Codec::toCSR(0x3b0),
        STF_REG_CSR_PMPADDR1       = Codec::toCSR(0x3b1),
        STF_REG_CSR_PMPADDR2       = Codec::toCSR(0x3b2),
        STF_REG_CSR_PMPADDR3       = Codec::toCSR(0x3b3),
        STF_REG_CSR_PMPADDR4       = Codec::toCSR(0x3b4),
        STF_REG_CSR_PMPADDR5       = Codec::toCSR(0x3b5),
        STF_REG_CSR_PMPADDR6       = Codec::toCSR(0x3b6),
        STF_REG_CSR_PMPADDR7       = Codec::toCSR(0x3b7),
        STF_REG_CSR_PMPADDR8       = Codec::toCSR(0x3b8),
        STF_REG_CSR_PMPADDR9       = Codec::toCSR(0x3b9),
        STF_REG_CSR_PMPADDR10      = Codec::toCSR(0x3ba),
        STF_REG_CSR_PMPADDR11      = Codec::toCSR(0x3bb),
        STF_REG_CSR_PMPADDR12      = Codec::toCSR(0x3bc),
        STF_REG_CSR_PMPADDR13      = Codec::toCSR(0x3bd),
        STF_REG_CSR_PMPADDR14      = Codec::toCSR(0x3be),
        STF_REG_CSR_PMPADDR15      = Codec::toCSR(0x3bf),
        STF_REG_CSR_PMPADDR16      = Codec::toCSR(0x3c0),
        STF_REG_CSR_PMPADDR17      = Codec::toCSR(0x3c1),
        STF_REG_CSR_PMPADDR18      = Codec::toCSR(0x3c2),
        STF_REG_CSR_PMPADDR19      = Codec::toCSR(0x3c3),
        STF_REG_CSR_PMPADDR20      = Codec::toCSR(0x3c4),
        STF_REG_CSR_PMPADDR21      = Codec::toCSR(0x3c5),
        STF_REG_CSR_PMPADDR22      = Codec::toCSR(0x3c6),
        STF_REG_CSR_PMPADDR23      = Codec::toCSR(0x3c7),
        STF_REG_CSR_PMPADDR24      = Codec::toCSR(0x3c8),
        STF_REG_CSR_PMPADDR25      = Codec::toCSR(0x3c9),
        STF_REG_CSR_PMPADDR26      = Codec::toCSR(0x3ca),
        STF_REG_CSR_PMPADDR27      = Codec::toCSR(0x3cb),
        STF_REG_CSR_PMPADDR28      = Codec::toCSR(0x3cc),
        STF_REG_CSR_PMPADDR29      = Codec::toCSR(0x3cd),
        STF_REG_CSR_PMPADDR30      = Codec::toCSR(0x3ce),
        STF_REG_CSR_PMPADDR31      = Codec::toCSR(0x3cf),
        STF_REG_CSR_PMPADDR32      = Codec::toCSR(0x3d0),
        STF_REG_CSR_PMPADDR33      = Codec::toCSR(0x3d1),
        STF_REG_CSR_PMPADDR34      = Codec::toCSR(0x3d2),
        STF_REG_CSR_PMPADDR35      = Codec::toCSR(0x3d3),
        STF_REG_CSR_PMPADDR36      = Codec::toCSR(0x3d4),
        STF_REG_CSR_PMPADDR37      = Codec::toCSR(0x3d5),
        STF_REG_CSR_PMPADDR38      = Codec::toCSR(0x3d6),
        STF_REG_CSR_PMPADDR39      = Codec::toCSR(0x3d7),
        STF_REG_CSR_PMPADDR40      = Codec::toCSR(0x3d8),
        STF_REG_CSR_PMPADDR41      = Codec::toCSR(0x3d9),
        STF_REG_CSR_PMPADDR42      = Codec::toCSR(0x3da),
        STF_REG_CSR_PMPADDR43      = Codec::toCSR(0x3db),
        STF_REG_CSR_PMPADDR44      = Codec::toCSR(0x3dc),
        STF_REG_CSR_PMPADDR45      = Codec::toCSR(0x3dd),
        STF_REG_CSR_PMPADDR46      = Codec::toCSR(0x3de),
        STF_REG_CSR_PMPADDR47      = Codec::toCSR(0x3df),
        STF_REG_CSR_PMPADDR48      = Codec::toCSR(0x3e0),
        STF_REG_CSR_PMPADDR49      = Codec::toCSR(0x3e1),
        STF_REG_CSR_PMPADDR50      = Codec::toCSR(0x3e2),
        STF_REG_CSR_PMPADDR51      = Codec::toCSR(0x3e3),
        STF_REG_CSR_PMPADDR52      = Codec::toCSR(0x3e4),
        STF_REG_CSR_PMPADDR53      = Codec::toCSR(0x3e5),
        STF_REG_CSR_PMPADDR54      = Codec::toCSR(0x3e6),
        STF_REG_CSR_PMPADDR55      = Codec::toCSR(0x3e7),
        STF_REG_CSR_PMPADDR56      = Codec::toCSR(0x3e8),
        STF_REG_CSR_PMPADDR57      = Codec::toCSR(0x3e9),
        STF_REG_CSR_PMPADDR58      = Codec::toCSR(0x3ea),
        STF_REG_CSR_PMPADDR59      = Codec::toCSR(0x3eb),
        STF_REG_CSR_PMPADDR60      = Codec::toCSR(0x3ec),
        STF_REG_CSR_PMPADDR61      = Codec::toCSR(0x3ed),
        STF_REG_CSR_PMPADDR62      = Codec::toCSR(0x3ee),
        STF_REG_CSR_PMPADDR63      = Codec::toCSR(0x3ef),

        STF_REG_CSR_SCONTEXT       = Codec::toCSR(0x5a8),

        // Hypervisor
        STF_REG_CSR_HSTATUS        = Codec::toCSR(0x600),
        STF_REG_CSR_HEDELEG        = Codec::toCSR(0x602),
        STF_REG_CSR_HIDELEG        = Codec::toCSR(0x603),
        STF_REG_CSR_HIE            = Codec::toCSR(0x604),
        STF_REG_CSR_HTIMEDELTA     = Codec::toCSR(0x605),
        STF_REG_CSR_HCOUNTEREN     = Codec::toCSR(0x606),
        STF_REG_CSR_HGEIE          = Codec::toCSR(0x607),
        STF_REG_CSR_HVIEN          = Codec::toCSR(0x608),
        STF_REG_CSR_HVICTL         = Codec::toCSR(0x609),
        STF_REG_CSR_HENVCFG        = Codec::toCSR(0x60a),
        STF_REG_CSR_HSTATEEN0      = Codec::toCSR(0x60c),
        STF_REG_CSR_HSTATEEN1      = Codec::toCSR(0x60d),
        STF_REG_CSR_HSTATEEN2      = Codec::toCSR(0x60e),
        STF_REG_CSR_HSTATEEN3      = Codec::toCSR(0x60f),
        STF_REG_CSR_HEDELEGH       = Codec::toCSR(0x612),
        STF_REG_CSR_HIDELEGH       = Codec::toCSR(0x613),
        STF_REG_CSR_HTIMEDELTAH    = Codec::toCSR(0x615),
        STF_REG_CSR_HVIENH         = Codec::toCSR(0x618),
        STF_REG_CSR_HENVCFGH       = Codec::toCSR(0x61a),
        STF_REG_CSR_HSTATEEN0H     = Codec::toCSR(0x61c),
        STF_REG_CSR_HSTATEEN1H     = Codec::toCSR(0x61d),
        STF_REG_CSR_HSTATEEN2H     = Codec::toCSR(0x61e),
        STF_REG_CSR_HSTATEEN3H     = Codec::toCSR(0x61f),
        STF_REG_CSR_HTVAL          = Codec::toCSR(0x643),
        STF_REG_CSR_HIP            = Codec::toCSR(0x644),
        STF_REG_CSR_HVIP           = Codec::toCSR(0x645),
        STF_REG_CSR_HVIPRIO1       = Codec::toCSR(0x646),
        STF_REG_CSR_HVIPRIO2       = Codec::toCSR(0x647),
        STF_REG_CSR_HTINST         = Codec::toCSR(0x64a),
        STF_REG_CSR_HVIPH          = Codec::toCSR(0x655),
        STF_REG_CSR_HVIPRIO1H      = Codec::toCSR(0x656),
        STF_REG_CSR_HVIPRIO2H      = Codec::toCSR(0x657),
        STF_REG_CSR_HGATP          = Codec::toCSR(0x680),
        STF_REG_CSR_HCONTEXT       = Codec::toCSR(0x6a8),

        // Machine Mode Monitoring Counters (upper half values, RV32 only)
        STF_REG_CSR_MCYCLECFGH     = Codec::toCSR(0x721),
        STF_REG_CSR_MINSTRETCFGH   = Codec::toCSR(0x722),
        STF_REG_CSR_MHPMEVENT3H    = Codec::toCSR(0x723),
        STF_REG_CSR_MHPMEVENT4H    = Codec::toCSR(0x724),
        STF_REG_CSR_MHPMEVENT5H    = Codec::toCSR(0x725),
        STF_REG_CSR_MHPMEVENT6H    = Codec::toCSR(0x726),
        STF_REG_CSR_MHPMEVENT7H    = Codec::toCSR(0x727),
        STF_REG_CSR_MHPMEVENT8H    = Codec::toCSR(0x728),
        STF_REG_CSR_MHPMEVENT9H    = Codec::toCSR(0x729),
        STF_REG_CSR_MHPMEVENT10H   = Codec::toCSR(0x72a),
        STF_REG_CSR_MHPMEVENT11H   = Codec::toCSR(0x72b),
        STF_REG_CSR_MHPMEVENT12H   = Codec::toCSR(0x72c),
        STF_REG_CSR_MHPMEVENT13H   = Codec::toCSR(0x72d),
        STF_REG_CSR_MHPMEVENT14H   = Codec::toCSR(0x72e),
        STF_REG_CSR_MHPMEVENT15H   = Codec::toCSR(0x72f),
        STF_REG_CSR_MHPMEVENT16H   = Codec::toCSR(0x730),
        STF_REG_CSR_MHPMEVENT17H   = Codec::toCSR(0x731),
        STF_REG_CSR_MHPMEVENT18H   = Codec::toCSR(0x732),
        STF_REG_CSR_MHPMEVENT19H   = Codec::toCSR(0x733),
        STF_REG_CSR_MHPMEVENT20H   = Codec::toCSR(0x734),
        STF_REG_CSR_MHPMEVENT21H   = Codec::toCSR(0x735),
        STF_REG_CSR_MHPMEVENT22H   = Codec::toCSR(0x736),
        STF_REG_CSR_MHPMEVENT23H   = Codec::toCSR(0x737),
        STF_REG_CSR_MHPMEVENT24H   = Codec::toCSR(0x738),
        STF_REG_CSR_MHPMEVENT25H   = Codec::toCSR(0x739),
        STF_REG_CSR_MHPMEVENT26H   = Codec::toCSR(0x73a),
        STF_REG_CSR_MHPMEVENT27H   = Codec::toCSR(0x73b),
        STF_REG_CSR_MHPMEVENT28H   = Codec::toCSR(0x73c),
        STF_REG_CSR_MHPMEVENT29H   = Codec::toCSR(0x73d),
        STF_REG_CSR_MHPMEVENT30H   = Codec::toCSR(0x73e),
        STF_REG_CSR_MHPMEVENT31H   = Codec::toCSR(0x73f),

        // Machine non-maskable interrupts
        STF_REG_CSR_MNSCRATCH      = Codec::toCSR(0x740),
        STF_REG_CSR_MNEPC          = Codec::toCSR(0x741),
        STF_REG_CSR_MNCAUSE        = Codec::toCSR(0x742),
        STF_REG_CSR_MNSTATUS       = Codec::toCSR(0x744),

        // Machine Security Config Registers
        STF_REG_CSR_MSECCFG        = Codec::toCSR(0x747),
        STF_REG_CSR_MSECCFGH       = Codec::toCSR(0x757),

        // Basic Machine Counters
        STF_REG_CSR_MCYCLE         = Codec::toCSR(0xb00),
        STF_REG_CSR_MINSTRET       = Codec::toCSR(0xb02),
        STF_REG_CSR_MHPMCOUNTER3   = Codec::toCSR(0xb03),
        STF_REG_CSR_MHPMCOUNTER4   = Codec::toCSR(0xb04),
        STF_REG_CSR_MHPMCOUNTER5   = Codec::toCSR(0xb05),
        STF_REG_CSR_MHPMCOUNTER6   = Codec::toCSR(0xb06),
        STF_REG_CSR_MHPMCOUNTER7   = Codec::toCSR(0xb07),
        STF_REG_CSR_MHPMCOUNTER8   = Codec::toCSR(0xb08),
        STF_REG_CSR_MHPMCOUNTER9   = Codec::toCSR(0xb09),
        STF_REG_CSR_MHPMCOUNTER10  = Codec::toCSR(0xb0a),
        STF_REG_CSR_MHPMCOUNTER11  = Codec::toCSR(0xb0b),
        STF_REG_CSR_MHPMCOUNTER12  = Codec::toCSR(0xb0c),
        STF_REG_CSR_MHPMCOUNTER13  = Codec::toCSR(0xb0d),
        STF_REG_CSR_MHPMCOUNTER14  = Codec::toCSR(0xb0e),
        STF_REG_CSR_MHPMCOUNTER15  = Codec::toCSR(0xb0f),
        STF_REG_CSR_MHPMCOUNTER16  = Codec::toCSR(0xb10),
        STF_REG_CSR_MHPMCOUNTER17  = Codec::toCSR(0xb11),
        STF_REG_CSR_MHPMCOUNTER18  = Codec::toCSR(0xb12),
        STF_REG_CSR_MHPMCOUNTER19  = Codec::toCSR(0xb13),
        STF_REG_CSR_MHPMCOUNTER20  = Codec::toCSR(0xb14),
        STF_REG_CSR_MHPMCOUNTER21  = Codec::toCSR(0xb15),
        STF_REG_CSR_MHPMCOUNTER22  = Codec::toCSR(0xb16),
        STF_REG_CSR_MHPMCOUNTER23  = Codec::toCSR(0xb17),
        STF_REG_CSR_MHPMCOUNTER24  = Codec::toCSR(0xb18),
        STF_REG_CSR_MHPMCOUNTER25  = Codec::toCSR(0xb19),
        STF_REG_CSR_MHPMCOUNTER26  = Codec::toCSR(0xb1a),
        STF_REG_CSR_MHPMCOUNTER27  = Codec::toCSR(0xb1b),
        STF_REG_CSR_MHPMCOUNTER28  = Codec::toCSR(0xb1c),
        STF_REG_CSR_MHPMCOUNTER29  = Codec::toCSR(0xb1d),
        STF_REG_CSR_MHPMCOUNTER30  = Codec::toCSR(0xb1e),
        STF_REG_CSR_MHPMCOUNTER31  = Codec::toCSR(0xb1f),

        // Basic Machine Performance Monitoring Counters
        STF_REG_CSR_MCYCLEH        = Codec::toCSR(0xb80),
        STF_REG_CSR_MINSTRETH      = Codec::toCSR(0xb82),
        STF_REG_CSR_MHPMCOUNTER3H  = Codec::toCSR(0xb83),
        STF_REG_CSR_MHPMCOUNTER4H  = Codec::toCSR(0xb84),
        STF_REG_CSR_MHPMCOUNTER5H  = Codec::toCSR(0xb85),
        STF_REG_CSR_MHPMCOUNTER6H  = Codec::toCSR(0xb86),
        STF_REG_CSR_MHPMCOUNTER7H  = Codec::toCSR(0xb87),
        STF_REG_CSR_MHPMCOUNTER8H  = Codec::toCSR(0xb88),
        STF_REG_CSR_MHPMCOUNTER9H  = Codec::toCSR(0xb89),
        STF_REG_CSR_MHPMCOUNTER10H = Codec::toCSR(0xb8a),
        STF_REG_CSR_MHPMCOUNTER11H = Codec::toCSR(0xb8b),
        STF_REG_CSR_MHPMCOUNTER12H = Codec::toCSR(0xb8c),
        STF_REG_CSR_MHPMCOUNTER13H = Codec::toCSR(0xb8d),
        STF_REG_CSR_MHPMCOUNTER14H = Codec::toCSR(0xb8e),
        STF_REG_CSR_MHPMCOUNTER15H = Codec::toCSR(0xb8f),
        STF_REG_CSR_MHPMCOUNTER16H = Codec::toCSR(0xb90),
        STF_REG_CSR_MHPMCOUNTER17H = Codec::toCSR(0xb91),
        STF_REG_CSR_MHPMCOUNTER18H = Codec::toCSR(0xb92),
        STF_REG_CSR_MHPMCOUNTER19H = Codec::toCSR(0xb93),
        STF_REG_CSR_MHPMCOUNTER20H = Codec::toCSR(0xb94),
        STF_REG_CSR_MHPMCOUNTER21H = Codec::toCSR(0xb95),
        STF_REG_CSR_MHPMCOUNTER22H = Codec::toCSR(0xb96),
        STF_REG_CSR_MHPMCOUNTER23H = Codec::toCSR(0xb97),
        STF_REG_CSR_MHPMCOUNTER24H = Codec::toCSR(0xb98),
        STF_REG_CSR_MHPMCOUNTER25H = Codec::toCSR(0xb99),
        STF_REG_CSR_MHPMCOUNTER26H = Codec::toCSR(0xb9a),
        STF_REG_CSR_MHPMCOUNTER27H = Codec::toCSR(0xb9b),
        STF_REG_CSR_MHPMCOUNTER28H = Codec::toCSR(0xb9c),
        STF_REG_CSR_MHPMCOUNTER29H = Codec::toCSR(0xb9d),
        STF_REG_CSR_MHPMCOUNTER30H = Codec::toCSR(0xb9e),
        STF_REG_CSR_MHPMCOUNTER31H = Codec::toCSR(0xb9f),

        // Basic User Counters
        STF_REG_CSR_CYCLE          = Codec::toCSR(0xc00),
        STF_REG_CSR_TIME           = Codec::toCSR(0xc01),
        STF_REG_CSR_INSTRET        = Codec::toCSR(0xc02),
        STF_REG_CSR_HPMCOUNTER3    = Codec::toCSR(0xc03),
        STF_REG_CSR_HPMCOUNTER4    = Codec::toCSR(0xc04),
        STF_REG_CSR_HPMCOUNTER5    = Codec::toCSR(0xc05),
        STF_REG_CSR_HPMCOUNTER6    = Codec::toCSR(0xc06),
        STF_REG_CSR_HPMCOUNTER7    = Codec::toCSR(0xc07),
        STF_REG_CSR_HPMCOUNTER8    = Codec::toCSR(0xc08),
        STF_REG_CSR_HPMCOUNTER9    = Codec::toCSR(0xc09),
        STF_REG_CSR_HPMCOUNTER10   = Codec::toCSR(0xc0a),
        STF_REG_CSR_HPMCOUNTER11   = Codec::toCSR(0xc0b),
        STF_REG_CSR_HPMCOUNTER12   = Codec::toCSR(0xc0c),
        STF_REG_CSR_HPMCOUNTER13   = Codec::toCSR(0xc0d),
        STF_REG_CSR_HPMCOUNTER14   = Codec::toCSR(0xc0e),
        STF_REG_CSR_HPMCOUNTER15   = Codec::toCSR(0xc0f),
        STF_REG_CSR_HPMCOUNTER16   = Codec::toCSR(0xc10),
        STF_REG_CSR_HPMCOUNTER17   = Codec::toCSR(0xc11),
        STF_REG_CSR_HPMCOUNTER18   = Codec::toCSR(0xc12),
        STF_REG_CSR_HPMCOUNTER19   = Codec::toCSR(0xc13),
        STF_REG_CSR_HPMCOUNTER20   = Codec::toCSR(0xc14),
        STF_REG_CSR_HPMCOUNTER21   = Codec::toCSR(0xc15),
        STF_REG_CSR_HPMCOUNTER22   = Codec::toCSR(0xc16),
        STF_REG_CSR_HPMCOUNTER23   = Codec::toCSR(0xc17),
        STF_REG_CSR_HPMCOUNTER24   = Codec::toCSR(0xc18),
        STF_REG_CSR_HPMCOUNTER25   = Codec::toCSR(0xc19),
        STF_REG_CSR_HPMCOUNTER26   = Codec::toCSR(0xc1a),
        STF_REG_CSR_HPMCOUNTER27   = Codec::toCSR(0xc1b),
        STF_REG_CSR_HPMCOUNTER28   = Codec::toCSR(0xc1c),
        STF_REG_CSR_HPMCOUNTER29   = Codec::toCSR(0xc1d),
        STF_REG_CSR_HPMCOUNTER30   = Codec::toCSR(0xc1e),
        STF_REG_CSR_HPMCOUNTER31   = Codec::toCSR(0xc1f),

        // Vector
        STF_REG_CSR_VL             = Codec::toCSR(0xc20),
        STF_REG_CSR_VTYPE          = Codec::toCSR(0xc21),
        STF_REG_CSR_VLENB          = Codec::toCSR(0xc22),

        // Basic User Performance Monitoring Counters (upper half values, RV32 only)
        STF_REG_CSR_CYCLEH         = Codec::toCSR(0xc80),
        STF_REG_CSR_TIMEH          = Codec::toCSR(0xc81),
        STF_REG_CSR_INSTRETH       = Codec::toCSR(0xc82),
        STF_REG_CSR_HPMCOUNTER3H   = Codec::toCSR(0xc83),
        STF_REG_CSR_HPMCOUNTER4H   = Codec::toCSR(0xc84),
        STF_REG_CSR_HPMCOUNTER5H   = Codec::toCSR(0xc85),
        STF_REG_CSR_HPMCOUNTER6H   = Codec::toCSR(0xc86),
        STF_REG_CSR_HPMCOUNTER7H   = Codec::toCSR(0xc87),
        STF_REG_CSR_HPMCOUNTER8H   = Codec::toCSR(0xc88),
        STF_REG_CSR_HPMCOUNTER9H   = Codec::toCSR(0xc89),
        STF_REG_CSR_HPMCOUNTER10H  = Codec::toCSR(0xc8a),
        STF_REG_CSR_HPMCOUNTER11H  = Codec::toCSR(0xc8b),
        STF_REG_CSR_HPMCOUNTER12H  = Codec::toCSR(0xc8c),
        STF_REG_CSR_HPMCOUNTER13H  = Codec::toCSR(0xc8d),
        STF_REG_CSR_HPMCOUNTER14H  = Codec::toCSR(0xc8e),
        STF_REG_CSR_HPMCOUNTER15H  = Codec::toCSR(0xc8f),
        STF_REG_CSR_HPMCOUNTER16H  = Codec::toCSR(0xc90),
        STF_REG_CSR_HPMCOUNTER17H  = Codec::toCSR(0xc91),
        STF_REG_CSR_HPMCOUNTER18H  = Codec::toCSR(0xc92),
        STF_REG_CSR_HPMCOUNTER19H  = Codec::toCSR(0xc93),
        STF_REG_CSR_HPMCOUNTER20H  = Codec::toCSR(0xc94),
        STF_REG_CSR_HPMCOUNTER21H  = Codec::toCSR(0xc95),
        STF_REG_CSR_HPMCOUNTER22H  = Codec::toCSR(0xc96),
        STF_REG_CSR_HPMCOUNTER23H  = Codec::toCSR(0xc97),
        STF_REG_CSR_HPMCOUNTER24H  = Codec::toCSR(0xc98),
        STF_REG_CSR_HPMCOUNTER25H  = Codec::toCSR(0xc99),
        STF_REG_CSR_HPMCOUNTER26H  = Codec::toCSR(0xc9a),
        STF_REG_CSR_HPMCOUNTER27H  = Codec::toCSR(0xc9b),
        STF_REG_CSR_HPMCOUNTER28H  = Codec::toCSR(0xc9c),
        STF_REG_CSR_HPMCOUNTER29H  = Codec::toCSR(0xc9d),
        STF_REG_CSR_HPMCOUNTER30H  = Codec::toCSR(0xc9e),
        STF_REG_CSR_HPMCOUNTER31H  = Codec::toCSR(0xc9f),

        // Additional supervisor
        STF_REG_CSR_SCOUNTOVF      = Codec::toCSR(0xda0),
        STF_REG_CSR_STOPI          = Codec::toCSR(0xdb0),

        // Additional hypervisor
        STF_REG_CSR_HGEIP          = Codec::toCSR(0xe12),

        // Additional virtual supervisor
        STF_REG_CSR_VSTOPI         = Codec::toCSR(0xeb0),

        // Machine Information Registers
        STF_REG_CSR_MVENDORID      = Codec::toCSR(0xf11),
        STF_REG_CSR_MARCHID        = Codec::toCSR(0xf12),
        STF_REG_CSR_MIMPID         = Codec::toCSR(0xf13),
        STF_REG_CSR_MHARTID        = Codec::toCSR(0xf14),
        STF_REG_CSR_MCONFIGPTR     = Codec::toCSR(0xf15),

        // Additional machine mode
        STF_REG_CSR_MTOPI          = Codec::toCSR(0xfb0),

        STF_REG_INVALID            = std::numeric_limits<STF_REG_int>::max()
    };

    namespace register_utils
    {
        /**
         * List of all registers that are only available in RV32
         */
        static constexpr auto RV32_CSRs = std::to_array({
            Registers::STF_REG::STF_REG_CSR_SIEH,
            Registers::STF_REG::STF_REG_CSR_SIPH,
            Registers::STF_REG::STF_REG_CSR_STIMECMPH,
            Registers::STF_REG::STF_REG_CSR_VSIEH,
            Registers::STF_REG::STF_REG_CSR_VSIPH,
            Registers::STF_REG::STF_REG_CSR_VSTIMECMPH,
            Registers::STF_REG::STF_REG_CSR_MSTATUSH,
            Registers::STF_REG::STF_REG_CSR_MEDELEGH,
            Registers::STF_REG::STF_REG_CSR_MIDELEGH,
            Registers::STF_REG::STF_REG_CSR_MIEH,
            Registers::STF_REG::STF_REG_CSR_MVIENH,
            Registers::STF_REG::STF_REG_CSR_MVIPH,
            Registers::STF_REG::STF_REG_CSR_MENVCFGH,
            Registers::STF_REG::STF_REG_CSR_MSTATEEN0H,
            Registers::STF_REG::STF_REG_CSR_MSTATEEN1H,
            Registers::STF_REG::STF_REG_CSR_MSTATEEN2H,
            Registers::STF_REG::STF_REG_CSR_MSTATEEN3H,
            Registers::STF_REG::STF_REG_CSR_MIPH,
            Registers::STF_REG::STF_REG_CSR_HEDELEGH,
            Registers::STF_REG::STF_REG_CSR_HIDELEGH,
            Registers::STF_REG::STF_REG_CSR_HTIMEDELTAH,
            Registers::STF_REG::STF_REG_CSR_HVIENH,
            Registers::STF_REG::STF_REG_CSR_HENVCFGH,
            Registers::STF_REG::STF_REG_CSR_HSTATEEN0H,
            Registers::STF_REG::STF_REG_CSR_HSTATEEN1H,
            Registers::STF_REG::STF_REG_CSR_HSTATEEN2H,
            Registers::STF_REG::STF_REG_CSR_HSTATEEN3H,
            Registers::STF_REG::STF_REG_CSR_HVIPH,
            Registers::STF_REG::STF_REG_CSR_HVIPRIO1H,
            Registers::STF_REG::STF_REG_CSR_HVIPRIO2H,
            Registers::STF_REG::STF_REG_CSR_MCYCLECFGH,
            Registers::STF_REG::STF_REG_CSR_MINSTRETCFGH,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT3H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT4H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT5H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT6H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT7H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT8H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT9H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT10H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT11H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT12H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT13H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT14H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT15H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT16H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT17H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT18H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT19H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT20H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT21H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT22H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT23H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT24H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT25H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT26H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT27H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT28H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT29H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT30H,
            Registers::STF_REG::STF_REG_CSR_MHPMEVENT31H,
            Registers::STF_REG::STF_REG_CSR_MSECCFGH,
            Registers::STF_REG::STF_REG_CSR_MCYCLEH,
            Registers::STF_REG::STF_REG_CSR_MINSTRETH,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER3H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER4H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER5H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER6H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER7H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER8H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER9H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER10H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER11H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER12H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER13H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER14H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER15H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER16H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER17H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER18H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER19H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER20H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER21H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER22H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER23H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER24H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER25H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER26H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER27H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER28H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER29H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER30H,
            Registers::STF_REG::STF_REG_CSR_MHPMCOUNTER31H,
            Registers::STF_REG::STF_REG_CSR_CYCLEH,
            Registers::STF_REG::STF_REG_CSR_TIMEH,
            Registers::STF_REG::STF_REG_CSR_INSTRETH,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER3H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER4H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER5H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER6H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER7H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER8H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER9H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER10H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER11H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER12H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER13H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER14H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER15H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER16H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER17H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER18H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER19H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER20H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER21H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER22H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER23H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER24H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER25H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER26H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER27H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER28H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER29H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER30H,
            Registers::STF_REG::STF_REG_CSR_HPMCOUNTER31H,
        });
    }

    /**
     * Writes an STF_REG to an ostream
     * \param os ostream to use
     * \param reg register to format
     */
    std::ostream& operator<<(std::ostream& os, Registers::STF_REG reg);

    template<size_t num_bits>
    static constexpr uint64_t calcRegMask() {
        return byte_utils::bitMask<uint64_t, num_bits>();
    }

} // end namespace stf

//end of __STF_REGISTER_DEF_H_
#endif
