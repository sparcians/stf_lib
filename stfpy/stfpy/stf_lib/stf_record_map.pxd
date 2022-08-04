# distutils: language = c++

from libc.stdint cimport *
from libcpp.pair cimport pair
from stfpy.stf_lib.boost_small_vector cimport small_vector
from stfpy.stf_lib.stf_record cimport STFRecordUniqueHandle
from stfpy.stf_lib.stf_descriptor cimport _InternalDescriptor

cdef extern from "<array>" namespace "std":
    cdef cppclass array[T, N]:
        ctypedef const T& const_reference
        cppclass const_iterator:
            const_reference operator*()
            const_iterator& operator++()
            bint operator==()
            bint operator!=()


cdef extern from "stf_record_map.hpp" namespace "stf":
    cdef cppclass RecordMap:
        cppclass SmallVector:
            ctypedef size_t _DEFAULT_VEC_SIZE "1"
            ctypedef small_vector[STFRecordUniqueHandle, _DEFAULT_VEC_SIZE].const_iterator const_iterator
            const_iterator begin()
            const_iterator end()
            bint empty()
            size_t size()
            const STFRecordUniqueHandle& at()
            const STFRecordUniqueHandle& operator[]()

        cppclass ArrayMap:
            ctypedef pair[_InternalDescriptor, SmallVector] value_type
            cppclass StaticVector:
                ctypedef size_t _ARRAY_TYPE_SIZE "stf::descriptors::internal::RESERVED_END"
                ctypedef array[value_type, _ARRAY_TYPE_SIZE].const_iterator const_iterator
            ctypedef StaticVector.const_iterator const_iterator

        ctypedef ArrayMap.const_iterator const_iterator
        cppclass sorted_const_iterator:
            ctypedef ArrayMap.value_type value_type
            ctypedef const value_type& reference
            reference operator*()
            sorted_const_iterator& operator++()
            bint operator==(const sorted_const_iterator&)
            bint operator!=(const sorted_const_iterator&)

        ctypedef SmallVector.const_iterator const_rec_iterator
        ctypedef uint8_t key_type
        const SmallVector& at(_InternalDescriptor)
        const SmallVector& at(key_type)
        size_t size()
        const_iterator begin()
        const_iterator end()
        sorted_const_iterator sorted_begin()
        sorted_const_iterator sorted_end()
        const_rec_iterator begin(_InternalDescriptor)
        const_rec_iterator end(_InternalDescriptor)
        size_t count(_InternalDescriptor)
