
// <STF_reg_state> -*- HPP -*-

/**
 * \brief  This file defines the STF common helper functions
 * 1. Parse the register name;
 */

#ifndef __STF_REG_STATE_HPP__
#define __STF_REG_STATE_HPP__

#include <cstdint>
#include <stdexcept>

#include "boost_wrappers/flat_map.hpp"

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
    class RegMapInfo {
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
            explicit RegMapInfo(const Registers::STF_REG reg, const uint64_t mask = MASK64) :
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
            RegMapInfo(const Registers::STF_REG reg,
                       const Registers::STF_REG mapped_reg,
                       const uint64_t mask = MASK64,
                       const uint32_t shift = 0):
                reg_(reg),
                mapped_reg_(mapped_reg),
                mask_(mask),
                shiftbits_(shift)
            {
            }

            /**
             * Gets register number
             */
            inline Registers::STF_REG getReg() const {
                return reg_;
            }

            /**
             * Gets mapped register number
             */
            inline Registers::STF_REG getMappedReg() const {
                return mapped_reg_;
            }

            /**
             * Gets mask
             */
            inline uint64_t getMask() const {
                return mask_;
            }

            /**
             * Gets shift bits
             */
            inline uint32_t getShiftBits() const {
                return shiftbits_;
            }

            /**
             * Applies the givel field value with the correct shift and mask bits into the parent register value
             * \param parent_reg_val Current value of the parent register
             * \param field_val Field value to apply
             */
            inline uint64_t apply(const uint64_t parent_reg_val, const uint64_t field_val) const {
                return (parent_reg_val & ~(mask_ << shiftbits_)) | ((field_val & mask_) << shiftbits_);
            }
    };

    /**
     * \typedef RegStateMap
     * Maps Registers::STF_REG to corresponding InstRegRecord
     */
    using RegStateMap = boost::container::flat_map<Registers::STF_REG, STFRecord::Handle<InstRegRecord>>;

    /**
     * \typedef RegBankMap
     * Maps Registers::STF_REG to corresponding RegMapInfo
     */
    using RegBankMap = boost::container::flat_map<Registers::STF_REG, RegMapInfo>;

    /**
     * \class STFRegState
     *
     * Class that holds register state information
     */
    class STFRegState {
        private:
            RegStateMap regstate_;
            RegBankMap regbank_;

            /**
             * Inserts a simple (non-mapped) register
             * \param reg register number
             * \param mask mask to apply to register values
             */
            inline auto insertSimpleRegister_(const Registers::STF_REG reg, const uint64_t mask = RegMapInfo::MASK64) {
                return regbank_.emplace(std::piecewise_construct,
                                        std::forward_as_tuple(reg),
                                        std::forward_as_tuple(reg, mask));
            }

            /**
             * Inserts a mapped (simple register sub-field) register
             * \param reg field register number
             * \param mapped_reg parent register number
             * \param mask mask to apply to field values
             * \param shift shift to apply to field values
             */
            inline auto insertMappedRegister_(const Registers::STF_REG reg,
                                              const Registers::STF_REG mapped_reg,
                                              const uint64_t mask,
                                              const uint32_t shift = 0) {
                stf_assert(!Registers::isVector(reg), "Mapped vector registers are not currently supported");
                return regbank_.emplace(std::piecewise_construct,
                                        std::forward_as_tuple(reg),
                                        std::forward_as_tuple(reg, mapped_reg, mask, shift));
            }

            /**
             * Looks up a register in a map, throwing an exception if it isn't found
             * \param map Map to use
             * \param reg Register number
             */
            template<typename MapType>
            static inline typename MapType::const_iterator find_(const MapType& map, const Registers::STF_REG reg) {
                auto it = map.find(reg);
                if(STF_EXPECT_FALSE(it == map.end())) {
                    throw RegNotFoundException(reg);
                }
                return it;
            }

            /**
             * Looks up a register in a map, throwing an exception if it isn't found
             * \param map Map to use
             * \param reg Register number
             */
            template<typename MapType>
            static inline typename MapType::iterator find_(MapType& map, const Registers::STF_REG reg) {
                auto it = map.find(reg);
                if(STF_EXPECT_FALSE(it == map.end())) {
                    throw RegNotFoundException(reg);
                }
                return it;
            }

        public:
            /**
             * \class RegNotFoundException
             *
             * Exception type thrown by getRegScalarValue/getRegVectorValue if a register is not found
             */
            class RegNotFoundException : public std::exception {
                private:
                    const std::string msg_;

                    /**
                     * Generates the exception message
                     * \param reg Register number
                     */
                    static inline std::string genMsg_(const Registers::STF_REG reg) {
                        std::stringstream ss;
                        ss << "Unknown register specified: " << reg;
                        return ss.str();
                    }

                public:
                    /**
                     * RegNotFoundException constructor
                     * \param reg Register number that was not found
                     */
                    explicit RegNotFoundException(const Registers::STF_REG reg) :
                        msg_(genMsg_(reg))
                    {
                    }

                    /**
                     * Gets the exception message
                     */
                    const char* what() const noexcept final {
                        return msg_.c_str();
                    }
            };

            /**
             * Constructs and initializes an STFRegState
             * \param isa ISA
             * \param iem Instruction encoding
             */
            STFRegState(const ISA isa, const INST_IEM iem) {
                initRegBank(isa, iem);
            }

            /**
             * Copy constructor
             */
            STFRegState(const STFRegState&) = default;

            /**
             * Move constructor
             */
            STFRegState(STFRegState&&) = default;

            /**
             * Move assignment operator
             */
            STFRegState& operator=(STFRegState&& rhs) = default;

            /**
             * Copy assignment operator
             */
            STFRegState& operator=(const STFRegState& rhs) {
                regbank_ = rhs.regbank_;
                regstate_.clear();
                for(const auto& r: rhs.regstate_) {
                    regstate_.emplace(r.first, r.second->copy());
                }
                return *this;
            }

            /**
             * Reinitializes state
             * \param isa ISA
             * \param iem Instruction encoding
             */
            inline void resetArch(const ISA isa, const INST_IEM iem) {
                clear();
                initRegBank(isa, iem);
            }

            /**
             * Initializes the register bank
             * \param isa ISA
             * \param iem Instruction encoding
             */
            void initRegBank(ISA isa, INST_IEM iem);

            /**
             * Clears register value state
             */
            inline void stateClear() {
                regstate_.clear();
            }

            /**
             * Clears EVERYTHING
             */
            inline void clear() {
                stateClear();
                regbank_.clear();
            }

            /**
             * Updates register state
             * \param rec State is updated from this record
             */
            inline void regStateUpdate(const InstRegRecord& rec) {
                const auto reg_num = rec.getReg();
                RegBankMap::iterator rbit;
                try {
                    rbit = find_(regbank_, reg_num);
                }
                catch(const RegNotFoundException&) {
                    if(!Registers::Codec::isNonstandardCSR(reg_num)) {
                        throw;
                    }
                    // It's a nonstandard (e.g. vendor-specific) CSR. Go ahead and register it.
                    rbit = insertSimpleRegister_(reg_num).first;
                }

                const Registers::STF_REG mapped_reg_num = rbit->second.getMappedReg();

                // If reg_num != mapped_reg_num, that means this register is mapped as a field of another, larger register
                // In this case, we also need to update the mapped register value
                if(STF_EXPECT_FALSE(reg_num != mapped_reg_num)) {
                    const auto mapped_it = regstate_.find(mapped_reg_num);
                    // Insert the map entry if it doesn't exist
                    if(STF_EXPECT_FALSE(mapped_it == regstate_.end())) {
                        regstate_.emplace(mapped_reg_num,
                                          InstRegRecord::make(mapped_reg_num,
                                                              Registers::STF_REG_OPERAND_TYPE::REG_STATE,
                                                              rbit->second.apply(0, rec.getScalarData())));
                    }
                    // Otherwise update the entry
                    else {
                        auto& mapped_reg = *mapped_it->second;
                        mapped_reg.setScalarData(rbit->second.apply(mapped_reg.getScalarData(), rec.getScalarData()));
                    }
                }

                const auto it = regstate_.find(reg_num);
                // Insert the entry if it doesn't exist
                if(STF_EXPECT_FALSE(it == regstate_.end())) {
                    regstate_.emplace(reg_num,
                                      InstRegRecord::make(rec, Registers::STF_REG_OPERAND_TYPE::REG_STATE));
                }
                // Otherwise update the entry
                else {
                    it->second->copyFrom(rec);
                }
            }

            /**
             * Gets register value (scalar version)
             * \param regno Register number to look up
             */
            inline uint64_t getRegScalarValue(const Registers::STF_REG regno) const {
                Registers::STF_REG mapped_reg;

                const auto rbit = find_(regbank_, regno);
                mapped_reg = rbit->second.getMappedReg();

                const auto it = find_(regstate_, mapped_reg);
                return (it->second->getScalarData() >> rbit->second.getShiftBits()) & rbit->second.getMask();
            }

            /**
             * Gets register value (vector version)
             * \param regno Register number to look up
             */
            const InstRegRecord::VectorType& getRegVectorValue(const Registers::STF_REG regno) const {
                Registers::STF_REG mapped_reg;

                const auto rbit = find_(regbank_, regno);
                mapped_reg = rbit->second.getMappedReg();

                const auto it = find_(regstate_, mapped_reg);
                return it->second->getVectorData();
            }

            /**
             * Iterates over the register state, applying a callback function to each entry
             * \param func Callback function
             */
            template<typename FuncType>
            __attribute__((always_inline))
            inline void applyRegState(FuncType&& func) const {
                for(const auto& r: regstate_) {
                    func(r);
                }
            }

            /**
             * Gets the number of register states currently stored
             */
            inline size_t size() const {
                return regstate_.size();
            }

            /**
             * Writes register state to STFWriter
             * \param stf_writer STFWriter to use
             */
            void writeRegState(STFWriter& stf_writer) const;
   };
} //end namespace stf

#endif //#define __STF_COMMON_HELPER_HPP__
