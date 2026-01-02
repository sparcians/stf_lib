################################################################################
#              RequiredLibraries
################################################################################

if(CMAKE_CXX_COMPILER_ID MATCHES GNU AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.4.0)
    message(FATAL_ERROR "Provided gcc version (${CMAKE_CXX_COMPILER_VERSION}) is older than required version (7.4.0)")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_MODULE_PATH})

# zstd
find_package(PkgConfig)
pkg_check_modules(zstd REQUIRED libzstd)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

include_directories (${zstd_INCLUDE_DIRS})

set (STF_LINK_LIBS ${CMAKE_THREAD_LIBS_INIT} stf ${zstd_LINK_LIBRARIES})

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set (STF_LINK_LIBS ${STF_LINK_LIBS} PARENT_SCOPE)
endif()

include(stf_linker_setup)

setup_stf_linker(true)

add_compile_options(-Werror -fPIC -Wall -Wextra -pedantic -Wconversion -Wno-unused-parameter -Wno-unused-function -pipe)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wno-gnu-zero-variadic-macro-arguments)

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.0)
        add_compile_options(-Wno-c++20-extensions)
    endif()
endif()

# The Boost containers trigger some warnings in recent Clang:
# https://github.com/boostorg/container/issues/300
if (Boost_VERSION_STRING VERSION_LESS_EQUAL 1.88.0)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 20.0)
            add_compile_options(-Wno-nontrivial-memcall)
        endif()
    endif()
endif()

if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    if(NOT DISABLE_STF_DOXYGEN)
        option(BUILD_DOC "Build documentation" ON)

        find_package(Doxygen)
        if (DOXYGEN_FOUND)
            # set input and output files
            set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
            set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

            # request to configure the file
            configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
            message("Doxygen build started")

            # note the option ALL which allows to build the docs together with the application
            add_custom_target( doc_doxygen ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM )
        else (DOXYGEN_FOUND)
          message("Doxygen need to be installed to generate the doxygen documentation")
        endif (DOXYGEN_FOUND)
    endif()
endif()
