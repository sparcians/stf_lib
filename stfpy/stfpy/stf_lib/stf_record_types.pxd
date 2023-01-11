# distutils: language = c++

from libc.stdint cimport *
from stfpy.stf_lib.boost_small_vector cimport small_vector

cdef extern from "stf_record_types.hpp":
    ctypedef size_t _VectorTypeSize "1"

ctypedef small_vector[uint64_t, _VectorTypeSize] VectorValueType
ctypedef small_vector[uint64_t, _VectorTypeSize].const_iterator VectorValueTypeIterator

cdef extern from "stf_record_types.hpp" namespace "stf":
    cdef cppclass InstRegRecord:
        ctypedef VectorValueType ValueType

cdef extern from "stf_record_types.hpp" namespace "stf::EventRecord":
    cdef enum _EVENT_RECORD_TYPE 'stf::EventRecord::TYPE':
        _INST_ADDR_MISALIGN 'stf::EventRecord::TYPE::INST_ADDR_MISALIGN'
        _INST_ADDR_FAULT 'stf::EventRecord::TYPE::INST_ADDR_FAULT'
        _ILLEGAL_INST 'stf::EventRecord::TYPE::ILLEGAL_INST'
        _BREAKPOINT 'stf::EventRecord::TYPE::BREAKPOINT'
        _LOAD_ADDR_MISALIGN 'stf::EventRecord::TYPE::LOAD_ADDR_MISALIGN'
        _LOAD_ACCESS_FAULT 'stf::EventRecord::TYPE::LOAD_ACCESS_FAULT'
        _STORE_ADDR_MISALIGN 'stf::EventRecord::TYPE::STORE_ADDR_MISALIGN'
        _STORE_ACCESS_FAULT 'stf::EventRecord::TYPE::STORE_ACCESS_FAULT'
        _USER_ECALL 'stf::EventRecord::TYPE::USER_ECALL'
        _SUPERVISOR_ECALL 'stf::EventRecord::TYPE::SUPERVISOR_ECALL'
        _HYPERVISOR_ECALL 'stf::EventRecord::TYPE::HYPERVISOR_ECALL'
        _MACHINE_ECALL 'stf::EventRecord::TYPE::MACHINE_ECALL'
        _INST_PAGE_FAULT 'stf::EventRecord::TYPE::INST_PAGE_FAULT'
        _LOAD_PAGE_FAULT 'stf::EventRecord::TYPE::LOAD_PAGE_FAULT'
        _STORE_PAGE_FAULT 'stf::EventRecord::TYPE::STORE_PAGE_FAULT'
        _GUEST_INST_PAGE_FAULT 'stf::EventRecord::TYPE::GUEST_INST_PAGE_FAULT'
        _GUEST_LOAD_PAGE_FAULT 'stf::EventRecord::TYPE::GUEST_LOAD_PAGE_FAULT'
        _VIRTUAL_INST 'stf::EventRecord::TYPE::VIRTUAL_INST'
        _GUEST_STORE_PAGE_FAULT 'stf::EventRecord::TYPE::GUEST_STORE_PAGE_FAULT'
        _INT_USER_SOFTWARE 'stf::EventRecord::TYPE::INT_USER_SOFTWARE'
        _INT_SUPERVISOR_SOFTWARE 'stf::EventRecord::TYPE::INT_SUPERVISOR_SOFTWARE'
        _INT_HYPERVISOR_SOFTWARE 'stf::EventRecord::TYPE::INT_HYPERVISOR_SOFTWARE'
        _INT_MACHINE_SOFTWARE 'stf::EventRecord::TYPE::INT_MACHINE_SOFTWARE'
        _INT_USER_TIMER 'stf::EventRecord::TYPE::INT_USER_TIMER'
        _INT_SUPERVISOR_TIMER 'stf::EventRecord::TYPE::INT_SUPERVISOR_TIMER'
        _INT_HYPERVISOR_TIMER 'stf::EventRecord::TYPE::INT_HYPERVISOR_TIMER'
        _INT_MACHINE_TIMER 'stf::EventRecord::TYPE::INT_MACHINE_TIMER'
        _INT_USER_EXT 'stf::EventRecord::TYPE::INT_USER_EXT'
        _INT_SUPERVISOR_EXT 'stf::EventRecord::TYPE::INT_SUPERVISOR_EXT'
        _INT_HYPERVISOR_EXT 'stf::EventRecord::TYPE::INT_HYPERVISOR_EXT'
        _INT_MACHINE_EXT 'stf::EventRecord::TYPE::INT_MACHINE_EXT'
        _INT_COPROCESSOR 'stf::EventRecord::TYPE::INT_COPROCESSOR'
        _INT_HOST 'stf::EventRecord::TYPE::INT_HOST'
        _MODE_CHANGE 'stf::EventRecord::TYPE::MODE_CHANGE'
