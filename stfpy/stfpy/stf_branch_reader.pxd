# distutils: language = c++

from stfpy.stf_lib.stf_branch_reader cimport STFBranchReader as _STFBranchReader

ctypedef _STFBranchReader.iterator _STFBranchReaderIterator

cdef class STFBranchReaderIterator:
    cdef _STFBranchReaderIterator c_it
    cdef _STFBranchReaderIterator c_end_it

    @staticmethod
    cdef inline STFBranchReaderIterator _construct(_STFBranchReader* rdr):
        it = STFBranchReaderIterator()
        it.c_it = _STFBranchReaderIterator(rdr, False)
        it.c_end_it = _STFBranchReaderIterator(rdr, True)
        return it

cdef class STFBranchReader:
    cdef _STFBranchReader* c_reader
