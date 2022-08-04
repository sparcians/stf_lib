#ifndef __STF_COMPRESSION_BUFFER_HPP__
#define __STF_COMPRESSION_BUFFER_HPP__

#include <cstdint>
#include <memory>

#include "stf_exception.hpp"
#include "util.hpp"

namespace stf {
    /**
     * \class STFCompressionBufferBase
     * Base class for compression buffers. Contains read/write pointers, size, and pointer to data.
     * Does not perform any memory management.
     */
    template<typename PointerType>
    class STFCompressionBufferBase {
        protected:
            PointerType buf_; /**< The underlying buffer. Subclasses may choose to manage this memory themselves or not. */
            size_t buf_actual_size_ = 0; /**< Actual allocated size of the buffer. Allows for fast resizes if the memory is already allocated. */
            size_t buf_size_ = 0; /**< Apparent size of the buffer. May not match the allocated size. */
            size_t buf_write_ptr_ = 0; /**< Pointer to the next byte that can be written in this buffer. */
            size_t buf_read_ptr_ = 0; /**< Pointer to the next byte that can be read from this buffer. */


            STFCompressionBufferBase() = default;

            /**
             * Constructs an STFCompressionBufferBase, initializing the buffer with the given pointer.
             * \param ptr Buffer to use
             */
            explicit STFCompressionBufferBase(const PointerType ptr) :
                buf_(ptr)
            {
            }

            /**
             * Constructs an STFCompressionBufferBase, initializing the buffer with the given pointer and size.
             * \param ptr Buffer to use
             * \param size Size of the buffer
             */
            STFCompressionBufferBase(PointerType&& ptr, const size_t size) :
                buf_(std::move(ptr)),
                buf_actual_size_(size),
                buf_size_(size)
            {
            }

            /**
             * Default move constructor
             */
            STFCompressionBufferBase(STFCompressionBufferBase&&) noexcept = default;

            /**
             * Default move assignment operator
             */
            STFCompressionBufferBase& operator=(STFCompressionBufferBase&&) noexcept = default;

            /**
             * Default destructor
             */
            inline virtual ~STFCompressionBufferBase() = default;

        public:
            /**
             * Gets the underlying raw pointer managed by an std::unique_ptr
             */
            template<typename T = PointerType>
            std::enable_if_t<pointer_utils::is_unique_ptr_v<T>, typename std::pointer_traits<T>::element_type*>
            get() const {
                return buf_.get();
            }

            /**
             * Gets the buffer if it is already a raw pointer
             */
            template<typename T = PointerType>
            std::enable_if_t<std::is_pointer_v<T>, T>
            get() const {
                return buf_;
            }

            /**
             * Gets the buffer if it is already a raw pointer
             */
            template<typename T = PointerType>
            std::enable_if_t<std::is_pointer_v<T>, const T>
            get() const {
                return buf_;
            }

            /**
             * Reinterprets the underlying buffer as the specified type
             */
            template<typename T>
            const T* getPtrAs() const {
                return reinterpret_cast<const T*>(get());
            }

            /**
             * Reinterprets the underlying buffer as the specified type
             */
            template<typename T, typename U = PointerType>
            std::enable_if_t<std::negation_v<std::is_const<typename std::remove_pointer_t<U>>>, T*>
            getPtrAs() {
                return reinterpret_cast<T*>(get());
            }

            /**
             * Gets the apparent size of the buffer
             */
            size_t size() const {
                return buf_size_;
            }

            /**
             * Gets the actual size of the buffer
             */
            size_t getActualSize() const {
                return buf_actual_size_;
            }

            /**
             * Gets the end of the written data
             */
            size_t end() const {
                return buf_write_ptr_;
            }

            /**
             * Indicate that all of the data has been read from the pointer - resets the read pointer to the beginning.
             */
            void consume() {
                buf_read_ptr_ = buf_size_;
            }

            /**
             * Sets the read pointer to the specified value.
             * \param ptr Value to set the read pointer to
             */
            void setReadPtr(const size_t ptr) {
                buf_read_ptr_ = ptr;
            }

            /**
             * Advances the read pointer by the specified value.
             * \param num Amount to advance the read pointer
             */
            void advanceReadPtr(const size_t num) {
                buf_read_ptr_ += num;
            }

