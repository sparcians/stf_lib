/**
 * \brief  This file defines a buffered STF trace reader class
 *
 */

#ifndef __STF_BUFFERED_READER_HPP__
#define __STF_BUFFERED_READER_HPP__

#include <sys/stat.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include "stf_enums.hpp"
#include "stf_exception.hpp"
#include "stf_item.hpp"
#include "stf_reader_base.hpp"
#include "stf_record.hpp"
#include "stf_record_types.hpp"

/**
 * \namespace stf
 * \brief Defines all STF related classes
 *
 */
namespace stf {

    /**
     * \class STFBufferedReader
     * \brief The STFBufferedReader provides an iterator to a buffered stream of objects constructed from a trace
     */
    template<typename ItemType, typename FilterType, typename ReaderType, typename BaseReaderType>
    class STFBufferedReader: public BaseReaderType {
        private:
            static_assert(std::is_base_of_v<STFReaderBase, BaseReaderType>,
                          "BaseReaderType must inherit from STFReaderBase");

            void resetBuffer_() {
                buf_.reset();
                head_ = 0;
                tail_ = 0;
            }

        protected:
            /**
             * \typedef IntDescriptor
             * \brief Internal descriptor type
             */
            using IntDescriptor = descriptors::internal::Descriptor;

            static constexpr size_t DEFAULT_BUFFER_SIZE_ = 1024; /**< Default buffer size */

            const size_t buffer_size_; /**< Size of the buffer */
            const size_t buffer_mask_; /**< Mask value used to index into the buffer */

            bool last_item_read_ = false; /**< If true, we have read to the end of the trace */

            /**
             * \typedef BufferT
             * \brief Underlying buffer type
             */
            using BufferT = ItemType[]; // NOLINT: Use C-array here so we can use [] operator on the unique_ptr
            std::unique_ptr<BufferT> buf_; /**< Circular buffer */

            size_t head_ = 0;         /**< index of head of current item in circular buffer */
            size_t tail_ = 0;         /**< index of tail of current item in circular buffer */
            size_t num_items_read_ = 0; /**< Counts number of items read from the buffer */
            bool buffer_is_empty_ = true; /**< True if the buffer contains no items */
            size_t num_skipped_items_ = 0; /**< Counts number of skipped items so that item indices can be adjusted */

            FilterType filter_; /**< Filter type used to skip over certain record types */

            /**
             * Default skipped_ method
             * \param item Item to check for skipping
             */
            __attribute__((always_inline))
            static inline bool skipped_(const ItemType& item) {
                return false;
            }

            /**
             * Checks whether an item should be skipped. Delegates the check to subclass if it defines a skipped_ method.
             * \param item Item to check for skipping
             */
            __attribute__((always_inline))
            inline bool itemSkipped_(const ItemType& item) const {
                return ReaderType::skipped_(item);
            }

            /**
             * Increments the skipped item counter
             * \param skip_item If true, increment skipped item counter
             */
            __attribute__((always_inline))
            inline void countSkipped_(const bool skip_item) {
                num_skipped_items_ += skip_item;
            }

            /**
             * Default item skipping cleanup callback. Does nothing.
             */
            __attribute__((always_inline))
            inline void skippedCleanup_() {
            }

            /**
             * Invokes subclass callback to clean up any addtional state when an item is skipped
             */
            __attribute__((always_inline))
            inline void skippedItemCleanup_() {
                static_cast<ReaderType*>(this)->skippedCleanup_();
            }

            /**
             * Initializes the internal buffer
             */
            bool initItemBuffer_() {
                buf_ = std::make_unique<BufferT>(static_cast<size_t>(buffer_size_));

                size_t i = 0;
                while(i < buffer_size_) {
                    try {
                        readNextItem_(buf_[i]);
                        if(STF_EXPECT_FALSE(itemSkipped_(buf_[i]))) {
                            skippedItemCleanup_();
                            continue;
                        }
                    }
                    catch(const EOFException&) {
                        last_item_read_ = true;
                        break;
                    }
                    ++tail_;
                    ++i;
                }

                // no items in the file;
                if (STF_EXPECT_FALSE(tail_ == 0)) {
                    buffer_is_empty_ = true;
                    return false;
                }

                buffer_is_empty_ = false;
                --tail_; // Make tail_ point to the last item read instead of one past the last item
                head_ = 0;
                return true;
            }

            /**
             * Internal function to validate item by index to prevent buffer under/overrun
             * \param index Index value to check
             */
            __attribute__((always_inline))
            inline void validateItemIndex_(const uint64_t index) const {
                const auto& tail = buf_[tail_];
                stf_assert(index >= buf_[head_].index() && (itemSkipped_(tail) || index <= tail.index()),
                           "sliding window index out of range");
            }

