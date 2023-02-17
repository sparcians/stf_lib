#ifndef __STF_USER_MODE_SKIPPING_READER_HPP__
#define __STF_USER_MODE_SKIPPING_READER_HPP__

#include "stf_buffered_reader.hpp"
#include "stf_reader.hpp"

namespace stf {

    /**
     * \class STFUserModeSkippingReader
     * \brief A buffered STF reader that can skip past non-user mode code
     */
    template<typename ItemType, typename FilterType, typename ReaderType>
    class STFUserModeSkippingReader : public STFBufferedReader<ItemType, FilterType, ReaderType, STFReader> {
        private:
            const bool only_user_mode_ = false; /**< skips non-user-mode instructions if true */
            bool skipping_enabled_ = false; /**< Marks all items read as skipped while true */
            bool disable_skipping_on_next_item_ = false; /**< If true, disables skipping when the next instruction is read */

        protected:
            /**
             * \typedef BufferedReader
             * Parent STFBufferedReader type
             */
            using BufferedReader = STFBufferedReader<ItemType, FilterType, ReaderType, STFReader>;
            /// \cond DOXYGEN_IGNORED
            friend BufferedReader;
            /// \endcond

            using BufferedReader::DEFAULT_BUFFER_SIZE_;
            using BufferedReader::getItem_;
            using BufferedReader::rawNumItemsRead_;
            using BufferedReader::numItemsSkipped_;

            /**
             * Returns whether the the specified item shouldbe skipped.
             * \param index Item index
             * \param loc Item buffer location
             */
            __attribute__((always_inline))
            inline bool readerSkipCallback_(uint64_t& index, size_t& loc) const {
                bool skip_item = false;
                if(only_user_mode_) {
                    const auto item = getItem_(index, loc);
                    skip_item = item->skipped();
                    index -= skip_item;
                }
                return skip_item;
            }

            /**
             * Updates skipping flags based on whether we are returning to user mode
             */
            __attribute__((hot, always_inline))
            inline void updateSkipping_() {
                if(STF_EXPECT_FALSE(disable_skipping_on_next_item_)) {
                    skipping_enabled_ = false;
                    disable_skipping_on_next_item_ = false;
                }
            }

            /**
             * Updates skipping flags based on whether we are leaving or returning to user mode
             */
            __attribute__((hot, always_inline))
            inline void checkSkipping_(const bool is_mode_change, const bool is_change_to_user) {
                if(STF_EXPECT_FALSE(is_mode_change && only_user_mode_)) { // cppcheck-suppress knownArgument
                    disable_skipping_on_next_item_ |= is_change_to_user;
                    skipping_enabled_ |= !is_change_to_user;
                }
            }

            /**
             * Disables fast seeking when non-user mode skipping is enabled
             */
            __attribute__((always_inline))
            inline bool slowSeek_() const {
                return only_user_mode_;
            }

            /**
             * Returns whether non-user mode skipping is enabled
             */
            __attribute__((always_inline))
            inline bool onlyUserMode_() const {
                return only_user_mode_;
            }

            /**
             * Returns whether we are currently skipping non-user code
             */
            __attribute__((always_inline))
            inline bool skippingEnabled_() const {
                return skipping_enabled_;
            }

            /**
             * Returns whether an item is skipped
             * \param item Item to check
             */
            __attribute__((always_inline))
            static inline bool skipped_(const ItemType& item) {
                return item.skipped();
            }

            /**
             * Initializes item index
             */
            __attribute__((always_inline))
            inline void initItemIndex_(ItemType& item) const {
                const auto unskipped_index = rawNumItemsRead_();
                delegates::STFSkippableItemDelegate::setIndex_(item,
                                                               unskipped_index - numItemsSkipped_(),
                                                               unskipped_index);
            }

        public:
            using BufferedReader::getISA;
            using BufferedReader::getInitialIEM;

            /**
             * \brief Constructor
             * \param only_user_mode If true, non-user-mode instructions will be skipped
             * \param buffer_size The size of the instruction sliding window
             */
            explicit STFUserModeSkippingReader(const bool only_user_mode = false,
                                               const size_t buffer_size = DEFAULT_BUFFER_SIZE_) :
                BufferedReader(buffer_size),
                only_user_mode_(only_user_mode)
            {
                // Does NOT call open() by default - must be handled by subclass
            }
    };
} // end namespace stf

#endif
