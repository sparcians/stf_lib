# distutils: language = c++

from stfpy.stf_lib.stf_inst_reader cimport STFInstReader as _STFInstReader

ctypedef _STFInstReader.iterator _STFInstReaderIterator

cdef class STFInstReaderIterator:
    cdef _STFInstReaderIterator c_it
    cdef _STFInstReaderIterator c_end_it

    @staticmethod
    cdef inline STFInstReaderIterator _construct(_STFInstReader* rdr):
        it = STFInstReaderIterator()
        it.c_it = _STFInstReaderIterator(rdr, False)
        it.c_end_it = _STFInstReaderIterator(rdr, True)
        return it

cdef class STFInstReader:
    cdef _STFInstReader* c_reader
