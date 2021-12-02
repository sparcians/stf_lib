# distutils: language = c++

from cython.operator cimport dereference as deref
from stfpy.stf_lib.stf_reg_def cimport STF_REG

cdef class STFBranch:
    def index(self):
        return deref(self.c_branch).index()

    def isTaken(self):
        return deref(self.c_branch).isTaken()

    def getPC(self):
        return deref(self.c_branch).getPC()

    def getTargetPC(self):
        return deref(self.c_branch).getTargetPC()

    def getOpcode(self):
        return deref(self.c_branch).getOpcode()

    def getTargetOpcode(self):
        return deref(self.c_branch).getTargetOpcode()

    def isIndirect(self):
        return deref(self.c_branch).isIndirect()

    def isCall(self):
        return deref(self.c_branch).isCall()

    def isReturn(self):
        return deref(self.c_branch).isReturn()

    def isConditional(self):
        return deref(self.c_branch).isConditional()

    def isBackwards(self):
        return deref(self.c_branch).isBackwards()

    def isCompareEqual(self):
        return deref(self.c_branch).isCompareEqual()

    def isCompareNotEqual(self):
        return deref(self.c_branch).isCompareNotEqual()

    def isCompareGreaterThanOrEqual(self):
        return deref(self.c_branch).isCompareGreaterThanOrEqual()

    def isCompareLessThan(self):
        return deref(self.c_branch).isCompareLessThan()

    def isCompareUnsigned(self):
        return deref(self.c_branch).isCompareUnsigned()

    def getRS1(self):
        return STF_REG(deref(self.c_branch).getRS1())

    def getRS1Value(self):
        return deref(self.c_branch).getRS1Value()

    def getRS2(self):
        return STF_REG(deref(self.c_branch).getRS1())

    def getRS2Value(self):
        return deref(self.c_branch).getRS1Value()
