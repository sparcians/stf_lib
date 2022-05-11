# distutils: language = c++

from libc.stdint cimport *
from stfpy.stf_lib.stf_reg_def cimport _STF_REG

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
        bint isMillicall()
        bint isMillireturn()
        bint isConditional()
        bint isBackwards()
        bint isCompareEqual()
        bint isCompareNotEqual()
        bint isCompareGreaterThanOrEqual()
        bint isCompareLessThan()
        bint isCompareUnsigned()
        _STF_REG getRS1()
        uint64_t getRS1Value()
        _STF_REG getRS2()
        uint64_t getRS2Value()
        _STF_REG getRD()
        uint64_t getRDValue()
