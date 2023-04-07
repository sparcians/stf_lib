#ifndef __STF_POOL_HPP__
#define __STF_POOL_HPP__

#include <array>
#include <memory>

#ifdef DEBUG_CACHE
#include <iostream>
#endif

#include "boost_wrappers/pool.hpp"

//#define BOOST_POOL_NO_MT
#pragma push_macro("S1")
#undef S1
#include <boost/container/static_vector.hpp>
#pragma pop_macro("S1")
//#undef BOOST_POOL_NO_MT

#include "stf_enum_utils.hpp"
#include "type_utils.hpp"
#include "util.hpp"

namespace stf {
    class STFIFstream;

    /**
     * \class STFPool
     *
     * Thread-safe pool allocator for generic objects and derived classes
     */
    template<typename BaseObjectType, typename Enum>
    class STFPool {
        private:
            /**
             * \struct Deleter
             * Functor struct used to delete objects allocated by STFPool
             */
            struct Deleter {
                /**
                 * Deletes an Object
                 * \param obj Object to delete
                 */
                __attribute__((always_inline))
                inline void operator()(const BaseObjectType* obj) const {
                    STFPool::deleter(obj);
                }
            };

            using DeleterFuncType = void(*)(const BaseObjectType*);

            using DeleterArray = enums::EnumArray<DeleterFuncType, Enum>;

            /**
             * Default deleter for unregistered objects. Just throws an exception.
             */
            static inline void defaultDeleter_(const BaseObjectType*) {
                invalid_descriptor_throw("Attempted to delete unregistered object");
            }

            template<Enum ObjectId, typename dummy_type = void>
            struct deleter_generator {
                /**
                 * Translates an object ID to the deleter for that object
                 */
                static inline constexpr DeleterFuncType get();
            };

            template<typename dummy_type>
            struct deleter_generator<Enum::__RESERVED_START, dummy_type> {
                /**
                 * get specialization for __RESERVED_START object ID.
                 * Ensures we always throw an exception if we attempt to delete a __RESERVED_START ID.
                 */
                static inline constexpr DeleterFuncType get() {
                    return &defaultDeleter_;
                }
            };

            template<typename dummy_type>
            struct deleter_generator<Enum::__RESERVED_END, dummy_type> {
                /**
                 * get specialization for __RESERVED_END object ID.
                 * Ensures we always throw an exception if we attempt to delete a __RESERVED_END ID.
                 */
                static inline constexpr DeleterFuncType get() {
                    return &defaultDeleter_;
                }
            };

            /**
             * Used to initialize the deleters_ array at compile time.
             * Automatically iterates over all object IDs and adds their respective deleter functions to the array.
             */
            static inline constexpr DeleterArray populateDeleterArray_() {
                return enums::populateEnumArray<DeleterFuncType, Enum, &defaultDeleter_>(
                    [](auto Index, DeleterArray deleter_array) {
                        deleter_array[enums::to_int(Index())] = deleter_generator<Index>::get();
                        return deleter_array;
                    }
                );
            }

            template<typename T>
            using DeleterPointer = std::unique_ptr<T, Deleter>;

            static inline constexpr DeleterArray deleters_ = populateDeleterArray_();

            /**
             * Gets the deleter for the given object ID
             * \param object_id ID of object to delete
             */
            static DeleterFuncType getDeleter_(const size_t object_id);

            __attribute__((always_inline))
            static inline auto getDeleter_(const Enum object_id) {
                return getDeleter_(static_cast<size_t>(object_id));
            }

            __attribute__((always_inline))
            static inline auto getDeleter_(const BaseObjectType* obj) {
                return getDeleter_(obj->getId());
            }

        public:
            /**
             * \typedef base_type
             * Base class of objects constructed by this pool
             */
            using base_type = BaseObjectType;

            /**
             * \typedef id_type
             * Enum type used by this pool to identify objects
             */
            using id_type = Enum;

            /**
             * \typedef BaseObjectPointer
             * Pointer to a base class object
             */
            using BaseObjectPointer = DeleterPointer<BaseObjectType>;

            /**
             * \typedef ConstBaseObjectPointer
             * Pointer to a const base class object
             */
            using ConstBaseObjectPointer = DeleterPointer<const BaseObjectType>;

            /**
             * \typedef ObjectPointer
             * Pointer to a constructed object
             */
            template<typename ObjectType>
            using ObjectPointer = DeleterPointer<ObjectType>;

