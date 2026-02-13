project(stfpy)

find_package(Python3 COMPONENTS Interpreter Development)
find_package(PkgConfig)
pkg_check_modules(zstd REQUIRED libzstd)

set(stfpy_compile_opts -Wno-sign-conversion -Wno-deprecated-declarations -Wno-deprecated-copy -Wno-implicit-int-conversion -Wno-missing-field-initializers)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(stfpy_compile_opts ${stfpy_compile_opts} -Wno-ignored-optimization-argument -Wno-unknown-warning-option -Wno-gnu-anonymous-struct -Wno-nested-anon-types)
    set(stfpy_link_opts -Wno-unused-command-line-argument -Wno-ignored-optimization-argument)
elseif (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    # Some of the Python headers generate warnings in newer GCC compilers
    # because they're actually C code
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14)
        set(stfpy_compile_opts ${stfpy_compile_opts} -Wno-pedantic -Wno-conversion)
    endif()
endif()

# Disable _FORTIFY_SOURCE unless we're in Release mode
if (NOT CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    set(stfpy_compile_opts ${stfpy_compile_opts} -U_FORTIFY_SOURCE -Wp,-U_FORTIFY_SOURCE)
endif()

get_target_property(stf_include_dirs stf INCLUDE_DIRECTORIES)
get_target_property(stf_compile_opts stf COMPILE_OPTIONS)
get_target_property(stf_link_opts stf LINK_OPTIONS)
get_target_property(stf_ipo stf INTERPROCEDURAL_OPTIMIZATION)

set(stfpy_include_dirs ${stf_include_dirs})
set(stfpy_compile_opts ${stf_compile_opts} ${stfpy_compile_opts})
string(REPLACE " " ";" env_link_opts $ENV{LDFLAGS})
set(stfpy_link_opts ${env_link_opts} ${stf_link_opts} ${stfpy_link_opts})

if(stf_ipo OR CMAKE_INTERPROCEDURAL_OPTIMIZATION)
    set(stfpy_compile_opts ${stfpy_compile_opts} -flto)
    set(stfpy_link_opts ${stfpy_link_opts} -flto)
endif()

set(extra_lib_dirs $<TARGET_FILE_DIR:stf>)
string(REPLACE ";" ":" stfpy_include_dirs "${stfpy_include_dirs}")
string(REPLACE ";" ":" extra_lib_dirs "${extra_lib_dirs}")

# Get thread library name without -l
set(thread_lib ${CMAKE_THREAD_LIBS_INIT})
string(REPLACE "-l" "" thread_lib "${thread_lib}")
# Convert -pthread -> pthread
string(REPLACE "-p" "p" thread_lib "${thread_lib}")

set(stf_libraries "stf ${thread_lib} zstd")

file(GLOB py_files "${CMAKE_CURRENT_SOURCE_DIR}/stfpy/*.py")
file(GLOB pyx_files "${CMAKE_CURRENT_SOURCE_DIR}/stfpy/*.pyx" "${CMAKE_CURRENT_SOURCE_DIR}/stfpy/stf_lib/*.pyx")
file(GLOB pxd_files "${CMAKE_CURRENT_SOURCE_DIR}/stfpy/*.pxd" "${CMAKE_CURRENT_SOURCE_DIR}/stfpy/stf_lib/*.pxd")

if(APPLE)
    set(CYTHON_PARALLEL_FLAG "")
else()
    set(CYTHON_PARALLEL_FLAG "-j4")
endif()

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/stfpy.stamp
  COMMAND ${CMAKE_COMMAND} -E env --unset=CFLAGS
                                  --unset=CXXFLAGS
                                  CC="${CMAKE_C_COMPILER}"
                                  CXX="${CMAKE_CXX_COMPILER}"
                                  LD="${CMAKE_CXX_COMPILER_LINKER}"
                                  CPPFLAGS="${stfpy_compile_opts}"
                                  LDFLAGS="${stfpy_link_opts}"
                                  STFPY_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}"
                                  ${Python3_EXECUTABLE} setup.py build_ext -f
                                                                           ${CYTHON_PARALLEL_FLAG}
                                                                           --include-dirs "${stfpy_include_dirs}"
                                                                           --libraries "${stf_libraries}"
                                                                           --library-dirs "${extra_lib_dirs}:${zstd_LIBRARY_DIRS}"
  COMMAND ${CMAKE_COMMAND} -E env STFPY_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}"
                                  ${Python3_EXECUTABLE} setup.py bdist_wheel --bdist-dir "${CMAKE_CURRENT_BINARY_DIR}/bdist_wheel"
                                                                             --dist-dir "${CMAKE_CURRENT_BINARY_DIR}"
  COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/stfpy.stamp
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/setup.py
    ${py_files}
    ${pyx_files}
    ${pxd_files}
    stf
  COMMENT "Building Python bindings to STF library"
)

add_custom_target(stfpy ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/stfpy.stamp)

if(STFPY_INSTALL_DIR)
    install(CODE "execute_process(COMMAND pip install --prefix ${STFPY_INSTALL_DIR} --no-index --find-links=${CMAKE_CURRENT_BINARY_DIR} stfpy)")
endif()
if(STF_INSTALL_DIR)
    install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/stfpython DESTINATION ${STF_INSTALL_DIR})
    install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/stfpy_init.py DESTINATION ${STF_INSTALL_DIR})
endif()
