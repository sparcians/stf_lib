project(libstf)

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC STF_GIT_SHA1)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/stf_git_version.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/stf_git_version.cpp" @ONLY)

include_directories(${STF_BASE}/stf-inc)

file(GLOB libstf_SRC "*.cpp")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_source_files_properties(stf_inst_reader.cpp PROPERTIES COMPILE_FLAGS -fno-jump-tables)
endif()

add_library(stf ${libstf_SRC} "${CMAKE_CURRENT_BINARY_DIR}/stf_git_version.cpp")
