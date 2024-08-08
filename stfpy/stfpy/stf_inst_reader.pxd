# distutils: language = c++

from cython.operator cimport dereference
from stfpy.stf_lib.stf_inst_reader cimport STFInstReader as _STFInstReader
from stfpy.stf_lib.stf_inst_reader cimport HeaderCommentsType as _HeaderCommentsType, HeaderCommentsTypeIterator as _HeaderCommentsTypeIterator

ctypedef _STFInstReader.iterator _STFInstReaderIterator

cdef class HeaderCommentsTypeIterator:
    cdef _HeaderCommentsTypeIterator c_it
    cdef _HeaderCommentsTypeIterator c_end_it

    @staticmethod
    cdef inline HeaderCommentsTypeIterator _construct(const _HeaderCommentsType* vec):
        it = HeaderCommentsTypeIterator()
        it.c_it = dereference(vec).begin()
        it.c_end_it = dereference(vec).end()
        return it

cdef class HeaderCommentsType:
    cdef const _HeaderCommentsType* c_vec

    @staticmethod
    cdef inline HeaderCommentsType _construct(const _HeaderCommentsType& vec):
        new_vec = HeaderCommentsType()
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
