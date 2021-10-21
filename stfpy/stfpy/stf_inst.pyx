# distutils: language = c++

from cython.operator cimport dereference, preincrement
from stfpy.stf_lib.stf_enums cimport INST_MEM_ACCESS
from stfpy.stf_lib.stf_reg_def cimport STF_REG, STF_REG_OPERAND_TYPE
from stfpy.stf_record_map cimport STFRecordMap, STFRecordMapSmallVector
from stfpy.stf_record_map import STFRecordMap, STFRecordMapSmallVector

cdef class EventDataVectorIterator:
    def __next__(self):
        preincrement(self.c_it)
        if self.c_it != self.c_end_it:
            return dereference(self.c_it)
        else:
            raise StopIteration

cdef class EventDataVector:
    def __iter__(self):
        return EventDataVectorIterator._construct(self.c_vec)

    def __len__(self):
        return dereference(self.c_vec).size()

    def __getitem__(self, idx):
        return dereference(self.c_vec).at(idx)

    def __bool__(self):
        return not dereference(self.c_vec).empty()

cdef class Event:
    def getData(self):
        return EventDataVector._construct(dereference(self.c_event).getData())

    def getTarget(self):
        return dereference(self.c_event).getTarget()

    def dataValid(self):
        return dereference(self.c_event).dataValid()

    def targetValid(self):
        return dereference(self.c_event).targetValid()

    def isFault(self):
        return dereference(self.c_event).isFault()

    def isInterrupt(self):
        return dereference(self.c_event).isInterrupt()

    def isModeChange(self):
        return dereference(self.c_event).isModeChange()

    def isSyscall(self):
        return dereference(self.c_event).isSyscall()

cdef class EventVectorIterator:
    def __next__(self):
        preincrement(self.c_it)
        if self.c_it != self.c_end_it:
            return Event._construct(dereference(self.c_it))
        else:
            raise StopIteration

cdef class EventVector:
    def __iter__(self):
        return EventVectorIterator._construct(self.c_vec)

    def __len__(self):
        return dereference(self.c_vec).size()

    def __getitem__(self, idx):
        return Event._construct(dereference(self.c_vec).at(idx))

    def __bool__(self):
        return not dereference(self.c_vec).empty()

cdef class VectorValueTypeIterator:
    def __next__(self):
        preincrement(self.c_it)
        if self.c_it != self.c_end_it:
            return dereference(self.c_it)
        else:
            raise StopIteration

cdef class VectorValueType:
    def __iter__(self):
        return VectorValueTypeIterator._construct(self.c_vec)

    def __len__(self):
        return dereference(self.c_vec).size()

    def __getitem__(self, idx):
        return dereference(self.c_vec).at(idx)

    def __bool__(self):
        return not dereference(self.c_vec).empty()

cdef class Operand:
    def getScalarValue(self):
        return dereference(self.c_op).getScalarValue()

    def getReg(self):
        return STF_REG(dereference(self.c_op).getReg())

    def getType(self):
        return STF_REG_OPERAND_TYPE(dereference(self.c_op).getReg())

    def isVector(self):
        return dereference(self.c_op).isVector()

    def getVectorValue(self):
        return VectorValueType._construct(dereference(self.c_op).getVectorValue())

    def getVLen(self):
        return dereference(self.c_op).getVLen()

cdef class OperandVectorIterator:
    def __next__(self):
        preincrement(self.c_it)
        if self.c_it != self.c_end_it:
            return Operand._construct(dereference(self.c_it))
        else:
            raise StopIteration

cdef class OperandVector:
    def __iter__(self):
        return OperandVectorIterator._construct(self.c_vec)

    def __len__(self):
        return dereference(self.c_vec).size()

    def __getitem__(self, idx):
        return Operand._construct(dereference(self.c_vec).at(idx))

    def __bool__(self):
        return not dereference(self.c_vec).empty()

cdef class MemAccess:
    def getSize(self):
        return dereference(self.c_mem).getSize()

    def getAddress(self):
        return dereference(self.c_mem).getAddress()

    def getData(self):
        return dereference(self.c_mem).getData()

    def getType(self):
        return INST_MEM_ACCESS(dereference(self.c_mem).getType())

    def getAttr(self):
        return dereference(self.c_mem).getAttr()

cdef class MemAccessVectorIterator:
    def __next__(self):
        preincrement(self.c_it)
        if self.c_it != self.c_end_it:
            return MemAccess._construct(dereference(self.c_it))
        else:
            raise StopIteration

cdef class MemAccessVector:
    def __iter__(self):
        return MemAccessVectorIterator._construct(self.c_vec)

    def __len__(self):
        return dereference(self.c_vec).size()

    def __getitem__(self, idx):
        return MemAccess._construct(dereference(self.c_vec).at(idx))

    def __bool__(self):
        return not dereference(self.c_vec).empty()

cdef class STFInst:
    def isTakenBranch(self):
        return self.c_inst.isTakenBranch()

    def isOpcode16(self):
        return self.c_inst.isOpcode16()

    def isCoF(self):
        return self.c_inst.isCoF()

    def isLoad(self):
        return self.c_inst.isLoad()

    def isStore(self):
        return self.c_inst.isStore()

    def isSyscall(self):
        return self.c_inst.isSyscall()

    def isFault(self):
        return self.c_inst.isFault()

    def isFP(self):
        return self.c_inst.isFP()

    def isVector(self):
        return self.c_inst.isVector()

    def isChangeFromUserMode(self):
        return self.c_inst.isChangeFromUserMode()

    def isChangeToUserMode(self):
        return self.c_inst.isChangeToUserMode()

    def branchTarget(self):
        return self.c_inst.branchTarget()

    def getOrigRecords(self):
        return STFRecordMap._construct(self.c_inst.getOrigRecords())

    def getRegisterStates(self):
        return OperandVector._construct(self.c_inst.getRegisterStates())

    def getComments(self):
        return STFRecordMapSmallVector._construct(self.c_inst.getComments())

    def getMicroOps(self):
        return STFRecordMapSmallVector._construct(self.c_inst.getMicroOps())

    def getReadyRegs(self):
        return STFRecordMapSmallVector._construct(self.c_inst.getReadyRegs())

    def getEvents(self):
        return EventVector._construct(self.c_inst.getEvents())

    def getSourceOperands(self):
        return OperandVector._construct(self.c_inst.getSourceOperands())

    def getDestOperands(self):
        return OperandVector._construct(self.c_inst.getDestOperands())

    def getMemoryReads(self):
        return MemAccessVector._construct(self.c_inst.getMemoryReads())

    def getMemoryWrites(self):
        return MemAccessVector._construct(self.c_inst.getMemoryReads())

    def totalMemAccessSize(self):
        return self.c_inst.totalMemAccessSize()

    def asid(self):
        return self.c_inst.asid()

    def tid(self):
        return self.c_inst.tid()

    def tgid(self):
        return self.c_inst.tgid()

    def pc(self):
        return self.c_inst.pc()

    def opcode(self):
        return self.c_inst.opcode()

    def opcodeSize(self):
        return self.c_inst.opcodeSize()

    def valid(self):
        return self.c_inst.valid()

    def isBranch(self):
        return self.c_inst.isBranch()

