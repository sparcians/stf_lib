#ifndef __STF_VECTOR_VIEW_HPP__
#define __STF_VECTOR_VIEW_HPP__

#include <vector>
#include "boost_small_vector.hpp"
#include "stf_exception.hpp"

namespace stf {
    /**
     * \class VectorView
     * Used to provide an offset view into a vector. Intended for deserializing sub-ranges of vectors.
     */
    template<typename T>
    class VectorView {
        private:
            T* data_;
            size_t size_;

        public:
            /**
             * Constructs a VectorView from an std::vector
             * \param src_vec Source vector
             * \param offset Offset into the vector
             */
            explicit VectorView(std::vector<T>& src_vec, const size_t offset = 0) :
                data_(src_vec.data() + offset),
                size_(src_vec.size() - offset)
            {
                stf_assert(offset < src_vec.size(),
                           "Attempted to specify an offset greater than the size of the vector");
            }

            /**
             * Constructs a VectorView from a boost::container::small_vector
             * \param src_vec Source vector
             * \param offset Offset into the vector
             */
            template<size_t N>
            explicit VectorView(boost::container::small_vector<T, N>& src_vec, const size_t offset = 0) :
                data_(src_vec.data() + offset),
                size_(src_vec.size() - offset)
            {
                stf_assert(offset < src_vec.size(),
                           "Attempted to specify an offset greater than the size of the vector");
            }

            /**
             * Gets the number of elements in the range referenced by this view
             */
            inline size_t size() const {
                return size_;
            }

            /**
             * Gets the pointer to the underlying data referenced by this view
             */
            inline T* data() {
                return data_;
            }
    };

    /**
     * \class ConstVectorView
     * Used to provide an offset view into a vector. Intended for serializing sub-ranges of vectors.
     */
    template<typename T>
    class ConstVectorView {
        private:
            const T* data_;
            size_t size_;

        public:
            /**
             * Constructs a ConstVectorView from an std::vector
             * \param src_vec Source vector
             * \param offset Offset into the vector
             */
            explicit ConstVectorView(const std::vector<T>& src_vec, const size_t offset = 0) :
                data_(src_vec.data() + offset),
                size_(src_vec.size() - offset)
            {
                stf_assert(offset < src_vec.size(),
                           "Attempted to specify an offset greater than the size of the vector");
            }

            /**
             * Constructs a ConstVectorView from a boost::container::small_vector
             * \param src_vec Source vector
             * \param offset Offset into the vector
             */
            template<size_t N>
            explicit ConstVectorView(const boost::container::small_vector<T, N>& src_vec, const size_t offset = 0) :
                data_(src_vec.data() + offset),
                size_(src_vec.size() - offset)
            {
                stf_assert(offset < src_vec.size(),
                           "Attempted to specify an offset greater than the size of the vector");
            }

            /**
             * Gets the number of elements in the range referenced by this view
             */
            inline size_t size() const {
                return size_;
            }

            /**
             * Gets the pointer to the underlying data referenced by this view
             */
            inline const T* data() const {
                return data_;
            }
    };
} // end namespace stf

#endif
