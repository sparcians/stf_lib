#ifndef __STF_ITEM_HPP__
#define __STF_ITEM_HPP__

#include <cstdint>

namespace stf {
    template<typename ItemType, typename FilterType, typename ReaderType, typename BaseReaderType, bool assume_filtered>
    class STFBufferedReader;

    template<typename ItemType, typename FilterType, typename ReaderType>
    class STFUserModeSkippingReader;

    namespace delegates {
        class STFItemDelegate;
        class STFSkippableItemDelegate;
    } // end namespace delegates

    /**
     * \class STFItem
     * \brief Base class for items stored and iterated over by readers derived from STFBufferedReader
     */
    class STFItem {
        private:
            friend class delegates::STFItemDelegate;
            uint64_t index_ = 0; /**< Index of the current item within the trace */

        protected:
            /**
             * Resets the item
             */
            __attribute__((always_inline))
            inline void reset_() {
                index_ = 0;
            }

            /**
             * Sets the index
             * \param index Index value to set
             */
            __attribute__((always_inline))
            inline void setIndex_(const uint64_t index) {
                index_ = index;
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

    /**
     * \class STFSkippableItem
     * \brief Adds a skipped_ field to STFItem
     */
    class STFSkippableItem : public STFItem {
        private:
            friend class delegates::STFSkippableItemDelegate;
            uint64_t unskipped_index_ = 0; /**< Corresponds to actual index in the trace, i.e. index item would have if there was no skipping enabled */
            bool skipped_ = false; /**< If true, item should be skipped */

        protected:
            /**
             * Resets the item
             */
            __attribute__((always_inline))
            inline void reset_() {
                STFItem::reset_();
                skipped_ = false;
            }

            /**
             * \brief Sets whether an item should be skipped
             * \param skipped If true, the item will be skipped
             */
            __attribute__((always_inline))
            inline void setSkipped_(const bool skipped) {
                skipped_ = skipped;
            }

            /**
             * Sets the index
             * \param index Index value to set
             * \param unskipped_index Actual index within the trace (i.e., the index the item would
             * have if there was no skipping)
             */
            __attribute__((always_inline))
            inline void setIndex_(const uint64_t index, const uint64_t unskipped_index) {
                STFItem::setIndex_(index);
                unskipped_index_ = unskipped_index;
            }

        public:
            /**
             * \brief Returns whether an item should be skipped
             * \return True if the item should be skipped
             */
            inline bool skipped() const {
                return skipped_;
            }

            /**
             * \brief Returns the index this item would have if there was no skipping
             * \return Unskipped index
             */
            inline uint64_t unskippedIndex() const {
                return unskipped_index_;
            }
    };

    namespace delegates {
        /**
         * \class STFItemDelegate
         * Delegate class used to hide any non-const methods from non-reader classes
         */
        class STFItemDelegate {
            protected:
                /**
                 * Sets the index
                 * \param item Item to modify
                 * \param index Index value to set
                 */
                __attribute__((always_inline))
                static inline void setIndex_(STFItem& item, const uint64_t index) {
                    item.setIndex_(index);
                }

                template<typename ItemType, typename FilterType, typename ReaderType, typename BaseReaderType, bool assume_filtered>
                friend class stf::STFBufferedReader;
        };

        /**
         * \class STFSkippableItemDelegate
         * Delegate class used to hide any non-const methods from non-reader classes
         */
        class STFSkippableItemDelegate : public STFItemDelegate {
            protected:
                /**
                 * \brief Sets whether an item should be skipped
                 * \param item Item to modify
                 * \param skipped If true, the item will be skipped
                 */
                __attribute__((always_inline))
                static inline void setSkipped_(STFSkippableItem& item, const bool skipped) {
                    item.setSkipped_(skipped);
                }

                /**
                 * Sets the index
                 * \param item Item to modify
                 * \param index Index value to set
                 * \param unskipped_index Actual index within the trace (i.e., the index the item would
                 * have if there was no skipping)
                 */
                __attribute__((always_inline))
                static inline void setIndex_(STFSkippableItem& item,
                                             const uint64_t index,
                                             const uint64_t unskipped_index) {
                    item.setIndex_(index, unskipped_index);
                }

                template<typename ItemType, typename FilterType, typename ReaderType>
                friend class stf::STFUserModeSkippingReader;
        };
    } // end namespace delegates
} // end namespace stf

#endif
