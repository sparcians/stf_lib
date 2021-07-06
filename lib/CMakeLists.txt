project(libstf)

include(GitVersionGenerator)
git_version_generator("${CMAKE_CURRENT_SOURCE_DIR}/stf_git_version.cpp.in"
                      "${CMAKE_CURRENT_BINARY_DIR}/stf_git_version.cpp"
                      "STF_GIT_SHA1")

include_directories(${STF_BASE}/stf-inc)

file(GLOB libstf_SRC "*.cpp")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_source_files_properties(stf_inst_reader.cpp PROPERTIES COMPILE_FLAGS -fno-jump-tables)
endif()

add_library(stf ${libstf_SRC} "${CMAKE_CURRENT_BINARY_DIR}/stf_git_version.cpp")
