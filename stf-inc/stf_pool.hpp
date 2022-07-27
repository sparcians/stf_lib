#ifndef __STF_POOL_HPP__
#define __STF_POOL_HPP__

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

#include "stf_object.hpp"

namespace stf {
    class STFIFstream;

    /**
     * \struct PoolDeleter
     * Functor struct used to delete objects allocated by STFPool
     */
    template<typename BaseObjectType, typename PoolType>
    struct PoolDeleter {
        /**
         * Deletes an Object
         * \param obj Object to delete
         */
        __attribute__((always_inline))
        inline void operator()(const BaseObjectType* obj) {
            PoolType::deleter(obj);
        }
    };

    /**
     * \class STFPool
     *
     * Pool allocator for generic objects and derived classes
     *
     * NOT thread-safe
     */
    template<typename BaseObjectType, typename Enum, size_t NUM_OBJECTS>
    class STFPool {
        private:
            template<typename ObjectType>
            class STFObjectCache {
                private:
                    using FuncType = void(*)(const ObjectType*);

                    static constexpr size_t MAX_SIZE_ = 3072;

#ifdef DEBUG_CACHE
                    size_t num_insertions_ = 0;
                    size_t num_hits_ = 0;
                    mutable size_t num_misses_ = 0;
#endif
                    std::array<FuncType, NUM_OBJECTS> deleters_;

                    std::array<boost::container::static_vector<ObjectType*, MAX_SIZE_>, NUM_OBJECTS> cache_;

                    bool deleters_initialized_ = false;

