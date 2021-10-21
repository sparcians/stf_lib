# distutils: language = c++

from cython.operator cimport dereference, preincrement
from stfpy.stf_lib.stf_record_types cimport VectorValueType as _VectorValueType, VectorValueTypeIterator as _VectorValueTypeIterator
from stfpy.stf_lib.stf_inst cimport EventDataVector as _EventDataVector, EventDataVectorIterator as _EventDataVectorIterator, EventVectorIterator as _EventVectorIterator, OperandVectorIterator as _OperandVectorIterator, MemAccessVectorIterator as _MemAccessVectorIterator, Event as _Event, Operand as _Operand, MemAccess as _MemAccess, STFInst as _STFInst

cdef class EventDataVectorIterator:
    cdef _EventDataVectorIterator c_it
    cdef _EventDataVectorIterator c_end_it

    @staticmethod
    cdef inline EventDataVectorIterator _construct(const _EventDataVector* vec):
        it = EventDataVectorIterator()
        it.c_it = dereference(vec).begin()
        it.c_end_it = dereference(vec).end()
        return it

cdef class EventDataVector:
    cdef const _EventDataVector* c_vec

    @staticmethod
    cdef inline EventDataVector _construct(const _EventDataVector& vec):
        new_vec = EventDataVector()
        new_vec.c_vec = &vec
        return new_vec

cdef class Event:
    cdef const _Event* c_event

    @staticmethod
    cdef inline Event _construct(const _Event& event):
        new_event = Event()
        new_event.c_event = &event
        return new_event

cdef class EventVectorIterator:
    cdef _EventVectorIterator c_it
    cdef _EventVectorIterator c_end_it

    @staticmethod
    cdef inline EventVectorIterator _construct(const _STFInst._EventVector* vec):
        it = EventVectorIterator()
        it.c_it = dereference(vec).begin()
        it.c_end_it = dereference(vec).end()
        return it

cdef class EventVector:
    cdef const _STFInst._EventVector* c_vec

    @staticmethod
    cdef inline EventVector _construct(const _STFInst._EventVector& vec):
        new_vec = EventVector()
        new_vec.c_vec = &vec
        return new_vec

cdef class VectorValueTypeIterator:
    cdef _VectorValueTypeIterator c_it
    cdef _VectorValueTypeIterator c_end_it

    @staticmethod
    cdef inline VectorValueTypeIterator _construct(const _VectorValueType* vec):
        it = VectorValueTypeIterator()
        it.c_it = dereference(vec).begin()
        it.c_end_it = dereference(vec).end()
        return it

cdef class VectorValueType:
    cdef const _VectorValueType* c_vec

    @staticmethod
    cdef inline VectorValueType _construct(const _VectorValueType& vec):
        new_vec = VectorValueType()
        new_vec.c_vec = &vec
        return new_vec

cdef class Operand:
    cdef const _Operand* c_op

    @staticmethod
    cdef inline Operand _construct(const _Operand& op):
        new_op = Operand()
        new_op.c_op = &op
        return new_op

cdef class OperandVectorIterator:
    cdef _OperandVectorIterator c_it
    cdef _OperandVectorIterator c_end_it

    @staticmethod
    cdef inline OperandVectorIterator _construct(const _STFInst._OperandVector* vec):
        it = OperandVectorIterator()
        it.c_it = dereference(vec).begin()
        it.c_end_it = dereference(vec).end()
        return it

cdef class OperandVector:
    cdef const _STFInst._OperandVector* c_vec

    @staticmethod
    cdef inline OperandVector _construct(const _STFInst._OperandVector& vec):
        new_vec = OperandVector()
        new_vec.c_vec = &vec
        return new_vec

cdef class MemAccess:
    cdef const _MemAccess* c_mem

    @staticmethod
    cdef inline MemAccess _construct(const _MemAccess& mem):
        new_mem = MemAccess()
        new_mem.c_mem = &mem
        return new_mem

cdef class MemAccessVectorIterator:
    cdef _MemAccessVectorIterator c_it
    cdef _MemAccessVectorIterator c_end_it

    @staticmethod
    cdef inline MemAccessVectorIterator _construct(const _STFInst._MemAccessVector* vec):
        it = MemAccessVectorIterator()
        it.c_it = dereference(vec).begin()
        it.c_end_it = dereference(vec).end()
        return it

cdef class MemAccessVector:
    cdef const _STFInst._MemAccessVector* c_vec

    @staticmethod
    cdef inline MemAccessVector _construct(const _STFInst._MemAccessVector& vec):
        new_vec = MemAccessVector()
        new_vec.c_vec = &vec
        return new_vec

cdef class STFInst:
    cdef const _STFInst* c_inst

    @staticmethod
    cdef inline STFInst _construct(const _STFInst& inst):
        new_inst = STFInst()
        new_inst.c_inst = &inst
        return new_inst
