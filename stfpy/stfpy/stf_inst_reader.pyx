# distutils: language = c++
# cython: c_string_type=unicode, c_string_encoding=utf8

from libcpp.string cimport string
from stfpy.stf_inst cimport STFInst
from stfpy.stf_inst import STFInst
from cython.operator cimport dereference, preincrement
include "stfpy/stf_lib/stf_reader_constants.pxi"

cdef class STFInstReaderIterator:
    def __next__(self):
        preincrement(self.c_it)
        if self.c_it != self.c_end_it:
            return STFInst._construct(dereference(self.c_it))
        else:
            raise StopIteration

cdef class STFInstReader:
    def __cinit__(self,
                  string filename,
                  bint only_user_mode = False,
                  bint enable_address_translation = False,
                  bint filter_mode_change_events = False,
                  size_t buffer_size = __DEFAULT_BUFFER_SIZE,
                  bint force_single_threaded_stream = False):
        self.c_reader = new _STFInstReader(filename,
                                          only_user_mode,
                                          enable_address_translation,
                                          filter_mode_change_events,
                                          buffer_size,
                                          force_single_threaded_stream)

    def __dealloc__(self):
        del self.c_reader

    def __iter__(self):
        return STFInstReaderIterator._construct(self.c_reader)

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def close(self):
        dereference(self.c_reader).close()
