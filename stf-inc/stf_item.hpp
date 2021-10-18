#ifndef __STF_ITEM_HPP__
#define __STF_ITEM_HPP__

#include <cstdint>

namespace stf {
    /**
     * \class STFItem
     * \brief Base class for items stored and iterated over by readers derived from STFBufferedReader
     */
    class STFItem {
        protected:
            uint64_t index_ = 0; /**< Index of the current item within the trace */

            /**
             * Resets the item
             */
            inline void reset_() {
                index_ = 0;
            }

        public:
            /**
             * \brief Item index (starting from 1)
             * \return Item index
             */
            inline uint64_t index() const {
                return index_;
            }
    };
} // end namespace stf

#endif
