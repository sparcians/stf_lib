#include "stf_ifstream.hpp"
#include "stf_record.hpp"

namespace stf {
    void STFIFstream::seek(size_t num_marker_records) {
        const size_t end_marker_num = num_marker_records_ + num_marker_records;
        STFRecord::UniqueHandle rec;

        try {
            while(operator bool() && (num_marker_records_ < end_marker_num)) {
                operator>>(rec);
            }
        }
        catch(const EOFException&) {
            stf_throw("Attempted to seek past the end of the trace");
        }
    }
} // end namespace stf
