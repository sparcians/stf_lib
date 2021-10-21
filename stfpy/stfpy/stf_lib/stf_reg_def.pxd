# distutils: language = c++

from libc.stdint cimport *

cdef extern from "stf_reg_def.hpp" namespace "stf::Registers":
    ctypedef uint32_t STF_REG_int
    ctypedef uint8_t STF_REG_metadata_int

    cdef enum _STF_REG 'stf::Registers::STF_REG':
        _STF_REG_X0 'stf::Registers::STF_REG::STF_REG_X0'
        _STF_REG_X1 'stf::Registers::STF_REG::STF_REG_X1'
        _STF_REG_X2 'stf::Registers::STF_REG::STF_REG_X2'
        _STF_REG_X3 'stf::Registers::STF_REG::STF_REG_X3'
        _STF_REG_X4 'stf::Registers::STF_REG::STF_REG_X4'
        _STF_REG_X5 'stf::Registers::STF_REG::STF_REG_X5'
        _STF_REG_X6 'stf::Registers::STF_REG::STF_REG_X6'
        _STF_REG_X7 'stf::Registers::STF_REG::STF_REG_X7'
        _STF_REG_X8 'stf::Registers::STF_REG::STF_REG_X8'
        _STF_REG_X9 'stf::Registers::STF_REG::STF_REG_X9'
        _STF_REG_X10 'stf::Registers::STF_REG::STF_REG_X10'
        _STF_REG_X11 'stf::Registers::STF_REG::STF_REG_X11'
        _STF_REG_X12 'stf::Registers::STF_REG::STF_REG_X12'
        _STF_REG_X13 'stf::Registers::STF_REG::STF_REG_X13'
        _STF_REG_X14 'stf::Registers::STF_REG::STF_REG_X14'
        _STF_REG_X15 'stf::Registers::STF_REG::STF_REG_X15'
        _STF_REG_X16 'stf::Registers::STF_REG::STF_REG_X16'
        _STF_REG_X17 'stf::Registers::STF_REG::STF_REG_X17'
        _STF_REG_X18 'stf::Registers::STF_REG::STF_REG_X18'
        _STF_REG_X19 'stf::Registers::STF_REG::STF_REG_X19'
        _STF_REG_X20 'stf::Registers::STF_REG::STF_REG_X20'
        _STF_REG_X21 'stf::Registers::STF_REG::STF_REG_X21'
        _STF_REG_X22 'stf::Registers::STF_REG::STF_REG_X22'
        _STF_REG_X23 'stf::Registers::STF_REG::STF_REG_X23'
        _STF_REG_X24 'stf::Registers::STF_REG::STF_REG_X24'
        _STF_REG_X25 'stf::Registers::STF_REG::STF_REG_X25'
        _STF_REG_X26 'stf::Registers::STF_REG::STF_REG_X26'
        _STF_REG_X27 'stf::Registers::STF_REG::STF_REG_X27'
        _STF_REG_X28 'stf::Registers::STF_REG::STF_REG_X28'
        _STF_REG_X29 'stf::Registers::STF_REG::STF_REG_X29'
        _STF_REG_X30 'stf::Registers::STF_REG::STF_REG_X30'
        _STF_REG_X31 'stf::Registers::STF_REG::STF_REG_X31'

        # 32 floating point registers
        _STF_REG_F0 'stf::Registers::STF_REG::STF_REG_F0'
        _STF_REG_F1 'stf::Registers::STF_REG::STF_REG_F1'
        _STF_REG_F2 'stf::Registers::STF_REG::STF_REG_F2'
        _STF_REG_F3 'stf::Registers::STF_REG::STF_REG_F3'
        _STF_REG_F4 'stf::Registers::STF_REG::STF_REG_F4'
        _STF_REG_F5 'stf::Registers::STF_REG::STF_REG_F5'
        _STF_REG_F6 'stf::Registers::STF_REG::STF_REG_F6'
        _STF_REG_F7 'stf::Registers::STF_REG::STF_REG_F7'
        _STF_REG_F8 'stf::Registers::STF_REG::STF_REG_F8'
        _STF_REG_F9 'stf::Registers::STF_REG::STF_REG_F9'
        _STF_REG_F10 'stf::Registers::STF_REG::STF_REG_F10'
        _STF_REG_F11 'stf::Registers::STF_REG::STF_REG_F11'
        _STF_REG_F12 'stf::Registers::STF_REG::STF_REG_F12'
        _STF_REG_F13 'stf::Registers::STF_REG::STF_REG_F13'
        _STF_REG_F14 'stf::Registers::STF_REG::STF_REG_F14'
        _STF_REG_F15 'stf::Registers::STF_REG::STF_REG_F15'
        _STF_REG_F16 'stf::Registers::STF_REG::STF_REG_F16'
        _STF_REG_F17 'stf::Registers::STF_REG::STF_REG_F17'
        _STF_REG_F18 'stf::Registers::STF_REG::STF_REG_F18'
        _STF_REG_F19 'stf::Registers::STF_REG::STF_REG_F19'
        _STF_REG_F20 'stf::Registers::STF_REG::STF_REG_F20'
        _STF_REG_F21 'stf::Registers::STF_REG::STF_REG_F21'
        _STF_REG_F22 'stf::Registers::STF_REG::STF_REG_F22'
        _STF_REG_F23 'stf::Registers::STF_REG::STF_REG_F23'
        _STF_REG_F24 'stf::Registers::STF_REG::STF_REG_F24'
        _STF_REG_F25 'stf::Registers::STF_REG::STF_REG_F25'
        _STF_REG_F26 'stf::Registers::STF_REG::STF_REG_F26'
        _STF_REG_F27 'stf::Registers::STF_REG::STF_REG_F27'
        _STF_REG_F28 'stf::Registers::STF_REG::STF_REG_F28'
        _STF_REG_F29 'stf::Registers::STF_REG::STF_REG_F29'
        _STF_REG_F30 'stf::Registers::STF_REG::STF_REG_F30'
        _STF_REG_F31 'stf::Registers::STF_REG::STF_REG_F31'

        # 32 vector registers
        _STF_REG_V0 'stf::Registers::STF_REG::STF_REG_V0'
        _STF_REG_V1 'stf::Registers::STF_REG::STF_REG_V1'
        _STF_REG_V2 'stf::Registers::STF_REG::STF_REG_V2'
        _STF_REG_V3 'stf::Registers::STF_REG::STF_REG_V3'
        _STF_REG_V4 'stf::Registers::STF_REG::STF_REG_V4'
        _STF_REG_V5 'stf::Registers::STF_REG::STF_REG_V5'
        _STF_REG_V6 'stf::Registers::STF_REG::STF_REG_V6'
        _STF_REG_V7 'stf::Registers::STF_REG::STF_REG_V7'
        _STF_REG_V8 'stf::Registers::STF_REG::STF_REG_V8'
        _STF_REG_V9 'stf::Registers::STF_REG::STF_REG_V9'
        _STF_REG_V10 'stf::Registers::STF_REG::STF_REG_V10'
        _STF_REG_V11 'stf::Registers::STF_REG::STF_REG_V11'
        _STF_REG_V12 'stf::Registers::STF_REG::STF_REG_V12'
        _STF_REG_V13 'stf::Registers::STF_REG::STF_REG_V13'
        _STF_REG_V14 'stf::Registers::STF_REG::STF_REG_V14'
        _STF_REG_V15 'stf::Registers::STF_REG::STF_REG_V15'
        _STF_REG_V16 'stf::Registers::STF_REG::STF_REG_V16'
        _STF_REG_V17 'stf::Registers::STF_REG::STF_REG_V17'
        _STF_REG_V18 'stf::Registers::STF_REG::STF_REG_V18'
        _STF_REG_V19 'stf::Registers::STF_REG::STF_REG_V19'
        _STF_REG_V20 'stf::Registers::STF_REG::STF_REG_V20'
        _STF_REG_V21 'stf::Registers::STF_REG::STF_REG_V21'
        _STF_REG_V22 'stf::Registers::STF_REG::STF_REG_V22'
        _STF_REG_V23 'stf::Registers::STF_REG::STF_REG_V23'
        _STF_REG_V24 'stf::Registers::STF_REG::STF_REG_V24'
        _STF_REG_V25 'stf::Registers::STF_REG::STF_REG_V25'
        _STF_REG_V26 'stf::Registers::STF_REG::STF_REG_V26'
        _STF_REG_V27 'stf::Registers::STF_REG::STF_REG_V27'
        _STF_REG_V28 'stf::Registers::STF_REG::STF_REG_V28'
        _STF_REG_V29 'stf::Registers::STF_REG::STF_REG_V29'
        _STF_REG_V30 'stf::Registers::STF_REG::STF_REG_V30'
        _STF_REG_V31 'stf::Registers::STF_REG::STF_REG_V31'

        # Control and status registers
        # User
        _STF_REG_CSR_USTATUS 'stf::Registers::STF_REG::STF_REG_CSR_USTATUS'
        _STF_REG_CSR_FFLAGS 'stf::Registers::STF_REG::STF_REG_CSR_FFLAGS'
        _STF_REG_CSR_FRM 'stf::Registers::STF_REG::STF_REG_CSR_FRM'
        _STF_REG_CSR_FCSR 'stf::Registers::STF_REG::STF_REG_CSR_FCSR'
        _STF_REG_CSR_UIE 'stf::Registers::STF_REG::STF_REG_CSR_UIE'
        _STF_REG_CSR_UTVEC 'stf::Registers::STF_REG::STF_REG_CSR_UTVEC'
        _STF_REG_CSR_UTVT 'stf::Registers::STF_REG::STF_REG_CSR_UTVT'
        _STF_REG_CSR_VSTART 'stf::Registers::STF_REG::STF_REG_CSR_VSTART'
        _STF_REG_CSR_VXSAT 'stf::Registers::STF_REG::STF_REG_CSR_VXSAT'
        _STF_REG_CSR_VXRM 'stf::Registers::STF_REG::STF_REG_CSR_VXRM'
        _STF_REG_CSR_VCSR 'stf::Registers::STF_REG::STF_REG_CSR_VCSR'
        _STF_REG_CSR_USCRATCH 'stf::Registers::STF_REG::STF_REG_CSR_USCRATCH'
        _STF_REG_CSR_UEPC 'stf::Registers::STF_REG::STF_REG_CSR_UEPC'
        _STF_REG_CSR_UCAUSE 'stf::Registers::STF_REG::STF_REG_CSR_UCAUSE'
        _STF_REG_CSR_UTVAL 'stf::Registers::STF_REG::STF_REG_CSR_UTVAL'
        _STF_REG_CSR_UIP 'stf::Registers::STF_REG::STF_REG_CSR_UIP'
        _STF_REG_CSR_UNXTI 'stf::Registers::STF_REG::STF_REG_CSR_UNXTI'
        _STF_REG_CSR_UINTSTATUS 'stf::Registers::STF_REG::STF_REG_CSR_UINTSTATUS'
        _STF_REG_CSR_USCRATCHCSW 'stf::Registers::STF_REG::STF_REG_CSR_USCRATCHCSW'
        _STF_REG_CSR_USCRATCHCSWL 'stf::Registers::STF_REG::STF_REG_CSR_USCRATCHCSWL'

        # Supervisor
        _STF_REG_CSR_SSTATUS 'stf::Registers::STF_REG::STF_REG_CSR_SSTATUS'
        _STF_REG_CSR_SIE 'stf::Registers::STF_REG::STF_REG_CSR_SIE'
        _STF_REG_CSR_STVEC 'stf::Registers::STF_REG::STF_REG_CSR_STVEC'
        _STF_REG_CSR_SCOUNTEREN 'stf::Registers::STF_REG::STF_REG_CSR_SCOUNTEREN'
        _STF_REG_CSR_STVT 'stf::Registers::STF_REG::STF_REG_CSR_STVT'
        _STF_REG_CSR_SSCRATCH 'stf::Registers::STF_REG::STF_REG_CSR_SSCRATCH'
        _STF_REG_CSR_SEPC 'stf::Registers::STF_REG::STF_REG_CSR_SEPC'
        _STF_REG_CSR_SCAUSE 'stf::Registers::STF_REG::STF_REG_CSR_SCAUSE'
        _STF_REG_CSR_STVAL 'stf::Registers::STF_REG::STF_REG_CSR_STVAL'
        _STF_REG_CSR_SIP 'stf::Registers::STF_REG::STF_REG_CSR_SIP'
        _STF_REG_CSR_SNXTI 'stf::Registers::STF_REG::STF_REG_CSR_SNXTI'
        _STF_REG_CSR_SINTSTATUS 'stf::Registers::STF_REG::STF_REG_CSR_SINTSTATUS'
        _STF_REG_CSR_SSCRATCHCSW 'stf::Registers::STF_REG::STF_REG_CSR_SSCRATCHCSW'
        _STF_REG_CSR_SSCRATCHCSWL 'stf::Registers::STF_REG::STF_REG_CSR_SSCRATCHCSWL'
        _STF_REG_CSR_SATP 'stf::Registers::STF_REG::STF_REG_CSR_SATP'

        # ???
        _STF_REG_CSR_VSSTATUS 'stf::Registers::STF_REG::STF_REG_CSR_VSSTATUS'
        _STF_REG_CSR_VSIE 'stf::Registers::STF_REG::STF_REG_CSR_VSIE'
        _STF_REG_CSR_VSTVEC 'stf::Registers::STF_REG::STF_REG_CSR_VSTVEC'
        _STF_REG_CSR_VSSCRATCH 'stf::Registers::STF_REG::STF_REG_CSR_VSSCRATCH'
        _STF_REG_CSR_VSEPC 'stf::Registers::STF_REG::STF_REG_CSR_VSEPC'
        _STF_REG_CSR_VSCAUSE 'stf::Registers::STF_REG::STF_REG_CSR_VSCAUSE'
        _STF_REG_CSR_VSTVAL 'stf::Registers::STF_REG::STF_REG_CSR_VSTVAL'
        _STF_REG_CSR_VSIP 'stf::Registers::STF_REG::STF_REG_CSR_VSIP'
        _STF_REG_CSR_VSATP 'stf::Registers::STF_REG::STF_REG_CSR_VSATP'

        # Machine
        _STF_REG_CSR_MSTATUS 'stf::Registers::STF_REG::STF_REG_CSR_MSTATUS'
        _STF_REG_CSR_MISA 'stf::Registers::STF_REG::STF_REG_CSR_MISA'
        _STF_REG_CSR_MEDELEG 'stf::Registers::STF_REG::STF_REG_CSR_MEDELEG'
        _STF_REG_CSR_MIDELEG 'stf::Registers::STF_REG::STF_REG_CSR_MIDELEG'
        _STF_REG_CSR_MIE 'stf::Registers::STF_REG::STF_REG_CSR_MIE'
        _STF_REG_CSR_MTVEC 'stf::Registers::STF_REG::STF_REG_CSR_MTVEC'
        _STF_REG_CSR_MCOUNTEREN 'stf::Registers::STF_REG::STF_REG_CSR_MCOUNTEREN'
        _STF_REG_CSR_MTVT 'stf::Registers::STF_REG::STF_REG_CSR_MTVT'
        _STF_REG_CSR_MSCRATCH 'stf::Registers::STF_REG::STF_REG_CSR_MSCRATCH'
        _STF_REG_CSR_MEPC 'stf::Registers::STF_REG::STF_REG_CSR_MEPC'
        _STF_REG_CSR_MCAUSE 'stf::Registers::STF_REG::STF_REG_CSR_MCAUSE'
        _STF_REG_CSR_MTVAL 'stf::Registers::STF_REG::STF_REG_CSR_MTVAL'
        _STF_REG_CSR_MIP 'stf::Registers::STF_REG::STF_REG_CSR_MIP'
        _STF_REG_CSR_MNXTI 'stf::Registers::STF_REG::STF_REG_CSR_MNXTI'
        _STF_REG_CSR_MINTSTATUS 'stf::Registers::STF_REG::STF_REG_CSR_MINTSTATUS'
        _STF_REG_CSR_MSCRATCHCSW 'stf::Registers::STF_REG::STF_REG_CSR_MSCRATCHCSW'
        _STF_REG_CSR_MSCRATCHCSWL 'stf::Registers::STF_REG::STF_REG_CSR_MSCRATCHCSWL'

        # Machine Memory Protection
        _STF_REG_CSR_PMPCFG0 'stf::Registers::STF_REG::STF_REG_CSR_PMPCFG0'
        _STF_REG_CSR_PMPCFG1 'stf::Registers::STF_REG::STF_REG_CSR_PMPCFG1'
        _STF_REG_CSR_PMPCFG2 'stf::Registers::STF_REG::STF_REG_CSR_PMPCFG2'
        _STF_REG_CSR_PMPCFG3 'stf::Registers::STF_REG::STF_REG_CSR_PMPCFG3'
        _STF_REG_CSR_PMPADDR0 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR0'
        _STF_REG_CSR_PMPADDR1 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR1'
        _STF_REG_CSR_PMPADDR2 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR2'
        _STF_REG_CSR_PMPADDR3 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR3'
        _STF_REG_CSR_PMPADDR4 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR4'
        _STF_REG_CSR_PMPADDR5 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR5'
        _STF_REG_CSR_PMPADDR6 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR6'
        _STF_REG_CSR_PMPADDR7 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR7'
        _STF_REG_CSR_PMPADDR8 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR8'
        _STF_REG_CSR_PMPADDR9 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR9'
        _STF_REG_CSR_PMPADDR10 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR10'
        _STF_REG_CSR_PMPADDR11 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR11'
        _STF_REG_CSR_PMPADDR12 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR12'
        _STF_REG_CSR_PMPADDR13 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR13'
        _STF_REG_CSR_PMPADDR14 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR14'
        _STF_REG_CSR_PMPADDR15 'stf::Registers::STF_REG::STF_REG_CSR_PMPADDR15'

        # ???
        _STF_REG_CSR_HSTATUS 'stf::Registers::STF_REG::STF_REG_CSR_HSTATUS'
        _STF_REG_CSR_HEDELEG 'stf::Registers::STF_REG::STF_REG_CSR_HEDELEG'
        _STF_REG_CSR_HIDELEG 'stf::Registers::STF_REG::STF_REG_CSR_HIDELEG'
        _STF_REG_CSR_HCOUNTEREN 'stf::Registers::STF_REG::STF_REG_CSR_HCOUNTEREN'
        _STF_REG_CSR_HGATP 'stf::Registers::STF_REG::STF_REG_CSR_HGATP'

        # Basic Machine Counters
        _STF_REG_CSR_MCYCLE 'stf::Registers::STF_REG::STF_REG_CSR_MCYCLE'
        _STF_REG_CSR_MINSTRET 'stf::Registers::STF_REG::STF_REG_CSR_MINSTRET'

        # Basic Machine Performance Monitoring Counters
        _STF_REG_CSR_MCYCLEH 'stf::Registers::STF_REG::STF_REG_CSR_MCYCLEH'
        _STF_REG_CSR_MINSTRETH 'stf::Registers::STF_REG::STF_REG_CSR_MINSTRETH'

        # Basic User Counters
        _STF_REG_CSR_CYCLE 'stf::Registers::STF_REG::STF_REG_CSR_CYCLE'
        _STF_REG_CSR_TIME 'stf::Registers::STF_REG::STF_REG_CSR_TIME'
        _STF_REG_CSR_INSTRET 'stf::Registers::STF_REG::STF_REG_CSR_INSTRET'

        # Vector
        _STF_REG_CSR_VL 'stf::Registers::STF_REG::STF_REG_CSR_VL'
        _STF_REG_CSR_VTYPE 'stf::Registers::STF_REG::STF_REG_CSR_VTYPE'
        _STF_REG_CSR_VLENB 'stf::Registers::STF_REG::STF_REG_CSR_VLENB'

        # Basica User Performance Monitoring Counters
        _STF_REG_CSR_CYCLEH 'stf::Registers::STF_REG::STF_REG_CSR_CYCLEH'
        _STF_REG_CSR_TIMEH 'stf::Registers::STF_REG::STF_REG_CSR_TIMEH'
        _STF_REG_CSR_INSTRETH 'stf::Registers::STF_REG::STF_REG_CSR_INSTRETH'

        # Machine Information Registers
        _STF_REG_CSR_MVENDORID 'stf::Registers::STF_REG::STF_REG_CSR_MVENDORID'
        _STF_REG_CSR_MARCHID 'stf::Registers::STF_REG::STF_REG_CSR_MARCHID'
        _STF_REG_CSR_MIMPID 'stf::Registers::STF_REG::STF_REG_CSR_MIMPID'
        _STF_REG_CSR_MHARTID 'stf::Registers::STF_REG::STF_REG_CSR_MHARTID'
        _STF_REG_END_DEF 'stf::Registers::STF_REG::STF_REG_END_DEF'
        _STF_REG_INVALID 'stf::Registers::STF_REG::STF_REG_INVALID'

    cdef enum _STF_REG_OPERAND_TYPE 'stf::Registers::STF_REG_OPERAND_TYPE':
        _REG_RESERVED 'stf::Registers::STF_REG_OPERAND_TYPE::REG_RESERVED'
        _REG_STATE 'stf::Registers::STF_REG_OPERAND_TYPE::REG_STATE'
        _REG_SOURCE 'stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE'
        _REG_DEST 'stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST'

