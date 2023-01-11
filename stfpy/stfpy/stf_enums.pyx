from stfpy.stf_lib.stf_enums cimport *

cpdef enum INST_MEM_ACCESS:
    INVALID = <uint8_t> _INVALID
    READ = <uint8_t> _READ
    WRITE = <uint8_t> _WRITE
