#ifndef __STF_GENERATOR_HPP__
#define __STF_GENERATOR_HPP__

#include <ostream>

#include "stf_enum_utils.hpp"

namespace stf {
    /**
     * \enum STF_GEN
     *
     * The trace generator
     *
     */
    STF_ENUM(
        STF_ENUM_CONFIG(AUTO_PRINT, OVERRIDE_START, OVERRIDE_END),
        STF_GEN,
        uint8_t,
        STF_GEN_RESERVED,                                                           /**< reserved */
        STF_ENUM_STR(STF_GEN_QEMU, "QEMU"),                                         /**< QEMU */
        STF_ENUM_STR(STF_GEN_ANDROID_EMULATOR, "Android Emulator"),                 /**< Android emulator */
        STF_ENUM_STR(STF_GEN_GEM5, "GEM5"),                                         /**< GEM5 */
        STF_ENUM_STR(STF_GEN_PTE_GEN, "PTE_GEN"),                                   /**< PTE generator  */
        STF_ENUM_STR(STF_GEN_IMPERAS, "Imperas"),                                   /**< Imperas */
        STF_ENUM_STR(STF_GEN_SPIKE, "Spike"),                                       /**< Spike */
        STF_ENUM_STR(STF_GEN_STF_EXTRACT, "stf_extract"),                           /**< stf_extract */
        STF_ENUM_STR(STF_GEN_STF_MERGE, "stf_merge"),                               /**< stf_merge */
        STF_ENUM_STR(STF_GEN_STF_FILTER_EVT, "stf_filter_evt"),                     /**< stf_filter_evt */
        STF_ENUM_STR(STF_GEN_ATHENA, "Athena"),                                     /**< Athena */
        STF_ENUM_STR(STF_GEN_STF_MORPH, "stf_morph"),                               /**< stf_morph */
        STF_ENUM_STR(STF_GEN_DROMAJO, "Dromajo"),                                   /**< Dromajo */
        STF_ENUM_STR(STF_GEN_PEGASUS, "Pegasus"),                                   /**< Pegasus */
        STF_ENUM_STR(STF_GEN_STF_TRANSACTION_EXTRACT, "stf_transaction_extract"),   /**< stf_transaction_extract */
        // 0x40 is the beginning of example tools
        STF_ENUM_VAL(STF_TRANSACTION_EXAMPLE, 0x40, "stf_transaction_example"),

        /*
         * Note that if you add anything to this list, you need to
         * update all the users, too. For instance, stf_check will reject
         * your newly-generated stf if you don't add your new generator.
         */
        STF_ENUM_VAL(STF_GEN_RESERVED_END, 0xFF)
    );
}

#endif
