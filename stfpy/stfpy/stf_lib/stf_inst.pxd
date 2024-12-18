# distutils: language = c++

from libc.stdint cimport *
from stfpy.stf_lib.boost_small_vector cimport small_vector
from stfpy.stf_lib.stf_enums cimport _INST_MEM_ACCESS
from stfpy.stf_lib.stf_reg_def cimport _STF_REG, _STF_REG_OPERAND_TYPE
from stfpy.stf_lib.stf_record_map cimport RecordMap
from stfpy.stf_lib.stf_record_types cimport VectorValueType, _EVENT_RECORD_TYPE
from stfpy.stf_lib.stf_serializable_container cimport SerializableVector

cdef extern from "stf_vlen.hpp" namespace "stf":
    ctypedef uint32_t vlen_t

ctypedef SerializableVector[uint64_t, uint8_t] EventDataVector
ctypedef SerializableVector[uint64_t, uint8_t].const_iterator EventDataVectorIterator

cdef extern from "stf_inst.hpp" namespace "stf":
    cdef cppclass MemAccess:
        cppclass ContentValueView:
            cppclass iterator:
                uint64_t operator*()
                bint operator==(const iterator&)
                iterator& operator++()
            iterator begin()
            iterator end()
        uint64_t getSize()
        uint64_t getAddress()
        ContentValueView getData()
        _INST_MEM_ACCESS getType()
        uint16_t getAttr()

    cdef cppclass Operand:
        uint64_t getScalarValue()
        _STF_REG getReg()
        _STF_REG_OPERAND_TYPE getType()
        bint isVector()
        const VectorValueType& getVectorValue()
        vlen_t getVLen()

    cdef cppclass Event:
        _EVENT_RECORD_TYPE getEvent()
        const EventDataVector& getData()
        uint64_t getTarget()
        bint dataValid()
        bint targetValid()
        bint isFault()
        bint isInterrupt()
        bint isModeChange()
        bint isSyscall()

cdef extern from "stf_inst.hpp":
    ctypedef size_t _EventVectorSize "4"
    ctypedef size_t _OperandVectorSize "2"
    ctypedef size_t _MemAccessVectorSize "1"

ctypedef small_vector[Event, _EventVectorSize] _EventVector
ctypedef small_vector[Event, _EventVectorSize].const_iterator EventVectorIterator

ctypedef small_vector[Operand, _OperandVectorSize] _OperandVector
ctypedef small_vector[Operand, _OperandVectorSize].const_iterator OperandVectorIterator

ctypedef small_vector[MemAccess, _MemAccessVectorSize] _MemAccessVector
ctypedef small_vector[MemAccess, _MemAccessVectorSize].const_iterator MemAccessVectorIterator

cdef extern from "stf_inst.hpp" namespace "stf":
    cdef cppclass STFInst:
        ctypedef _EventVector EventVector
        ctypedef _OperandVector OperandVector
        ctypedef _MemAccessVector MemAccessVector

        uint64_t index()
        bint isTakenBranch()
        bint isOpcode16()
        bint isCoF()
        bint isLoad()
        bint isStore()
        bint isSyscall()
        bint isFault()
        bint isFP()
        bint isVector()
        bint isChangeFromUserMode()
        bint isChangeToUserMode()
        uint64_t branchTarget()
        const RecordMap& getOrigRecords()
        const OperandVector& getRegisterStates()
        const RecordMap.SmallVector& getComments()
        const RecordMap.SmallVector& getMicroOps()
        const RecordMap.SmallVector& getReadyRegs()
        const EventVector& getEvents()
        const OperandVector& getSourceOperands()
        const OperandVector& getDestOperands()
        const MemAccessVector& getMemoryReads()
        const MemAccessVector& getMemoryWrites()
        uint64_t totalMemAccessSize()
        uint32_t hwtid()
        uint32_t pid()
        uint32_t tid()
        uint64_t pc()
        uint32_t opcode()
        uint8_t opcodeSize()
        bint valid()
        bint isBranch()

