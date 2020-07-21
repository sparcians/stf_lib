#ifndef __STF_RECORD_POOL_HPP__
#define __STF_RECORD_POOL_HPP__

#include <array>
#include <memory>

#include "boost_pool.hpp"
#define BOOST_POOL_NO_MT
#pragma push_macro("S1")
#undef S1
#include <boost/container/static_vector.hpp>
#pragma pop_macro("S1")
#undef BOOST_POOL_NO_MT
#include "util.hpp"

namespace stf {
    class STFIFstream;

    template<typename T>
    class TypeAwareSTFRecord;

    /**
     * \struct RecordDeleter
     * Functor struct used to delete records allocated by STFRecordPool
     */
    template<typename PoolType>
    struct RecordDeleter {
        /**
         * Deletes an STFRecord
         * \param rec STFRecord to delete
         */
        __attribute__((always_inline))
        inline void operator()(const STFRecord* rec) {
            PoolType::deleter(rec);
        }
    };

    /**
     * \class STFRecordPool
     *
     * Pool allocator for STFRecords and derived classes
     *
     * NOT thread-safe
     */
    class STFRecordPool {
        private:
            template<typename RecordType>
            class STFRecordCache {
                private:
                    using FuncType = void(*)(const RecordType*);

                    static constexpr size_t MAX_SIZE_ = 3072;
#ifdef DEBUG_CACHE
                    size_t num_insertions_ = 0;
                    size_t num_hits_ = 0;
                    mutable size_t num_misses_ = 0;
#endif
                    std::array<FuncType, descriptors::internal::NUM_DESCRIPTORS> deleters_;

                    std::array<boost::container::static_vector<RecordType*, MAX_SIZE_>,
                               descriptors::internal::NUM_DESCRIPTORS> cache_;

                    bool deleters_initialized_ = false;

                    ~STFRecordCache() {
#ifdef DEBUG_CACHE
                        std::cerr << "Insertions: " << num_insertions_ << std::endl
                                  << "Hits: " << num_hits_ << std::endl
                                  << "Misses: " << num_misses_ << std::endl;
#endif
                        for(size_t i = 0; i < cache_.size(); ++i) {
                            const auto deleter = getDeleter_(i);
#ifdef DEBUG_CACHE
                            auto& cache_entry = cache_[i];
                            std::cerr << "Cache entry: " << descriptors::internal::Descriptor(i) << std::endl
                                      << "Cache size: " << cache_entry.size() << std::endl;
                            for(auto& rec: cache_entry) {
#else
                            for(auto& rec: cache_[i]) {
#endif
                                deleter(rec);
                            }
                        }
                    }

                    __attribute__((always_inline))
                    inline auto getDeleter_(const size_t desc) const {
                        return deleters_[desc];
                    }

                    __attribute__((always_inline))
                    inline auto getDeleter_(const descriptors::internal::Descriptor desc) const {
                        return getDeleter_(static_cast<size_t>(desc));
                    }

                    __attribute__((always_inline))
                    inline auto getDeleter_(const RecordType* rec) const {
                        return getDeleter_(rec->getDescriptor());
                    }

                public:
                    __attribute__((always_inline))
                    inline static STFRecordCache& get() {
                        static STFRecordCache cache;
                        return cache;
                    }

                    __attribute__((always_inline))
                    inline bool add(const RecordType* rec) {
                        auto& cache_entry = cache_[static_cast<size_t>(rec->getDescriptor())];
#ifdef DEBUG_CACHE
                        ++num_insertions_;
#endif
                        if(STF_EXPECT_FALSE(cache_entry.size() == MAX_SIZE_)) {
#ifdef DEBUG_CACHE
                            ++num_misses_;
#endif
                            return false;
                        }
                        cache_entry.emplace_back(const_cast<RecordType*>(rec));
                        return true;
                    }

                    __attribute__((always_inline))
                    inline RecordType* pop(const descriptors::internal::Descriptor desc) {
                        auto& cache_entry = cache_[static_cast<size_t>(desc)];
                        if(STF_EXPECT_FALSE(cache_entry.empty())) {
                            return nullptr;
                        }

#ifdef DEBUG_CACHE
                        ++num_hits_;
#endif
                        const auto ptr = cache_entry.back();
                        cache_entry.pop_back();
                        return ptr;
                    }

                    __attribute__((always_inline))
                    inline void deleter(const RecordType* rec) {
                        getDeleter_(rec->getDescriptor())(rec);
                    }

                    __attribute__((always_inline))
                    inline void registerDeleter(const descriptors::internal::Descriptor desc, const FuncType func) {
                        deleters_[static_cast<size_t>(desc)] = func;
                    }
            };

