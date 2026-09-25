#include <ostream>
#include <iomanip>
#include "format_utils.hpp"
#include "stf_reg_def.hpp"

namespace stf {
    void Registers::formatCSR_(std::ostream& os, const Registers::STF_REG regno) {
        switch(regno) {
            case STF_REG::STF_REG_CSR_FFLAGS:
                os << "REG_CSR_FFLAGS";
                return;
            case STF_REG::STF_REG_CSR_FRM:
                os << "REG_CSR_FRM";
                return;
            case STF_REG::STF_REG_CSR_FCSR:
                os << "REG_CSR_FCSR";
                return;
            case STF_REG::STF_REG_CSR_VSTART:
                os << "REG_CSR_VSTART";
                return;
            case STF_REG::STF_REG_CSR_VXSAT:
                os << "REG_CSR_VXSAT";
                return;
            case STF_REG::STF_REG_CSR_VXRM:
                os << "REG_CSR_VXRM";
                return;
            case STF_REG::STF_REG_CSR_VCSR:
                os << "REG_CSR_VCSR";
                return;
            case STF_REG::STF_REG_CSR_SSP:
                os << "REG_CSR_SSP";
                return;
            case STF_REG::STF_REG_CSR_SEED:
                os << "REG_CSR_SEED";
                return;
            case STF_REG::STF_REG_CSR_JVT:
                os << "REG_CSR_JVT";
                return;
            case STF_REG::STF_REG_CSR_SSTATUS:
                os << "REG_CSR_SSTATUS";
                return;
            case STF_REG::STF_REG_CSR_SEDELEG:
                os << "REG_CSR_SEDELEG";
                return;
            case STF_REG::STF_REG_CSR_SIDELEG:
                os << "REG_CSR_SIDELEG";
                return;
            case STF_REG::STF_REG_CSR_SIE:
                os << "REG_CSR_SIE";
                return;
            case STF_REG::STF_REG_CSR_STVEC:
                os << "REG_CSR_STVEC";
                return;
            case STF_REG::STF_REG_CSR_SCOUNTEREN:
                os << "REG_CSR_SCOUNTEREN";
                return;
            case STF_REG::STF_REG_CSR_SENVCFG:
                os << "REG_CSR_SENVCFG";
                return;
            case STF_REG::STF_REG_CSR_SSTATEEN0:
                os << "REG_CSR_SSTATEEN0";
                return;
            case STF_REG::STF_REG_CSR_SSTATEEN1:
                os << "REG_CSR_SSTATEEN1";
                return;
            case STF_REG::STF_REG_CSR_SSTATEEN2:
                os << "REG_CSR_SSTATEEN2";
                return;
            case STF_REG::STF_REG_CSR_SSTATEEN3:
                os << "REG_CSR_SSTATEEN3";
                return;
            case STF_REG::STF_REG_CSR_SIEH:
                os << "REG_CSR_SIEH";
                return;
            case STF_REG::STF_REG_CSR_SCOUNTINHIBIT:
                os << "REG_CSR_SCOUNTINHIBIT";
                return;
            case STF_REG::STF_REG_CSR_SSCRATCH:
                os << "REG_CSR_SSCRATCH";
                return;
            case STF_REG::STF_REG_CSR_SEPC:
                os << "REG_CSR_SEPC";
                return;
            case STF_REG::STF_REG_CSR_SCAUSE:
                os << "REG_CSR_SCAUSE";
                return;
            case STF_REG::STF_REG_CSR_STVAL:
                os << "REG_CSR_STVAL";
                return;
            case STF_REG::STF_REG_CSR_SIP:
                os << "REG_CSR_SIP";
                return;
            case STF_REG::STF_REG_CSR_STIMECMP:
                os << "REG_CSR_STIMECMP";
                return;
            case STF_REG::STF_REG_CSR_SCTRCTL:
                os << "REG_CSR_SCTRCTL";
                return;
            case STF_REG::STF_REG_CSR_SCTRSTATUS:
                os << "REG_CSR_SCTRSTATUS";
                return;
            case STF_REG::STF_REG_CSR_SISELECT:
                os << "REG_CSR_SISELECT";
                return;
            case STF_REG::STF_REG_CSR_SIREG:
                os << "REG_CSR_SIREG";
                return;
            case STF_REG::STF_REG_CSR_SIREG2:
                os << "REG_CSR_SIREG2";
                return;
            case STF_REG::STF_REG_CSR_SIREG3:
                os << "REG_CSR_SIREG3";
                return;
            case STF_REG::STF_REG_CSR_SIPH:
                os << "REG_CSR_SIPH";
                return;
            case STF_REG::STF_REG_CSR_SIREG4:
                os << "REG_CSR_SIREG4";
                return;
            case STF_REG::STF_REG_CSR_SIREG5:
                os << "REG_CSR_SIREG5";
                return;
            case STF_REG::STF_REG_CSR_SIREG6:
                os << "REG_CSR_SIREG6";
                return;
            case STF_REG::STF_REG_CSR_STOPEI:
                os << "REG_CSR_STOPEI";
                return;
            case STF_REG::STF_REG_CSR_STIMECMPH:
                os << "REG_CSR_STIMECMPH";
                return;
            case STF_REG::STF_REG_CSR_SCTRDEPTH:
                os << "REG_CSR_SCTRDEPTH";
                return;
            case STF_REG::STF_REG_CSR_SATP:
                os << "REG_CSR_SATP";
                return;
            case STF_REG::STF_REG_CSR_SRMCFG:
                os << "REG_CSR_SRMCFG";
                return;
            case STF_REG::STF_REG_CSR_SENVCFG_COMPAT:
                os << "REG_CSR_SENVCFG";
                return;
            case STF_REG::STF_REG_CSR_TSELECT:
                os << "REG_CSR_TSELECT";
                return;
            case STF_REG::STF_REG_CSR_TDATA1:
                os << "REG_CSR_TDATA1";
                return;
            case STF_REG::STF_REG_CSR_TDATA2:
                os << "REG_CSR_TDATA2";
                return;
            case STF_REG::STF_REG_CSR_TDATA3:
                os << "REG_CSR_TDATA3";
                return;
            case STF_REG::STF_REG_CSR_TINFO:
                os << "REG_CSR_TINFO";
                return;
            case STF_REG::STF_REG_CSR_TCONTROL:
                os << "REG_CSR_TCONTROL";
                return;
            case STF_REG::STF_REG_CSR_MCONTEXT:
                os << "REG_CSR_MCONTEXT";
                return;
            case STF_REG::STF_REG_CSR_MSCONTEXT:
                os << "REG_CSR_MSCONTEXT";
                return;
            case STF_REG::STF_REG_CSR_DCSR:
                os << "REG_CSR_DCSR";
                return;
            case STF_REG::STF_REG_CSR_DPC:
                os << "REG_CSR_DPC";
                return;
            case STF_REG::STF_REG_CSR_DSCRATCH0:
                os << "REG_CSR_DSCRATCH0";
                return;
            case STF_REG::STF_REG_CSR_DSCRATCH1:
                os << "REG_CSR_DSCRATCH1";
                return;
            case STF_REG::STF_REG_CSR_VSSTATUS:
                os << "REG_CSR_VSSTATUS";
                return;
            case STF_REG::STF_REG_CSR_VSIE:
                os << "REG_CSR_VSIE";
                return;
            case STF_REG::STF_REG_CSR_VSTVEC:
                os << "REG_CSR_VSTVEC";
                return;
            case STF_REG::STF_REG_CSR_VSIEH:
                os << "REG_CSR_VSIEH";
                return;
            case STF_REG::STF_REG_CSR_VSSCRATCH:
                os << "REG_CSR_VSSCRATCH";
                return;
            case STF_REG::STF_REG_CSR_VSEPC:
                os << "REG_CSR_VSEPC";
                return;
            case STF_REG::STF_REG_CSR_VSCAUSE:
                os << "REG_CSR_VSCAUSE";
                return;
            case STF_REG::STF_REG_CSR_VSTVAL:
                os << "REG_CSR_VSTVAL";
                return;
            case STF_REG::STF_REG_CSR_VSIP:
                os << "REG_CSR_VSIP";
                return;
            case STF_REG::STF_REG_CSR_VSTIMECMP:
                os << "REG_CSR_VSTIMECMP";
                return;
            case STF_REG::STF_REG_CSR_VSCTRCTL:
                os << "REG_CSR_VSCTRCTL";
                return;
            case STF_REG::STF_REG_CSR_VSISELECT:
                os << "REG_CSR_VSISELECT";
                return;
            case STF_REG::STF_REG_CSR_VSIREG:
                os << "REG_CSR_VSIREG";
                return;
            case STF_REG::STF_REG_CSR_VSIREG2:
                os << "REG_CSR_VSIREG2";
                return;
            case STF_REG::STF_REG_CSR_VSIREG3:
                os << "REG_CSR_VSIREG3";
                return;
            case STF_REG::STF_REG_CSR_VSIPH:
                os << "REG_CSR_VSIPH";
                return;
            case STF_REG::STF_REG_CSR_VSIREG4:
                os << "REG_CSR_VSIREG4";
                return;
            case STF_REG::STF_REG_CSR_VSIREG5:
                os << "REG_CSR_VSIREG5";
                return;
            case STF_REG::STF_REG_CSR_VSIREG6:
                os << "REG_CSR_VSIREG6";
                return;
            case STF_REG::STF_REG_CSR_VSTOPEI:
                os << "REG_CSR_VSTOPEI";
                return;
            case STF_REG::STF_REG_CSR_VSTIMECMPH:
                os << "REG_CSR_VSTIMECMPH";
                return;
            case STF_REG::STF_REG_CSR_VSATP:
                os << "REG_CSR_VSATP";
                return;
            case STF_REG::STF_REG_CSR_MSTATUS:
                os << "REG_CSR_MSTATUS";
                return;
            case STF_REG::STF_REG_CSR_MISA:
                os << "REG_CSR_MISA";
                return;
            case STF_REG::STF_REG_CSR_MEDELEG:
                os << "REG_CSR_MEDELEG";
                return;
            case STF_REG::STF_REG_CSR_MIDELEG:
                os << "REG_CSR_MIDELEG";
                return;
            case STF_REG::STF_REG_CSR_MIE:
                os << "REG_CSR_MIE";
                return;
            case STF_REG::STF_REG_CSR_MTVEC:
                os << "REG_CSR_MTVEC";
                return;
            case STF_REG::STF_REG_CSR_MCOUNTEREN:
                os << "REG_CSR_MCOUNTEREN";
                return;
            case STF_REG::STF_REG_CSR_MVIEN:
                os << "REG_CSR_MVIEN";
                return;
            case STF_REG::STF_REG_CSR_MVIP:
                os << "REG_CSR_MVIP";
                return;
            case STF_REG::STF_REG_CSR_MENVCFG:
                os << "REG_CSR_MENVCFG";
                return;
            case STF_REG::STF_REG_CSR_MSTATEEN0:
                os << "REG_CSR_MSTATEEN0";
                return;
            case STF_REG::STF_REG_CSR_MSTATEEN1:
                os << "REG_CSR_MSTATEEN1";
                return;
            case STF_REG::STF_REG_CSR_MSTATEEN2:
                os << "REG_CSR_MSTATEEN2";
                return;
            case STF_REG::STF_REG_CSR_MSTATEEN3:
                os << "REG_CSR_MSTATEEN3";
                return;
            case STF_REG::STF_REG_CSR_MSTATUSH:
                os << "REG_CSR_MSTATUSH";
                return;
            case STF_REG::STF_REG_CSR_MEDELEGH:
                os << "REG_CSR_MEDELEGH";
                return;
            case STF_REG::STF_REG_CSR_MIDELEGH:
                os << "REG_CSR_MIDELEGH";
                return;
            case STF_REG::STF_REG_CSR_MIEH:
                os << "REG_CSR_MIEH";
                return;
            case STF_REG::STF_REG_CSR_MVIENH:
                os << "REG_CSR_MVIENH";
                return;
            case STF_REG::STF_REG_CSR_MVIPH:
                os << "REG_CSR_MVIPH";
                return;
            case STF_REG::STF_REG_CSR_MENVCFGH:
                os << "REG_CSR_MENVCFGH";
                return;
            case STF_REG::STF_REG_CSR_MSTATEEN0H:
                os << "REG_CSR_MSTATEEN0H";
                return;
            case STF_REG::STF_REG_CSR_MSTATEEN1H:
                os << "REG_CSR_MSTATEEN1H";
                return;
            case STF_REG::STF_REG_CSR_MSTATEEN2H:
                os << "REG_CSR_MSTATEEN2H";
                return;
            case STF_REG::STF_REG_CSR_MSTATEEN3H:
                os << "REG_CSR_MSTATEEN3H";
                return;
            case STF_REG::STF_REG_CSR_MCOUNTINHIBIT:
                os << "REG_CSR_MCOUNTINHIBIT";
                return;
            case STF_REG::STF_REG_CSR_MCYCLECFG:
                os << "REG_CSR_MCYCLECFG";
                return;
            case STF_REG::STF_REG_CSR_MINSTRETCFG:
                os << "REG_CSR_MINSTRETCFG";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT3:
                os << "REG_CSR_MHPMEVENT3";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT4:
                os << "REG_CSR_MHPMEVENT4";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT5:
                os << "REG_CSR_MHPMEVENT5";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT6:
                os << "REG_CSR_MHPMEVENT6";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT7:
                os << "REG_CSR_MHPMEVENT7";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT8:
                os << "REG_CSR_MHPMEVENT8";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT9:
                os << "REG_CSR_MHPMEVENT9";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT10:
                os << "REG_CSR_MHPMEVENT10";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT11:
                os << "REG_CSR_MHPMEVENT11";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT12:
                os << "REG_CSR_MHPMEVENT12";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT13:
                os << "REG_CSR_MHPMEVENT13";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT14:
                os << "REG_CSR_MHPMEVENT14";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT15:
                os << "REG_CSR_MHPMEVENT15";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT16:
                os << "REG_CSR_MHPMEVENT16";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT17:
                os << "REG_CSR_MHPMEVENT17";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT18:
                os << "REG_CSR_MHPMEVENT18";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT19:
                os << "REG_CSR_MHPMEVENT19";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT20:
                os << "REG_CSR_MHPMEVENT20";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT21:
                os << "REG_CSR_MHPMEVENT21";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT22:
                os << "REG_CSR_MHPMEVENT22";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT23:
                os << "REG_CSR_MHPMEVENT23";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT24:
                os << "REG_CSR_MHPMEVENT24";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT25:
                os << "REG_CSR_MHPMEVENT25";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT26:
                os << "REG_CSR_MHPMEVENT26";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT27:
                os << "REG_CSR_MHPMEVENT27";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT28:
                os << "REG_CSR_MHPMEVENT28";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT29:
                os << "REG_CSR_MHPMEVENT29";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT30:
                os << "REG_CSR_MHPMEVENT30";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT31:
                os << "REG_CSR_MHPMEVENT31";
                return;
            case STF_REG::STF_REG_CSR_MSCRATCH:
                os << "REG_CSR_MSCRATCH";
                return;
            case STF_REG::STF_REG_CSR_MEPC:
                os << "REG_CSR_MEPC";
                return;
            case STF_REG::STF_REG_CSR_MCAUSE:
                os << "REG_CSR_MCAUSE";
                return;
            case STF_REG::STF_REG_CSR_MTVAL:
                os << "REG_CSR_MTVAL";
                return;
            case STF_REG::STF_REG_CSR_MIP:
                os << "REG_CSR_MIP";
                return;
            case STF_REG::STF_REG_CSR_MTINST:
                os << "REG_CSR_MTINST";
                return;
            case STF_REG::STF_REG_CSR_MTVAL2:
                os << "REG_CSR_MTVAL2";
                return;
            case STF_REG::STF_REG_CSR_MCTRCTL:
                os << "REG_CSR_MCTRCTL";
                return;
            case STF_REG::STF_REG_CSR_MISELECT:
                os << "REG_CSR_MISELECT";
                return;
            case STF_REG::STF_REG_CSR_MIREG:
                os << "REG_CSR_MIREG";
                return;
            case STF_REG::STF_REG_CSR_MIREG2:
                os << "REG_CSR_MIREG2";
                return;
            case STF_REG::STF_REG_CSR_MIREG3:
                os << "REG_CSR_MIREG3";
                return;
            case STF_REG::STF_REG_CSR_MIPH:
                os << "REG_CSR_MIPH";
                return;
            case STF_REG::STF_REG_CSR_MIREG4:
                os << "REG_CSR_MIREG4";
                return;
            case STF_REG::STF_REG_CSR_MIREG5:
                os << "REG_CSR_MIREG5";
                return;
            case STF_REG::STF_REG_CSR_MIREG6:
                os << "REG_CSR_MIREG6";
                return;
            case STF_REG::STF_REG_CSR_MTOPEI:
                os << "REG_CSR_MTOPEI";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG0:
                os << "REG_CSR_PMPCFG0";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG1:
                os << "REG_CSR_PMPCFG1";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG2:
                os << "REG_CSR_PMPCFG2";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG3:
                os << "REG_CSR_PMPCFG3";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG4:
                os << "REG_CSR_PMPCFG4";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG5:
                os << "REG_CSR_PMPCFG5";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG6:
                os << "REG_CSR_PMPCFG6";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG7:
                os << "REG_CSR_PMPCFG7";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG8:
                os << "REG_CSR_PMPCFG8";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG9:
                os << "REG_CSR_PMPCFG9";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG10:
                os << "REG_CSR_PMPCFG10";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG11:
                os << "REG_CSR_PMPCFG11";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG12:
                os << "REG_CSR_PMPCFG12";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG13:
                os << "REG_CSR_PMPCFG13";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG14:
                os << "REG_CSR_PMPCFG14";
                return;
            case STF_REG::STF_REG_CSR_PMPCFG15:
                os << "REG_CSR_PMPCFG15";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR0:
                os << "REG_CSR_PMPADDR0";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR1:
                os << "REG_CSR_PMPADDR1";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR2:
                os << "REG_CSR_PMPADDR2";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR3:
                os << "REG_CSR_PMPADDR3";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR4:
                os << "REG_CSR_PMPADDR4";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR5:
                os << "REG_CSR_PMPADDR5";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR6:
                os << "REG_CSR_PMPADDR6";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR7:
                os << "REG_CSR_PMPADDR7";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR8:
                os << "REG_CSR_PMPADDR8";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR9:
                os << "REG_CSR_PMPADDR9";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR10:
                os << "REG_CSR_PMPADDR10";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR11:
                os << "REG_CSR_PMPADDR11";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR12:
                os << "REG_CSR_PMPADDR12";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR13:
                os << "REG_CSR_PMPADDR13";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR14:
                os << "REG_CSR_PMPADDR14";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR15:
                os << "REG_CSR_PMPADDR15";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR16:
                os << "REG_CSR_PMPADDR16";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR17:
                os << "REG_CSR_PMPADDR17";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR18:
                os << "REG_CSR_PMPADDR18";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR19:
                os << "REG_CSR_PMPADDR19";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR20:
                os << "REG_CSR_PMPADDR20";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR21:
                os << "REG_CSR_PMPADDR21";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR22:
                os << "REG_CSR_PMPADDR22";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR23:
                os << "REG_CSR_PMPADDR23";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR24:
                os << "REG_CSR_PMPADDR24";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR25:
                os << "REG_CSR_PMPADDR25";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR26:
                os << "REG_CSR_PMPADDR26";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR27:
                os << "REG_CSR_PMPADDR27";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR28:
                os << "REG_CSR_PMPADDR28";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR29:
                os << "REG_CSR_PMPADDR29";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR30:
                os << "REG_CSR_PMPADDR30";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR31:
                os << "REG_CSR_PMPADDR31";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR32:
                os << "REG_CSR_PMPADDR32";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR33:
                os << "REG_CSR_PMPADDR33";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR34:
                os << "REG_CSR_PMPADDR34";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR35:
                os << "REG_CSR_PMPADDR35";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR36:
                os << "REG_CSR_PMPADDR36";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR37:
                os << "REG_CSR_PMPADDR37";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR38:
                os << "REG_CSR_PMPADDR38";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR39:
                os << "REG_CSR_PMPADDR39";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR40:
                os << "REG_CSR_PMPADDR40";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR41:
                os << "REG_CSR_PMPADDR41";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR42:
                os << "REG_CSR_PMPADDR42";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR43:
                os << "REG_CSR_PMPADDR43";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR44:
                os << "REG_CSR_PMPADDR44";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR45:
                os << "REG_CSR_PMPADDR45";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR46:
                os << "REG_CSR_PMPADDR46";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR47:
                os << "REG_CSR_PMPADDR47";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR48:
                os << "REG_CSR_PMPADDR48";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR49:
                os << "REG_CSR_PMPADDR49";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR50:
                os << "REG_CSR_PMPADDR50";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR51:
                os << "REG_CSR_PMPADDR51";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR52:
                os << "REG_CSR_PMPADDR52";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR53:
                os << "REG_CSR_PMPADDR53";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR54:
                os << "REG_CSR_PMPADDR54";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR55:
                os << "REG_CSR_PMPADDR55";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR56:
                os << "REG_CSR_PMPADDR56";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR57:
                os << "REG_CSR_PMPADDR57";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR58:
                os << "REG_CSR_PMPADDR58";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR59:
                os << "REG_CSR_PMPADDR59";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR60:
                os << "REG_CSR_PMPADDR60";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR61:
                os << "REG_CSR_PMPADDR61";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR62:
                os << "REG_CSR_PMPADDR62";
                return;
            case STF_REG::STF_REG_CSR_PMPADDR63:
                os << "REG_CSR_PMPADDR63";
                return;
            case STF_REG::STF_REG_CSR_SCONTEXT:
                os << "REG_CSR_SCONTEXT";
                return;
            case STF_REG::STF_REG_CSR_HSTATUS:
                os << "REG_CSR_HSTATUS";
                return;
            case STF_REG::STF_REG_CSR_HEDELEG:
                os << "REG_CSR_HEDELEG";
                return;
            case STF_REG::STF_REG_CSR_HIDELEG:
                os << "REG_CSR_HIDELEG";
                return;
            case STF_REG::STF_REG_CSR_HIE:
                os << "REG_CSR_HIE";
                return;
            case STF_REG::STF_REG_CSR_HTIMEDELTA:
                os << "REG_CSR_HTIMEDELTA";
                return;
            case STF_REG::STF_REG_CSR_HCOUNTEREN:
                os << "REG_CSR_HCOUNTEREN";
                return;
            case STF_REG::STF_REG_CSR_HGEIE:
                os << "REG_CSR_HGEIE";
                return;
            case STF_REG::STF_REG_CSR_HVIEN:
                os << "REG_CSR_HVIEN";
                return;
            case STF_REG::STF_REG_CSR_HVICTL:
                os << "REG_CSR_HVICTL";
                return;
            case STF_REG::STF_REG_CSR_HENVCFG:
                os << "REG_CSR_HENVCFG";
                return;
            case STF_REG::STF_REG_CSR_HSTATEEN0:
                os << "REG_CSR_HSTATEEN0";
                return;
            case STF_REG::STF_REG_CSR_HSTATEEN1:
                os << "REG_CSR_HSTATEEN1";
                return;
            case STF_REG::STF_REG_CSR_HSTATEEN2:
                os << "REG_CSR_HSTATEEN2";
                return;
            case STF_REG::STF_REG_CSR_HSTATEEN3:
                os << "REG_CSR_HSTATEEN3";
                return;
            case STF_REG::STF_REG_CSR_HEDELEGH:
                os << "REG_CSR_HEDELEGH";
                return;
            case STF_REG::STF_REG_CSR_HIDELEGH:
                os << "REG_CSR_HIDELEGH";
                return;
            case STF_REG::STF_REG_CSR_HTIMEDELTAH:
                os << "REG_CSR_HTIMEDELTAH";
                return;
            case STF_REG::STF_REG_CSR_HVIENH:
                os << "REG_CSR_HVIENH";
                return;
            case STF_REG::STF_REG_CSR_HENVCFGH:
                os << "REG_CSR_HENVCFGH";
                return;
            case STF_REG::STF_REG_CSR_HSTATEEN0H:
                os << "REG_CSR_HSTATEEN0H";
                return;
            case STF_REG::STF_REG_CSR_HSTATEEN1H:
                os << "REG_CSR_HSTATEEN1H";
                return;
            case STF_REG::STF_REG_CSR_HSTATEEN2H:
                os << "REG_CSR_HSTATEEN2H";
                return;
            case STF_REG::STF_REG_CSR_HSTATEEN3H:
                os << "REG_CSR_HSTATEEN3H";
                return;
            case STF_REG::STF_REG_CSR_HTVAL:
                os << "REG_CSR_HTVAL";
                return;
            case STF_REG::STF_REG_CSR_HIP:
                os << "REG_CSR_HIP";
                return;
            case STF_REG::STF_REG_CSR_HVIP:
                os << "REG_CSR_HVIP";
                return;
            case STF_REG::STF_REG_CSR_HVIPRIO1:
                os << "REG_CSR_HVIPRIO1";
                return;
            case STF_REG::STF_REG_CSR_HVIPRIO2:
                os << "REG_CSR_HVIPRIO2";
                return;
            case STF_REG::STF_REG_CSR_HTINST:
                os << "REG_CSR_HTINST";
                return;
            case STF_REG::STF_REG_CSR_HVIPH:
                os << "REG_CSR_HVIPH";
                return;
            case STF_REG::STF_REG_CSR_HVIPRIO1H:
                os << "REG_CSR_HVIPRIO1H";
                return;
            case STF_REG::STF_REG_CSR_HVIPRIO2H:
                os << "REG_CSR_HVIPRIO2H";
                return;
            case STF_REG::STF_REG_CSR_HGATP:
                os << "REG_CSR_HGATP";
                return;
            case STF_REG::STF_REG_CSR_HCONTEXT:
                os << "REG_CSR_HCONTEXT";
                return;
            case STF_REG::STF_REG_CSR_MCYCLECFGH:
                os << "REG_CSR_MCYCLECFGH";
                return;
            case STF_REG::STF_REG_CSR_MINSTRETCFGH:
                os << "REG_CSR_MINSTRETCFGH";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT3H:
                os << "REG_CSR_MHPMEVENT3H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT4H:
                os << "REG_CSR_MHPMEVENT4H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT5H:
                os << "REG_CSR_MHPMEVENT5H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT6H:
                os << "REG_CSR_MHPMEVENT6H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT7H:
                os << "REG_CSR_MHPMEVENT7H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT8H:
                os << "REG_CSR_MHPMEVENT8H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT9H:
                os << "REG_CSR_MHPMEVENT9H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT10H:
                os << "REG_CSR_MHPMEVENT10H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT11H:
                os << "REG_CSR_MHPMEVENT11H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT12H:
                os << "REG_CSR_MHPMEVENT12H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT13H:
                os << "REG_CSR_MHPMEVENT13H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT14H:
                os << "REG_CSR_MHPMEVENT14H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT15H:
                os << "REG_CSR_MHPMEVENT15H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT16H:
                os << "REG_CSR_MHPMEVENT16H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT17H:
                os << "REG_CSR_MHPMEVENT17H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT18H:
                os << "REG_CSR_MHPMEVENT18H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT19H:
                os << "REG_CSR_MHPMEVENT19H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT20H:
                os << "REG_CSR_MHPMEVENT20H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT21H:
                os << "REG_CSR_MHPMEVENT21H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT22H:
                os << "REG_CSR_MHPMEVENT22H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT23H:
                os << "REG_CSR_MHPMEVENT23H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT24H:
                os << "REG_CSR_MHPMEVENT24H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT25H:
                os << "REG_CSR_MHPMEVENT25H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT26H:
                os << "REG_CSR_MHPMEVENT26H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT27H:
                os << "REG_CSR_MHPMEVENT27H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT28H:
                os << "REG_CSR_MHPMEVENT28H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT29H:
                os << "REG_CSR_MHPMEVENT29H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT30H:
                os << "REG_CSR_MHPMEVENT30H";
                return;
            case STF_REG::STF_REG_CSR_MHPMEVENT31H:
                os << "REG_CSR_MHPMEVENT31H";
                return;
            case STF_REG::STF_REG_CSR_MNSCRATCH:
                os << "REG_CSR_MNSCRATCH";
                return;
            case STF_REG::STF_REG_CSR_MNEPC:
                os << "REG_CSR_MNEPC";
                return;
            case STF_REG::STF_REG_CSR_MNCAUSE:
                os << "REG_CSR_MNCAUSE";
                return;
            case STF_REG::STF_REG_CSR_MNSTATUS:
                os << "REG_CSR_MNSTATUS";
                return;
            case STF_REG::STF_REG_CSR_MSECCFG:
                os << "REG_CSR_MSECCFG";
                return;
            case STF_REG::STF_REG_CSR_MSECCFGH:
                os << "REG_CSR_MSECCFGH";
                return;
            case STF_REG::STF_REG_CSR_MCYCLE:
                os << "REG_CSR_MCYCLE";
                return;
            case STF_REG::STF_REG_CSR_MINSTRET:
                os << "REG_CSR_MINSTRET";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER3:
                os << "REG_CSR_MHPMCOUNTER3";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER4:
                os << "REG_CSR_MHPMCOUNTER4";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER5:
                os << "REG_CSR_MHPMCOUNTER5";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER6:
                os << "REG_CSR_MHPMCOUNTER6";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER7:
                os << "REG_CSR_MHPMCOUNTER7";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER8:
                os << "REG_CSR_MHPMCOUNTER8";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER9:
                os << "REG_CSR_MHPMCOUNTER9";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER10:
                os << "REG_CSR_MHPMCOUNTER10";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER11:
                os << "REG_CSR_MHPMCOUNTER11";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER12:
                os << "REG_CSR_MHPMCOUNTER12";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER13:
                os << "REG_CSR_MHPMCOUNTER13";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER14:
                os << "REG_CSR_MHPMCOUNTER14";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER15:
                os << "REG_CSR_MHPMCOUNTER15";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER16:
                os << "REG_CSR_MHPMCOUNTER16";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER17:
                os << "REG_CSR_MHPMCOUNTER17";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER18:
                os << "REG_CSR_MHPMCOUNTER18";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER19:
                os << "REG_CSR_MHPMCOUNTER19";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER20:
                os << "REG_CSR_MHPMCOUNTER20";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER21:
                os << "REG_CSR_MHPMCOUNTER21";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER22:
                os << "REG_CSR_MHPMCOUNTER22";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER23:
                os << "REG_CSR_MHPMCOUNTER23";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER24:
                os << "REG_CSR_MHPMCOUNTER24";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER25:
                os << "REG_CSR_MHPMCOUNTER25";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER26:
                os << "REG_CSR_MHPMCOUNTER26";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER27:
                os << "REG_CSR_MHPMCOUNTER27";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER28:
                os << "REG_CSR_MHPMCOUNTER28";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER29:
                os << "REG_CSR_MHPMCOUNTER29";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER30:
                os << "REG_CSR_MHPMCOUNTER30";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER31:
                os << "REG_CSR_MHPMCOUNTER31";
                return;
            case STF_REG::STF_REG_CSR_MCYCLEH:
                os << "REG_CSR_MCYCLEH";
                return;
            case STF_REG::STF_REG_CSR_MINSTRETH:
                os << "REG_CSR_MINSTRETH";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER3H:
                os << "REG_CSR_MHPMCOUNTER3H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER4H:
                os << "REG_CSR_MHPMCOUNTER4H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER5H:
                os << "REG_CSR_MHPMCOUNTER5H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER6H:
                os << "REG_CSR_MHPMCOUNTER6H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER7H:
                os << "REG_CSR_MHPMCOUNTER7H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER8H:
                os << "REG_CSR_MHPMCOUNTER8H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER9H:
                os << "REG_CSR_MHPMCOUNTER9H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER10H:
                os << "REG_CSR_MHPMCOUNTER10H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER11H:
                os << "REG_CSR_MHPMCOUNTER11H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER12H:
                os << "REG_CSR_MHPMCOUNTER12H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER13H:
                os << "REG_CSR_MHPMCOUNTER13H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER14H:
                os << "REG_CSR_MHPMCOUNTER14H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER15H:
                os << "REG_CSR_MHPMCOUNTER15H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER16H:
                os << "REG_CSR_MHPMCOUNTER16H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER17H:
                os << "REG_CSR_MHPMCOUNTER17H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER18H:
                os << "REG_CSR_MHPMCOUNTER18H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER19H:
                os << "REG_CSR_MHPMCOUNTER19H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER20H:
                os << "REG_CSR_MHPMCOUNTER20H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER21H:
                os << "REG_CSR_MHPMCOUNTER21H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER22H:
                os << "REG_CSR_MHPMCOUNTER22H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER23H:
                os << "REG_CSR_MHPMCOUNTER23H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER24H:
                os << "REG_CSR_MHPMCOUNTER24H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER25H:
                os << "REG_CSR_MHPMCOUNTER25H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER26H:
                os << "REG_CSR_MHPMCOUNTER26H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER27H:
                os << "REG_CSR_MHPMCOUNTER27H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER28H:
                os << "REG_CSR_MHPMCOUNTER28H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER29H:
                os << "REG_CSR_MHPMCOUNTER29H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER30H:
                os << "REG_CSR_MHPMCOUNTER30H";
                return;
            case STF_REG::STF_REG_CSR_MHPMCOUNTER31H:
                os << "REG_CSR_MHPMCOUNTER31H";
                return;
            case STF_REG::STF_REG_CSR_CYCLE:
                os << "REG_CSR_CYCLE";
                return;
            case STF_REG::STF_REG_CSR_TIME:
                os << "REG_CSR_TIME";
                return;
            case STF_REG::STF_REG_CSR_INSTRET:
                os << "REG_CSR_INSTRET";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER3:
                os << "REG_CSR_HPMCOUNTER3";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER4:
                os << "REG_CSR_HPMCOUNTER4";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER5:
                os << "REG_CSR_HPMCOUNTER5";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER6:
                os << "REG_CSR_HPMCOUNTER6";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER7:
                os << "REG_CSR_HPMCOUNTER7";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER8:
                os << "REG_CSR_HPMCOUNTER8";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER9:
                os << "REG_CSR_HPMCOUNTER9";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER10:
                os << "REG_CSR_HPMCOUNTER10";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER11:
                os << "REG_CSR_HPMCOUNTER11";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER12:
                os << "REG_CSR_HPMCOUNTER12";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER13:
                os << "REG_CSR_HPMCOUNTER13";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER14:
                os << "REG_CSR_HPMCOUNTER14";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER15:
                os << "REG_CSR_HPMCOUNTER15";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER16:
                os << "REG_CSR_HPMCOUNTER16";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER17:
                os << "REG_CSR_HPMCOUNTER17";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER18:
                os << "REG_CSR_HPMCOUNTER18";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER19:
                os << "REG_CSR_HPMCOUNTER19";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER20:
                os << "REG_CSR_HPMCOUNTER20";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER21:
                os << "REG_CSR_HPMCOUNTER21";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER22:
                os << "REG_CSR_HPMCOUNTER22";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER23:
                os << "REG_CSR_HPMCOUNTER23";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER24:
                os << "REG_CSR_HPMCOUNTER24";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER25:
                os << "REG_CSR_HPMCOUNTER25";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER26:
                os << "REG_CSR_HPMCOUNTER26";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER27:
                os << "REG_CSR_HPMCOUNTER27";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER28:
                os << "REG_CSR_HPMCOUNTER28";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER29:
                os << "REG_CSR_HPMCOUNTER29";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER30:
                os << "REG_CSR_HPMCOUNTER30";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER31:
                os << "REG_CSR_HPMCOUNTER31";
                return;
            case STF_REG::STF_REG_CSR_VL:
                os << "REG_CSR_VL";
                return;
            case STF_REG::STF_REG_CSR_VTYPE:
                os << "REG_CSR_VTYPE";
                return;
            case STF_REG::STF_REG_CSR_VLENB:
                os << "REG_CSR_VLENB";
                return;
            case STF_REG::STF_REG_CSR_CYCLEH:
                os << "REG_CSR_CYCLEH";
                return;
            case STF_REG::STF_REG_CSR_TIMEH:
                os << "REG_CSR_TIMEH";
                return;
            case STF_REG::STF_REG_CSR_INSTRETH:
                os << "REG_CSR_INSTRETH";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER3H:
                os << "REG_CSR_HPMCOUNTER3H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER4H:
                os << "REG_CSR_HPMCOUNTER4H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER5H:
                os << "REG_CSR_HPMCOUNTER5H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER6H:
                os << "REG_CSR_HPMCOUNTER6H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER7H:
                os << "REG_CSR_HPMCOUNTER7H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER8H:
                os << "REG_CSR_HPMCOUNTER8H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER9H:
                os << "REG_CSR_HPMCOUNTER9H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER10H:
                os << "REG_CSR_HPMCOUNTER10H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER11H:
                os << "REG_CSR_HPMCOUNTER11H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER12H:
                os << "REG_CSR_HPMCOUNTER12H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER13H:
                os << "REG_CSR_HPMCOUNTER13H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER14H:
                os << "REG_CSR_HPMCOUNTER14H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER15H:
                os << "REG_CSR_HPMCOUNTER15H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER16H:
                os << "REG_CSR_HPMCOUNTER16H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER17H:
                os << "REG_CSR_HPMCOUNTER17H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER18H:
                os << "REG_CSR_HPMCOUNTER18H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER19H:
                os << "REG_CSR_HPMCOUNTER19H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER20H:
                os << "REG_CSR_HPMCOUNTER20H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER21H:
                os << "REG_CSR_HPMCOUNTER21H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER22H:
                os << "REG_CSR_HPMCOUNTER22H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER23H:
                os << "REG_CSR_HPMCOUNTER23H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER24H:
                os << "REG_CSR_HPMCOUNTER24H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER25H:
                os << "REG_CSR_HPMCOUNTER25H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER26H:
                os << "REG_CSR_HPMCOUNTER26H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER27H:
                os << "REG_CSR_HPMCOUNTER27H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER28H:
                os << "REG_CSR_HPMCOUNTER28H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER29H:
                os << "REG_CSR_HPMCOUNTER29H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER30H:
                os << "REG_CSR_HPMCOUNTER30H";
                return;
            case STF_REG::STF_REG_CSR_HPMCOUNTER31H:
                os << "REG_CSR_HPMCOUNTER31H";
                return;
            case STF_REG::STF_REG_CSR_SCOUNTOVF:
                os << "REG_CSR_SCOUNTOVF";
                return;
            case STF_REG::STF_REG_CSR_STOPI:
                os << "REG_CSR_STOPI";
                return;
            case STF_REG::STF_REG_CSR_HGEIP:
                os << "REG_CSR_HGEIP";
                return;
            case STF_REG::STF_REG_CSR_VSTOPI:
                os << "REG_CSR_VSTOPI";
                return;
            case STF_REG::STF_REG_CSR_MVENDORID:
                os << "REG_CSR_MVENDORID";
                return;
            case STF_REG::STF_REG_CSR_MARCHID:
                os << "REG_CSR_MARCHID";
                return;
            case STF_REG::STF_REG_CSR_MIMPID:
                os << "REG_CSR_MIMPID";
                return;
            case STF_REG::STF_REG_CSR_MHARTID:
                os << "REG_CSR_MHARTID";
                return;
            case STF_REG::STF_REG_CSR_MCONFIGPTR:
                os << "REG_CSR_MCONFIGPTR";
                return;
            case STF_REG::STF_REG_CSR_MTOPI:
                os << "REG_CSR_MTOPI";
                return;
            case STF_REG::STF_REG_X0:
            case STF_REG::STF_REG_X1:
            case STF_REG::STF_REG_X2:
            case STF_REG::STF_REG_X3:
            case STF_REG::STF_REG_X4:
            case STF_REG::STF_REG_X5:
            case STF_REG::STF_REG_X6:
            case STF_REG::STF_REG_X7:
            case STF_REG::STF_REG_X8:
            case STF_REG::STF_REG_X9:
            case STF_REG::STF_REG_X10:
            case STF_REG::STF_REG_X11:
            case STF_REG::STF_REG_X12:
            case STF_REG::STF_REG_X13:
            case STF_REG::STF_REG_X14:
            case STF_REG::STF_REG_X15:
            case STF_REG::STF_REG_X16:
            case STF_REG::STF_REG_X17:
            case STF_REG::STF_REG_X18:
            case STF_REG::STF_REG_X19:
            case STF_REG::STF_REG_X20:
            case STF_REG::STF_REG_X21:
            case STF_REG::STF_REG_X22:
            case STF_REG::STF_REG_X23:
            case STF_REG::STF_REG_X24:
            case STF_REG::STF_REG_X25:
            case STF_REG::STF_REG_X26:
            case STF_REG::STF_REG_X27:
            case STF_REG::STF_REG_X28:
            case STF_REG::STF_REG_X29:
            case STF_REG::STF_REG_X30:
            case STF_REG::STF_REG_X31:
            case STF_REG::STF_REG_PC:
            case STF_REG::STF_REG_F0:
            case STF_REG::STF_REG_F1:
            case STF_REG::STF_REG_F2:
            case STF_REG::STF_REG_F3:
            case STF_REG::STF_REG_F4:
            case STF_REG::STF_REG_F5:
            case STF_REG::STF_REG_F6:
            case STF_REG::STF_REG_F7:
            case STF_REG::STF_REG_F8:
            case STF_REG::STF_REG_F9:
            case STF_REG::STF_REG_F10:
            case STF_REG::STF_REG_F11:
            case STF_REG::STF_REG_F12:
            case STF_REG::STF_REG_F13:
            case STF_REG::STF_REG_F14:
            case STF_REG::STF_REG_F15:
            case STF_REG::STF_REG_F16:
            case STF_REG::STF_REG_F17:
            case STF_REG::STF_REG_F18:
            case STF_REG::STF_REG_F19:
            case STF_REG::STF_REG_F20:
            case STF_REG::STF_REG_F21:
            case STF_REG::STF_REG_F22:
            case STF_REG::STF_REG_F23:
            case STF_REG::STF_REG_F24:
            case STF_REG::STF_REG_F25:
            case STF_REG::STF_REG_F26:
            case STF_REG::STF_REG_F27:
            case STF_REG::STF_REG_F28:
            case STF_REG::STF_REG_F29:
            case STF_REG::STF_REG_F30:
            case STF_REG::STF_REG_F31:
            case STF_REG::STF_REG_V0:
            case STF_REG::STF_REG_V1:
            case STF_REG::STF_REG_V2:
            case STF_REG::STF_REG_V3:
            case STF_REG::STF_REG_V4:
            case STF_REG::STF_REG_V5:
            case STF_REG::STF_REG_V6:
            case STF_REG::STF_REG_V7:
            case STF_REG::STF_REG_V8:
            case STF_REG::STF_REG_V9:
            case STF_REG::STF_REG_V10:
            case STF_REG::STF_REG_V11:
            case STF_REG::STF_REG_V12:
            case STF_REG::STF_REG_V13:
            case STF_REG::STF_REG_V14:
            case STF_REG::STF_REG_V15:
            case STF_REG::STF_REG_V16:
            case STF_REG::STF_REG_V17:
            case STF_REG::STF_REG_V18:
            case STF_REG::STF_REG_V19:
            case STF_REG::STF_REG_V20:
            case STF_REG::STF_REG_V21:
            case STF_REG::STF_REG_V22:
            case STF_REG::STF_REG_V23:
            case STF_REG::STF_REG_V24:
            case STF_REG::STF_REG_V25:
            case STF_REG::STF_REG_V26:
            case STF_REG::STF_REG_V27:
            case STF_REG::STF_REG_V28:
            case STF_REG::STF_REG_V29:
            case STF_REG::STF_REG_V30:
            case STF_REG::STF_REG_V31:
            case STF_REG::STF_REG_INVALID:
                stf_throw("Attempted to format a non-CSR register: " << std::hex << Codec::packRegNum(regno));
        }

        // An unknown (to us) but likely valid CSR register number
        std::ostringstream ss;

        if(Codec::isNonstandardCSR(regno)) {
            ss << "REG_CSR_NONST_";
        }
        else {
            ss << "REG_CSR_UNK_";
        }
        ss << std::hex << std::left << Codec::packRegNum(regno);
        os << ss.str();
    }

