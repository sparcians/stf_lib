#ifndef __STF_RECORD_MAP_HPP__
#define __STF_RECORD_MAP_HPP__

#include <deque>
#include <iterator>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

#include "boost_wrappers/pool.hpp"
#include "boost_wrappers/small_vector.hpp"

#include "stf_enums.hpp"
#include "stf_record.hpp"
#include "stf_valid_value.hpp"
#include "util.hpp"

namespace stf {
    class SortedRecordMapIterable;

    /**
     * \class RecordMap
     * Class used by STFInstReader and others to organize STF records read from a file by type
     *
     * Records are stored in a map of vectors
     *
     * Not thread safe!
     */
    class RecordMap {
        private:
            static constexpr size_t DEFAULT_VEC_SIZE_ = 1;

            /**
             * \typedef key_type
             * Type used as key in the underlying map
             */
            using key_type = enums::int_t<descriptors::internal::Descriptor>;

        public:
            /**
             * \class Index
             * Holds an index that can be used to reference a specific record within a RecordMap
             */
            class Index {
                private:
                    const key_type key_;
                    const size_t index_;

                public:
                    /**
                     * Constructs an Index from a descriptor and a sub-array index
                     * \param desc Descriptor value
                     * \param index Sub-array index
                     */
                    Index(const descriptors::internal::Descriptor desc, const size_t index) :
                        Index(static_cast<key_type>(desc), index)
                    {
                    }

                    /**
                     * Constructs an Index from a key and a sub-array index
                     * \param key Key
                     * \param index Sub-array index
                     */
                    Index(const key_type key, const size_t index) :
                        key_(key),
                        index_(index)
                    {
                    }

                    /**
                     * Gets the key for the record
                     */
                    key_type getKey() const { return key_; }

                    /**
                     * Gets the index of the record within its key-specific vector
                     */
                    size_t getIndex() const { return index_; }
            };

            /**
             * \class SmallVector
             * Wraps a boost::small_vector. Optimizes the common case for STF records (most instructions have
             * at most one of any particular record type) by always having one record slot available (optimizes out some
             * branch mispredicts in the boost::small_vector::emplace_back method).
             */
            class SmallVector {
                private:
                    using vec_type = boost::container::small_vector<STFRecord::UniqueHandle,
                                                                    DEFAULT_VEC_SIZE_>;
                    vec_type data_;

                    void copyFrom_(const SmallVector& rhs) {
                        std::transform(std::begin(rhs),
                                       std::end(rhs),
                                       std::back_inserter(*this),
                                       [](const auto& ptr) {
                                           return ptr->clone();
                                       }
                        );
                    }

                    auto remove_(const STFRecord* rec) {
                        return std::remove_if(data_.begin(), data_.end(), [rec](const STFRecord::UniqueHandle& item) { return item.get() == rec; });
                    }

                public:
                    /**
                     * \typedef iterator
                     * iterator to the underlying vector
                     */
                    using iterator = vec_type::iterator;

                    /**
                     * \typedef const_iterator
                     * const_iterator to the underlying vector
                     */
                    using const_iterator = vec_type::const_iterator;

                    /**
                     * \typedef value_type
                     * value_type of underlying vector
                     */
                    using value_type = vec_type::value_type;

                    SmallVector() :
                        data_(1)
                    {
                    }

                    /**
                     * Copy constructor
                     * \param rhs SmallVector to copy
                     */
                    SmallVector(const SmallVector& rhs) {
                        copyFrom_(rhs);
                    }

                    /**
                     * Assignment operator
                     * \param rhs SmallVector to copy
                     */
                    SmallVector& operator=(const SmallVector& rhs) {
                        clear();
                        copyFrom_(rhs);
                        return *this;
                    }

                    /**
                     * Move constructor
                     */
                    SmallVector(SmallVector&&) = default;

                    /**
                     * Move assignment operator
                     */
                    SmallVector& operator=(SmallVector&&) = default;

                    /**
                     * Gets an iterator to the beginning of the vector
                     */
                    inline iterator begin() {
                        if(STF_EXPECT_TRUE(data_.size() == 1)) {
                            if(!data_.front()) {
                                return data_.end();
                            }
                        }
                        return data_.begin();
                    }

                    /**
                     * Gets a const_iterator to the beginning of the vector
                     */
                    inline const_iterator begin() const {
                        if(STF_EXPECT_TRUE(data_.size() == 1)) {
                            if(!data_.front()) {
                                return data_.end();
                            }
                        }
                        return data_.begin();
                    }

