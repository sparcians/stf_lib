#include "stf_factory.hpp"
#include "stf_ifstream.hpp"
#include "stf_record.hpp"
#include "stf_record_types.hpp"
#include "stf_reg_def.hpp"

namespace stf {
    // REQUIRED to properly instantiate RecordFactory and all STFRecord types
    // Should only be specified ONCE in a .cpp file
    FINALIZE_FACTORY(STFRecord)
} // end namespace stf
