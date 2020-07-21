#include "stf_fstream.hpp"

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
} // end namespace stf
