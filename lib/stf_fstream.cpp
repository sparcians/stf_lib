#include "stf_fstream.hpp"
#include "stf_record_types.hpp"
#include "util.hpp"

namespace stf {
    std::mutex STFFstream::open_streams_mutex_;
    bool STFFstream::lock_open_streams_ = false;
    std::set<STFFstream*> STFFstream::open_streams_;
    bool STFFstream::atexit_handler_registered_ = false;

    void STFFstream::cleanupStreams_() {
        std::lock_guard<std::mutex> l(open_streams_mutex_);

        // Inform close() methods that we will handle deregistering
        lock_open_streams_ = true;
        for(auto s: open_streams_) {
            s->close();
        }

        // Clean up in the unlikely event this gets called from somewhere besides atexit
        open_streams_.clear();
        lock_open_streams_ = false;
    }

    void STFFstream::setVLen(const vlen_t vlen) {
        static constexpr size_t MIN_VLEN = byte_utils::bitSize<InstRegRecord::ValueType>();
        stf_assert(!vlen_, "Attempted to set VLen multiple times");
        stf_assert(vlen >= MIN_VLEN,
                   "Vlen parameter ("
                   << vlen
                   << ") must be greater than or equal to "
                   << MIN_VLEN);
        vlen_ = vlen;
    }

} // end namespace stf
