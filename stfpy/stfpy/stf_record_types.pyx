from stfpy.stf_lib.stf_record_types cimport *

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