cpdef enum STF_REG:
    STF_REG_X0 = <STF_REG_int> _STF_REG_X0
    STF_REG_X1 = <STF_REG_int> _STF_REG_X1
    STF_REG_X2 = <STF_REG_int> _STF_REG_X2
    STF_REG_X3 = <STF_REG_int> _STF_REG_X3
    STF_REG_X4 = <STF_REG_int> _STF_REG_X4
    STF_REG_X5 = <STF_REG_int> _STF_REG_X5
    STF_REG_X6 = <STF_REG_int> _STF_REG_X6
    STF_REG_X7 = <STF_REG_int> _STF_REG_X7
    STF_REG_X8 = <STF_REG_int> _STF_REG_X8
    STF_REG_X9 = <STF_REG_int> _STF_REG_X9
    STF_REG_X10 = <STF_REG_int> _STF_REG_X10
    STF_REG_X11 = <STF_REG_int> _STF_REG_X11
    STF_REG_X12 = <STF_REG_int> _STF_REG_X12
    STF_REG_X13 = <STF_REG_int> _STF_REG_X13
    STF_REG_X14 = <STF_REG_int> _STF_REG_X14
    STF_REG_X15 = <STF_REG_int> _STF_REG_X15
    STF_REG_X16 = <STF_REG_int> _STF_REG_X16
    STF_REG_X17 = <STF_REG_int> _STF_REG_X17
    STF_REG_X18 = <STF_REG_int> _STF_REG_X18
    STF_REG_X19 = <STF_REG_int> _STF_REG_X19
    STF_REG_X20 = <STF_REG_int> _STF_REG_X20
    STF_REG_X21 = <STF_REG_int> _STF_REG_X21
    STF_REG_X22 = <STF_REG_int> _STF_REG_X22
    STF_REG_X23 = <STF_REG_int> _STF_REG_X23
    STF_REG_X24 = <STF_REG_int> _STF_REG_X24
    STF_REG_X25 = <STF_REG_int> _STF_REG_X25
    STF_REG_X26 = <STF_REG_int> _STF_REG_X26
    STF_REG_X27 = <STF_REG_int> _STF_REG_X27
    STF_REG_X28 = <STF_REG_int> _STF_REG_X28
    STF_REG_X29 = <STF_REG_int> _STF_REG_X29
    STF_REG_X30 = <STF_REG_int> _STF_REG_X30
    STF_REG_X31 = <STF_REG_int> _STF_REG_X31

    # 32 floating point registers
    STF_REG_F0 = <STF_REG_int> _STF_REG_F0
    STF_REG_F1 = <STF_REG_int> _STF_REG_F1
    STF_REG_F2 = <STF_REG_int> _STF_REG_F2
    STF_REG_F3 = <STF_REG_int> _STF_REG_F3
    STF_REG_F4 = <STF_REG_int> _STF_REG_F4
    STF_REG_F5 = <STF_REG_int> _STF_REG_F5
    STF_REG_F6 = <STF_REG_int> _STF_REG_F6
    STF_REG_F7 = <STF_REG_int> _STF_REG_F7
    STF_REG_F8 = <STF_REG_int> _STF_REG_F8
    STF_REG_F9 = <STF_REG_int> _STF_REG_F9
    STF_REG_F10 = <STF_REG_int> _STF_REG_F10
    STF_REG_F11 = <STF_REG_int> _STF_REG_F11
    STF_REG_F12 = <STF_REG_int> _STF_REG_F12
    STF_REG_F13 = <STF_REG_int> _STF_REG_F13
    STF_REG_F14 = <STF_REG_int> _STF_REG_F14
    STF_REG_F15 = <STF_REG_int> _STF_REG_F15
    STF_REG_F16 = <STF_REG_int> _STF_REG_F16
    STF_REG_F17 = <STF_REG_int> _STF_REG_F17
    STF_REG_F18 = <STF_REG_int> _STF_REG_F18
    STF_REG_F19 = <STF_REG_int> _STF_REG_F19
    STF_REG_F20 = <STF_REG_int> _STF_REG_F20
    STF_REG_F21 = <STF_REG_int> _STF_REG_F21
    STF_REG_F22 = <STF_REG_int> _STF_REG_F22
    STF_REG_F23 = <STF_REG_int> _STF_REG_F23
    STF_REG_F24 = <STF_REG_int> _STF_REG_F24
    STF_REG_F25 = <STF_REG_int> _STF_REG_F25
    STF_REG_F26 = <STF_REG_int> _STF_REG_F26
    STF_REG_F27 = <STF_REG_int> _STF_REG_F27
    STF_REG_F28 = <STF_REG_int> _STF_REG_F28
    STF_REG_F29 = <STF_REG_int> _STF_REG_F29
    STF_REG_F30 = <STF_REG_int> _STF_REG_F30
    STF_REG_F31 = <STF_REG_int> _STF_REG_F31

    # 32 vector registers
    STF_REG_V0 = <STF_REG_int> _STF_REG_V0
    STF_REG_V1 = <STF_REG_int> _STF_REG_V1
    STF_REG_V2 = <STF_REG_int> _STF_REG_V2
    STF_REG_V3 = <STF_REG_int> _STF_REG_V3
    STF_REG_V4 = <STF_REG_int> _STF_REG_V4
    STF_REG_V5 = <STF_REG_int> _STF_REG_V5
    STF_REG_V6 = <STF_REG_int> _STF_REG_V6
    STF_REG_V7 = <STF_REG_int> _STF_REG_V7
    STF_REG_V8 = <STF_REG_int> _STF_REG_V8
    STF_REG_V9 = <STF_REG_int> _STF_REG_V9
    STF_REG_V10 = <STF_REG_int> _STF_REG_V10
    STF_REG_V11 = <STF_REG_int> _STF_REG_V11
    STF_REG_V12 = <STF_REG_int> _STF_REG_V12
    STF_REG_V13 = <STF_REG_int> _STF_REG_V13
    STF_REG_V14 = <STF_REG_int> _STF_REG_V14
    STF_REG_V15 = <STF_REG_int> _STF_REG_V15
    STF_REG_V16 = <STF_REG_int> _STF_REG_V16
    STF_REG_V17 = <STF_REG_int> _STF_REG_V17
    STF_REG_V18 = <STF_REG_int> _STF_REG_V18
    STF_REG_V19 = <STF_REG_int> _STF_REG_V19
    STF_REG_V20 = <STF_REG_int> _STF_REG_V20
    STF_REG_V21 = <STF_REG_int> _STF_REG_V21
    STF_REG_V22 = <STF_REG_int> _STF_REG_V22
    STF_REG_V23 = <STF_REG_int> _STF_REG_V23
    STF_REG_V24 = <STF_REG_int> _STF_REG_V24
    STF_REG_V25 = <STF_REG_int> _STF_REG_V25
    STF_REG_V26 = <STF_REG_int> _STF_REG_V26
    STF_REG_V27 = <STF_REG_int> _STF_REG_V27
    STF_REG_V28 = <STF_REG_int> _STF_REG_V28
    STF_REG_V29 = <STF_REG_int> _STF_REG_V29
    STF_REG_V30 = <STF_REG_int> _STF_REG_V30
    STF_REG_V31 = <STF_REG_int> _STF_REG_V31

    # Control and status registers
    # User
    STF_REG_CSR_USTATUS = <STF_REG_int> _STF_REG_CSR_USTATUS
    STF_REG_CSR_FFLAGS = <STF_REG_int> _STF_REG_CSR_FFLAGS
    STF_REG_CSR_FRM = <STF_REG_int> _STF_REG_CSR_FRM
    STF_REG_CSR_FCSR = <STF_REG_int> _STF_REG_CSR_FCSR
    STF_REG_CSR_UIE = <STF_REG_int> _STF_REG_CSR_UIE
    STF_REG_CSR_UTVEC = <STF_REG_int> _STF_REG_CSR_UTVEC
    STF_REG_CSR_UTVT = <STF_REG_int> _STF_REG_CSR_UTVT
    STF_REG_CSR_VSTART = <STF_REG_int> _STF_REG_CSR_VSTART
    STF_REG_CSR_VXSAT = <STF_REG_int> _STF_REG_CSR_VXSAT
    STF_REG_CSR_VXRM = <STF_REG_int> _STF_REG_CSR_VXRM
    STF_REG_CSR_VCSR = <STF_REG_int> _STF_REG_CSR_VCSR
    STF_REG_CSR_USCRATCH = <STF_REG_int> _STF_REG_CSR_USCRATCH
    STF_REG_CSR_UEPC = <STF_REG_int> _STF_REG_CSR_UEPC
    STF_REG_CSR_UCAUSE = <STF_REG_int> _STF_REG_CSR_UCAUSE
    STF_REG_CSR_UTVAL = <STF_REG_int> _STF_REG_CSR_UTVAL
    STF_REG_CSR_UIP = <STF_REG_int> _STF_REG_CSR_UIP
    STF_REG_CSR_UNXTI = <STF_REG_int> _STF_REG_CSR_UNXTI
    STF_REG_CSR_UINTSTATUS = <STF_REG_int> _STF_REG_CSR_UINTSTATUS
    STF_REG_CSR_USCRATCHCSW = <STF_REG_int> _STF_REG_CSR_USCRATCHCSW
    STF_REG_CSR_USCRATCHCSWL = <STF_REG_int> _STF_REG_CSR_USCRATCHCSWL

    # Supervisor
    STF_REG_CSR_SSTATUS = <STF_REG_int> _STF_REG_CSR_SSTATUS
    STF_REG_CSR_SIE = <STF_REG_int> _STF_REG_CSR_SIE
    STF_REG_CSR_STVEC = <STF_REG_int> _STF_REG_CSR_STVEC
    STF_REG_CSR_SCOUNTEREN = <STF_REG_int> _STF_REG_CSR_SCOUNTEREN
    STF_REG_CSR_STVT = <STF_REG_int> _STF_REG_CSR_STVT
    STF_REG_CSR_SSCRATCH = <STF_REG_int> _STF_REG_CSR_SSCRATCH
    STF_REG_CSR_SEPC = <STF_REG_int> _STF_REG_CSR_SEPC
    STF_REG_CSR_SCAUSE = <STF_REG_int> _STF_REG_CSR_SCAUSE
    STF_REG_CSR_STVAL = <STF_REG_int> _STF_REG_CSR_STVAL
    STF_REG_CSR_SIP = <STF_REG_int> _STF_REG_CSR_SIP
    STF_REG_CSR_SNXTI = <STF_REG_int> _STF_REG_CSR_SNXTI
    STF_REG_CSR_SINTSTATUS = <STF_REG_int> _STF_REG_CSR_SINTSTATUS
    STF_REG_CSR_SSCRATCHCSW = <STF_REG_int> _STF_REG_CSR_SSCRATCHCSW
    STF_REG_CSR_SSCRATCHCSWL = <STF_REG_int> _STF_REG_CSR_SSCRATCHCSWL
    STF_REG_CSR_SATP = <STF_REG_int> _STF_REG_CSR_SATP

    # ???
    STF_REG_CSR_VSSTATUS = <STF_REG_int> _STF_REG_CSR_VSSTATUS
    STF_REG_CSR_VSIE = <STF_REG_int> _STF_REG_CSR_VSIE
    STF_REG_CSR_VSTVEC = <STF_REG_int> _STF_REG_CSR_VSTVEC
    STF_REG_CSR_VSSCRATCH = <STF_REG_int> _STF_REG_CSR_VSSCRATCH
    STF_REG_CSR_VSEPC = <STF_REG_int> _STF_REG_CSR_VSEPC
    STF_REG_CSR_VSCAUSE = <STF_REG_int> _STF_REG_CSR_VSCAUSE
    STF_REG_CSR_VSTVAL = <STF_REG_int> _STF_REG_CSR_VSTVAL
    STF_REG_CSR_VSIP = <STF_REG_int> _STF_REG_CSR_VSIP
    STF_REG_CSR_VSATP = <STF_REG_int> _STF_REG_CSR_VSATP

    # Machine
    STF_REG_CSR_MSTATUS = <STF_REG_int> _STF_REG_CSR_MSTATUS
    STF_REG_CSR_MISA = <STF_REG_int> _STF_REG_CSR_MISA
    STF_REG_CSR_MEDELEG = <STF_REG_int> _STF_REG_CSR_MEDELEG
    STF_REG_CSR_MIDELEG = <STF_REG_int> _STF_REG_CSR_MIDELEG
    STF_REG_CSR_MIE = <STF_REG_int> _STF_REG_CSR_MIE
    STF_REG_CSR_MTVEC = <STF_REG_int> _STF_REG_CSR_MTVEC
    STF_REG_CSR_MCOUNTEREN = <STF_REG_int> _STF_REG_CSR_MCOUNTEREN
    STF_REG_CSR_MTVT = <STF_REG_int> _STF_REG_CSR_MTVT
    STF_REG_CSR_MSCRATCH = <STF_REG_int> _STF_REG_CSR_MSCRATCH
    STF_REG_CSR_MEPC = <STF_REG_int> _STF_REG_CSR_MEPC
    STF_REG_CSR_MCAUSE = <STF_REG_int> _STF_REG_CSR_MCAUSE
    STF_REG_CSR_MTVAL = <STF_REG_int> _STF_REG_CSR_MTVAL
    STF_REG_CSR_MIP = <STF_REG_int> _STF_REG_CSR_MIP
    STF_REG_CSR_MNXTI = <STF_REG_int> _STF_REG_CSR_MNXTI
    STF_REG_CSR_MINTSTATUS = <STF_REG_int> _STF_REG_CSR_MINTSTATUS
    STF_REG_CSR_MSCRATCHCSW = <STF_REG_int> _STF_REG_CSR_MSCRATCHCSW
    STF_REG_CSR_MSCRATCHCSWL = <STF_REG_int> _STF_REG_CSR_MSCRATCHCSWL

    # Machine Memory Protection
    STF_REG_CSR_PMPCFG0 = <STF_REG_int> _STF_REG_CSR_PMPCFG0
    STF_REG_CSR_PMPCFG1 = <STF_REG_int> _STF_REG_CSR_PMPCFG1
    STF_REG_CSR_PMPCFG2 = <STF_REG_int> _STF_REG_CSR_PMPCFG2
    STF_REG_CSR_PMPCFG3 = <STF_REG_int> _STF_REG_CSR_PMPCFG3
    STF_REG_CSR_PMPADDR0 = <STF_REG_int> _STF_REG_CSR_PMPADDR0
    STF_REG_CSR_PMPADDR1 = <STF_REG_int> _STF_REG_CSR_PMPADDR1
    STF_REG_CSR_PMPADDR2 = <STF_REG_int> _STF_REG_CSR_PMPADDR2
    STF_REG_CSR_PMPADDR3 = <STF_REG_int> _STF_REG_CSR_PMPADDR3
    STF_REG_CSR_PMPADDR4 = <STF_REG_int> _STF_REG_CSR_PMPADDR4
    STF_REG_CSR_PMPADDR5 = <STF_REG_int> _STF_REG_CSR_PMPADDR5
    STF_REG_CSR_PMPADDR6 = <STF_REG_int> _STF_REG_CSR_PMPADDR6
    STF_REG_CSR_PMPADDR7 = <STF_REG_int> _STF_REG_CSR_PMPADDR7
    STF_REG_CSR_PMPADDR8 = <STF_REG_int> _STF_REG_CSR_PMPADDR8
    STF_REG_CSR_PMPADDR9 = <STF_REG_int> _STF_REG_CSR_PMPADDR9
    STF_REG_CSR_PMPADDR10 = <STF_REG_int> _STF_REG_CSR_PMPADDR10
    STF_REG_CSR_PMPADDR11 = <STF_REG_int> _STF_REG_CSR_PMPADDR11
    STF_REG_CSR_PMPADDR12 = <STF_REG_int> _STF_REG_CSR_PMPADDR12
    STF_REG_CSR_PMPADDR13 = <STF_REG_int> _STF_REG_CSR_PMPADDR13
    STF_REG_CSR_PMPADDR14 = <STF_REG_int> _STF_REG_CSR_PMPADDR14
    STF_REG_CSR_PMPADDR15 = <STF_REG_int> _STF_REG_CSR_PMPADDR15

    # ???
    STF_REG_CSR_HSTATUS = <STF_REG_int> _STF_REG_CSR_HSTATUS
    STF_REG_CSR_HEDELEG = <STF_REG_int> _STF_REG_CSR_HEDELEG
    STF_REG_CSR_HIDELEG = <STF_REG_int> _STF_REG_CSR_HIDELEG
    STF_REG_CSR_HCOUNTEREN = <STF_REG_int> _STF_REG_CSR_HCOUNTEREN
    STF_REG_CSR_HGATP = <STF_REG_int> _STF_REG_CSR_HGATP

    # Basic Machine Counters
    STF_REG_CSR_MCYCLE = <STF_REG_int> _STF_REG_CSR_MCYCLE
    STF_REG_CSR_MINSTRET = <STF_REG_int> _STF_REG_CSR_MINSTRET

    # Basic Machine Performance Monitoring Counters
    STF_REG_CSR_MCYCLEH = <STF_REG_int> _STF_REG_CSR_MCYCLEH
    STF_REG_CSR_MINSTRETH = <STF_REG_int> _STF_REG_CSR_MINSTRETH

    # Basic User Counters
    STF_REG_CSR_CYCLE = <STF_REG_int> _STF_REG_CSR_CYCLE
    STF_REG_CSR_TIME = <STF_REG_int> _STF_REG_CSR_TIME
    STF_REG_CSR_INSTRET = <STF_REG_int> _STF_REG_CSR_INSTRET

    # Vector
    STF_REG_CSR_VL = <STF_REG_int> _STF_REG_CSR_VL
    STF_REG_CSR_VTYPE = <STF_REG_int> _STF_REG_CSR_VTYPE
    STF_REG_CSR_VLENB = <STF_REG_int> _STF_REG_CSR_VLENB

    # Basica User Performance Monitoring Counters
    STF_REG_CSR_CYCLEH = <STF_REG_int> _STF_REG_CSR_CYCLEH
    STF_REG_CSR_TIMEH = <STF_REG_int> _STF_REG_CSR_TIMEH
    STF_REG_CSR_INSTRETH = <STF_REG_int> _STF_REG_CSR_INSTRETH

    # Machine Information Registers
    STF_REG_CSR_MVENDORID = <STF_REG_int> _STF_REG_CSR_MVENDORID
    STF_REG_CSR_MARCHID = <STF_REG_int> _STF_REG_CSR_MARCHID
    STF_REG_CSR_MIMPID = <STF_REG_int> _STF_REG_CSR_MIMPID
    STF_REG_CSR_MHARTID = <STF_REG_int> _STF_REG_CSR_MHARTID
    STF_REG_END_DEF = <STF_REG_int> _STF_REG_END_DEF
    STF_REG_INVALID = <STF_REG_int> _STF_REG_INVALID

cpdef enum STF_REG_OPERAND_TYPE:
    REG_RESERVED = <STF_REG_metadata_int> _REG_RESERVED
    REG_STATE = <STF_REG_metadata_int> _REG_STATE
    REG_SOURCE = <STF_REG_metadata_int> _REG_SOURCE
    REG_DEST = <STF_REG_metadata_int> _REG_DEST
