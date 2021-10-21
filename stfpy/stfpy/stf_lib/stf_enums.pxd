from libc.stdint cimport *

cdef extern from "stf_inst.hpp" namespace "stf":
    cdef enum _INST_MEM_ACCESS 'stf::INST_MEM_ACCESS':
        _INVALID 'stf::INST_MEM_ACCESS::INVALID'
        _READ 'stf::INST_MEM_ACCESS::READ'
        _WRITE 'stf::INST_MEM_ACCESS::WRITE'

cpdef enum INST_MEM_ACCESS:
    INVALID = <uint8_t> _INVALID
    READ = <uint8_t> _READ
    WRITE = <uint8_t> _WRITE
