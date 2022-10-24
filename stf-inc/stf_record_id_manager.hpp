#ifndef __STF_RECORD_ID_MANAGER_HPP__
#define __STF_RECORD_ID_MANAGER_HPP__

#include <cstdint>

namespace stf {
    /**
     * \class RecordIdManager
     * Tracks the ID values for a single stream of records
     */
    class RecordIdManager {
        private:
            uint64_t next_id_ = 0;

        public:
            /**
             * Gets the next sequential ID in the record stream
             */
            inline uint64_t getNextId() {
                return ++next_id_;
            }
    };
}

#endif
