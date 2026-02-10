# STF Library

The acronym STF stands for Simulation Trace Format. This is intended to be used
with Sparta-based simulators, but that's not necessary.

You can view the STF specification at [stf_spec](https://github.com/sparcians/stf_spec).

This repo contains the following:

1. Trace generators (writers)
1. Trace consumers (readers)

## Required Packages

The Docker images include all required packages.

To install on **Mac**:
`brew install zstd`

To install on **Ubuntu**:
`apt-get install zstd libzstd-dev`

To build documentation on **Ubuntu**:
`apt install texlive-font-utils`

## Building

```
mkdir release
cd release
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
## Quick Start on the API

### CMake Config

Add the following to your project's CMakeLists.txt:
```
add_subdirectory(stf_lib)
target_link_libraries(my_project stf)
```

### Reading a Trace

API documentation: `doc_doxygen/html/classstf_1_1STFInstReaderBase.html`
Example Usage:
```
#include "stf-inc/stf_inst_reader.hpp"
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
    std::cout << std::hex << "PC: 0x" << inst.pc() << " Opcode: 0x" << inst.opcode() << std::endl;
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
// make sure to setVLen when there are vector instructions in the trace 
// stf_writer.setVLen( /*VLEN*/ );
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

## Unit Tests for STF_LIB

A regression target `regress` is available for unit testing STF_LIB. These tests are based on CTest and can be run using the following commands:

```sh
mkdir -p release/
cd release/
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j32 regress
cd ..
```

This setup provides a stub of CTest-based unit tests to ensure the functionality and reliability of the library. Please note that these tests do not yet cover the STF_LIB extensively and are currently just a stub.
