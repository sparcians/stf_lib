#ifndef __STF_RECORD_MAP_HPP__
#define __STF_RECORD_MAP_HPP__

#include <deque>
#include <iterator>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

#include "boost_pool.hpp"

#define BOOST_POOL_NO_MT
#include <boost/container/small_vector.hpp>
#undef BOOST_POOL_NO_MT

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
             * \typedef VecType
             * Vector type used as values in the underlying map
             */
            using VecType = boost::container::small_vector<STFRecord::UniqueHandle,
                                                           DEFAULT_VEC_SIZE_>;
        private:
            /**
             * \class ArrayMap
             *
             * Map-like structure implemented as a Boost static_vector of small_vectors for speed
             */
            class ArrayMap {
                public:
                    using value_type = std::pair<descriptors::internal::Descriptor, VecType>;

                private:
                    class StaticVector {
                        private:
                            size_t size_ = 0;
                            using ArrayType = std::array<value_type, descriptors::internal::NUM_DESCRIPTORS>;
                            ArrayType arr_;

                        public:
                            using iterator = ArrayType::iterator;
                            using const_iterator = ArrayType::const_iterator;

                            StaticVector() {
                                for(key_type i = 0; i < arr_.size(); ++i) {
                                    arr_[i].first = static_cast<descriptors::internal::Descriptor>(i);
                                }
                            }

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
                                    auto& vec = arr_[i].second;
                                    if(STF_EXPECT_TRUE(!vec.empty())) {
                                        vec.resize(1);
                                        vec.front().reset();
                                    }
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
                        auto& vec = vec_array_[static_cast<key_type>(rec->getDescriptor())].second;
                        if(STF_EXPECT_TRUE(vec.size() == 1)) {
                            if(auto& entry = vec.front(); STF_EXPECT_TRUE(!entry)) {
                                entry = std::move(rec);
                                return entry.get();
                            }
                        }
                        return vec.emplace_back(std::move(rec)).get();
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

                const auto& vec = it->second;
                const auto vec_size = vec.size();
                if(vec_size == 1 && !vec.front()) {
                    return 0;
                }

                return vec_size;
            }

        public:
            /**
             * Default constructor
             */
            RecordMap() = default;

            /**
             * Copy constructor
             * \param rhs RecordMap that is being copied
             */
            RecordMap(const RecordMap& rhs) {
                for(const auto& p: rhs.map_) {
                    auto& vec = map_[p.first];
                    for(const auto& r: p.second) {
                        vec.emplace_back(r->clone());
                    }
                }
            }

            /**
             * Move constructor
             */
            RecordMap(RecordMap&&) = default;

            /**
             * Copy assignment operator
             * \param rhs RecordMap that is being copied
             */
            RecordMap& operator=(const RecordMap& rhs) {
                for(const auto& p: rhs.map_) {
                    auto& vec = map_[p.first];
                    for(const auto& r: p.second) {
                        vec.emplace_back(r->clone());
                    }
                }

                return *this;
            }

            /**
             * Move assignment operator
             */
            RecordMap& operator=(RecordMap&&) = default;

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
            using const_rec_iterator = VecType::const_iterator;

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
            inline const VecType& at(const descriptors::internal::Descriptor desc) const {
                return at(enums::to_int(desc));
            }

            /**
             * Gets the vector of records for the specified key
             * \param key Key
             */
            inline const VecType& at(const key_type key) const {
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
