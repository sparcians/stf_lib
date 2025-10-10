#include "stf_ifstream.hpp"
#include "stf_record.hpp"

namespace stf {
    // cppcheck-suppress unusedFunction
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

    void STFIFstream::seekFromOffset(const size_t offset, const size_t num_markers_at_offset, const size_t num_markers_to_seek) {
        fseek(stream_, static_cast<ssize_t>(offset), SEEK_SET);
        num_marker_records_ = num_markers_at_offset;

        if(num_markers_to_seek) {
            seek(num_markers_to_seek);
        }
    }

    void STFIFstream::rewind() {
        num_marker_records_ = 0;
        fseek(stream_, static_cast<ssize_t>(trace_start_), SEEK_SET);
        pc_tracker_.forcePC(initial_pc_);
    }

    size_t STFIFstream::tell() const {
        return static_cast<size_t>(ftell(stream_));
    }

    void STFIFstream::setTraceStart() {
        trace_start_ = tell();
    }
} // end namespace stf
