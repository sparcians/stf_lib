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
        STF_GEN_ATHENA,                     /**< Athena */
        STF_GEN_STF_MORPH,                  /**< stf_morph */

        /*
         * Note that if you add anything to this list, you need to
         * update all the users, too. For instance, stf_check will reject
         * your newly-generated stf if you don't add your new generator.
         */
        STF_GEN_RESERVED_END = 0x80         /**< reserved end of STF_GEN*/
    };

    /**
     * Writes a string representation of an STF_GEN enum to an ostream
     */
    std::ostream& operator<<(std::ostream& os, STF_GEN generator);
}

#endif
