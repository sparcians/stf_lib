#ifndef __STF_SATP_DECODER_HPP__
#define __STF_SATP_DECODER_HPP__

#include "stf_enums.hpp"
#include "stf_exception.hpp"
#include "stf_virt_addr_modes.hpp"
#include "util.hpp"

namespace stf {
    /**
     * \struct SatpProperties
     * \brief Defines SATP register properties based on INST_IEM
     */
    template<INST_IEM>
    struct SatpProperties;

    /**
     * \struct SatpProperties<INST_IEM::STF_INST_IEM_RV32>
     * \brief Defines SATP register properties for RV32
     */
    template<>
    struct SatpProperties<INST_IEM::STF_INST_IEM_RV32> {
        static inline constexpr unsigned int PPN_BITS = 22; /**< Size of the PPN field */
        static inline constexpr unsigned int MODE_SHIFT = 31; /**< Number of bits to shift out to get mode field */
        static inline constexpr unsigned int MODE_BITS = 1; /**< Number of bits in mode field */
    };

    /**
     * \struct SatpProperties<INST_IEM::STF_INST_IEM_RV64>
     * \brief Defines SATP register properties for RV64
     */
    template<>
    struct SatpProperties<INST_IEM::STF_INST_IEM_RV64> {
        static inline constexpr unsigned int PPN_BITS = 44; /**< Size of the PPN field */
        static inline constexpr unsigned int MODE_SHIFT = 60; /**< Number of bits to shift out to get mode field */
        static inline constexpr unsigned int MODE_BITS = 4; /**< Number of bits in mode field */
    };

    /**
     * \class Satp
     * \brief Holds decoded SATP register values
     */
    class Satp {
        private:
            const uint64_t base_addr_;
            const VAMode va_mode_;

        public:
            /**
             * Constructor
             */
            Satp(const uint64_t base_addr, const VAMode va_mode) :
                base_addr_(base_addr),
                va_mode_(va_mode)
            {
            }

            /**
             * Gets the base address
             */
            __attribute__((always_inline))
            inline uint64_t getBaseAddr() const {
                return base_addr_;
            }

            /**
             * Gets the virtual addressing mode
             */
            __attribute__((always_inline))
            inline VAMode getVAMode() const {
                return va_mode_;
            }
    };

    /**
     * \struct SatpDecoderBase
     * \brief Base class that implements methods used by all SATP decoders
     */
    template<typename DecoderType>
    struct SatpDecoderBase {
        /**
         * \typedef ModeType
         * \brief Integer type used to hold SATP mode bits
         */
        using ModeType = uint8_t;

        /**
         * Extracts mode bits
         */
        static inline constexpr ModeType getModeBits(const uint64_t satp_value) {
            static_assert(byte_utils::bitSize<uint64_t>() > DecoderType::Properties::MODE_SHIFT);
            static_assert(DecoderType::Properties::MODE_BITS <= byte_utils::bitSize<ModeType>());

            constexpr uint64_t MODE_MASK = byte_utils::bitMask<uint64_t, DecoderType::Properties::MODE_BITS>();
            return static_cast<ModeType>((satp_value >> DecoderType::Properties::MODE_SHIFT) & MODE_MASK);
        }

        /**
         * Extracts base address
         */
        static inline constexpr uint64_t getBaseAddress(const uint64_t satp_value) {
            constexpr uint64_t PPN_MASK = byte_utils::bitMask<uint64_t, DecoderType::Properties::PPN_BITS>();
            return (satp_value & PPN_MASK) << PAGE_OFFSET_SIZE;
        }

        /**
         * Fully decodes an SATP register value
         */
        __attribute__((always_inline))
        static inline Satp decode(const uint64_t satp_value) {
            return Satp(getBaseAddress(satp_value),
                        DecoderType::getVAMode(satp_value));
        }
    };

    /**
     * \struct SatpDecoder
     * \brief Cass that decodes SATP register values according to INST_IEM
     */
    template<INST_IEM iem>
    struct SatpDecoder : public SatpDecoderBase<SatpDecoder<iem>> {
        /**
         * \typedef Properties
         * \brief Points to the SATP properties corresponding to this decoder
         */
        using Properties = SatpProperties<iem>;

        using SatpDecoderBase<SatpDecoder<iem>>::getModeBits;

        /**
         * Gets the VA mode
         */
        static inline VAMode getVAMode(const uint64_t satp_value);
    };

    /**
     * Gets the VA mode
     */
    template<>
    __attribute__((always_inline))
    inline VAMode SatpDecoder<INST_IEM::STF_INST_IEM_RV32>::getVAMode(const uint64_t satp_value) {
        const auto satp_mode = getModeBits(satp_value);
        // There's only 1 mode bit
        return satp_mode ? VAMode::SV32 : VAMode::NO_TRANSLATION;
    }

    /**
     * Gets the VA mode
     */
    template<>
    __attribute__((always_inline))
    inline VAMode SatpDecoder<INST_IEM::STF_INST_IEM_RV64>::getVAMode(const uint64_t satp_value) {
        const auto satp_mode = getModeBits(satp_value);
        switch(satp_mode) {
            case 0:
                return VAMode::NO_TRANSLATION;
            case 8:
                return VAMode::SV39;
            case 9:
                return VAMode::SV48;
            case 10:
                return VAMode::SV57;
        }

        stf_throw("Invalid SATP mode value: " << std::hex << static_cast<uint16_t>(satp_mode));
    }
} // end namespace stf

#endif