            /**
             * Gets item based on index and buffer location
             * \param index Index of item (used only for validation)
             * \param loc Location of item within the buffer
             */
            __attribute__((always_inline))
            inline const ItemType* getItem_(const uint64_t index, const size_t loc) const {
                validateItemIndex_(index);
                return &buf_[loc];
            }

            /**
             * Gets item based on index and buffer location
             * \param index Index of item (used only for validation)
             * \param loc Location of item within the buffer
             */
            __attribute__((always_inline))
            inline ItemType* getItem_(const uint64_t index, const size_t loc) {
                validateItemIndex_(index);
                return &buf_[loc];
            }

            /**
             * Checks if the specified item is the last one in the trace
             * \param index Index of item (used only for validation)
             * \param loc Location of item within the buffer
             */
            inline bool isLastItem_(const uint64_t index, const size_t loc) {
                validateItemIndex_(index);

                if(STF_EXPECT_TRUE(!last_item_read_)) {
                    return false;
                }

                return loc == tail_;
            }

            /**
             * Returns whether the reader decided to skip the specified item. Can be overridden by a subclass.
             * \param index Item index
             * \param loc Item buffer location
             */
            __attribute__((always_inline))
            inline bool readerSkipCallback_(uint64_t& index, size_t& loc) const {
                return false;
            }

            /**
             * Calls the subclass implementation of readerSkipCallback_
             * \param index Item index
             * \param loc Item buffer location
             */
            __attribute__((always_inline))
            inline bool callReaderSkipCallback_(uint64_t& index, size_t& loc) const {
                return static_cast<const ReaderType*>(this)->readerSkipCallback_(index, loc);
            }

            /**
             * Helper function for item iteration. The function does the following work;
             * - refill the circular buffer if iterates to 2nd last item in the buffer;
             * - handle buffer location crossing boundary;
             * - pair the item index and buffer location;
             * \param index Item index
             * \param loc Item buffer location
             */

            __attribute__((always_inline))
            inline bool moveToNextItem_(uint64_t &index, size_t &loc) {
                bool skip_item = false;

                do {
                    // validate the item index;
                    validateItemIndex_(index);

                    // if current location is the 2nd last item in buffer;
                    // refill the half of the buffer;
                    if (STF_EXPECT_FALSE(loc == tail_ -1)) {
                        fillHalfBuffer_();
                    }

                    // since 2nd last is used for refill;
                    // the tail is absolute the end of trace;
                    if (STF_EXPECT_FALSE(loc == tail_)) {
                        return false;
                    }

                    index++;
                    loc = (loc + 1) & buffer_mask_;
                    // Check to see if the subclass decided to skip this item
                    skip_item = callReaderSkipCallback_(index, loc);
                }
                while(skip_item);

                num_items_read_ = index;
                return true;
            }

            /**
             * Returns the number of items read so far without filtering
             */
            __attribute__((always_inline))
            inline size_t rawNumItemsRead_() const {
                return static_cast<const ReaderType*>(this)->rawNumRead_();
            }

            /**
             * Returns the number of items read from the reader so far with filtering
             */
            __attribute__((always_inline))
            inline size_t numItemsReadFromReader_() const {
                return rawNumItemsRead_() - num_skipped_items_;
            }

            /**
             * Initializes item index
             */
            __attribute__((always_inline))
            inline void initItemIndex_(ItemType& item) const {
                delegates::STFItemDelegate::setIndex_(item, numItemsReadFromReader_());
            }

            /**
             * Returns the number of items read from the buffer so far with filtering
             */
            inline size_t numItemsRead_() const {
                return num_items_read_;
            }

            /**
             * Fill item into the half of the circular buffer;
             */
            size_t fillHalfBuffer_() {
                size_t pos = tail_;
                const size_t init_item_cnt = numItemsReadFromReader_();
                const size_t max_item_cnt = init_item_cnt + (buffer_size_ / 2);
                while(numItemsReadFromReader_() < max_item_cnt) {
                    pos = (pos + 1) & buffer_mask_;

                    try {
                        readNextItem_(buf_[pos]);
                        if(STF_EXPECT_FALSE(itemSkipped_(buf_[pos]))) {
                            skippedItemCleanup_();
                            pos = (pos - 1) & buffer_mask_;
                        }
                    }
                    catch(const EOFException&) {
                        last_item_read_ = true;
                        break;
                    }
                }

                const size_t item_cnt = numItemsReadFromReader_() - init_item_cnt;
                // adjust head and tail;
                if (STF_EXPECT_TRUE(item_cnt != 0)) {
                    tail_ = (tail_ + item_cnt) & buffer_mask_;
                    head_ = (head_ + item_cnt) & buffer_mask_;
                }

                return item_cnt;
            }

