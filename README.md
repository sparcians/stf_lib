# STF Library

The acronym STF stands for Simulation Tracing Format. This is intended to be used
with Sparta-based simulators, but that's not necessary.
This repo contains the following:

1. The STF definition
1. Trace generators (writers)
1. Trace consumers (readers)

## Required Packages

The Docker images include all required packages.

To install on **Mac**:
`brew install zstd`

To install on **Ubuntu**:
`apt-get install zstd libzstd-dev`
`apt install texlive-font-utils`  # For documentation

## Building

```
mkdir release
cd release
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
## Quick Start on the API

### Reading a Trace

API documentation: `doc_doxygen/html/classstf_1_1STFInstReaderBase.html`
Example Usage:
```
#include "stf-inc/stf_reader.hpp"
#include "stf-inc/stf_record_types.hpp"

constexpr bool skip_nonuser_mode = true; // example
constexpr bool CHECK_FOR_STF_PTE = false; // If true, search for an stf-pte file alongside this trace.
constexpr bool FILTER_MODE_CHANGE_EVENTS = true;
constexpr size_t BUFFER_SIZE = 4096;
stf::STFInstReader reader(trace_filename,  // *.zstf, *.stf
                          skip_nonuser_mode,
                          CHECK_FOR_STF_PTE, 
                          FILTER_MODE_CHANGE_EVENTS,
                          BUFFER_SIZE));

// Iterate through the trace
for(const stf::STFInst & inst : reader) {
    std::cout << std::hex << "PC: 0x" << inst.pc() << "Opcode: 0x" << inst.getOpcode() << std::endl;
}

```

### Writing a Trace

API documentation: `doc_doxygen/html/classstf_1_1STFWriter.html`
Example Usage:
```
#include "stf-inc/stf_writer.hpp"
#include "stf-inc/stf_record_types.hpp"

uint64_t pc = 0x1000;

////////////////////
// Initialize/setup
////////////////////
stf::STFWriter stf_writer;
stf_writer.open("my_trace.zstf");  // If you do not want compression, drop the 'z': my_trace.stf
stf_writer.addTraceInfo(stf::TraceInfoRecord(stf::STF_GEN::STF_GEN_IMPERAS, 1, 2, 0, "Trace from Imperas"));
stf_writer.setISA(stf::ISA::RISCV);
stf_writer.setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV64);
stf_writer.setTraceFeature(stf::TRACE_FEATURES::STF_CONTAIN_RV64);
stf_writer.setTraceFeature(stf::TRACE_FEATURES::STF_CONTAIN_PHYSICAL_ADDRESS);
stf_writer.setHeaderPC(pc);
stf_writer.finalizeHeader();

////////////////////
// Write records
////////////////////

// Simple 32-bit add    x14,x12,x11
stf_writer << stf::InstRegRecord(11,
                                 stf::Registers::STF_REG_TYPE::INTEGER,
                                 stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                                 x11_value);
stf_writer << stf::InstRegRecord(12,
                                 stf::Registers::STF_REG_TYPE::INTEGER,
                                 stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                                 x12_value);
stf_writer << stf::InstRegRecord(14,
                                 stf::Registers::STF_REG_TYPE::INTEGER,
                                 stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                 x14_value);
stf_writer << stf::InstOpcode32Record(0x00b60733);

// A branch
stf_writer << stf::InstPCTargetRecord(branch_target_pc);
stf_writer << stf::InstOpcode32Record(0xfee59ce3);

```

