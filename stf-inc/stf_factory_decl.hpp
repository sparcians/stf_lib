#ifndef __STF_FACTORY_DECL_HPP__
#define __STF_FACTORY_DECL_HPP__

#include <cstddef>

namespace stf {
    template<typename PoolType, typename Enum = typename PoolType::base_type::id_type>
    class Factory;

    template<typename T>
    struct factory_lookup;
}

#endif