                    ~STFObjectCache() {
#ifdef DEBUG_CACHE
                        std::cerr << "Insertions: " << num_insertions_ << std::endl
                                  << "Hits: " << num_hits_ << std::endl
                                  << "Misses: " << num_misses_ << std::endl;
#endif
                        for(size_t i = 0; i < cache_.size(); ++i) {
                            const auto deleter = getDeleter_(i);
#ifdef DEBUG_CACHE
                            auto& cache_entry = cache_[i];
                            std::cerr << "Cache entry: " << Enum(i) << std::endl
                                      << "Cache size: " << cache_entry.size() << std::endl;
                            for(auto& obj: cache_entry) {
#else
                            for(auto& obj: cache_[i]) {
#endif
                                deleter(obj);
                            }
                        }
                    }

                    __attribute__((always_inline))
                    inline auto getDeleter_(const size_t object_id) const {
                        return deleters_[object_id];
                    }

                    __attribute__((always_inline))
                    inline auto getDeleter_(const Enum object_id) const {
                        return getDeleter_(static_cast<size_t>(object_id));
                    }

                    __attribute__((always_inline))
                    inline auto getDeleter_(const ObjectType* obj) const {
                        return getDeleter_(obj->getId());
                    }

                public:
                    __attribute__((always_inline))
                    inline static STFObjectCache& get() {
                        static STFObjectCache cache;
                        return cache;
                    }

                    __attribute__((always_inline))
                    inline bool add(const ObjectType* obj) {
                        auto& cache_entry = cache_[static_cast<size_t>(obj->getId())];
#ifdef DEBUG_CACHE
                        ++num_insertions_;
#endif
                        if(STF_EXPECT_FALSE(cache_entry.size() == MAX_SIZE_)) {
#ifdef DEBUG_CACHE
                            ++num_misses_;
#endif
                            return false;
                        }
                        cache_entry.emplace_back(const_cast<ObjectType*>(obj));
                        return true;
                    }

                    __attribute__((always_inline))
                    inline ObjectType* pop(const Enum object_id) {
                        auto& cache_entry = cache_[static_cast<size_t>(object_id)];
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
                    inline void deleter(const ObjectType* obj) {
                        getDeleter_(obj)(obj);
                    }

                    __attribute__((always_inline))
                    inline void registerDeleter(const Enum object_id, const FuncType func) {
                        deleters_[static_cast<size_t>(object_id)] = func;
                    }
            };

            /**
             * \typedef PoolAllocator
             *
             * Pool allocator used for generic objects and derived classes
             */
            template<typename ObjectType>
            using PoolAllocator = boost::fast_pool_allocator<ObjectType>;

            /**
             * Gets the pool allocator for the specified record type
             */
            template<typename ObjectType>
            __attribute__((always_inline))
            static inline PoolAllocator<ObjectType>& getPool_() {
                static thread_local PoolAllocator<ObjectType> pool;
                return pool;
            }

            /**
             * Constructs a record of the specified type, returning it as another type
             * \param args Arguments to pass to record constructor
             */
            template<typename ReturnType, typename ObjectType, typename ... Args>
            __attribute__((always_inline))
            static inline typename std::enable_if<sizeof...(Args) != 1 || !type_utils::are_same<STFIFstream, std::remove_reference_t<Args>...>::value, ReturnType>::type
            construct_(Args&&... args) {
                auto& pool = getPool_<ObjectType>();
                auto ptr = pool.allocate();
                stf_assert(ptr, "Failed to allocate new object from pool");
                pool.construct(ptr, std::forward<Args>(args)...);
                return ReturnType(ptr);
            }

            /**
             * Constructs a record of the specified type, returning it as another type
             * \param args Arguments to pass to record constructor
             */
            template<typename ReturnType, typename ObjectType, typename ... Args>
            __attribute__((always_inline))
            static inline typename std::enable_if<sizeof...(Args) == 1 && type_utils::are_same<STFIFstream, std::remove_reference_t<Args>...>::value, ReturnType>::type
            construct_(Args&&... args) {
                auto ptr = STFObjectCache<BaseObjectType>::get().pop(TypeAwareSTFObject<ObjectType, Enum>::getTypeId());
                if(STF_EXPECT_FALSE(!ptr)) {
                    auto& pool = getPool_<ObjectType>();
                    auto new_ptr = pool.allocate();
                    stf_assert(new_ptr, "Failed to allocate new object from pool");
                    pool.construct(new_ptr, std::forward<Args>(args)...);
                    ptr = new_ptr;
                }
                else {
                    static_cast<ObjectType*>(ptr)->unpack_impl(std::forward<Args>(args)...);
                }
                return ReturnType(ptr);
            }

            template<typename ObjectType>
            __attribute__((always_inline))
            static inline void deleter_(const BaseObjectType* obj) {
                const auto ptr = const_cast<ObjectType*>(static_cast<const ObjectType*>(obj));
                auto& pool = getPool_<ObjectType>();
                pool.destroy(ptr);
                pool.deallocate(ptr);
            }

        public:
            /**
             * Type-specific deleter function
             * \param obj Object to delete
             */
            __attribute__((always_inline))
            static inline void deleter(const BaseObjectType* obj) {
                if(STF_EXPECT_FALSE(!obj)) {
                    return;
                }

                if(auto& cache = STFObjectCache<BaseObjectType>::get(); STF_EXPECT_FALSE(!cache.add(obj))) {
                    cache.deleter(obj);
                }
            }

            /**
             * Constructs a record of the specified type, returning it as an STFRecordConstUniqueHandle
             * \param args Arguments to pass to record constructor
             */
            template<typename ObjectType, typename ... Args>
            __attribute__((always_inline))
            static inline std::unique_ptr<const BaseObjectType, PoolDeleter<BaseObjectType, STFPool>> construct(Args&&... args) {
                return construct_<std::unique_ptr<const BaseObjectType, PoolDeleter<BaseObjectType, STFPool>>, ObjectType, Args...>(args...);
            }

            /**
             * Constructs a record of the specified type, returning it as a UniqueRecordHandle
             * \param args Arguments to pass to record constructor
             */
            template<typename ObjectType, typename ... Args>
            __attribute__((always_inline))
            static inline std::unique_ptr<ObjectType, PoolDeleter<BaseObjectType, STFPool>> make(Args&&... args) {
                return construct_<std::unique_ptr<ObjectType, PoolDeleter<BaseObjectType, STFPool>>, ObjectType, Args...>(args...);
            }

            /**
             * Registers a deleter method in the STFRecordCache for a particular record type
             * \param object_id Descriptor for the record type being registered
             */
            template<typename ObjectType>
            __attribute__((always_inline))
            static inline void registerDeleter(const Enum object_id) {
                STFObjectCache<BaseObjectType>::get().registerDeleter(object_id, &STFPool::deleter_<ObjectType>);
            }
    };

} // end namespace stf

#endif
