#ifndef __STF_VLEN_HPP__
#define __STF_VLEN_HPP__

#include <cstdint>

namespace stf {
    /**
     * \typedef vlen_t
     * Type used to represent the vector vlen parameter
     *
     * There are no bounds on the maximum possible vlen in RISC-V, but
     * https://github.com/riscv/riscv-v-spec/issues/204 indicates that
     * 32 bits should be sufficient for the vast majority of cases.
     */
    using vlen_t = uint32_t;
} // end namespace stf

#endif
