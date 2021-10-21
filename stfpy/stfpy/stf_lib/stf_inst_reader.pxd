# distutils: language = c++

from stfpy.stf_lib.stf_inst cimport STFInst

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
