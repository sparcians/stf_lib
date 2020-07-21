#include "stf_ofstream.hpp"
#include "stf_record.hpp"

namespace stf {
    STFOFstream& STFOFstream::operator<<(const std::string& data) {
        writeFromPtr_(data.data(), data.size());
        return *this;
    }

    STFOFstream& STFOFstream::operator<<(const STFRecord& rec) {
        *this << descriptors::conversion::toEncoded(rec.getDescriptor());
        rec.pack(*this);
        return *this;
    }
} // end namespace stf
