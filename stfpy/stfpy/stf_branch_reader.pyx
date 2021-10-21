# distutils: language = c++
# cython: c_string_type=unicode, c_string_encoding=utf8

from libcpp.string cimport string
from stfpy.stf_branch cimport STFBranch
from stfpy.stf_branch import STFBranch
from cython.operator cimport dereference, preincrement
include "stfpy/stf_lib/stf_reader_constants.pxi"

cdef class STFBranchReaderIterator:
    def __next__(self):
        preincrement(self.c_it)
        if self.c_it != self.c_end_it:
            return STFBranch._construct(dereference(self.c_it))
        else:
            raise StopIteration

cdef class STFBranchReader:
    def __cinit__(self,
                  string filename,
                  bint only_user_mode = False,
                  size_t buffer_size = __DEFAULT_BUFFER_SIZE,
                  bint force_single_threaded_stream = False):
        self.c_reader = new _STFBranchReader(filename,
                                             only_user_mode,
                                             buffer_size,
                                             force_single_threaded_stream)

    def __dealloc__(self):
        del self.c_reader

    def __iter__(self):
        return STFBranchReaderIterator._construct(self.c_reader)

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def close(self):
        dereference(self.c_reader).close()
