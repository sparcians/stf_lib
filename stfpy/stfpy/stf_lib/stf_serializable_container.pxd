# distutils: language = c++

from libc.stdint cimport *

cdef extern from "stf_serializable_container.hpp" namespace "stf":
    cdef cppclass SerializableVector[T, SerializedSizeT]:
        ctypedef const T* const_pointer
        ctypedef const T& const_reference
        cppclass const_iterator:
            const_reference operator*()
            const_iterator& operator++()
            bint operator==(const const_iterator&)
            bint operator!=(const const_iterator&)

        const_reference front()
        const_reference back()
        bint empty()
        size_t size()
        const_reference operator[](size_t)
        const_reference at(size_t)
        const_iterator begin()
        const_iterator end()
