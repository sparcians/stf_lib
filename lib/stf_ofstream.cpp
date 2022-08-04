#include "stf_ofstream.hpp"
#include "stf_object.hpp"

namespace stf {
    STFOFstream& STFOFstream::operator<<(const std::string& data) {
        writeFromPtr_(data.data(), data.size());
        return *this;
    }

    STFOFstream& STFOFstream::operator<<(const STFBaseObject& rec) {
        rec.pack(*this);
        return *this;
    }
} // end namespace stf
