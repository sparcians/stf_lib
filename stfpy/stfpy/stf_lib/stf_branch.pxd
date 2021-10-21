# distutils: language = c++

from libc.stdint cimport *

cdef extern from "stf_branch.hpp" namespace "stf":
    cdef cppclass STFBranch:
        uint64_t index()
        bint isTaken()
        uint64_t getPC()
        uint64_t getTargetPC()
        uint32_t getOpcode()
        uint32_t getTargetOpcode()
        bint isIndirect()
        bint isCall()
        bint isReturn()
        bint isConditional()
        bint isBackwards()
