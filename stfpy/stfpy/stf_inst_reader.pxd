# distutils: language = c++

from cython.operator cimport dereference
from stfpy.stf_lib.stf_inst_reader cimport STFInstReader as _STFInstReader
from stfpy.stf_lib.stf_inst_reader cimport StringVector as _StringVector, StringVectorIterator as _StringVectorIterator

ctypedef _STFInstReader.iterator _STFInstReaderIterator

cdef class StringVectorIterator:
    cdef _StringVectorIterator c_it
    cdef _StringVectorIterator c_end_it

    @staticmethod
    cdef inline StringVectorIterator _construct(const _StringVector* vec):
        it = StringVectorIterator()
        it.c_it = dereference(vec).begin()
        it.c_end_it = dereference(vec).end()
        return it

cdef class StringVector:
    cdef const _StringVector* c_vec

    @staticmethod
    cdef inline StringVector _construct(const _StringVector& vec):
        new_vec = StringVector()
        new_vec.c_vec = &vec
        return new_vec

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
