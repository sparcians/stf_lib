cmake_minimum_required(VERSION 3.17)
project(stf_writer_test)

set(DISABLE_STF_DOXYGEN 1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(
    ${PROJECT_SOURCE_DIR}/../.. 
)

add_compile_options(
    -g
    -Wall
    -Wno-parentheses
    -MMD
    -D_FILE_OFFSET_BITS=64
    -D_LARGEFILE_SOURCE
    -D_GNU_SOURCE
    -D__STDC_FORMAT_MACROS
    -Werror
    -Wdeprecated
    -Wextra
    -Winline
    -Winit-self
    -Wno-unused-function
    -Wuninitialized
    -Wno-sequence-point
    -Wno-inline
    -Wno-unknown-pragmas
    -Woverloaded-virtual
    -Wno-unused-parameter
    -Wno-missing-field-initializers
    -Wno-unused-command-line-argument
)

add_compile_options($<$<CONFIG:Release>:-O3> $<$<CONFIG:Release>:-ffast-math>)

set(SOURCES
    main.cpp
)

add_executable(stf_writer_test ${SOURCES})

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    include(lto)
    target_enable_lto(stf_writer_test)
endif()

add_test(NAME stf_writer_basic_test
         COMMAND bash -c "${PROJECT_BINARY_DIR}/stf_writer_test && \
         diff ${PROJECT_BINARY_DIR}/stf_write_test.zstf \
              ${PROJECT_SOURCE_DIR}/golden/stf_write_test.zstf")

target_link_libraries(stf_writer_test PRIVATE stf)
