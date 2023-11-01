from libc.stdint cimport *

cdef extern from "stf_inst.hpp" namespace "stf":
    cdef enum _INST_MEM_ACCESS 'stf::_STF_ENUM_INST_MEM_ACCESS::INST_MEM_ACCESS':
        _INVALID 'stf::INST_MEM_ACCESS::INVALID'
        _READ 'stf::INST_MEM_ACCESS::READ'
        _WRITE 'stf::INST_MEM_ACCESS::WRITE'