                    /**
                     * Gets a const_iterator to the beginning of the vector
                     */
                    inline const_iterator cbegin() const {
                        if(STF_EXPECT_TRUE(data_.size() == 1)) {
                            if(!data_.front()) {
                                return data_.cend();
                            }
                        }
                        return data_.cbegin();
                    }

                    /**
                     * Gets an iterator to the end of the vector
                     */
                    inline iterator end() {
                        return data_.end();
                    }

                    /**
                     * Gets a const_iterator to the end of the vector
                     */
                    inline const_iterator end() const {
                        return data_.end();
                    }

                    /**
                     * Gets a const_iterator to the end of the vector
                     */
                    inline const_iterator cend() const {
                        return data_.cend();
                    }

                    /**
                     * Clears the vector
                     */
                    inline void clear() {
                        if(STF_EXPECT_TRUE(!data_.empty())) {
                            // Instead of really clearing the vector, just reduce it to one invalid entry
                            data_.resize(1);
                            data_.front().reset();
                        }
                    }

                    /**
                     * Appends a new element to the back of the vector
                     * \param rec New record to append
                     */
                    inline const STFRecord* emplace_back(STFRecord::UniqueHandle&& rec) {
                        // Replace the dummy entry if we have one
                        if(STF_EXPECT_TRUE(data_.size() == 1)) {
                            if(auto& entry = data_.front(); STF_EXPECT_TRUE(!entry)) {
                                entry = std::move(rec);
                                return entry.get();
                            }
                        }

                        return data_.emplace_back(std::move(rec)).get();
                    }

                    /**
                     * Appends a new element to the back of the vector
                     * \param rec New record to append
                     */
                    inline void push_back(STFRecord::UniqueHandle&& rec) {
                        emplace_back(std::move(rec));
                    }

                    /**
                     * Removes the element at the given index from the vector
                     * \param idx Index of the element that should be removed
                     */
                    inline void remove(const size_t idx) {
                        data_.erase(std::next(data_.begin(), static_cast<ssize_t>(idx)));
                    }

                    /**
                     * Removes the given record from the vector
                     * \param rec Record to be removed
                     */
                    inline void remove(const STFRecord* rec) {
                        const auto it = remove_(rec);
                        data_.erase(it, data_.end());
                    }

                    /**
                     * Extracts the element at the given index from the vector
                     * \param idx Index of the element that should be extracted
                     */
                    STFRecord::UniqueHandle extract(const size_t idx) {
                        STFRecord::UniqueHandle temp(std::move(data_[idx]));
                        remove(idx);
                        return temp;
                    }

                    /**
                     * Extracts the given record from the vector
                     * \param rec Record to be extracted
                     */
                    STFRecord::UniqueHandle extract(const STFRecord* rec) {
                        auto it = remove_(rec);
                        STFRecord::UniqueHandle temp(std::move(*it));
                        data_.erase(it, data_.end());
                        return temp;
                    }

                    /**
                     * Gets the index of the given record
                     * \param rec Record to query
                     */
                    size_t getIndex(const STFRecord* rec) const {
                        const auto it = std::find_if(data_.begin(), data_.end(), [rec](const STFRecord::UniqueHandle& item) { return item.get() == rec; });
                        stf_assert(it != data_.end(), "Record does not belong to this array");
                        return static_cast<size_t>(std::distance(data_.begin(), it));
                    }

                    /**
                     * Returns whether the vector is empty
                     */
                    inline bool empty() const {
                        return size() == 0;
                    }

                    /**
                     * Returns the size of the vector
                     */
                    inline size_t size() const {
                        const auto actual_size = data_.size();
                        if(STF_EXPECT_TRUE(actual_size == 1)) {
                            if(!data_.front()) {
                                return 0;
                            }
                        }

                        return actual_size;
                    }

                    /**
                     * Gets the element at the specified index
                     * \param idx Index of the desired element
                     */
                    inline const STFRecord::UniqueHandle& at(const size_t idx) const {
                        return data_.at(idx);
                    }

                    /**
                     * Gets the element at the specified index
                     * \param idx Index of the desired element
                     */
                    inline STFRecord::UniqueHandle& operator[](const size_t idx) {
                        return data_[idx];
                    }

