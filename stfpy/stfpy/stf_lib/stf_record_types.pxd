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

cpdef enum EVENT_RECORD_TYPE:
    INST_ADDR_MISALIGN = <uint64_t> _INST_ADDR_MISALIGN
    INST_ADDR_FAULT = <uint64_t> _INST_ADDR_FAULT
    ILLEGAL_INST = <uint64_t> _ILLEGAL_INST
    BREAKPOINT = <uint64_t> _BREAKPOINT
    LOAD_ADDR_MISALIGN = <uint64_t> _LOAD_ADDR_MISALIGN
    LOAD_ACCESS_FAULT = <uint64_t> _LOAD_ACCESS_FAULT
    STORE_ADDR_MISALIGN = <uint64_t> _STORE_ADDR_MISALIGN
    STORE_ACCESS_FAULT = <uint64_t> _STORE_ACCESS_FAULT
    USER_ECALL = <uint64_t> _USER_ECALL
    SUPERVISOR_ECALL = <uint64_t> _SUPERVISOR_ECALL
    HYPERVISOR_ECALL = <uint64_t> _HYPERVISOR_ECALL
    MACHINE_ECALL = <uint64_t> _MACHINE_ECALL
    INST_PAGE_FAULT = <uint64_t> _INST_PAGE_FAULT
    LOAD_PAGE_FAULT = <uint64_t> _LOAD_PAGE_FAULT
    STORE_PAGE_FAULT = <uint64_t> _STORE_PAGE_FAULT
    GUEST_INST_PAGE_FAULT = <uint64_t> _GUEST_INST_PAGE_FAULT
    GUEST_LOAD_PAGE_FAULT = <uint64_t> _GUEST_LOAD_PAGE_FAULT
    VIRTUAL_INST = <uint64_t> _VIRTUAL_INST
    GUEST_STORE_PAGE_FAULT = <uint64_t> _GUEST_STORE_PAGE_FAULT
    INT_USER_SOFTWARE = <uint64_t> _INT_USER_SOFTWARE
    INT_SUPERVISOR_SOFTWARE = <uint64_t> _INT_SUPERVISOR_SOFTWARE
    INT_HYPERVISOR_SOFTWARE = <uint64_t> _INT_HYPERVISOR_SOFTWARE
    INT_MACHINE_SOFTWARE = <uint64_t> _INT_MACHINE_SOFTWARE
    INT_USER_TIMER = <uint64_t> _INT_USER_TIMER
    INT_SUPERVISOR_TIMER = <uint64_t> _INT_SUPERVISOR_TIMER
    INT_HYPERVISOR_TIMER = <uint64_t> _INT_HYPERVISOR_TIMER
    INT_MACHINE_TIMER = <uint64_t> _INT_MACHINE_TIMER
    INT_USER_EXT = <uint64_t> _INT_USER_EXT
    INT_SUPERVISOR_EXT = <uint64_t> _INT_SUPERVISOR_EXT
    INT_HYPERVISOR_EXT = <uint64_t> _INT_HYPERVISOR_EXT
    INT_MACHINE_EXT = <uint64_t> _INT_MACHINE_EXT
    INT_COPROCESSOR = <uint64_t> _INT_COPROCESSOR
    INT_HOST = <uint64_t> _INT_HOST
    MODE_CHANGE = <uint64_t> _MODE_CHANGE
