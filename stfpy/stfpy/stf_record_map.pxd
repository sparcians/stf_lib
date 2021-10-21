# distutils: language = c++

from stfpy.stf_lib.stf_record_map cimport RecordMap

cdef class STFRecordMapSmallVector:
    cdef const RecordMap.SmallVector* c_vec

    @staticmethod
    cdef inline STFRecordMapSmallVector _construct(const RecordMap.SmallVector& vec):
        new_vec = STFRecordMapSmallVector()
        new_vec.c_vec = &vec
        return new_vec

cdef class STFRecordMap:
    cdef const RecordMap* c_map

    @staticmethod
    cdef inline STFRecordMap _construct(const RecordMap& map):
        new_map = STFRecordMap()
        new_map.c_map = &map
        return new_map