    void Registers::formatFPR_(std::ostream& os, const Registers::STF_REG regno) {
        stf_assert(isFPR(regno), "Attempted to format a non-FP register: " << std::hex << enums::to_int(regno));
        os << "REG_F" + std::to_string(Registers::getArchRegIndex(regno));
    }

    void Registers::formatGPR_(std::ostream& os, const Registers::STF_REG regno) {
        stf_assert(isGPR(regno), "Attempted to format a non-GP register: " << std::hex << enums::to_int(regno));
        os << "REG_" + std::to_string(Registers::getArchRegIndex(regno));
    }

    void Registers::formatVector_(std::ostream& os, const Registers::STF_REG regno) {
        stf_assert(isVector(regno), "Attempted to format a non-vector register: " << std::hex << enums::to_int(regno));
        os << "REG_V" + std::to_string(Registers::getArchRegIndex(regno));
    }

    Registers::STF_REG_packed_int Registers::getArchRegIndex(const Registers::STF_REG regno) {
        return Codec::packRegNum(regno);
    }

    inline void Registers::format(std::ostream& os, const Registers::STF_REG regno) {
        format_utils::FlagSaver flags(os);

        if(STF_EXPECT_FALSE(regno == Registers::STF_REG::STF_REG_PC)) {
            os << "PC";
        }
        else if (Registers::isFPR(regno)) {
            Registers::formatFPR_(os, regno);
        }
        else if (Registers::isCSR(regno)) {
            Registers::formatCSR_(os, regno);
        }
        else if (Registers::isGPR(regno)) {
            Registers::formatGPR_(os, regno);
        }
        else if (Registers::isVector(regno)) {
            Registers::formatVector_(os, regno);
        }
        else {
            stf_throw("Invalid STF_REG_TYPE: " << Registers::Codec::getRegType(regno));
        }
    }

    std::ostream& operator<<(std::ostream& os, const Registers::STF_REG regno) {
        Registers::format(os, regno);
        return os;
    }
} // end namespace stf