            /**
             * \typedef PoolAllocator
             *
             * Pool allocator used for STFRecords and derived classes
             */
            template<typename RecordType>
            using PoolAllocator = boost::fast_pool_allocator<RecordType>;

            /**
             * Gets the pool allocator for the specified record type
             */
            template<typename RecordType>
            __attribute__((always_inline))
            static inline PoolAllocator<RecordType>& getPool_() {
                static thread_local PoolAllocator<RecordType> pool;
                return pool;
            }

            /**
             * Constructs a record of the specified type, returning it as another type
             * \param args Arguments to pass to record constructor
             */
            template<typename ReturnType, typename RecordType, typename ... Args>
            __attribute__((always_inline))
            static inline typename std::enable_if<sizeof...(Args) != 1 || !type_utils::are_same<STFIFstream, std::remove_reference_t<Args>...>::value, ReturnType>::type
            construct_(Args&&... args) {
                auto& pool = getPool_<RecordType>();
                auto ptr = pool.allocate();
                stf_assert(ptr, "Failed to allocate new record from pool");
                pool.construct(ptr, std::forward<Args>(args)...);
                return ReturnType(ptr);
            }

            /**
             * Constructs a record of the specified type, returning it as another type
             * \param args Arguments to pass to record constructor
             */
            template<typename ReturnType, typename RecordType, typename ... Args>
            __attribute__((always_inline))
            static inline typename std::enable_if<sizeof...(Args) == 1 && type_utils::are_same<STFIFstream, std::remove_reference_t<Args>...>::value, ReturnType>::type
            construct_(Args&&... args) {
                auto ptr = STFRecordCache<STFRecord>::get().pop(TypeAwareSTFRecord<RecordType>::getTypeDescriptor());
                if(STF_EXPECT_FALSE(!ptr)) {
                    auto& pool = getPool_<RecordType>();
                    auto new_ptr = pool.allocate();
                    stf_assert(new_ptr, "Failed to allocate new record from pool");
                    pool.construct(new_ptr, std::forward<Args>(args)...);
                    ptr = new_ptr;
                }
                else {
                    static_cast<RecordType*>(ptr)->unpack_impl(std::forward<Args>(args)...);
                }
                return ReturnType(ptr);
            }

            template<typename RecordType>
            __attribute__((always_inline))
            static inline void deleter_(const STFRecord* rec) {
                const auto ptr = const_cast<RecordType*>(static_cast<const RecordType*>(rec));
                auto& pool = getPool_<RecordType>();
                pool.destroy(ptr);
                pool.deallocate(ptr);
            }

        public:
            /**
             * Type-specific deleter function
             * \param rec Record to delete
             */
            __attribute__((always_inline))
            static inline void deleter(const STFRecord* rec) {
                if(STF_EXPECT_FALSE(!rec)) {
                    return;
                }

                if(auto& cache = STFRecordCache<STFRecord>::get(); STF_EXPECT_FALSE(!cache.add(rec))) {
                    cache.deleter(rec);
                }
            }

            /**
             * Constructs a record of the specified type, returning it as an STFRecordConstUniqueHandle
             * \param args Arguments to pass to record constructor
             */
            template<typename RecordType, typename ... Args>
            __attribute__((always_inline))
            static inline std::unique_ptr<const STFRecord, RecordDeleter<STFRecordPool>> construct(Args&&... args) {
                return construct_<std::unique_ptr<const STFRecord, RecordDeleter<STFRecordPool>>, RecordType, Args...>(args...);
            }

            /**
             * Constructs a record of the specified type, returning it as a UniqueRecordHandle
             * \param args Arguments to pass to record constructor
             */
            template<typename RecordType, typename ... Args>
            __attribute__((always_inline))
            static inline std::unique_ptr<RecordType, RecordDeleter<STFRecordPool>> make(Args&&... args) {
                return construct_<std::unique_ptr<RecordType, RecordDeleter<STFRecordPool>>, RecordType, Args...>(args...);
            }

            /**
             * Registers a deleter method in the STFRecordCache for a particular record type
             * \param desc Descriptor for the record type being registered
             */
            template<typename RecordType>
            __attribute__((always_inline))
            static inline void registerDeleter(const descriptors::internal::Descriptor desc) {
                STFRecordCache<STFRecord>::get().registerDeleter(desc, &STFRecordPool::deleter_<RecordType>);
            }
    };

} // end namespace stf

#endif
