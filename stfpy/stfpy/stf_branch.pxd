# distutils: language = c++

from stfpy.stf_lib.stf_branch cimport STFBranch as _STFBranch

cdef class STFBranch:
    cdef const _STFBranch* c_branch

    @staticmethod
    cdef inline STFBranch _construct(const _STFBranch& branch):
        new_branch = STFBranch()
        new_branch.c_branch = &branch
        return new_branch
