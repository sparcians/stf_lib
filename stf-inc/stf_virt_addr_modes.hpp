#ifndef __STF_VIRT_ADDR_MODES_HPP__
#define __STF_VIRT_ADDR_MODES_HPP__

#include <cstdint>

namespace stf {
    /**
     * \enum VAMode
     * \brief Virtual addressing modes
     */
    enum class VAMode : uint8_t {
        SV32,
        SV39,
        SV48,
        SV57,
        NO_TRANSLATION
    };

    static inline constexpr unsigned int PAGE_OFFSET_SIZE = 12; /**< Page offset size in bits */

    /**
     * \struct VAProperties
     * \brief Defines various properties for different virtual addressing modes
     */
    template<VAMode>
    struct VAProperties;

    /**
     * \struct VAProperties<VAMode::SV32>
     * \brief Defines VA properties for SV32
     */
    template<>
    struct VAProperties<VAMode::SV32> {
        static inline constexpr unsigned int PHYS_ADDR_SIZE = 34; /**< Physical address size */
        static inline constexpr unsigned int VIRT_ADDR_SIZE = 32; /**< Virtual address size */
        static inline constexpr unsigned int VPN_SIZE = 10; /**< Size of each VPN field */
        static inline constexpr unsigned int PTE_SHIFT = 2; /**< Shift amount used when calculating PTE addresses */
    };

    /**
     * \struct VAProperties<VAMode::SV39>
     * \brief Defines VA properties for SV39
     */
    template<>
    struct VAProperties<VAMode::SV39> {
        static inline constexpr unsigned int PHYS_ADDR_SIZE = 56; /**< Physical address size */
        static inline constexpr unsigned int VIRT_ADDR_SIZE = 39; /**< Virtual address size */
        static inline constexpr unsigned int VPN_SIZE = 9; /**< Size of each VPN field */
        static inline constexpr unsigned int PTE_SHIFT = 3; /**< Shift amount used when calculating PTE addresses */
    };

    /**
     * \struct VAProperties<VAMode::SV48>
     * \brief Defines VA properties for SV48
     */
    template<>
    struct VAProperties<VAMode::SV48> {
        static inline constexpr unsigned int PHYS_ADDR_SIZE = 56; /**< Physical address size */
        static inline constexpr unsigned int VIRT_ADDR_SIZE = 48; /**< Virtual address size */
        static inline constexpr unsigned int VPN_SIZE = 9; /**< Size of each VPN field */
        static inline constexpr unsigned int PTE_SHIFT = 3; /**< Shift amount used when calculating PTE addresses */
    };

    /**
     * \struct VAProperties<VAMode::SV57>
     * \brief Defines VA properties for SV57
     */
    template<>
    struct VAProperties<VAMode::SV57> {
        static inline constexpr unsigned int PHYS_ADDR_SIZE = 56; /**< Physical address size */
        static inline constexpr unsigned int VIRT_ADDR_SIZE = 57; /**< Virtual address size */
        static inline constexpr unsigned int VPN_SIZE = 9; /**< Size of each VPN field */
        static inline constexpr unsigned int PTE_SHIFT = 3; /**< Shift amount used when calculating PTE addresses */
    };
} // end namespace stf

#endif
