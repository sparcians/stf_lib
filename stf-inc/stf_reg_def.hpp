
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
            static constexpr size_t TYPE_SHIFT_AMT_ = sizeof(STF_REG_packed_int) * 8;
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
                static constexpr size_t FFLAGS_WIDTH = 5; /**< Width of FFLAGS register */
                static constexpr size_t FRM_WIDTH = 3; /**< Width of FRM register */
                static constexpr size_t FRM_SHIFT = FFLAGS_WIDTH; /**< FRM shift amount */

                static constexpr size_t UIE_WIDTH = 1; /**< Width of UIE register */
                static constexpr size_t SIE_WIDTH = 1; /**< Width of SIE register */
                static constexpr size_t SIE_SHIFT = UIE_WIDTH; /**< SIE shift amount */

                static constexpr size_t MCYCLEH_WIDTH = 32; /**< Width of MCYCLEH register */
                static constexpr size_t MCYCLEH_SHIFT = 32; /**< MCYCLEH shift amount */

                static constexpr size_t MINSTRETH_WIDTH = 32; /**< Width of MINSTRETH register */
                static constexpr size_t MINSTRETH_SHIFT = 32; /**< MINSTRETH shift amount */

                static constexpr size_t CYCLEH_WIDTH = 32; /**< Width of CYCLEH register */
                static constexpr size_t CYCLEH_SHIFT = 32; /**< CYCLEH shift amount */

                static constexpr size_t TIMEH_WIDTH = 32; /**< Width of TIMEH register */
                static constexpr size_t TIMEH_SHIFT = 32; /**< TIMEH shift amount */

                static constexpr size_t INSTRETH_WIDTH = 32; /**< Width of INSTRETH register */
                static constexpr size_t INSTRETH_SHIFT = 32; /**< INSTRETH shift amount */
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
             */
            static STF_REG_packed_int getArchRegIndex(STF_REG regno);

            /**
             * Checks whether a register is a CSR
             */
            static inline constexpr bool isCSR(const STF_REG regno) {
                return Registers::Codec::getRegType(regno) == Registers::STF_REG_TYPE::CSR;
            }

            /**
             * Checks whether a register is an FPR
             */
            static inline constexpr bool isFPR(const STF_REG regno) {
                return Registers::Codec::getRegType(regno) == Registers::STF_REG_TYPE::FLOATING_POINT;
            }

            /**
             * Checks whether a register is a GPR
             */
            static inline constexpr bool isGPR(const STF_REG regno) {
                return Registers::Codec::getRegType(regno) == Registers::STF_REG_TYPE::INTEGER;
            }

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
                     * Packs an STF_REG into an STF_REG_packed_int
                     * \param reg STF_REG to pack
                     */
                    static inline STF_REG_packed_int packRegNum(const STF_REG reg) {
                        return enums::to_int(reg) & REG_MASK_;
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
             * Converts a CSR into its corresponding string representation
             */
            static void formatCSR_(std::ostream& os, Registers::STF_REG regno);

    };

    enum class Registers::STF_REG : Registers::STF_REG_int {
        // 32 int registers
        STF_REG_X0               = Codec::combineRegType(0x0000, STF_REG_TYPE::INTEGER),
        STF_REG_X1               = Codec::combineRegType(0x0001, STF_REG_TYPE::INTEGER),
        STF_REG_X2               = Codec::combineRegType(0x0002, STF_REG_TYPE::INTEGER),
        STF_REG_X3               = Codec::combineRegType(0x0003, STF_REG_TYPE::INTEGER),
        STF_REG_X4               = Codec::combineRegType(0x0004, STF_REG_TYPE::INTEGER),
        STF_REG_X5               = Codec::combineRegType(0x0005, STF_REG_TYPE::INTEGER),
        STF_REG_X6               = Codec::combineRegType(0x0006, STF_REG_TYPE::INTEGER),
        STF_REG_X7               = Codec::combineRegType(0x0007, STF_REG_TYPE::INTEGER),
        STF_REG_X8               = Codec::combineRegType(0x0008, STF_REG_TYPE::INTEGER),
        STF_REG_X9               = Codec::combineRegType(0x0009, STF_REG_TYPE::INTEGER),
        STF_REG_X10              = Codec::combineRegType(0x000A, STF_REG_TYPE::INTEGER),
        STF_REG_X11              = Codec::combineRegType(0x000B, STF_REG_TYPE::INTEGER),
        STF_REG_X12              = Codec::combineRegType(0x000C, STF_REG_TYPE::INTEGER),
        STF_REG_X13              = Codec::combineRegType(0x000D, STF_REG_TYPE::INTEGER),
        STF_REG_X14              = Codec::combineRegType(0x000E, STF_REG_TYPE::INTEGER),
        STF_REG_X15              = Codec::combineRegType(0x000F, STF_REG_TYPE::INTEGER),
        STF_REG_X16              = Codec::combineRegType(0x0010, STF_REG_TYPE::INTEGER),
        STF_REG_X17              = Codec::combineRegType(0x0011, STF_REG_TYPE::INTEGER),
        STF_REG_X18              = Codec::combineRegType(0x0012, STF_REG_TYPE::INTEGER),
        STF_REG_X19              = Codec::combineRegType(0x0013, STF_REG_TYPE::INTEGER),
        STF_REG_X20              = Codec::combineRegType(0x0014, STF_REG_TYPE::INTEGER),
        STF_REG_X21              = Codec::combineRegType(0x0015, STF_REG_TYPE::INTEGER),
        STF_REG_X22              = Codec::combineRegType(0x0016, STF_REG_TYPE::INTEGER),
        STF_REG_X23              = Codec::combineRegType(0x0017, STF_REG_TYPE::INTEGER),
        STF_REG_X24              = Codec::combineRegType(0x0018, STF_REG_TYPE::INTEGER),
        STF_REG_X25              = Codec::combineRegType(0x0019, STF_REG_TYPE::INTEGER),
        STF_REG_X26              = Codec::combineRegType(0x001A, STF_REG_TYPE::INTEGER),
        STF_REG_X27              = Codec::combineRegType(0x001B, STF_REG_TYPE::INTEGER),
        STF_REG_X28              = Codec::combineRegType(0x001C, STF_REG_TYPE::INTEGER),
        STF_REG_X29              = Codec::combineRegType(0x001D, STF_REG_TYPE::INTEGER),
        STF_REG_X30              = Codec::combineRegType(0x001E, STF_REG_TYPE::INTEGER),
        STF_REG_X31              = Codec::combineRegType(0x001F, STF_REG_TYPE::INTEGER),

        // 32 floating point  registers
        STF_REG_F0               = Codec::combineRegType(0x0000, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F1               = Codec::combineRegType(0x0001, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F2               = Codec::combineRegType(0x0002, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F3               = Codec::combineRegType(0x0003, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F4               = Codec::combineRegType(0x0004, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F5               = Codec::combineRegType(0x0005, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F6               = Codec::combineRegType(0x0006, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F7               = Codec::combineRegType(0x0007, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F8               = Codec::combineRegType(0x0008, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F9               = Codec::combineRegType(0x0009, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F10              = Codec::combineRegType(0x000A, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F11              = Codec::combineRegType(0x000B, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F12              = Codec::combineRegType(0x000C, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F13              = Codec::combineRegType(0x000D, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F14              = Codec::combineRegType(0x000E, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F15              = Codec::combineRegType(0x000F, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F16              = Codec::combineRegType(0x0010, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F17              = Codec::combineRegType(0x0011, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F18              = Codec::combineRegType(0x0012, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F19              = Codec::combineRegType(0x0013, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F20              = Codec::combineRegType(0x0014, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F21              = Codec::combineRegType(0x0015, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F22              = Codec::combineRegType(0x0016, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F23              = Codec::combineRegType(0x0017, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F24              = Codec::combineRegType(0x0018, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F25              = Codec::combineRegType(0x0019, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F26              = Codec::combineRegType(0x001A, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F27              = Codec::combineRegType(0x001B, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F28              = Codec::combineRegType(0x001C, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F29              = Codec::combineRegType(0x001D, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F30              = Codec::combineRegType(0x001E, STF_REG_TYPE::FLOATING_POINT),
        STF_REG_F31              = Codec::combineRegType(0x001F, STF_REG_TYPE::FLOATING_POINT),

        // Control and status registers
        // User
        STF_REG_CSR_USTATUS      = Codec::combineRegType(0x000, STF_REG_TYPE::CSR),
        STF_REG_CSR_FFLAGS       = Codec::combineRegType(0x001, STF_REG_TYPE::CSR),
        STF_REG_CSR_FRM          = Codec::combineRegType(0x002, STF_REG_TYPE::CSR),
        STF_REG_CSR_FCSR         = Codec::combineRegType(0x003, STF_REG_TYPE::CSR),
        STF_REG_CSR_UIE          = Codec::combineRegType(0x004, STF_REG_TYPE::CSR),
        STF_REG_CSR_UTVEC        = Codec::combineRegType(0x005, STF_REG_TYPE::CSR),
        STF_REG_CSR_UTVT         = Codec::combineRegType(0x007, STF_REG_TYPE::CSR),
        STF_REG_CSR_VSTART       = Codec::combineRegType(0x008, STF_REG_TYPE::CSR),
        STF_REG_CSR_VXSAT        = Codec::combineRegType(0x009, STF_REG_TYPE::CSR),
        STF_REG_CSR_VXRM         = Codec::combineRegType(0x00a, STF_REG_TYPE::CSR),
        STF_REG_CSR_USCRATCH     = Codec::combineRegType(0x040, STF_REG_TYPE::CSR),
        STF_REG_CSR_UEPC         = Codec::combineRegType(0x041, STF_REG_TYPE::CSR),
        STF_REG_CSR_UCAUSE       = Codec::combineRegType(0x042, STF_REG_TYPE::CSR),
        STF_REG_CSR_UTVAL        = Codec::combineRegType(0x043, STF_REG_TYPE::CSR),
        STF_REG_CSR_UIP          = Codec::combineRegType(0x044, STF_REG_TYPE::CSR),
        STF_REG_CSR_UNXTI        = Codec::combineRegType(0x045, STF_REG_TYPE::CSR),
        STF_REG_CSR_UINTSTATUS   = Codec::combineRegType(0x046, STF_REG_TYPE::CSR),
        STF_REG_CSR_USCRATCHCSW  = Codec::combineRegType(0x048, STF_REG_TYPE::CSR),
        STF_REG_CSR_USCRATCHCSWL = Codec::combineRegType(0x049, STF_REG_TYPE::CSR),

        // Supervisor
        STF_REG_CSR_SSTATUS      = Codec::combineRegType(0x100, STF_REG_TYPE::CSR),
        STF_REG_CSR_SIE          = Codec::combineRegType(0x104, STF_REG_TYPE::CSR),
        STF_REG_CSR_STVEC        = Codec::combineRegType(0x105, STF_REG_TYPE::CSR),
        STF_REG_CSR_SCOUNTEREN   = Codec::combineRegType(0x106, STF_REG_TYPE::CSR),
        STF_REG_CSR_STVT         = Codec::combineRegType(0x107, STF_REG_TYPE::CSR),
        STF_REG_CSR_SSCRATCH     = Codec::combineRegType(0x140, STF_REG_TYPE::CSR),
        STF_REG_CSR_SEPC         = Codec::combineRegType(0x141, STF_REG_TYPE::CSR),
        STF_REG_CSR_SCAUSE       = Codec::combineRegType(0x142, STF_REG_TYPE::CSR),
        STF_REG_CSR_STVAL        = Codec::combineRegType(0x143, STF_REG_TYPE::CSR),
        STF_REG_CSR_SIP          = Codec::combineRegType(0x144, STF_REG_TYPE::CSR),
        STF_REG_CSR_SNXTI        = Codec::combineRegType(0x145, STF_REG_TYPE::CSR),
        STF_REG_CSR_SINTSTATUS   = Codec::combineRegType(0x146, STF_REG_TYPE::CSR),
        STF_REG_CSR_SSCRATCHCSW  = Codec::combineRegType(0x148, STF_REG_TYPE::CSR),
        STF_REG_CSR_SSCRATCHCSWL = Codec::combineRegType(0x149, STF_REG_TYPE::CSR),
        STF_REG_CSR_SATP         = Codec::combineRegType(0x180, STF_REG_TYPE::CSR),

        // ???
        STF_REG_CSR_VSSTATUS     = Codec::combineRegType(0x200, STF_REG_TYPE::CSR),
        STF_REG_CSR_VSIE         = Codec::combineRegType(0x204, STF_REG_TYPE::CSR),
        STF_REG_CSR_VSTVEC       = Codec::combineRegType(0x205, STF_REG_TYPE::CSR),
        STF_REG_CSR_VSSCRATCH    = Codec::combineRegType(0x240, STF_REG_TYPE::CSR),
        STF_REG_CSR_VSEPC        = Codec::combineRegType(0x241, STF_REG_TYPE::CSR),
        STF_REG_CSR_VSCAUSE      = Codec::combineRegType(0x242, STF_REG_TYPE::CSR),
        STF_REG_CSR_VSTVAL       = Codec::combineRegType(0x243, STF_REG_TYPE::CSR),
        STF_REG_CSR_VSIP         = Codec::combineRegType(0x244, STF_REG_TYPE::CSR),
        STF_REG_CSR_VSATP        = Codec::combineRegType(0x280, STF_REG_TYPE::CSR),

        // Machine
        STF_REG_CSR_MSTATUS      = Codec::combineRegType(0x300, STF_REG_TYPE::CSR),
        STF_REG_CSR_MISA         = Codec::combineRegType(0x301, STF_REG_TYPE::CSR),
        STF_REG_CSR_MEDELEG      = Codec::combineRegType(0x302, STF_REG_TYPE::CSR),
        STF_REG_CSR_MIDELEG      = Codec::combineRegType(0x303, STF_REG_TYPE::CSR),
        STF_REG_CSR_MIE          = Codec::combineRegType(0x304, STF_REG_TYPE::CSR),
        STF_REG_CSR_MTVEC        = Codec::combineRegType(0x305, STF_REG_TYPE::CSR),
        STF_REG_CSR_MCOUNTEREN   = Codec::combineRegType(0x306, STF_REG_TYPE::CSR),
        STF_REG_CSR_MTVT         = Codec::combineRegType(0x307, STF_REG_TYPE::CSR),
        STF_REG_CSR_MSCRATCH     = Codec::combineRegType(0x340, STF_REG_TYPE::CSR),
        STF_REG_CSR_MEPC         = Codec::combineRegType(0x341, STF_REG_TYPE::CSR),
        STF_REG_CSR_MCAUSE       = Codec::combineRegType(0x342, STF_REG_TYPE::CSR),
        STF_REG_CSR_MTVAL        = Codec::combineRegType(0x343, STF_REG_TYPE::CSR),
        STF_REG_CSR_MIP          = Codec::combineRegType(0x344, STF_REG_TYPE::CSR),
        STF_REG_CSR_MNXTI        = Codec::combineRegType(0x345, STF_REG_TYPE::CSR),
        STF_REG_CSR_MINTSTATUS   = Codec::combineRegType(0x346, STF_REG_TYPE::CSR),
        STF_REG_CSR_MSCRATCHCSW  = Codec::combineRegType(0x348, STF_REG_TYPE::CSR),
        STF_REG_CSR_MSCRATCHCSWL = Codec::combineRegType(0x349, STF_REG_TYPE::CSR),

        // Machine Memory Protection
        STF_REG_CSR_PMPCFG0      = Codec::combineRegType(0x3a0, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPCFG1      = Codec::combineRegType(0x3a1, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPCFG2      = Codec::combineRegType(0x3a2, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPCFG3      = Codec::combineRegType(0x3a3, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR0     = Codec::combineRegType(0x3b0, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR1     = Codec::combineRegType(0x3b1, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR2     = Codec::combineRegType(0x3b2, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR3     = Codec::combineRegType(0x3b3, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR4     = Codec::combineRegType(0x3b4, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR5     = Codec::combineRegType(0x3b5, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR6     = Codec::combineRegType(0x3b6, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR7     = Codec::combineRegType(0x3b7, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR8     = Codec::combineRegType(0x3b8, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR9     = Codec::combineRegType(0x3b9, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR10    = Codec::combineRegType(0x3ba, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR11    = Codec::combineRegType(0x3bb, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR12    = Codec::combineRegType(0x3bc, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR13    = Codec::combineRegType(0x3bd, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR14    = Codec::combineRegType(0x3be, STF_REG_TYPE::CSR),
        STF_REG_CSR_PMPADDR15    = Codec::combineRegType(0x3bf, STF_REG_TYPE::CSR),

        // ???
        STF_REG_CSR_HSTATUS      = Codec::combineRegType(0x600, STF_REG_TYPE::CSR),
        STF_REG_CSR_HEDELEG      = Codec::combineRegType(0x602, STF_REG_TYPE::CSR),
        STF_REG_CSR_HIDELEG      = Codec::combineRegType(0x603, STF_REG_TYPE::CSR),
        STF_REG_CSR_HCOUNTEREN   = Codec::combineRegType(0x606, STF_REG_TYPE::CSR),
        STF_REG_CSR_HGATP        = Codec::combineRegType(0x680, STF_REG_TYPE::CSR),

        // Basic Machine Counters
        STF_REG_CSR_MCYCLE       = Codec::combineRegType(0xb00, STF_REG_TYPE::CSR),
        STF_REG_CSR_MINSTRET     = Codec::combineRegType(0xb02, STF_REG_TYPE::CSR),

        // Basic Machine Performance Monitoring Counters
        STF_REG_CSR_MCYCLEH      = Codec::combineRegType(0xb80, STF_REG_TYPE::CSR),
        STF_REG_CSR_MINSTRETH    = Codec::combineRegType(0xb82, STF_REG_TYPE::CSR),

        // Basic User Counters
        STF_REG_CSR_CYCLE        = Codec::combineRegType(0xc00, STF_REG_TYPE::CSR),
        STF_REG_CSR_TIME         = Codec::combineRegType(0xc01, STF_REG_TYPE::CSR),
        STF_REG_CSR_INSTRET      = Codec::combineRegType(0xc02, STF_REG_TYPE::CSR),

        // Basica User Performance Monitoring Counters
        STF_REG_CSR_CYCLEH       = Codec::combineRegType(0xc80, STF_REG_TYPE::CSR),
        STF_REG_CSR_TIMEH        = Codec::combineRegType(0xc81, STF_REG_TYPE::CSR),
        STF_REG_CSR_INSTRETH     = Codec::combineRegType(0xc82, STF_REG_TYPE::CSR),

        // Machine Information Registers
        STF_REG_CSR_MVENDORID    = Codec::combineRegType(0xf11, STF_REG_TYPE::CSR),
        STF_REG_CSR_MARCHID      = Codec::combineRegType(0xf12, STF_REG_TYPE::CSR),
        STF_REG_CSR_MIMPID       = Codec::combineRegType(0xf13, STF_REG_TYPE::CSR),
        STF_REG_CSR_MHARTID      = Codec::combineRegType(0xf14, STF_REG_TYPE::CSR),
        STF_REG_END_DEF,
        STF_REG_INVALID          = std::numeric_limits<STF_REG_int>::max()
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
