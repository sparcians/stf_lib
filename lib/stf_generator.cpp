#include "stf_generator.hpp"
#include "stf_exception.hpp"

namespace stf {
    std::ostream& operator<<(std::ostream& os, const STF_GEN generator) {
        switch(generator) {
            case STF_GEN::STF_GEN_QEMU:
                os << "QEMU";
                return os;
            case STF_GEN::STF_GEN_ANDROID_EMULATOR:
                os << "Android Emulator";
                return os;
            case STF_GEN::STF_GEN_GEM5:
                os << "GEM5";
                return os;
            case STF_GEN::STF_GEN_IMPERAS:
                os << "Imperas";
                return os;
            case STF_GEN::STF_GEN_PTE_GEN:
                os << "PTE_GEN";
                return os;
            case STF_GEN::STF_GEN_SPIKE:
                os << "Spike";
                return os;
            case STF_GEN::STF_GEN_ATHENA:
                os << "Athena";
                return os;
            case STF_GEN::STF_GEN_STF_EXTRACT:
                os << "stf_extract";
                return os;
            case STF_GEN::STF_GEN_STF_MERGE:
                os << "stf_merge";
                return os;
            case STF_GEN::STF_GEN_STF_FILTER_EVT:
                os << "stf_filter_evt";
                return os;
            case STF_GEN::STF_GEN_STF_MORPH:
                os << "stf_morph";
                return os;
            case STF_GEN::STF_GEN_DROMAJO:
                os << "Dromajo";
                return os;
            case STF_GEN::STF_GEN_RESERVED:
            case STF_GEN::STF_GEN_RESERVED_END:
                break;
        };

        stf_throw("Invalid STF_GEN value: " << enums::to_printable_int(generator));
    }

}