            /**
             * read STF records to construct a ItemType instance. readNext_ must be implemented by subclass
             * \param item Item to modify with the record
             */
            __attribute__((hot, always_inline))
            inline void readNextItem_(ItemType &item) {
                static_cast<ReaderType*>(this)->readNext_(item);
            }

            /**
             * Calls handleNewRecord_ callback in subclass
             * \param item Item to modify
             * \param urec Record used to modify item
             */
            __attribute__((hot, always_inline))
            inline const STFRecord* handleNewItemRecord_(ItemType& item, STFRecord::UniqueHandle&& urec) {
                return static_cast<ReaderType*>(this)->handleNewRecord_(item, std::move(urec));
            }

            /**
             * \brief Read the next STF record, optionally using it to modify the specified item
             * \param item Item to modify
             * \return pointer to record if it is valid; otherwise nullptr
             */
            __attribute__((hot, always_inline))
            inline const STFRecord* readRecord_(ItemType& item) {
                STFRecord::UniqueHandle urec;
                BaseReaderType::operator>>(urec);

                if(STF_EXPECT_FALSE(filter_.isFiltered(urec->getId()))) {
                    return nullptr;
                }

                return handleNewItemRecord_(item, std::move(urec));
            }

            /**
             * Gets the index of the first item in the buffer
             */
            inline uint64_t getFirstIndex_() {
                return buf_[head_].index();
            }

            /**
             * Returns whether the subclass thinks we should seek the slow way
             */
            inline bool slowSeek_() const {
                return false;
            }

            /**
             * Returns whether we should seek the slow way
             */
            inline bool slowItemSeek_() const {
                return static_cast<const ReaderType*>(this)->slowSeek_();
            }

            /**
             * \class base_iterator
             * \brief iterator of the item stream that hides the sliding window.
             * Decrement is not implemented. Rewinding is done by copying or assigning
             * an existing iterator, with range limited by the sliding window size.
             *
             * Using the iterator ++ operator may advance the underlying trace stream,
             * which is un-rewindable if the trace is compressed or via STDIN
             *
             */
            class base_iterator {
                friend class STFBufferedReader;

                private:
                    STFBufferedReader *sir_ = nullptr;  // the reader
                    uint64_t index_ = 1;            // index to the item stream
                    size_t loc_ = 0;                // location in the sliding window buffer;
                                                    // keep it to speed up casting;
                    bool end_ = true;               // whether this is an end iterator

                protected:
                    /**
                     * \brief whether pointing to the last item
                     * \return true if last item
                     */
                    inline bool isLastItem_() const {
                        return sir_->isLastItem_(index_, loc_);
                    }

                public:
                    /**
                     * \typedef difference_type
                     * Type used for finding difference between two iterators
                     */
                    using difference_type = std::ptrdiff_t;

                    /**
                     * \typedef value_type
                     * Type pointed to by this iterator
                     */
                    using value_type = ItemType;

                    /**
                     * \typedef pointer
                     * Pointer to a value_type
                     */
                    using pointer = const ItemType*;

                    /**
                     * \typedef reference
                     * Reference to a value_type
                     */
                    using reference = const value_type&;

                    /**
                     * \typedef iterator_category
                     * Iterator type - using forward_iterator_tag because backwards iteration is not currently supported
                     */
                     using iterator_category = std::forward_iterator_tag;

                    /**
                     * \brief Default constructor
                     *
                     */
                    base_iterator() = default;

                    /**
                     * \brief Constructor
                     * \param sir The STF reader to iterate
                     * \param end Whether this is an end iterator
                     *
                     */
                    explicit base_iterator(STFBufferedReader *sir, const bool end = false) :
                        sir_(sir),
                        end_(end)
                    {
                        if(!end) {
                            if (!sir_->buf_) {
                                if(!sir_->initItemBuffer_()) {
                                    end_ = true;
                                }
                                loc_ = 0;
                            }
                            else {
                                end_ = sir_->buffer_is_empty_;
                            }

                            index_ = sir_->getFirstIndex_();
                        }
                    }

                    /**
                     * \brief Copy constructor
                     * \param rv The existing iterator to copy from
                     *
                     */
                    base_iterator(const base_iterator & rv) = default;

                    /**
                     * \brief Assignment operator
                     * \param rv The existing iterator to copy from
                     *
                     */
                    base_iterator & operator=(const base_iterator& rv) = default;

                    /**
                     * \brief Pre-increment operator
                     */
                    __attribute__((always_inline))
                    inline base_iterator & operator++() {
                        stf_assert(!end_, "Can't increment the end iterator");

                        // index_ and loc_ are increased in moveToNextItem_;
                        if(STF_EXPECT_FALSE(!sir_->moveToNextItem_(index_, loc_))) {
                            end_ = true;
                        }

                        return *this;
                    }

