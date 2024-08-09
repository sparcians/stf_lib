# distutils: language = c++

from libc.stdint cimport *
from libcpp.vector cimport vector
from libcpp.string cimport string
from stfpy.stf_lib.stf_inst cimport STFInst

ctypedef vector[string] HeaderCommentsType
ctypedef vector[string].const_iterator HeaderCommentsTypeIterator

cdef extern from "stf_inst_reader.hpp" namespace "stf":
    cdef cppclass STFInstReader:
        cppclass iterator:
            iterator()
            iterator(STFInstReader*, bint)
            const STFInst& operator*()
            iterator& operator++()
            bint operator==(const iterator&)
            bint operator!=(const iterator&)

        STFInstReader STFInstReader[StrType](StrType, bint, bint, bint, size_t, bint) except +

        void close()
        iterator begin()
        iterator end()
        uint32_t major()
        uint32_t minor()
        const HeaderCommentsType& getHeaderCommentsString()
