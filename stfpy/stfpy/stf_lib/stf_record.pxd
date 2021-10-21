# distutils: language = c++

from libc.stdint cimport *
from libcpp.memory cimport unique_ptr

cdef extern from "stf_record.hpp" namespace "stf":
    cdef cppclass RecordDeleter[PoolType]:
        pass
    cdef cppclass STFRecord "stf::STFRecord":
        pass

cdef extern from "stf_record_pool.hpp" namespace "stf":
    cdef cppclass STFRecordPool "stf::STFRecordPool":
        pass

cdef extern from "stf_record_pointers.hpp" namespace "stf":
    ctypedef unique_ptr[const STFRecord, RecordDeleter[STFRecordPool]] STFRecordConstUniqueHandle