            /**
             * Gets the position of the read pointer
             */
            size_t getReadPos() const {
                return buf_read_ptr_;
            }

            /**
             * Sets the write pointer to the specified value.
             * \param ptr Value to set the write pointer to
             */
            void setWritePtr(const size_t ptr) {
                buf_write_ptr_ = ptr;
            }

            /**
             * Advances the write pointer by the specified value.
             * \param num Amount to advance the write pointer
             */
            void advanceWritePtr(const size_t num) {
                buf_write_ptr_ += num;
            }

            /**
             * Resets both read and write pointers
             */
            void reset() {
                buf_write_ptr_ = 0;
                buf_read_ptr_ = 0;
            }

            /**
             * Returns true if all of the data has been read
             */
            bool consumed() const {
                return buf_read_ptr_ == buf_size_;
            }

            /**
             * Returns true if no data has been written
             */
            bool empty() const {
                return buf_write_ptr_ == 0;
            }

            /**
             * Returns true if the buffer has been filled
             */
            bool full() const {
                return buf_write_ptr_ == buf_size_;
            }
    };

    /**
     * \class STFCompressionBuffer
     * Compression buffer class that manages its own memory
     */
    class STFCompressionBuffer : public STFCompressionBufferBase<std::unique_ptr<uint8_t[]>> { // NOLINT: modernize-avoid-c-arrays
        protected:
            size_t allocation_granule_ = 0; /**< Allocation granule - when growing the size of this buffer with fit, the actual allocated size will increase by a multiple of this */

            /**
             * Resizes the buffer, reallocating and copying into a new buffer if necessary.
             * \param size New apparent size
             * \param actual_size New allocated size
             */
            void resize_(const size_t size, const size_t actual_size) {
                buf_size_ = size;
                if(actual_size > buf_actual_size_) {
                    try {
                        auto new_buf = new uint8_t[actual_size];
                        if(buf_ && buf_write_ptr_) {
                            std::copy(buf_.get(), buf_.get() + buf_write_ptr_, new_buf);
                        }
                        buf_.reset(new_buf);
                        buf_actual_size_ = actual_size;
                    }
                    catch(const std::bad_alloc&) {
                        stf_throw("Failed to allocate " << actual_size << " bytes");
                    }
                }
            }

            /**
             * Gets the next physical size to store the desired new size
             * Can be overridden for e.g. exponential instead of linear growth
             * \param new_size Desired size to store
             */
            inline virtual size_t getNextSize_(const size_t new_size) {
                return allocation_granule_ * (1 + ((new_size - 1) / allocation_granule_));
            }

        public:
            STFCompressionBuffer() = default;

            /**
             * Constructs an STFCompressionBuffer with the specified size
             * \param size Size of the buffer in bytes
             */
            explicit STFCompressionBuffer(const size_t size) :
                STFCompressionBufferBase(std::make_unique<uint8_t[]>(size), size), // NOLINT: modernize-avoid-c-arrays
                allocation_granule_(size)
            {
            }

            /**
             * Resizes the buffer, reallocating and copying into a new buffer if necessary.
             * \param size New size
             */
            void resize(const size_t size) {
                resize_(size, size);
            }

            /**
             * Ensures that the buffer will fit the given size, while only growing by a multiple of the current size
             * \note Used to ensure that output buffers will always be some multiple of the FS block size
             * \param size Size to fit
             */
            void fit(const size_t size) {
                stf_assert(allocation_granule_, "Allocation granule must be set before calling fit()");
                const size_t new_size = size + buf_write_ptr_;
                // We need to actually grow the buffer
                if(new_size > buf_actual_size_) {
                    // Move up to the next multiple of the current size
                    resize_(new_size, getNextSize_(new_size));
                }
                // We need to grow or shrink without an allocation
                else if(new_size != buf_size_) {
                    resize(new_size);
                }
            }

