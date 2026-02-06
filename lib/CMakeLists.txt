project(libstf)

include(StfGitVersionGenerator)
stf_git_version_generator("${CMAKE_CURRENT_SOURCE_DIR}/stf_git_version.cpp.in"
                          "${CMAKE_CURRENT_BINARY_DIR}/stf_git_version.cpp"
                          "STF_GIT_SHA1")

file(GLOB libstf_SRC "*.cpp")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_source_files_properties(stf_inst_reader.cpp PROPERTIES COMPILE_FLAGS -fno-jump-tables)
endif()

add_library(stf ${libstf_SRC} "${CMAKE_CURRENT_BINARY_DIR}/stf_git_version.cpp")

target_include_directories(stf PUBLIC ${STF_BASE}/stf-inc)

target_include_directories (stf PUBLIC ${zstd_INCLUDE_DIRS})

target_include_directories (stf SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})

target_compile_options(stf PRIVATE -Werror -fPIC -Wall -Wextra -pedantic -Wconversion -Wno-unused-parameter -Wno-unused-function -pipe)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(stf PUBLIC -Wno-gnu-zero-variadic-macro-arguments)

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.0)
        target_compile_options(stf PUBLIC -Wno-c++20-extensions)
    endif()
endif()

# The Boost containers trigger some warnings in recent Clang:
# https://github.com/boostorg/container/issues/300
if (Boost_VERSION_STRING VERSION_LESS_EQUAL 1.88.0)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 20.0)
            target_compile_options(stf PUBLIC -Wno-nontrivial-memcall)
        endif()
    endif()
endif()

target_link_libraries(stf ${CMAKE_THREAD_LIBS_INIT} ${zstd_LINK_LIBRARIES} Boost::boost)

target_link_options(stf PUBLIC -Wno-stringop-overflow)

if (CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
  target_link_options(stf PRIVATE -O0 -g -pipe -fno-omit-frame-pointer)
  target_compile_options(stf PRIVATE -O0 -g -pipe -fno-omit-frame-pointer)
  set(NO_STF_LTO 1)
elseif (CMAKE_BUILD_TYPE MATCHES "^[Ff]ast[Dd]ebug")
  target_link_options(stf PRIVATE -O3 -g -pipe -fno-omit-frame-pointer)
  set(NO_STF_LTO 1)
else()
  target_link_options(stf PRIVATE -O3 -pipe)
  target_compile_options(stf PRIVATE -O3 -pipe)

  # Enable more aggressive inlining in Clang
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      target_compile_options(stf PRIVATE -mllvm -inline-threshold=1024)
  endif()

  if (CMAKE_BUILD_TYPE MATCHES "^[Pp]rofile")
    target_link_options(stf PRIVATE -g -fno-omit-frame-pointer)
    target_compile_options(stf PRIVATE -g -fno-omit-frame-pointer)
    if(STF_ENABLE_GPROF)
      target_link_options(stf PRIVATE -pg)
      target_compile_options(stf PRIVATE -pg)
    endif()
  else()
    target_link_options(stf PRIVATE -fomit-frame-pointer)
    target_compile_options(stf PRIVATE -fomit-frame-pointer)
  endif()
endif()

include(lto)
target_enable_lto(stf)
