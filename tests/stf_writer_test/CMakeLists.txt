cmake_minimum_required(VERSION 3.17)
project(stf_writer_test)

set(DISABLE_STF_DOXYGEN 1)
set(NO_STF_LTO 1)

set(CMAKE_CXX_STANDARD 17)
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
)

add_compile_options($<$<CONFIG:Release>:-Ofast>)

set(SOURCES
    main.cpp
)

add_executable(stf_writer_test ${SOURCES})
add_dependencies(stf_writer_test stf)

add_test(NAME stf_writer_basic_test
         COMMAND bash -c "${PROJECT_BINARY_DIR}/stf_writer_test && \
         diff ${PROJECT_BINARY_DIR}/stf_write_test.zstf \
              ${PROJECT_SOURCE_DIR}/golden/stf_write_test.zstf")

target_link_directories(stf_writer_test PRIVATE ${PROJECT_SOURCE_DIR}/../../stf_lib/release/lib)
target_link_libraries(stf_writer_test PRIVATE ${STF_LINK_LIBS})