                    /**
                     * Gets the element at the specified index
                     * \param idx Index of the desired element
                     */
                    inline const STFRecord::UniqueHandle& operator[](const size_t idx) const {
                        return data_[idx];
                    }
            };

        private:
            /**
             * \class ArrayMap
             *
             * Map-like structure implemented as a Boost static_vector of small_vectors for speed
             */
            class ArrayMap {
                public:
                    using value_type = std::pair<descriptors::internal::Descriptor, SmallVector>;

                private:
                    class StaticVector {
                        private:
                            size_t size_ = 0;
                            using ArrayType = enums::EnumArray<value_type, descriptors::internal::Descriptor>;
                            ArrayType arr_;

                        public:
                            using iterator = ArrayType::iterator;
                            using const_iterator = ArrayType::const_iterator;

                            StaticVector() {
                                for(key_type i = 0; i < arr_.size(); ++i) {
                                    arr_[i].first = static_cast<descriptors::internal::Descriptor>(i);
                                }
                            }

                            StaticVector(const StaticVector&) = default;
                            StaticVector(StaticVector&&) = default;
                            StaticVector& operator=(const StaticVector&) = default;
                            StaticVector& operator=(StaticVector&&) = default;

                            inline bool empty() const {
                                return size_ == 0;
                            }

                            inline auto size() const {
                                return size_;
                            }

                            inline auto nth(const key_type key) const {
                                return arr_.begin() + key;
                            }

                            inline auto begin() {
                                return arr_.begin();
                            }

                            inline auto end() {
                                return arr_.begin() + size_;
                            }

                            inline auto begin() const {
                                return arr_.begin();
                            }

                            inline auto end() const {
                                return arr_.begin() + size_;
                            }

                            inline auto cbegin() const {
                                return arr_.begin();
                            }

                            inline auto cend() const {
                                return arr_.begin() + size_;
                            }

                            inline void clear() {
                                for(size_t i = 0; i < size_; ++i) {
                                    arr_[i].second.clear();
                                }
                                size_ = 0;
                            }

                            inline auto& operator[](const key_type key) {
                                size_ = std::max(size_, static_cast<size_t>(key + 1));
                                return arr_[key];
                            }

                            inline const auto& operator[](const key_type key) const {
                                return arr_[key];
                            }

                            inline auto& at(const key_type key) {
                                stf_assert(key < size_, "Attempted to access invalid index");
                                return arr_[key];
                            }

                            inline const auto& at(const key_type key) const {
                                stf_assert(key < size_, "Attempted to access invalid index");
                                return arr_[key];
                            }
                    };

                    StaticVector vec_array_;

                public:
                    using const_iterator = StaticVector::const_iterator;

                    inline auto find(const key_type key) const {
                        if(vec_array_.size() <= key) {
                            return vec_array_.end();
                        }
                        return vec_array_.nth(key);
                    }

                    inline auto find(const descriptors::internal::Descriptor key) const {
                        return find(static_cast<key_type>(key));
                    }

                    inline const STFRecord* emplace(STFRecord::UniqueHandle&& rec) {
                        return vec_array_[static_cast<key_type>(rec->getId())].second.emplace_back(std::move(rec));
                    }

                    void remove(const Index& index) {
                        vec_array_[index.getKey()].second.remove(index.getIndex());
                    }

                    void remove(const STFRecord* rec) {
                        vec_array_[static_cast<key_type>(rec->getId())].second.remove(rec);
                    }

                    STFRecord::UniqueHandle extract(const Index& index) {
                        return vec_array_[index.getKey()].second.extract(index.getIndex());
                    }

                    STFRecord::UniqueHandle extract(const STFRecord* rec) {
                        return vec_array_[static_cast<key_type>(rec->getId())].second.extract(rec);
                    }

                    inline auto& operator[](const key_type key) {
                        return vec_array_[key].second;
                    }

                    inline auto& operator[](const descriptors::internal::Descriptor key) {
                        return operator[](static_cast<key_type>(key));
                    }

                    inline auto& at(const key_type key) {
                        return vec_array_.at(key).second;
                    }

                    inline const auto& at(const key_type key) const {
                        return vec_array_.at(key).second;
                    }

                    inline auto begin() {
                        return vec_array_.begin();
                    }

                    inline auto end() {
                        return vec_array_.end();
                    }

                    inline auto begin() const {
                        return vec_array_.begin();
                    }

                    inline auto end() const {
                        return vec_array_.end();
                    }

