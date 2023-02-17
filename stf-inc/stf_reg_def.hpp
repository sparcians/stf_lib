
// <STF_Reg_Def> -*- HPP -*-

#ifndef __STF_REGISTER_DEF_H_
#define __STF_REGISTER_DEF_H_

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
            enum class STF_REG_TYPE : STF_REG_metadata_int {
                RESERVED = 0,
                INTEGER = 1,
                FLOATING_POINT = 2,
                VECTOR = 3,
                CSR = 4
            };

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

                static constexpr size_t UIE_WIDTH = 1; /**< Width of UIE register */ // cppcheck-suppress unusedStructMember
                static constexpr size_t SIE_WIDTH = 1; /**< Width of SIE register */ // cppcheck-suppress unusedStructMember
                static constexpr size_t SIE_SHIFT = UIE_WIDTH; /**< SIE shift amount */ // cppcheck-suppress unusedStructMember

                static constexpr size_t MCYCLEH_WIDTH = 32; /**< Width of MCYCLEH register */ // cppcheck-suppress unusedStructMember
                static constexpr size_t MCYCLEH_SHIFT = 32; /**< MCYCLEH shift amount */ // cppcheck-suppress unusedStructMember

                static constexpr size_t MINSTRETH_WIDTH = 32; /**< Width of MINSTRETH register */ // cppcheck-suppress unusedStructMember
                static constexpr size_t MINSTRETH_SHIFT = 32; /**< MINSTRETH shift amount */ // cppcheck-suppress unusedStructMember

                static constexpr size_t CYCLEH_WIDTH = 32; /**< Width of CYCLEH register */ // cppcheck-suppress unusedStructMember
                static constexpr size_t CYCLEH_SHIFT = 32; /**< CYCLEH shift amount */ // cppcheck-suppress unusedStructMember

                static constexpr size_t TIMEH_WIDTH = 32; /**< Width of TIMEH register */ // cppcheck-suppress unusedStructMember
                static constexpr size_t TIMEH_SHIFT = 32; /**< TIMEH shift amount */ // cppcheck-suppress unusedStructMember

                static constexpr size_t INSTRETH_WIDTH = 32; /**< Width of INSTRETH register */ // cppcheck-suppress unusedStructMember
                static constexpr size_t INSTRETH_SHIFT = 32; /**< INSTRETH shift amount */ // cppcheck-suppress unusedStructMember

                static constexpr size_t MSECCFGH_WIDTH = 32; /**< Width of MSECCFGH register */ // cppcheck-suppress unusedStructMember
                static constexpr size_t MSECCFGH_SHIFT = 32; /**< MSECCFGH shift amount */ // cppcheck-suppress unusedStructMember

                static constexpr size_t MSTATUSH_WIDTH = 32; /**< Width of MSTATUSH register */ // cppcheck-suppress unusedStructMember
                static constexpr size_t MSTATUSH_SHIFT = 32; /**< MSTATUSH shift amount */ // cppcheck-suppress unusedStructMember

                static constexpr size_t MENVCFGH_WIDTH = 32; /**< Width of MENVCFGH register */ // cppcheck-suppress unusedStructMember
                static constexpr size_t MENVCFGH_SHIFT = 32; /**< MENVCFGH shift amount */ // cppcheck-suppress unusedStructMember
            };

            /**
             * \enum STF_REG_OPERAND_TYPE
             * Defines the different types of register records
             */
            enum class STF_REG_OPERAND_TYPE : STF_REG_metadata_int {
                REG_RESERVED    = 0,
                REG_STATE       = 1,
                REG_SOURCE      = 2,
                REG_DEST        = 3
            };

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
                        constexpr STF_REG_int NONSTANDARD_RW_START = toCSR(0x7c0);
                        constexpr STF_REG_int NONSTANDARD_RW_END = toCSR(0x7ff);
                        constexpr STF_REG_int NONSTANDARD_RW2_START = toCSR(0xbc0);
                        constexpr STF_REG_int NONSTANDARD_RW2_END = toCSR(0xbff);
                        constexpr STF_REG_int NONSTANDARD_RO_START = toCSR(0xfc0);
                        constexpr STF_REG_int NONSTANDARD_RO_END = toCSR(0xfff);

                        const STF_REG_int reg_num = enums::to_int(reg);

                        return ((NONSTANDARD_RW_START <= reg_num) && (reg_num <= NONSTANDARD_RW_END)) ||
                               ((NONSTANDARD_RW2_START <= reg_num) && (reg_num <= NONSTANDARD_RW2_END)) ||
                               ((NONSTANDARD_RO_START <= reg_num) && (reg_num <= NONSTANDARD_RO_END));
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

    enum class Registers::STF_REG : Registers::STF_REG_int {
        // 32 int registers
        STF_REG_X0                = Codec::toGPR(0x0000),
        STF_REG_X1                = Codec::toGPR(0x0001),
        STF_REG_X2                = Codec::toGPR(0x0002),
        STF_REG_X3                = Codec::toGPR(0x0003),
        STF_REG_X4                = Codec::toGPR(0x0004),
        STF_REG_X5                = Codec::toGPR(0x0005),
        STF_REG_X6                = Codec::toGPR(0x0006),
        STF_REG_X7                = Codec::toGPR(0x0007),
        STF_REG_X8                = Codec::toGPR(0x0008),
        STF_REG_X9                = Codec::toGPR(0x0009),
        STF_REG_X10               = Codec::toGPR(0x000A),
        STF_REG_X11               = Codec::toGPR(0x000B),
        STF_REG_X12               = Codec::toGPR(0x000C),
        STF_REG_X13               = Codec::toGPR(0x000D),
        STF_REG_X14               = Codec::toGPR(0x000E),
        STF_REG_X15               = Codec::toGPR(0x000F),
        STF_REG_X16               = Codec::toGPR(0x0010),
        STF_REG_X17               = Codec::toGPR(0x0011),
        STF_REG_X18               = Codec::toGPR(0x0012),
        STF_REG_X19               = Codec::toGPR(0x0013),
        STF_REG_X20               = Codec::toGPR(0x0014),
        STF_REG_X21               = Codec::toGPR(0x0015),
        STF_REG_X22               = Codec::toGPR(0x0016),
        STF_REG_X23               = Codec::toGPR(0x0017),
        STF_REG_X24               = Codec::toGPR(0x0018),
        STF_REG_X25               = Codec::toGPR(0x0019),
        STF_REG_X26               = Codec::toGPR(0x001A),
        STF_REG_X27               = Codec::toGPR(0x001B),
        STF_REG_X28               = Codec::toGPR(0x001C),
        STF_REG_X29               = Codec::toGPR(0x001D),
        STF_REG_X30               = Codec::toGPR(0x001E),
        STF_REG_X31               = Codec::toGPR(0x001F),
        STF_REG_PC                = Codec::toGPR(0x0020),

        // 32 floating point registers
        STF_REG_F0                = Codec::toFPR(0x0000),
        STF_REG_F1                = Codec::toFPR(0x0001),
        STF_REG_F2                = Codec::toFPR(0x0002),
        STF_REG_F3                = Codec::toFPR(0x0003),
        STF_REG_F4                = Codec::toFPR(0x0004),
        STF_REG_F5                = Codec::toFPR(0x0005),
        STF_REG_F6                = Codec::toFPR(0x0006),
        STF_REG_F7                = Codec::toFPR(0x0007),
        STF_REG_F8                = Codec::toFPR(0x0008),
        STF_REG_F9                = Codec::toFPR(0x0009),
        STF_REG_F10               = Codec::toFPR(0x000A),
        STF_REG_F11               = Codec::toFPR(0x000B),
        STF_REG_F12               = Codec::toFPR(0x000C),
        STF_REG_F13               = Codec::toFPR(0x000D),
        STF_REG_F14               = Codec::toFPR(0x000E),
        STF_REG_F15               = Codec::toFPR(0x000F),
        STF_REG_F16               = Codec::toFPR(0x0010),
        STF_REG_F17               = Codec::toFPR(0x0011),
        STF_REG_F18               = Codec::toFPR(0x0012),
        STF_REG_F19               = Codec::toFPR(0x0013),
        STF_REG_F20               = Codec::toFPR(0x0014),
        STF_REG_F21               = Codec::toFPR(0x0015),
        STF_REG_F22               = Codec::toFPR(0x0016),
        STF_REG_F23               = Codec::toFPR(0x0017),
        STF_REG_F24               = Codec::toFPR(0x0018),
        STF_REG_F25               = Codec::toFPR(0x0019),
        STF_REG_F26               = Codec::toFPR(0x001A),
        STF_REG_F27               = Codec::toFPR(0x001B),
        STF_REG_F28               = Codec::toFPR(0x001C),
        STF_REG_F29               = Codec::toFPR(0x001D),
        STF_REG_F30               = Codec::toFPR(0x001E),
        STF_REG_F31               = Codec::toFPR(0x001F),

        // 32 vector registers
        STF_REG_V0                = Codec::toVector(0x0000),
        STF_REG_V1                = Codec::toVector(0x0001),
        STF_REG_V2                = Codec::toVector(0x0002),
        STF_REG_V3                = Codec::toVector(0x0003),
        STF_REG_V4                = Codec::toVector(0x0004),
        STF_REG_V5                = Codec::toVector(0x0005),
        STF_REG_V6                = Codec::toVector(0x0006),
        STF_REG_V7                = Codec::toVector(0x0007),
        STF_REG_V8                = Codec::toVector(0x0008),
        STF_REG_V9                = Codec::toVector(0x0009),
        STF_REG_V10               = Codec::toVector(0x000A),
        STF_REG_V11               = Codec::toVector(0x000B),
        STF_REG_V12               = Codec::toVector(0x000C),
        STF_REG_V13               = Codec::toVector(0x000D),
        STF_REG_V14               = Codec::toVector(0x000E),
        STF_REG_V15               = Codec::toVector(0x000F),
        STF_REG_V16               = Codec::toVector(0x0010),
        STF_REG_V17               = Codec::toVector(0x0011),
        STF_REG_V18               = Codec::toVector(0x0012),
        STF_REG_V19               = Codec::toVector(0x0013),
        STF_REG_V20               = Codec::toVector(0x0014),
        STF_REG_V21               = Codec::toVector(0x0015),
        STF_REG_V22               = Codec::toVector(0x0016),
        STF_REG_V23               = Codec::toVector(0x0017),
        STF_REG_V24               = Codec::toVector(0x0018),
        STF_REG_V25               = Codec::toVector(0x0019),
        STF_REG_V26               = Codec::toVector(0x001A),
        STF_REG_V27               = Codec::toVector(0x001B),
        STF_REG_V28               = Codec::toVector(0x001C),
        STF_REG_V29               = Codec::toVector(0x001D),
        STF_REG_V30               = Codec::toVector(0x001E),
        STF_REG_V31               = Codec::toVector(0x001F),

        // Control and status registers
        // User
        STF_REG_CSR_USTATUS       = Codec::toCSR(0x000),
        STF_REG_CSR_FFLAGS        = Codec::toCSR(0x001),
        STF_REG_CSR_FRM           = Codec::toCSR(0x002),
        STF_REG_CSR_FCSR          = Codec::toCSR(0x003),
        STF_REG_CSR_UIE           = Codec::toCSR(0x004),
        STF_REG_CSR_UTVEC         = Codec::toCSR(0x005),
        STF_REG_CSR_UTVT          = Codec::toCSR(0x007),
        STF_REG_CSR_VSTART        = Codec::toCSR(0x008),
        STF_REG_CSR_VXSAT         = Codec::toCSR(0x009),
        STF_REG_CSR_VXRM          = Codec::toCSR(0x00a),
        STF_REG_CSR_VCSR          = Codec::toCSR(0x00f),
        STF_REG_CSR_USCRATCH      = Codec::toCSR(0x040),
        STF_REG_CSR_UEPC          = Codec::toCSR(0x041),
        STF_REG_CSR_UCAUSE        = Codec::toCSR(0x042),
        STF_REG_CSR_UTVAL         = Codec::toCSR(0x043),
        STF_REG_CSR_UIP           = Codec::toCSR(0x044),
        STF_REG_CSR_UNXTI         = Codec::toCSR(0x045),
        STF_REG_CSR_UINTSTATUS    = Codec::toCSR(0x046),
        STF_REG_CSR_USCRATCHCSW   = Codec::toCSR(0x048),
        STF_REG_CSR_USCRATCHCSWL  = Codec::toCSR(0x049),

        // Supervisor
        STF_REG_CSR_SSTATUS       = Codec::toCSR(0x100),
        STF_REG_CSR_SEDELEG       = Codec::toCSR(0x102),
        STF_REG_CSR_SIDELEG       = Codec::toCSR(0x103),
        STF_REG_CSR_SIE           = Codec::toCSR(0x104),
        STF_REG_CSR_STVEC         = Codec::toCSR(0x105),
        STF_REG_CSR_SCOUNTEREN    = Codec::toCSR(0x106),
        STF_REG_CSR_STVT          = Codec::toCSR(0x107),
        STF_REG_CSR_SENVCFG       = Codec::toCSR(0x10a),
        STF_REG_CSR_SSCRATCH      = Codec::toCSR(0x140),
        STF_REG_CSR_SEPC          = Codec::toCSR(0x141),
        STF_REG_CSR_SCAUSE        = Codec::toCSR(0x142),
        STF_REG_CSR_STVAL         = Codec::toCSR(0x143),
        STF_REG_CSR_SIP           = Codec::toCSR(0x144),
        STF_REG_CSR_SNXTI         = Codec::toCSR(0x145),
        STF_REG_CSR_SINTSTATUS    = Codec::toCSR(0x146),
        STF_REG_CSR_SSCRATCHCSW   = Codec::toCSR(0x148),
        STF_REG_CSR_SSCRATCHCSWL  = Codec::toCSR(0x149),
        STF_REG_CSR_SATP          = Codec::toCSR(0x180),
        // SENVCFG was (erroneously) defined as 0x19a originally. We need to keep this definition around until there
        // is an official 0x19a CSR to prevent breakage with existing traces
        STF_REG_CSR_SENVCFG_COMPAT = Codec::toCSR(0x19a),

        // External debug
        STF_REG_CSR_DMCONTROL     = Codec::toCSR(0x010),
        STF_REG_CSR_DMSTATUS      = Codec::toCSR(0x011),
        STF_REG_CSR_TSELECT       = Codec::toCSR(0x7a0),
        STF_REG_CSR_TDATA1        = Codec::toCSR(0x7a1),
        STF_REG_CSR_TDATA2        = Codec::toCSR(0x7a2),
        STF_REG_CSR_TDATA3        = Codec::toCSR(0x7a3),
        STF_REG_CSR_TINFO         = Codec::toCSR(0x7a4),
        STF_REG_CSR_TCONTROL      = Codec::toCSR(0x7a5),
        STF_REG_CSR_MCONTEXT      = Codec::toCSR(0x7a8),
        STF_REG_CSR_SCONTEXT      = Codec::toCSR(0x7aa),
        STF_REG_CSR_DCSR          = Codec::toCSR(0x7b0),
        STF_REG_CSR_DPC           = Codec::toCSR(0x7b1),
        STF_REG_CSR_DSCRATCH0     = Codec::toCSR(0x7b2),
        STF_REG_CSR_DSCRATCH1     = Codec::toCSR(0x7b3),

        // ???
        STF_REG_CSR_VSSTATUS      = Codec::toCSR(0x200),
        STF_REG_CSR_VSIE          = Codec::toCSR(0x204),
        STF_REG_CSR_VSTVEC        = Codec::toCSR(0x205),
        STF_REG_CSR_VSSCRATCH     = Codec::toCSR(0x240),
        STF_REG_CSR_VSEPC         = Codec::toCSR(0x241),
        STF_REG_CSR_VSCAUSE       = Codec::toCSR(0x242),
        STF_REG_CSR_VSTVAL        = Codec::toCSR(0x243),
        STF_REG_CSR_VSIP          = Codec::toCSR(0x244),
        STF_REG_CSR_VSATP         = Codec::toCSR(0x280),

        // Machine
        STF_REG_CSR_MSTATUS       = Codec::toCSR(0x300),
        STF_REG_CSR_MISA          = Codec::toCSR(0x301),
        STF_REG_CSR_MEDELEG       = Codec::toCSR(0x302),
        STF_REG_CSR_MIDELEG       = Codec::toCSR(0x303),
        STF_REG_CSR_MIE           = Codec::toCSR(0x304),
        STF_REG_CSR_MTVEC         = Codec::toCSR(0x305),
        STF_REG_CSR_MCOUNTEREN    = Codec::toCSR(0x306),
        STF_REG_CSR_MTVT          = Codec::toCSR(0x307),
        STF_REG_CSR_MENVCFG       = Codec::toCSR(0x30a),
        STF_REG_CSR_MSTATUSH      = Codec::toCSR(0x310),
        STF_REG_CSR_MENVCFGH      = Codec::toCSR(0x31a),
        STF_REG_CSR_MCOUNTINHIBIT = Codec::toCSR(0x320),
        STF_REG_CSR_MHPMEVENT3    = Codec::toCSR(0x323),
        STF_REG_CSR_MHPMEVENT4    = Codec::toCSR(0x324),
        STF_REG_CSR_MHPMEVENT5    = Codec::toCSR(0x325),
        STF_REG_CSR_MHPMEVENT6    = Codec::toCSR(0x326),
        STF_REG_CSR_MHPMEVENT7    = Codec::toCSR(0x327),
        STF_REG_CSR_MHPMEVENT8    = Codec::toCSR(0x328),
        STF_REG_CSR_MHPMEVENT9    = Codec::toCSR(0x329),
        STF_REG_CSR_MHPMEVENT10   = Codec::toCSR(0x32a),
        STF_REG_CSR_MHPMEVENT11   = Codec::toCSR(0x32b),
        STF_REG_CSR_MHPMEVENT12   = Codec::toCSR(0x32c),
        STF_REG_CSR_MHPMEVENT13   = Codec::toCSR(0x32d),
        STF_REG_CSR_MHPMEVENT14   = Codec::toCSR(0x32e),
        STF_REG_CSR_MHPMEVENT15   = Codec::toCSR(0x32f),
        STF_REG_CSR_MHPMEVENT16   = Codec::toCSR(0x330),
        STF_REG_CSR_MHPMEVENT17   = Codec::toCSR(0x331),
        STF_REG_CSR_MHPMEVENT18   = Codec::toCSR(0x332),
        STF_REG_CSR_MHPMEVENT19   = Codec::toCSR(0x333),
        STF_REG_CSR_MHPMEVENT20   = Codec::toCSR(0x334),
        STF_REG_CSR_MHPMEVENT21   = Codec::toCSR(0x335),
        STF_REG_CSR_MHPMEVENT22   = Codec::toCSR(0x336),
        STF_REG_CSR_MHPMEVENT23   = Codec::toCSR(0x337),
        STF_REG_CSR_MHPMEVENT24   = Codec::toCSR(0x338),
        STF_REG_CSR_MHPMEVENT25   = Codec::toCSR(0x339),
        STF_REG_CSR_MHPMEVENT26   = Codec::toCSR(0x33a),
        STF_REG_CSR_MHPMEVENT27   = Codec::toCSR(0x33b),
        STF_REG_CSR_MHPMEVENT28   = Codec::toCSR(0x33c),
        STF_REG_CSR_MHPMEVENT29   = Codec::toCSR(0x33d),
        STF_REG_CSR_MHPMEVENT30   = Codec::toCSR(0x33e),
        STF_REG_CSR_MHPMEVENT31   = Codec::toCSR(0x33f),
        STF_REG_CSR_MSCRATCH      = Codec::toCSR(0x340),
        STF_REG_CSR_MEPC          = Codec::toCSR(0x341),
        STF_REG_CSR_MCAUSE        = Codec::toCSR(0x342),
        STF_REG_CSR_MTVAL         = Codec::toCSR(0x343),
        STF_REG_CSR_MIP           = Codec::toCSR(0x344),
        STF_REG_CSR_MNXTI         = Codec::toCSR(0x345),
        STF_REG_CSR_MINTSTATUS    = Codec::toCSR(0x346),
        STF_REG_CSR_MSCRATCHCSW   = Codec::toCSR(0x348),
        STF_REG_CSR_MSCRATCHCSWL  = Codec::toCSR(0x349),
        STF_REG_CSR_MTINST        = Codec::toCSR(0x34a),
        STF_REG_CSR_MTVAL2        = Codec::toCSR(0x34b),

        // Machine Memory Protection
        STF_REG_CSR_PMPCFG0       = Codec::toCSR(0x3a0),
        STF_REG_CSR_PMPCFG1       = Codec::toCSR(0x3a1),
        STF_REG_CSR_PMPCFG2       = Codec::toCSR(0x3a2),
        STF_REG_CSR_PMPCFG3       = Codec::toCSR(0x3a3),
        STF_REG_CSR_PMPCFG4       = Codec::toCSR(0x3a4),
        STF_REG_CSR_PMPCFG5       = Codec::toCSR(0x3a5),
        STF_REG_CSR_PMPCFG6       = Codec::toCSR(0x3a6),
        STF_REG_CSR_PMPCFG7       = Codec::toCSR(0x3a7),
        STF_REG_CSR_PMPCFG8       = Codec::toCSR(0x3a8),
        STF_REG_CSR_PMPCFG9       = Codec::toCSR(0x3a9),
        STF_REG_CSR_PMPCFG10      = Codec::toCSR(0x3aa),
        STF_REG_CSR_PMPCFG11      = Codec::toCSR(0x3ab),
        STF_REG_CSR_PMPCFG12      = Codec::toCSR(0x3ac),
        STF_REG_CSR_PMPCFG13      = Codec::toCSR(0x3ad),
        STF_REG_CSR_PMPCFG14      = Codec::toCSR(0x3ae),
        STF_REG_CSR_PMPCFG15      = Codec::toCSR(0x3af),
        STF_REG_CSR_PMPADDR0      = Codec::toCSR(0x3b0),
        STF_REG_CSR_PMPADDR1      = Codec::toCSR(0x3b1),
        STF_REG_CSR_PMPADDR2      = Codec::toCSR(0x3b2),
        STF_REG_CSR_PMPADDR3      = Codec::toCSR(0x3b3),
        STF_REG_CSR_PMPADDR4      = Codec::toCSR(0x3b4),
        STF_REG_CSR_PMPADDR5      = Codec::toCSR(0x3b5),
        STF_REG_CSR_PMPADDR6      = Codec::toCSR(0x3b6),
        STF_REG_CSR_PMPADDR7      = Codec::toCSR(0x3b7),
        STF_REG_CSR_PMPADDR8      = Codec::toCSR(0x3b8),
        STF_REG_CSR_PMPADDR9      = Codec::toCSR(0x3b9),
        STF_REG_CSR_PMPADDR10     = Codec::toCSR(0x3ba),
        STF_REG_CSR_PMPADDR11     = Codec::toCSR(0x3bb),
        STF_REG_CSR_PMPADDR12     = Codec::toCSR(0x3bc),
        STF_REG_CSR_PMPADDR13     = Codec::toCSR(0x3bd),
        STF_REG_CSR_PMPADDR14     = Codec::toCSR(0x3be),
        STF_REG_CSR_PMPADDR15     = Codec::toCSR(0x3bf),
        STF_REG_CSR_PMPADDR16     = Codec::toCSR(0x3c0),
        STF_REG_CSR_PMPADDR17     = Codec::toCSR(0x3c1),
        STF_REG_CSR_PMPADDR18     = Codec::toCSR(0x3c2),
        STF_REG_CSR_PMPADDR19     = Codec::toCSR(0x3c3),
        STF_REG_CSR_PMPADDR20     = Codec::toCSR(0x3c4),
        STF_REG_CSR_PMPADDR21     = Codec::toCSR(0x3c5),
        STF_REG_CSR_PMPADDR22     = Codec::toCSR(0x3c6),
        STF_REG_CSR_PMPADDR23     = Codec::toCSR(0x3c7),
        STF_REG_CSR_PMPADDR24     = Codec::toCSR(0x3c8),
        STF_REG_CSR_PMPADDR25     = Codec::toCSR(0x3c9),
        STF_REG_CSR_PMPADDR26     = Codec::toCSR(0x3ca),
        STF_REG_CSR_PMPADDR27     = Codec::toCSR(0x3cb),
        STF_REG_CSR_PMPADDR28     = Codec::toCSR(0x3cc),
        STF_REG_CSR_PMPADDR29     = Codec::toCSR(0x3cd),
        STF_REG_CSR_PMPADDR30     = Codec::toCSR(0x3ce),
        STF_REG_CSR_PMPADDR31     = Codec::toCSR(0x3cf),
        STF_REG_CSR_PMPADDR32     = Codec::toCSR(0x3d0),
        STF_REG_CSR_PMPADDR33     = Codec::toCSR(0x3d1),
        STF_REG_CSR_PMPADDR34     = Codec::toCSR(0x3d2),
        STF_REG_CSR_PMPADDR35     = Codec::toCSR(0x3d3),
        STF_REG_CSR_PMPADDR36     = Codec::toCSR(0x3d4),
        STF_REG_CSR_PMPADDR37     = Codec::toCSR(0x3d5),
        STF_REG_CSR_PMPADDR38     = Codec::toCSR(0x3d6),
        STF_REG_CSR_PMPADDR39     = Codec::toCSR(0x3d7),
        STF_REG_CSR_PMPADDR40     = Codec::toCSR(0x3d8),
        STF_REG_CSR_PMPADDR41     = Codec::toCSR(0x3d9),
        STF_REG_CSR_PMPADDR42     = Codec::toCSR(0x3da),
        STF_REG_CSR_PMPADDR43     = Codec::toCSR(0x3db),
        STF_REG_CSR_PMPADDR44     = Codec::toCSR(0x3dc),
        STF_REG_CSR_PMPADDR45     = Codec::toCSR(0x3dd),
        STF_REG_CSR_PMPADDR46     = Codec::toCSR(0x3de),
        STF_REG_CSR_PMPADDR47     = Codec::toCSR(0x3df),
        STF_REG_CSR_PMPADDR48     = Codec::toCSR(0x3e0),
        STF_REG_CSR_PMPADDR49     = Codec::toCSR(0x3e1),
        STF_REG_CSR_PMPADDR50     = Codec::toCSR(0x3e2),
        STF_REG_CSR_PMPADDR51     = Codec::toCSR(0x3e3),
        STF_REG_CSR_PMPADDR52     = Codec::toCSR(0x3e4),
        STF_REG_CSR_PMPADDR53     = Codec::toCSR(0x3e5),
        STF_REG_CSR_PMPADDR54     = Codec::toCSR(0x3e6),
        STF_REG_CSR_PMPADDR55     = Codec::toCSR(0x3e7),
        STF_REG_CSR_PMPADDR56     = Codec::toCSR(0x3e8),
        STF_REG_CSR_PMPADDR57     = Codec::toCSR(0x3e9),
        STF_REG_CSR_PMPADDR58     = Codec::toCSR(0x3ea),
        STF_REG_CSR_PMPADDR59     = Codec::toCSR(0x3eb),
        STF_REG_CSR_PMPADDR60     = Codec::toCSR(0x3ec),
        STF_REG_CSR_PMPADDR61     = Codec::toCSR(0x3ed),
        STF_REG_CSR_PMPADDR62     = Codec::toCSR(0x3ee),
        STF_REG_CSR_PMPADDR63     = Codec::toCSR(0x3ef),

        // Hypervisor
        STF_REG_CSR_HSTATUS       = Codec::toCSR(0x600),
        STF_REG_CSR_HEDELEG       = Codec::toCSR(0x602),
        STF_REG_CSR_HIDELEG       = Codec::toCSR(0x603),
        STF_REG_CSR_HIE           = Codec::toCSR(0x604),
        STF_REG_CSR_HTIMEDELTA    = Codec::toCSR(0x605),
        STF_REG_CSR_HCOUNTEREN    = Codec::toCSR(0x606),
        STF_REG_CSR_HGEIE         = Codec::toCSR(0x607),
        STF_REG_CSR_HENVCFG       = Codec::toCSR(0x60a),
        STF_REG_CSR_HTVAL         = Codec::toCSR(0x643),
        STF_REG_CSR_HIP           = Codec::toCSR(0x644),
        STF_REG_CSR_HVIP          = Codec::toCSR(0x645),
        STF_REG_CSR_HTINST        = Codec::toCSR(0x64a),
        STF_REG_CSR_HGATP         = Codec::toCSR(0x680),
        STF_REG_CSR_HCONTEXT      = Codec::toCSR(0x6a8),
        STF_REG_CSR_HGEIP         = Codec::toCSR(0xe12),

        // Machine Security Config Registers
        STF_REG_CSR_MSECCFG       = Codec::toCSR(0x747),
        STF_REG_CSR_MSECCFGH      = Codec::toCSR(0x757),

        // Basic Machine Counters
        STF_REG_CSR_MCYCLE        = Codec::toCSR(0xb00),
        STF_REG_CSR_MINSTRET      = Codec::toCSR(0xb02),
        STF_REG_CSR_MHPMCOUNTER3  = Codec::toCSR(0xb03),
        STF_REG_CSR_MHPMCOUNTER4  = Codec::toCSR(0xb04),
        STF_REG_CSR_MHPMCOUNTER5  = Codec::toCSR(0xb05),
        STF_REG_CSR_MHPMCOUNTER6  = Codec::toCSR(0xb06),
        STF_REG_CSR_MHPMCOUNTER7  = Codec::toCSR(0xb07),
        STF_REG_CSR_MHPMCOUNTER8  = Codec::toCSR(0xb08),
        STF_REG_CSR_MHPMCOUNTER9  = Codec::toCSR(0xb09),
        STF_REG_CSR_MHPMCOUNTER10 = Codec::toCSR(0xb0a),
        STF_REG_CSR_MHPMCOUNTER11 = Codec::toCSR(0xb0b),
        STF_REG_CSR_MHPMCOUNTER12 = Codec::toCSR(0xb0c),
        STF_REG_CSR_MHPMCOUNTER13 = Codec::toCSR(0xb0d),
        STF_REG_CSR_MHPMCOUNTER14 = Codec::toCSR(0xb0e),
        STF_REG_CSR_MHPMCOUNTER15 = Codec::toCSR(0xb0f),
        STF_REG_CSR_MHPMCOUNTER16 = Codec::toCSR(0xb10),
        STF_REG_CSR_MHPMCOUNTER17 = Codec::toCSR(0xb11),
        STF_REG_CSR_MHPMCOUNTER18 = Codec::toCSR(0xb12),
        STF_REG_CSR_MHPMCOUNTER19 = Codec::toCSR(0xb13),
        STF_REG_CSR_MHPMCOUNTER20 = Codec::toCSR(0xb14),
        STF_REG_CSR_MHPMCOUNTER21 = Codec::toCSR(0xb15),
        STF_REG_CSR_MHPMCOUNTER22 = Codec::toCSR(0xb16),
        STF_REG_CSR_MHPMCOUNTER23 = Codec::toCSR(0xb17),
        STF_REG_CSR_MHPMCOUNTER24 = Codec::toCSR(0xb18),
        STF_REG_CSR_MHPMCOUNTER25 = Codec::toCSR(0xb19),
        STF_REG_CSR_MHPMCOUNTER26 = Codec::toCSR(0xb1a),
        STF_REG_CSR_MHPMCOUNTER27 = Codec::toCSR(0xb1b),
        STF_REG_CSR_MHPMCOUNTER28 = Codec::toCSR(0xb1c),
        STF_REG_CSR_MHPMCOUNTER29 = Codec::toCSR(0xb1d),
        STF_REG_CSR_MHPMCOUNTER30 = Codec::toCSR(0xb1e),
        STF_REG_CSR_MHPMCOUNTER31 = Codec::toCSR(0xb1f),

        // Basic Machine Performance Monitoring Counters
        STF_REG_CSR_MCYCLEH       = Codec::toCSR(0xb80),
        STF_REG_CSR_MINSTRETH     = Codec::toCSR(0xb82),

        // Basic User Counters
        STF_REG_CSR_CYCLE         = Codec::toCSR(0xc00),
        STF_REG_CSR_TIME          = Codec::toCSR(0xc01),
        STF_REG_CSR_INSTRET       = Codec::toCSR(0xc02),
        STF_REG_CSR_HPMCOUNTER3   = Codec::toCSR(0xc03),
        STF_REG_CSR_HPMCOUNTER4   = Codec::toCSR(0xc04),
        STF_REG_CSR_HPMCOUNTER5   = Codec::toCSR(0xc05),
        STF_REG_CSR_HPMCOUNTER6   = Codec::toCSR(0xc06),
        STF_REG_CSR_HPMCOUNTER7   = Codec::toCSR(0xc07),
        STF_REG_CSR_HPMCOUNTER8   = Codec::toCSR(0xc08),
        STF_REG_CSR_HPMCOUNTER9   = Codec::toCSR(0xc09),
        STF_REG_CSR_HPMCOUNTER10  = Codec::toCSR(0xc0a),
        STF_REG_CSR_HPMCOUNTER11  = Codec::toCSR(0xc0b),
        STF_REG_CSR_HPMCOUNTER12  = Codec::toCSR(0xc0c),
        STF_REG_CSR_HPMCOUNTER13  = Codec::toCSR(0xc0d),
        STF_REG_CSR_HPMCOUNTER14  = Codec::toCSR(0xc0e),
        STF_REG_CSR_HPMCOUNTER15  = Codec::toCSR(0xc0f),
        STF_REG_CSR_HPMCOUNTER16  = Codec::toCSR(0xc10),
        STF_REG_CSR_HPMCOUNTER17  = Codec::toCSR(0xc11),
        STF_REG_CSR_HPMCOUNTER18  = Codec::toCSR(0xc12),
        STF_REG_CSR_HPMCOUNTER19  = Codec::toCSR(0xc13),
        STF_REG_CSR_HPMCOUNTER20  = Codec::toCSR(0xc14),
        STF_REG_CSR_HPMCOUNTER21  = Codec::toCSR(0xc15),
        STF_REG_CSR_HPMCOUNTER22  = Codec::toCSR(0xc16),
        STF_REG_CSR_HPMCOUNTER23  = Codec::toCSR(0xc17),
        STF_REG_CSR_HPMCOUNTER24  = Codec::toCSR(0xc18),
        STF_REG_CSR_HPMCOUNTER25  = Codec::toCSR(0xc19),
        STF_REG_CSR_HPMCOUNTER26  = Codec::toCSR(0xc1a),
        STF_REG_CSR_HPMCOUNTER27  = Codec::toCSR(0xc1b),
        STF_REG_CSR_HPMCOUNTER28  = Codec::toCSR(0xc1c),
        STF_REG_CSR_HPMCOUNTER29  = Codec::toCSR(0xc1d),
        STF_REG_CSR_HPMCOUNTER30  = Codec::toCSR(0xc1e),
        STF_REG_CSR_HPMCOUNTER31  = Codec::toCSR(0xc1f),

        // Vector
        STF_REG_CSR_VL            = Codec::toCSR(0xc20),
        STF_REG_CSR_VTYPE         = Codec::toCSR(0xc21),
        STF_REG_CSR_VLENB         = Codec::toCSR(0xc22),

        // Basic User Performance Monitoring Counters
        STF_REG_CSR_CYCLEH        = Codec::toCSR(0xc80),
        STF_REG_CSR_TIMEH         = Codec::toCSR(0xc81),
        STF_REG_CSR_INSTRETH      = Codec::toCSR(0xc82),

        // Machine Information Registers
        STF_REG_CSR_MVENDORID     = Codec::toCSR(0xf11),
        STF_REG_CSR_MARCHID       = Codec::toCSR(0xf12),
        STF_REG_CSR_MIMPID        = Codec::toCSR(0xf13),
        STF_REG_CSR_MHARTID       = Codec::toCSR(0xf14),
        STF_REG_CSR_MCONFIGPTR    = Codec::toCSR(0xf15),

        STF_REG_INVALID           = std::numeric_limits<STF_REG_int>::max()
    };

    /**
     * Writes an STF_REG to an ostream
     * \param os ostream to use
     * \param reg register to format
     */
    std::ostream& operator<<(std::ostream& os, Registers::STF_REG reg);

    /**
     * Writes an STF_REG_TYPE to an ostream
     * \param os ostream to use
     * \param type register type to format
     */
    std::ostream& operator<<(std::ostream& os, Registers::STF_REG_TYPE type);

    /**
     * Writes an STF_REG_OPERAND_TYPE to an ostream
     * \param os ostream to use
     * \param type operand type to format
     */
    std::ostream& operator<<(std::ostream& os, Registers::STF_REG_OPERAND_TYPE type);

    template<size_t num_bits>
    static constexpr uint64_t calcRegMask() {
        return byte_utils::bitMask<uint64_t, num_bits>();
    }

} // end namespace stf

//end of __STF_REGISTER_DEF_H_
#endif