                    /**
                     * \brief Post-increment operator
                     */
                    __attribute__((always_inline))
                    inline base_iterator operator++(int) {
                        auto temp = *this;
                        operator++();
                        return temp;
                    }

                    /**
                     * \brief Return the ItemType pointer the iterator points to
                     */
                    inline pointer current() const {
                        if (STF_EXPECT_FALSE(end_)) {
                            return nullptr;
                        }
                        return sir_->getItem_(index_, loc_);
                    }

                    /**
                     * \brief Returns whether the iterator is still valid
                     */
                    inline bool valid() const {
                        try {
                            // Try to get a valid pointer
                            return current();
                        }
                        catch(const STFException&) {
                            // The item is outside the current window, so it's invalid
                            return false;
                        }
                    }

                    /**
                     * \brief Return the ItemType pointer the iterator points to
                     */
                    inline const value_type& operator*() const { return *current(); }

                    /**
                     * \brief Return the ItemType pointer the iterator points to
                     */
                    inline pointer operator->() const { return current(); }

                    /**
                     * \brief The equal operator to check ending
                     * \param rv The iterator to compare with
                     */
                    inline bool operator==(const base_iterator& rv) const {
                        return (end_ && rv.end_) || (!end_ && !rv.end_ && (index_ == rv.index_));
                    }

                    /**
                     * \brief The unequal operator to check ending
                     * \param rv The iterator to compare with
                     */
                    inline bool operator!=(const base_iterator& rv) const {
                        return !operator==(rv);
                    }
            };

        public:
            /**
             * \brief Constructor
             * \param buffer_size The size of the buffer sliding window
             */
            explicit STFBufferedReader(const size_t buffer_size = DEFAULT_BUFFER_SIZE_) :
                buffer_size_(buffer_size),
                buffer_mask_(buffer_size_ - 1)
            {
                // Does NOT call open() automatically! Must be handled by derived classes.
            }

            /**
             * \brief The beginning of the item stream
             *
             */
            template<typename U = ReaderType>
            inline typename U::iterator begin() { return typename U::iterator(static_cast<U*>(this)); }

            /**
             * \brief The beginning of the item stream
             * \param skip Skip this many items at the beginning
             *
             */
            template<typename U = ReaderType>
            inline typename U::iterator begin(const size_t skip) {
                if(skip) {
                    return seekFromBeginning(skip);
                }

                return typename U::iterator(static_cast<U*>(this));
            }

            /**
             * \brief The end of the item stream
             *
             */
            template<typename U = ReaderType>
            inline const typename U::iterator& end() {
                static const auto end_it = typename U::iterator(static_cast<U*>(this), true);
                return end_it;
            }

            /**
             * \brief Opens a file
             * \param filename The trace file name
             * \param force_single_threaded_stream If true, forces single threaded mode in reader
             */
            void open(const std::string_view filename,
                      const bool force_single_threaded_stream = false) {
                BaseReaderType::open(filename, force_single_threaded_stream);
                resetBuffer_();
            }

            /**
             * \brief Closes the file
             */
            int close() override {
                resetBuffer_();
                return BaseReaderType::close();
            }

            /**
             * Seeks the reader by the specified number of items and returns an iterator to that point
             * \note Intended for seeking the reader prior to reading any items. For seeking in a reader that
             * has already been iterated over, use the seek() method.
             * \param num_items Number of items to skip
             */
            template<typename U = ReaderType>
            inline typename U::iterator seekFromBeginning(const size_t num_items) {
                auto it = begin();
                seek(it, num_items);
                return it;
            }

            /**
             * \brief Seeks an iterator by the given number of items
             * \param it Iterator to seek
             * \param num_items Number of items to seek by
             */
            template<typename U = ReaderType>
            inline void seek(typename U::iterator& it, const size_t num_items) {
                const size_t num_buffered = tail_ - it.loc_ + 1;
                // If the items are already buffered or skipping mode is enabled,
                // we have to seek the slow way
                if(slowItemSeek_() || num_items <= num_buffered) {
                    const auto end_it = end();
                    for(size_t i = 0; i < num_items && it != end_it; ++i) {
                        ++it;
                    }
                }
                else {
                    // We don't need to seek the reader past items we've already read
                    const size_t num_to_skip = num_items - num_buffered;
                    BaseReaderType::seek(num_to_skip);
                    head_ = 0;
                    tail_ = 0;
                    initItemBuffer_();
                    it = begin();
                }
            }

            /**
             * Gets the filter object for this reader
             */
            FilterType& getFilter() {
                return filter_;
            }
    };
} //end namespace stf

// __STF_INST_READER_HPP__
#endif
