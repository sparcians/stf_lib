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

## Building

```
mkdir release
cd release
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
