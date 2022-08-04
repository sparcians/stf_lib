#ifndef __STF_PACKED_CONTAINER_HPP__
#define __STF_PACKED_CONTAINER_HPP__

#include <utility>

#include "stf_exception.hpp"
#include "util.hpp"

namespace stf {
    /**
     * \class PackedContainer
     *
     * Recursive template class that creates a trivially-copyable packed class of arbitrary types
     */
    template<typename T, typename ... Ts>
    class __attribute__ ((packed)) PackedContainer {
        static_assert(type_utils::are_trivially_copyable_v<T, Ts...>,
                      "All types held in a PackedContainer must be trivially copyable!");

        private:
            T data_;
            PackedContainer<Ts...> rest_of_data_;

        public:
            PackedContainer() = default;

            /**
             * Constructs a PackedContainer
             * \param arg Data stored in data_ member
             * \param args Data stored recursively in rest_of_data_ member
             */
            explicit PackedContainer(const T& arg, const Ts&... args) :
                data_(arg),
                rest_of_data_(std::forward<const Ts>(args)...)
            {
            }

            /**
             * Unpacks data from a PackedContainer
             * \param arg Data unpacked from data_ member
             * \param args Data unpacked recursively from rest_of_data_ member
             */
            __attribute__((always_inline))
            inline void unpack(T& arg, Ts&... args) const {
                arg = data_;
                rest_of_data_.unpack(args...);
            }
    };

    template<typename T>
    class __attribute__ ((packed)) PackedContainer<T> /**< Specialized definition for the single-member variant */ {
        static_assert(std::is_trivially_copyable_v<T>,
                      "All types held in a PackedContainer must be trivially copyable!");

        private:
            T data_;

        public:
            PackedContainer() = default;

            /**
             * Constructs a PackedContainer
             * \param arg Data stored in data_ member
             */
            explicit PackedContainer(const T& arg) :
                data_(arg)
            {
            }

            /**
             * Unpacks data from a PackedContainer
             * \param arg Data unpacked from data_ member
             */
            __attribute__((always_inline))
            inline void unpack(T& arg) const {
                arg = data_;
            }
    };

    /**
     * \class PackedContainerViewBase
     *
     * Base class for PackedContainerView
     */
    class PackedContainerViewBase {
        public:
            virtual inline ~PackedContainerViewBase() = default;

            /**
             * Sets the view to point to an unmanaged pointer
             * \param src_ptr Pointer to unmanaged block of memory
             */
            virtual void setView(void* src_ptr) = 0;

            /**
             * Allocates a managed block of memory in the view
             */
            virtual void allocateView() = 0;

            /**
             * Gets a raw pointer to the underlying memory
             */
            virtual void* get() = 0;

            /**
             * Gets a raw const pointer to the underlying memory
             */
            virtual const void* get() const = 0;
    };

    /**
     * \class PackedContainerView
     *
     * Provides a pointer-based view into a PackedContainer.
     *
     * The pointer can be to a heap-allocated PackedContainer managed by this class, or to an
     * arbitrary block of memory managed by a different class.
     *
     * Allows unpacking STFRecords from decompression buffers without an intermediate copy operation.
     */
    template<typename ... Ts>
    class PackedContainerView final : public PackedContainerViewBase {
        private:
            using container_type = PackedContainer<Ts...>;
            container_type* container_ptr_ = nullptr;
            bool owns_memory_ = false;

            inline void reset_() {
                if(owns_memory_) {
                    delete container_ptr_;
                    owns_memory_ = false;
                }
            }

        public:
            inline ~PackedContainerView() final {
                reset_();
            }

            inline void setView(void* src_ptr) final {
                reset_();
                container_ptr_ = reinterpret_cast<container_type*>(src_ptr);
            }

            inline void allocateView() final {
                // Don't need to reallocate if we're already allocated
                if(!owns_memory_) {
                    reset_();
                    container_ptr_ = new container_type();
                    owns_memory_ = true;
                }
            }

            inline void* get() final {
                return container_ptr_;
            }

            inline const void* get() const final {
                return container_ptr_;
            }

            /**
             * Unpacks data from the underlying memory
             * \param args Data to be unpacked
             */
            __attribute__((always_inline))
            inline void unpack(Ts&... args) const {
                stf_assert(container_ptr_, "Attempted to unpack from an invalid PackedContainerView");
                container_ptr_->unpack(args...);
            }

            /**
             * Gets the size of the PackedContainer pointed to by this view
             */
            static inline constexpr size_t size() {
                return sizeof(container_type);
            }
    };

} // end namespace stf

#endif