                    inline auto cbegin() const {
                        return vec_array_.cbegin();
                    }

                    inline auto cend() const {
                        return vec_array_.cend();
                    }

                    inline void clear() {
                        vec_array_.clear();
                    }

                    inline bool empty() const {
                        return vec_array_.empty();
                    }

                    inline Index getIndex(const STFRecord* rec) const {
                        const auto key = static_cast<key_type>(rec->getId());
                        return Index(key, vec_array_[key].second.getIndex(rec));
                    }
            };

        private:
            using MapType = ArrayMap;
            mutable MapType map_;
            size_t size_ = 0;

            inline size_t count_(const key_type key) const {
                const auto it = map_.find(key);
                if(it == map_.end()) {
                    return 0;
                }
                return it->second.size();
            }

        public:
            /**
             * \typedef const_iterator
             *
             * Map const iterator type
             */
            using const_iterator = MapType::const_iterator;

            /**
             * \class sorted_const_iterator
             *
             * Iterates over the map in the encoded::Descriptor order
             */
            class sorted_const_iterator : public std::forward_iterator_tag {
                private:
                    const MapType* const map_ = nullptr; /**< Map that is to be iterated over */

                    /**
                     * \typedef internal_it
                     * The iterator type used to generate pre-sorted indices into the map
                     */
                    using internal_it = descriptors::iterators::sorted_internal_iterator;
                    internal_it key_it_; /**< Iterator that will provide pre-sorted indices into the map */

                    inline auto find_() const {
                        return map_->find(*key_it_);
                    }

                    inline bool shouldSkip_() const {
                        return find_() == map_->end();
                    }

                public:
                    /**
                     * \typedef value_type
                     * Type pointed to by this iterator
                     */
                    using value_type = MapType::value_type;

                    /**
                     * \typedef reference
                     * Reference to type pointed to by this iterator
                     */
                    using reference = const MapType::value_type&;

                    /**
                     * Constructs a sorted_const_iterator
                     * If the map is empty or nullptr, this will construct an end iterator
                     * \param map Map that is to be iterated
                     */
                    explicit sorted_const_iterator(const MapType* map = nullptr) :
                        sorted_const_iterator(map, !map || map->empty())
                    {
                    }

                    /**
                     * Dereferences the iterator
                     */
                    const auto& operator*() {
                        stf_assert(map_, "Attempted to dereference an invalid RecordMap::sorted_const_iterator");
                        return *find_();
                    }

                    /**
                     * Increments the iterator. It will automatically skip keys that do not appear in the map.
                     */
                    auto& operator++() {
                        stf_assert(map_, "Attempted to increment an invalid RecordMap::sorted_const_iterator");

                        do {
                            ++key_it_;
                        }
                        while(shouldSkip_() && key_it_ != descriptors::iterators::SORTED_INTERNAL_ITERATOR_END);

                        return *this;
                    }

                    /**
                     * Post-fix increment operator
                     */
                    auto operator++(int) {
                        auto copy = *this;
                        ++(*this);
                        return copy;
                    }

                    /**
                     * Dereferences the iterator
                     */
                    auto& operator->() const {
                        return key_it_;
                    }

                    /**
                     * Equality operator
                     */
                    constexpr bool operator==(const sorted_const_iterator& rhs) {
                        return key_it_ == rhs.key_it_;
                    }

                    /**
                     * Inequality operator
                     */
                    constexpr bool operator!=(const sorted_const_iterator& rhs) {
                        return key_it_ != rhs.key_it_;
                    }

                private:
                    sorted_const_iterator(const MapType* map, bool is_end) :
                        map_(map),
                        key_it_(internal_it(is_end))
                    {
                        if(!is_end) {
                            // skip ahead to the first valid entry
                            if(shouldSkip_()) {
                                operator++();
                            }
                        }
                    }
            };

            /**
             * \typedef const_rec_iterator
             *
             * Vector const iterator type
             */
            using const_rec_iterator = SmallVector::const_iterator;

            /**
             * Emplaces a record in the map, returning a reference to the record
             * \param rec Record to emplace in the map
             */
            inline const STFRecord* emplace(STFRecord::UniqueHandle&& rec) {
                ++size_;
                return map_.emplace(std::forward<STFRecord::UniqueHandle>(rec));
            }

