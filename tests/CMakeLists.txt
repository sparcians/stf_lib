cmake_minimum_required(VERSION 3.17)
project(stf_tests)

include_directories(${PROJECT_SOURCE_DIR}/..)
include(CTest)

include(ProcessorCount)
ProcessorCount(NUM_CORES)

if (NOT NUM_CORES EQUAL 0)
  set(CTEST_BUILD_FLAGS -j${NUM_CORES})
  set(ctest_test_args "--parallel ${NUM_CORES}")
endif()

message(STATUS "Found " ${NUM_CORES} " cores in machine (for ctest)")

add_subdirectory(stf_writer_test)

add_custom_target(stf_test)
add_dependencies(stf_test stf stf_writer_test)

add_custom_command(TARGET stf_test POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "Running tests..."
                   COMMAND ${CMAKE_CTEST_COMMAND} ${ctest_test_args})