# distutils: language = c++

from stfpy.stf_lib.stf_branch cimport STFBranch

cdef extern from "stf_branch_reader.hpp" namespace "stf":
    cdef cppclass STFBranchReader:
        cppclass iterator:
            iterator()
            iterator(STFBranchReader*, bint)
            const STFBranch& operator*()
            iterator& operator++()
            bint operator==(const iterator&)
            bint operator!=(const iterator&)

        STFBranchReader STFBranchReader[StrType](StrType, bint, size_t, bint) except +

        void close()
        iterator begin()
        iterator end()
