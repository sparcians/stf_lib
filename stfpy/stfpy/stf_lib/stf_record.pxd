# distutils: language = c++

from libc.stdint cimport *
from libcpp.memory cimport unique_ptr

cdef extern from "stf_record.hpp" namespace "stf":
    cdef cppclass STFRecord "stf::STFRecord":
        pass
    cdef cppclass STFRecordUniqueHandle "stf::STFRecord::UniqueHandle":
        pass