            /**
             * Write into the buffer, automatically moving the write pointer
             * \param data Buffer to write from
             * \param size Size of an element in the buffer
             * \param num Number of elements in the buffer
             */
            void write(const void* data, const size_t size, const size_t num) {
                const auto u8_ptr = reinterpret_cast<const uint8_t*>(data);
                const size_t num_bytes = size * num;
                // Make sure we can fit the new data
                fit(num_bytes);
                // Write into the buffer so we can compress a whole record at a time
                std::copy(u8_ptr, u8_ptr + num_bytes, buf_.get() + buf_write_ptr_);
                buf_write_ptr_ += num_bytes;
            }

            /**
             * Sets the allocation granule used by fit()
             * \param granule Allocation granule to set
             */
            void setAllocationGranule(const size_t granule) {
                allocation_granule_ = granule;
            }

            /**
             * Initializes the buffer to fit the given size according to the given granule
             * \param size Size to fit
             * \param granule Allocation granule to use
             */
            void initSize(const size_t size, const size_t granule) {
                setAllocationGranule(granule);
                fit(size);
            }

            /**
             * Initializes the buffer to fit the given size. Allocation granule is also set to match size.
             * \param size Size to fit
             */
            void initSize(const size_t size) {
                initSize(size, size);
            }
    };

    /**
     * \class STFExponentialCompressionBuffer
     * Subclass of STFCompressionBuffer that grows its size exponentially instead of linearly
     * Useful for compression-side buffers that need to grow quickly
     */
    class STFExponentialCompressionBuffer : public STFCompressionBuffer {
        private:
            /**
             * Gets the next physical size to store the desired new size
             * Uses exponential growth
             * \param new_size Desired size to store
             */
            inline size_t getNextSize_(const size_t new_size) final {
                if(STF_EXPECT_FALSE(buf_actual_size_ == 0)) {
                    return allocation_granule_;
                }

                size_t new_fit_size = buf_actual_size_ << 1;
                while(new_fit_size < new_size) {
                    new_fit_size <<= 1;
                }
                return new_fit_size;
            }
    };

    /**
     * \class STFCompressionPointerWrapper
     * Compression buffer class that wraps an existing pointer. Does not manage the underlying buffer's memory.
     */
    template<typename PointerType>
    class STFCompressionPointerWrapper : public STFCompressionBufferBase<PointerType> {
        private:
            using STFCompressionBufferBase<PointerType>::buf_;
            using STFCompressionBufferBase<PointerType>::buf_size_;
            using STFCompressionBufferBase<PointerType>::buf_actual_size_;
            using STFCompressionBufferBase<PointerType>::buf_write_ptr_;

        public:
            STFCompressionPointerWrapper() = default;

            /**
             * Constructs an STFCompressionPointerWrapper with the given pointer
             * \param ptr Pointer to wrap
             */
            explicit STFCompressionPointerWrapper(PointerType ptr) :
                STFCompressionBufferBase<PointerType>(ptr)
            {
            }

            /**
             * Sets the underlying buffer to the given pointer
             */
            template<typename T>
            void setPointer(T* ptr) {
                buf_ = reinterpret_cast<PointerType>(ptr);
            }

            /**
             * Sets the size to the given value
             * \attention This must be the same as the allocated size of the wrapped pointer!
             * \param size Size to set
             */
            void resize(const size_t size) {
                buf_size_ = size;
                buf_actual_size_ = size;
                buf_write_ptr_ = size;
            }

            /**
             * Write into the buffer, automatically moving the write pointer
             * \param data Buffer to write from
             * \param size Size of an element in the buffer
             * \param num Number of elements in the buffer
             */
            template<typename T = PointerType>
            std::enable_if_t<std::negation_v<std::is_const<typename std::remove_pointer_t<T>>>>
            write(const void* data, const size_t size, const size_t num) {
                const auto ptr = reinterpret_cast<const typename std::pointer_traits<PointerType>::element_type*>(data);
                const size_t num_bytes = size * num;
                // Assert if we exceed the allocated buffer size since we don't manage its memory
                stf_assert(buf_actual_size_ >= num_bytes,
                           "Attempted to write " << num_bytes << " into a pointer with size " << buf_actual_size_);
                // Write into the buffer so we can compress a whole record at a time
                std::copy(ptr, ptr + num_bytes, buf_ + buf_write_ptr_);
                buf_write_ptr_ += num_bytes;
            }
    };

} // end namespace stf

#endif
