#include "format_utils.hpp"

namespace stf {
    // Static flag that tells format_utils to print physical addresses
    // Enable with format_utils::setShowPhys(true)
    // Disable with format_utils::setShowPhys(false)
    bool format_utils::show_phys_ = false;
}
