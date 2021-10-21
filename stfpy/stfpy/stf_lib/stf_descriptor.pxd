# distutils: language = c++

from libc.stdint cimport *
cdef extern from "stf_descriptor.hpp" namespace "stf::descriptors::internal":
    cdef enum _InternalDescriptor 'stf::descriptors::internal::Descriptor':
        _STF_INST_REG 'stf::descriptors::internal::Descriptor::STF_INST_REG'
        _STF_INST_OPCODE16 'stf::descriptors::internal::Descriptor::STF_INST_OPCODE16'
        _STF_INST_OPCODE32 'stf::descriptors::internal::Descriptor::STF_INST_OPCODE32'
        _STF_INST_MEM_ACCESS 'stf::descriptors::internal::Descriptor::STF_INST_MEM_ACCESS'
        _STF_INST_MEM_CONTENT 'stf::descriptors::internal::Descriptor::STF_INST_MEM_CONTENT'
        _STF_INST_PC_TARGET 'stf::descriptors::internal::Descriptor::STF_INST_PC_TARGET'
        _STF_EVENT 'stf::descriptors::internal::Descriptor::STF_EVENT'
        _STF_EVENT_PC_TARGET 'stf::descriptors::internal::Descriptor::STF_EVENT_PC_TARGET'
        _STF_PAGE_TABLE_WALK 'stf::descriptors::internal::Descriptor::STF_PAGE_TABLE_WALK'
        _STF_BUS_MASTER_ACCESS 'stf::descriptors::internal::Descriptor::STF_BUS_MASTER_ACCESS'
        _STF_BUS_MASTER_CONTENT 'stf::descriptors::internal::Descriptor::STF_BUS_MASTER_CONTENT'
        _STF_COMMENT 'stf::descriptors::internal::Descriptor::STF_COMMENT'
        _STF_FORCE_PC 'stf::descriptors::internal::Descriptor::STF_FORCE_PC'
        _STF_INST_READY_REG 'stf::descriptors::internal::Descriptor::STF_INST_READY_REG'
        _STF_PROCESS_ID_EXT 'stf::descriptors::internal::Descriptor::STF_PROCESS_ID_EXT'
        _STF_INST_MICROOP 'stf::descriptors::internal::Descriptor::STF_INST_MICROOP'
        _STF_IDENTIFIER 'stf::descriptors::internal::Descriptor::STF_IDENTIFIER'
        _STF_ISA 'stf::descriptors::internal::Descriptor::STF_ISA'
        _STF_INST_IEM 'stf::descriptors::internal::Descriptor::STF_INST_IEM'
        _STF_TRACE_INFO 'stf::descriptors::internal::Descriptor::STF_TRACE_INFO'
        _STF_TRACE_INFO_FEATURE 'stf::descriptors::internal::Descriptor::STF_TRACE_INFO_FEATURE'
        _STF_VERSION 'stf::descriptors::internal::Descriptor::STF_VERSION'
        _STF_VLEN_CONFIG 'stf::descriptors::internal::Descriptor::STF_VLEN_CONFIG'
        _STF_END_HEADER 'stf::descriptors::internal::Descriptor::STF_END_HEADER'
        _STF_RESERVED 'stf::descriptors::internal::Descriptor::STF_RESERVED'
        _STF_RESERVED_END 'stf::descriptors::internal::Descriptor::STF_RESERVED_END'

cpdef enum InternalDescriptor:
        STF_INST_REG = <uint8_t> _STF_INST_REG
        STF_INST_OPCODE16 = <uint8_t> _STF_INST_OPCODE16
        STF_INST_OPCODE32 = <uint8_t> _STF_INST_OPCODE32
        STF_INST_MEM_ACCESS = <uint8_t> _STF_INST_MEM_ACCESS
        STF_INST_MEM_CONTENT = <uint8_t> _STF_INST_MEM_CONTENT
        STF_INST_PC_TARGET = <uint8_t> _STF_INST_PC_TARGET
        STF_EVENT = <uint8_t> _STF_EVENT
        STF_EVENT_PC_TARGET = <uint8_t> _STF_EVENT_PC_TARGET
        STF_PAGE_TABLE_WALK = <uint8_t> _STF_PAGE_TABLE_WALK
        STF_BUS_MASTER_ACCESS = <uint8_t> _STF_BUS_MASTER_ACCESS
        STF_BUS_MASTER_CONTENT = <uint8_t> _STF_BUS_MASTER_CONTENT
        STF_COMMENT = <uint8_t> _STF_COMMENT
        STF_FORCE_PC = <uint8_t> _STF_FORCE_PC
        STF_INST_READY_REG = <uint8_t> _STF_INST_READY_REG
        STF_PROCESS_ID_EXT = <uint8_t> _STF_PROCESS_ID_EXT
        STF_INST_MICROOP = <uint8_t> _STF_INST_MICROOP
        STF_IDENTIFIER = <uint8_t> _STF_IDENTIFIER
        STF_ISA = <uint8_t> _STF_ISA
        STF_INST_IEM = <uint8_t> _STF_INST_IEM
        STF_TRACE_INFO = <uint8_t> _STF_TRACE_INFO
        STF_TRACE_INFO_FEATURE = <uint8_t> _STF_TRACE_INFO_FEATURE
        STF_VERSION = <uint8_t> _STF_VERSION
        STF_VLEN_CONFIG = <uint8_t> _STF_VLEN_CONFIG
        STF_END_HEADER = <uint8_t> _STF_END_HEADER
        STF_RESERVED = <uint8_t> _STF_RESERVED
        STF_RESERVED_END = <uint8_t> _STF_RESERVED_END
