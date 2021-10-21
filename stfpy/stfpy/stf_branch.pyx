# distutils: language = c++

from cython.operator cimport dereference as deref

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

