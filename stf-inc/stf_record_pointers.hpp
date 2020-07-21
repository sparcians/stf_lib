#ifndef __STF_RECORD_POINTERS_HPP__
#define __STF_RECORD_POINTERS_HPP__

#include <iostream>
#include <memory>
#include <vector>

#include "stf_descriptor.hpp"
#include "stf_exception.hpp"
#include "stf_record_pool.hpp"

namespace stf {
    class STFRecord;

    /**
     * \typedef UniqueRecordHandle
     *
     * Unique pointer to an STFRecord (or derived class) allocated by STFRecordPool
     */
    template<typename RecordType>
    using UniqueRecordHandle = std::unique_ptr<RecordType, RecordDeleter<STFRecordPool>>;

    /**
     * \typedef ConstUniqueRecordHandle
     *
     * Unique pointer to a const STFRecord (or derived class) allocated by STFRecordPool
     */
    template<typename RecordType>
    using ConstUniqueRecordHandle = UniqueRecordHandle<const RecordType>;

    /**
     * \typedef STFRecordUniqueHandle
     *
     * Unique pointer to an STFRecord allcoated by STFRecordPool
     */
    using STFRecordUniqueHandle = UniqueRecordHandle<STFRecord>;

    /**
     * \typedef STFRecordConstUniqueHandle
     *
     * Unique pointer to a const STFRecord allcoated by STFRecordPool
     */
    using STFRecordConstUniqueHandle = ConstUniqueRecordHandle<STFRecord>;

    namespace pointer_utils {
        template<typename BaseT, typename T>
        inline ConstUniqueRecordHandle<T> grabOwnership(ConstUniqueRecordHandle<BaseT>&& base_ptr) {
            const auto deleter = base_ptr.get_deleter();
            return ConstUniqueRecordHandle<T>(static_cast<const T*>(base_ptr.release()), deleter);
        }

        template<typename BaseT, typename T>
        inline UniqueRecordHandle<T> grabOwnership(UniqueRecordHandle<BaseT>&& base_ptr) {
            const auto deleter = base_ptr.get_deleter();
            return UniqueRecordHandle<T>(static_cast<T*>(base_ptr.release()), deleter);
        }

        template<typename BaseT, typename T>
        inline ConstUniqueRecordHandle<T> grabOwnership(ConstUniqueRecordHandle<BaseT>& base_ptr) {
            const auto deleter = base_ptr.get_deleter();
            return ConstUniqueRecordHandle<T>(static_cast<const T*>(base_ptr.release()), deleter);
        }

        template<typename BaseT, typename T>
        inline UniqueRecordHandle<T> grabOwnership(UniqueRecordHandle<BaseT>& base_ptr) {
            const auto deleter = base_ptr.get_deleter();
            return UniqueRecordHandle<T>(static_cast<T*>(base_ptr.release()), deleter);
        }

        template<typename BaseT, typename T>
        inline void grabOwnership(ConstUniqueRecordHandle<T>& new_ptr, ConstUniqueRecordHandle<BaseT>& base_ptr) {
            new_ptr = std::move(grabOwnership<BaseT, T>(base_ptr));
        }

        template<typename BaseT, typename T>
        inline void grabOwnership(UniqueRecordHandle<T>& new_ptr, UniqueRecordHandle<BaseT>& base_ptr) {
            new_ptr = std::move(grabOwnership<BaseT, T>(base_ptr));
        }
    } // end namespace pointer_utils
} // end namespace stf

#endif
