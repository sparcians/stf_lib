cmake_minimum_required(VERSION 3.17)
project(stf_tests)

include_directories(${PROJECT_SOURCE_DIR}/..)
include(CTest)

include(ProcessorCount)
ProcessorCount(NUM_CORES)

if (NOT NUM_CORES EQUAL 0)
  set(CTEST_BUILD_FLAGS -j${NUM_CORES})
  set(CTEST_PARALLEL_LEVEL ${NUM_CORES})
endif()

message(STATUS "Found " ${NUM_CORES} " cores in machine (for ctest)")

add_subdirectory(stf_writer_test)

add_custom_target(stf_regress)
add_dependencies(stf_regress stf stf_writer_test)

add_custom_command(TARGET stf_regress POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "Running tests..."
                   COMMAND ${CMAKE_CTEST_COMMAND})
