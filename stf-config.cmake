################################################################################
#              RequiredLibraries
################################################################################

if(CMAKE_CXX_COMPILER_ID MATCHES GNU AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.4.0)
    message(FATAL_ERROR "Provided gcc version (${CMAKE_CXX_COMPILER_VERSION}) is older than required version (7.4.0)")
endif()

# zstd
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake ${CMAKE_MODULE_PATH})
find_package(zstd)

include_directories (${zstd_INCLUDE_DIRS})

set (STF_LINK_LIBS pthread stf ${zstd_LIBRARIES})

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set (STF_LINK_LIBS ${STF_LINK_LIBS} PARENT_SCOPE)
endif()

if (NOT APPLE)
    find_program(GOLD "ld.gold")
    find_program(LLD "ld.lld")

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        if(LLD)
            SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
        elseif(GOLD)
            SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
        else()
            message(FATAL_ERROR "Either ld.lld or ld.gold are required when compiling with clang")
        endif()
    else()
        if(GOLD)
            SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
        else()
            message(FATAL_ERROR "ld.gold is required when compiling with gcc")
        endif()
    endif()
endif()

if(NOT NO_STF_LTO)
    message("-- Enabling link-time optimization in STF library")
    add_compile_options(-flto)
    add_link_options(-flto)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      if (CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
        SET(CMAKE_AR "ar")
      else()
        unset(LLVM_AR)
        unset(LLVM_AR CACHE)
        # using regular Clang or AppleClang
        find_program(LLVM_AR "llvm-ar")
        if (NOT LLVM_AR)
          unset(LLVM_AR)
          unset(LLVM_AR CACHE)
          find_program(LLVM_AR "llvm-ar-9")
          if (NOT LLVM_AR)
            message(FATAL_ERROR "llvm-ar is needed to link trace_tools on this system")
          else()
            SET(CMAKE_AR "llvm-ar-9")
          endif()
        else()
          SET(CMAKE_AR "llvm-ar")
        endif()
      endif()
    else ()
      SET(CMAKE_AR  "gcc-ar")
    endif()
else()
    message("-- Disabling link-time optimization in STF library")
endif()

# -DGEN_STF_GIT_VERSION
#set(CMAKE_CXX_FLAGS "${OPT_FLAGS} \
# -g -Werror -std=c++17 -fPIC \
# -Wall -Wextra -Winline -Winit-self -Wno-unused-function \
# -Wuninitialized -Wno-sequence-point -Wno-inline -Wno-unknown-pragmas \
# -Woverloaded-virtual -Wno-unused-parameter -Wno-missing-field-initializers")
add_compile_options(-Werror -std=c++17 -fPIC
  -Wall -Wextra -pedantic -Wconversion -Wno-unused-parameter -Wno-unused-function -pipe)

if (CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pipe")
elseif (CMAKE_BUILD_TYPE MATCHES "^[Pp]rofile")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-mllvm -inline-threshold=1024)
    endif()
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3 -pipe")
else()
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-mllvm -inline-threshold=1024)
    endif()
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3 -pipe -fomit-frame-pointer")
endif()

SET(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
SET(CMAKE_CXX_ARCHIVE_FINISH   true)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_STUDY "${CMAKE_CXX_FLAGS_STUDY} -O3 -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_PROFILE "-O3 -g -fomit-frame-pointer")

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