            /**
             * \typedef ConstObjectPointer
             * Pointer to a const constructed object
             */
            template<typename ObjectType>
            using ConstObjectPointer = ObjectPointer<const ObjectType>;

        private:
            template<typename ObjectType>
            class STFObjectCache {
                static_assert(std::is_base_of_v<BaseObjectType, ObjectType>,
                              "STFObjectCache type is not derived from the pool base class");

                private:
                    static inline constexpr size_t MAX_SIZE_ = 3072;

#ifdef DEBUG_CACHE
                    size_t num_insertions_ = 0;
                    size_t num_hits_ = 0;
                    mutable size_t num_misses_ = 0;
#endif
                    enums::EnumArray<boost::container::static_vector<ObjectType*, MAX_SIZE_>, Enum> cache_;

                    ~STFObjectCache() {
#ifdef DEBUG_CACHE
                        std::cerr << "Insertions: " << num_insertions_ << std::endl
                                  << "Hits: " << num_hits_ << std::endl
                                  << "Misses: " << num_misses_ << std::endl;
#endif
                        for(size_t i = 0; i < cache_.size(); ++i) {
                            const auto cur_deleter = getDeleter_(i);
#ifdef DEBUG_CACHE
                            auto& cache_entry = cache_[i];
                            std::cerr << "Cache entry: " << Enum(i) << std::endl
                                      << "Cache size: " << cache_entry.size() << std::endl;
                            for(auto& obj: cache_entry) {
#else
                            for(auto& obj: cache_[i]) {
#endif
                                cur_deleter(obj);
                            }
                        }
                    }

                public:
                    __attribute__((always_inline))
                    static inline STFObjectCache& get() {
                        static thread_local STFObjectCache cache;
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
            };

            /**
             * \typedef PoolAllocator
             *
             * Pool allocator used for generic objects and derived classes
             */
            template<typename ObjectType>
            using PoolAllocator = boost::fast_pool_allocator<ObjectType>;

            /**
             * Gets the pool allocator for the specified object type
             */
            template<typename ObjectType>
            __attribute__((always_inline))
            static inline PoolAllocator<ObjectType>& getPool_() {
                static_assert(std::is_base_of_v<BaseObjectType, ObjectType>,
                              "Pool allocated type is not derived from the pool base class");
                static thread_local PoolAllocator<ObjectType> pool;
                return pool;
            }

            /**
             * Constructs a object of the specified type, returning it as another type
             * \param args Arguments to pass to object constructor
             */
            template<typename ReturnType, typename ObjectType, typename ... Args>
            __attribute__((always_inline))
            static inline std::enable_if_t<std::negation_v<std::conjunction<type_utils::is_pack_size<1, Args...>, type_utils::are_same<STFIFstream, std::remove_reference_t<Args>...>>>, ReturnType>
            construct_(Args&&... args) {
                static_assert(std::is_base_of_v<BaseObjectType, ObjectType>,
                              "Pool allocated type is not derived from the pool base class");
                auto& pool = getPool_<ObjectType>();
                auto ptr = pool.allocate();
                stf_assert(ptr, "Failed to allocate new object from pool");
                pool.construct(ptr, std::forward<Args>(args)...);
                return ReturnType(ptr);
            }

            /**
             * Constructs a object of the specified type, returning it as another type
             * \param args Arguments to pass to object constructor
             */
            template<typename ReturnType, typename ObjectType, typename ... Args>
            __attribute__((always_inline))
            static inline std::enable_if_t<std::conjunction_v<type_utils::is_pack_size<1, Args...>,
                                                                       type_utils::are_same<STFIFstream, std::remove_reference_t<Args>...>>, ReturnType>
            construct_(Args&&... args) {
                static_assert(std::is_base_of_v<BaseObjectType, ObjectType>,
                              "Pool allocated type is not derived from the pool base class");
                auto ptr = STFObjectCache<BaseObjectType>::get().pop(ObjectType::getTypeId());
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
            static inline void deleter_func_(const BaseObjectType* obj) {
                static_assert(std::is_base_of_v<BaseObjectType, ObjectType>,
                              "Pool allocated type is not derived from the pool base class");
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
                    getDeleter_(obj)(obj);
                }
            }

            /**
             * Constructs a object of the specified type, returning it as a ConstBaseObjectPointer
             * \param args Arguments to pass to object constructor
             */
            template<typename ObjectType, typename ... Args>
            __attribute__((always_inline))
            static inline ConstBaseObjectPointer construct(Args&&... args) {
                static_assert(std::is_base_of_v<BaseObjectType, ObjectType>,
                              "Pool allocated type is not derived from the pool base class");
                return construct_<ConstBaseObjectPointer, ObjectType, Args...>(std::forward<Args>(args)...);
            }

            /**
             * Constructs a object of the specified type, returning it as an ObjectPointer
             * \param args Arguments to pass to object constructor
             */
            template<typename ObjectType, typename ... Args>
            __attribute__((always_inline))
            static inline ObjectPointer<ObjectType> make(Args&&... args) {
                static_assert(std::is_base_of_v<BaseObjectType, ObjectType>,
                              "Pool allocated type is not derived from the pool base class");
                return construct_<ObjectPointer<ObjectType>, ObjectType, Args...>(std::forward<Args>(args)...);
            }

            /**
             * Grabs ownership of the specified object, casting it to the desired type in the process
             * \param base_ptr Object to grab ownership of
             */
            template<typename ObjectType>
            static inline ConstObjectPointer<ObjectType> grabOwnership(ConstBaseObjectPointer&& base_ptr) {
                static_assert(std::is_base_of_v<BaseObjectType, ObjectType>,
                              "Converted type is not derived from the pool base class");
                const auto base_deleter = base_ptr.get_deleter();
                return ConstObjectPointer<ObjectType>(static_cast<const ObjectType*>(base_ptr.release()), base_deleter);
            }

            /**
             * Grabs ownership of the specified object, casting it to the desired type in the process
             * \param base_ptr Object to grab ownership of
             */
            template<typename ObjectType>
            static inline ObjectPointer<ObjectType> grabOwnership(BaseObjectPointer&& base_ptr) {
                static_assert(std::is_base_of_v<BaseObjectType, ObjectType>,
                              "Converted type is not derived from the pool base class");
                const auto base_deleter = base_ptr.get_deleter();
                return ObjectPointer<ObjectType>(static_cast<ObjectType*>(base_ptr.release()), base_deleter);
            }

            /**
             * Grabs ownership of the specified object, casting it to the desired type in the process
             * \param base_ptr Object to grab ownership of
             */
            template<typename ObjectType>
            static inline ConstObjectPointer<ObjectType> grabOwnership(ConstBaseObjectPointer& base_ptr) {
                static_assert(std::is_base_of_v<BaseObjectType, ObjectType>,
                              "Converted type is not derived from the pool base class");
                const auto base_deleter = base_ptr.get_deleter();
                return ConstObjectPointer<ObjectType>(static_cast<const ObjectType*>(base_ptr.release()), base_deleter);
            }

            /**
             * Grabs ownership of the specified object, casting it to the desired type in the process
             * \param base_ptr Object to grab ownership of
             */
            template<typename ObjectType>
            static inline ObjectPointer<ObjectType> grabOwnership(BaseObjectPointer& base_ptr) {
                static_assert(std::is_base_of_v<BaseObjectType, ObjectType>,
                              "Converted type is not derived from the pool base class");
                const auto base_deleter = base_ptr.get_deleter();
                return ObjectPointer<ObjectType>(static_cast<ObjectType*>(base_ptr.release()), base_deleter);
            }

            /**
             * Grabs ownership of the specified object, casting it to the desired type in the process
             * \param new_ptr Future owner of the object
             * \param base_ptr Object to grab ownership of
             */
            template<typename ObjectType>
            static inline void grabOwnership(ConstObjectPointer<ObjectType>& new_ptr, ConstBaseObjectPointer& base_ptr) {
                static_assert(std::is_base_of_v<BaseObjectType, ObjectType>,
                              "Converted type is not derived from the pool base class");
                new_ptr = std::move(grabOwnership<ObjectType>(base_ptr));
            }

            /**
             * Grabs ownership of the specified object, casting it to the desired type in the process
             * \param new_ptr Future owner of the object
             * \param base_ptr Object to grab ownership of
             */
            template<typename ObjectType>
            static inline void grabOwnership(ObjectPointer<ObjectType>& new_ptr, BaseObjectPointer& base_ptr) {
                static_assert(std::is_base_of_v<BaseObjectType, ObjectType>,
                              "Converted type is not derived from the pool base class");
                new_ptr = std::move(grabOwnership<ObjectType>(base_ptr));
            }
    };

} // end namespace stf

#endif
