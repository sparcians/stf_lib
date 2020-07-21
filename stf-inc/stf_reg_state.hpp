
// <STF_reg_state> -*- HPP -*-

/**
 * \brief  This file defines the STF common helper functions
 * 1. Parse the register name;
 */

#ifndef __STF_COMMON_HELPER_HPP__
#define __STF_COMMON_HELPER_HPP__

#include <cstdint>
#include <map>
#include <stdexcept>
#include "stf_reg_def.hpp"
#include "stf_record_types.hpp"
#include "util.hpp"

namespace stf {
    class STFWriter;

    /**
     * \class RegMapInfo
     *
     * Struct that holds register mapping information
     */
    class RegMapInfo
    {
        private:
            Registers::STF_REG reg_; /**< register number */
            Registers::STF_REG mapped_reg_; /**< mapped register number */
            uint64_t mask_; /**< mask */
            uint32_t shiftbits_; /**< shift amount */

        public:
            static constexpr uint64_t MASK64 = calcRegMask<64>(); /**< Mask for 64-bit register */
            static constexpr uint64_t MASK32 = calcRegMask<32>(); /**< Mask for 32-bit register */

            /**
             * Constructs a RegMapInfo
             * \param reg register number
             * \param mask mask
             */
            explicit RegMapInfo(Registers::STF_REG reg, uint64_t mask = MASK64) :
                RegMapInfo(reg, reg, mask)
            {
            }

            /**
             * Constructs a RegMapInfo
             * \param reg register number
             * \param mapped_reg mapped register number (used for registers that map onto other registers)
             * \param mask mask
             * \param shift shift amount (amount to shift value by when mapping onto mapped register)
             */
            RegMapInfo(Registers::STF_REG reg,
                       Registers::STF_REG mapped_reg,
                       uint64_t mask = MASK64,
                       uint32_t shift = 0):
                reg_(reg),
                mapped_reg_(mapped_reg),
                mask_(mask),
                shiftbits_(shift)
            {
            }

            /**
             * Gets register number
             */
            Registers::STF_REG getReg() const {
                return reg_;
            }

            /**
             * Gets mapped register number
             */
            Registers::STF_REG getMappedReg() const {
                return mapped_reg_;
            }

            /**
             * Gets mask
             */
            uint64_t getMask() const {
                return mask_;
            }

            /**
             * Gets shift bits
             */
            uint32_t getShiftBits() const {
                return shiftbits_;
            }
    };

    /**
     * \typedef RegStateMap
     * Maps Registers::STF_REG to corresponding InstRegRecord
     */
    using RegStateMap = std::map<Registers::STF_REG, InstRegRecord>;

    /**
     * \typedef RegBankMap
     * Maps Registers::STF_REG to corresponding RegMapInfo
     */
    using RegBankMap = std::map<Registers::STF_REG, RegMapInfo>;

    /**
     * \class STFRegState
     *
     * Class that holds register state information
     */
    class STFRegState {
        private:
            RegStateMap regstate;
            RegBankMap regbank;

            void insertSimpleRegister_(Registers::STF_REG reg, uint64_t mask = RegMapInfo::MASK64);
            void insertMappedRegister_(Registers::STF_REG reg, Registers::STF_REG mapped_reg, uint64_t mask, uint32_t shift = 0);

        public:
            /**
             * \class RegNotFoundException
             *
             * Exception type thrown by getRegValue if a register is not found
             */
            class RegNotFoundException : public std::exception {
            };

            STFRegState() = default;

            /**
             * Constructs and initializes an STFRegState
             * \param isa ISA
             * \param iem Instruction encoding
             */
            STFRegState(ISA isa, INST_IEM iem) {
                initRegBank(isa, iem);
            }

            /**
             * Reinitializes state
             * \param isa ISA
             * \param iem Instruction encoding
             */
            void resetArch(ISA isa, INST_IEM iem) {
                regStateClear();
                initRegBank(isa, iem);
            }

            /**
             * Initializes the register bank
             * \param isa ISA
             * \param iem Instruction encoding
             */
            void initRegBank(ISA isa, INST_IEM iem);

            /**
             * Clears state
             */
            void regStateClear() {
                regstate.clear();
                regbank.clear();
            }

            /**
             * Updates register state
             */
            bool regStateUpdate(const InstRegRecord& rec);

            /**
             * Updates register state
             */
            bool regStateUpdate(Registers::STF_REG regno, uint64_t data);

            /**
             * Gets register value
             */
            void getRegValue(Registers::STF_REG regno, uint64_t &data) const;

            /**
             * Writes register state to STFWriter
             */
            void writeRegState(STFWriter& stf_writer) const;

   };
} //end namespace stf

#endif //#define __STF_COMMON_HELPER_HPP__