            /**
             * Gets the vector of records for the specified STF descriptor
             * \param desc STF descriptor
             */
            inline const SmallVector& at(const descriptors::internal::Descriptor desc) const {
                return at(enums::to_int(desc));
            }

            /**
             * Gets the vector of records for the specified key
             * \param key Key
             */
            inline const SmallVector& at(const key_type key) const {
                return map_[key];
            }

            /**
             * Gets the number of records stored in the map
             */
            inline size_t size() const {
                return size_;
            }

            /**
             * Gets a const iterator to the beginning of the map
             */
            inline const_iterator begin() const {
                return map_.begin();
            }

            /**
             * Gets a const iterator to the end of the map
             */
            inline const_iterator end() const {
                return map_.end();
            }

            /**
             * Gets a sorted const iterator to the beginning of the map
             */
            inline sorted_const_iterator sorted_begin() const {
                return sorted_const_iterator(&map_);
            }

            /**
             * Gets a sorted const iterator to the end of the map
             */
            static inline sorted_const_iterator sorted_end() {
                return sorted_const_iterator();
            }

            /**
             * Gets a const iterator to the beginning of the vector for the specified STF descriptor
             * \param desc STF descriptor to look up
             */
            inline const_rec_iterator begin(const descriptors::internal::Descriptor desc) const {
                return at(desc).begin();
            }

            /**
             * Gets a const iterator to the end of the vector for the specified STF descriptor
             * \param desc STF descriptor to look up
             */
            inline const_rec_iterator end(const descriptors::internal::Descriptor desc) const {
                return at(desc).end();
            }

            /**
             * Gets a const iterator to the beginning of the vector for the specified STF descriptor
             * \param desc STF descriptor to look up
             */
            inline const_rec_iterator cbegin(const descriptors::internal::Descriptor desc) const {
                return at(desc).cbegin();
            }

            /**
             * Gets a const iterator to the end of the vector for the specified STF descriptor
             * \param desc STF descriptor to look up
             */
            inline const_rec_iterator cend(const descriptors::internal::Descriptor desc) const {
                return at(desc).cend();
            }

            /**
             * Gets the number of records with the specified STF descriptor
             * \param desc STF descriptor to look up
             */
            inline size_t count(const descriptors::internal::Descriptor desc) const {
                return count_(enums::to_int(desc));
            }

            /**
             * Clears the map
             */
            inline void clear() {
                size_ = 0;
                map_.clear();
            }

            /**
             * Removes the element at the given index from the map
             * \param index Index of the element that should be removed
             */
            inline void remove(const Index& index) {
                map_.remove(index);
            }

            /**
             * Removes the given record from the map
             * \param rec Record to be removed
             */
            inline void remove(const STFRecord* rec) {
                map_.remove(rec);
            }

            /**
             * Extracts the element at the given index from the map
             * \param index Index of the element that should be extracted
             */
            inline STFRecord::UniqueHandle extract(const Index& index) {
                return map_.extract(index);
            }

            /**
             * Extracts the given record from the map
             * \param rec Record to be extracted
             */
            inline STFRecord::UniqueHandle extract(const STFRecord* rec) {
                return map_.extract(rec);
            }

            /**
             * Gets the index of the given record within the map
             * \param rec Record to be queried
             */
            inline Index getIndex(const STFRecord* rec) const {
                return map_.getIndex(rec);
            }

            /**
             * Returns a SortedRecordMapIterable view of this object
             *
             * This allows us to efficiently iterate over the records in the order expected by the STF spec
             */
            SortedRecordMapIterable sorted() const;
    };

    /**
     * \class SortedRecordMapIterable
     *
     * Class that provides a sorted interface to a RecordMap, suitable for use with range-based for loops
     */
    class SortedRecordMapIterable {
        private:
            const RecordMap* map_; /**< Map we are iterating over */

        public:
            /**
             * Constructs a SortedRecordMapIterable from a RecordMap
             * \param map RecordMap to iterate over
             */
            explicit SortedRecordMapIterable(const RecordMap* map) :
                map_(map)
            {
            }

            /**
             * Gets an iterator to the sorted beginning of the map
             */
            inline auto begin() const {
                return map_->sorted_begin();
            }

            /**
             * Gets an iterator to the sorted end of the map
             */
            static inline auto end() {
                return RecordMap::sorted_end();
            }
    };

    inline SortedRecordMapIterable RecordMap::sorted() const {
        return SortedRecordMapIterable(this);
    }

} // end namespace stf

#endif
