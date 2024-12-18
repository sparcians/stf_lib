cmake_minimum_required(VERSION 3.17)
project (stf_lib CXX)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (STF_BASE ${CMAKE_CURRENT_SOURCE_DIR})

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set (STF_BASE ${STF_BASE} PARENT_SCOPE)
endif()

# Boost
find_package (Boost 1.49.0)
include_directories (SYSTEM ${Boost_INCLUDE_DIRS})

include (${STF_BASE}/cmake/stf-config.cmake)

add_subdirectory(lib)

if(BUILD_STF_PYTHON_LIB)
    add_subdirectory(stfpy)
endif()

if(NOT DISABLE_STF_TESTS)
    enable_testing()
    add_subdirectory (tests)
endif()
