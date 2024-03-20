project(stfpy)

find_package(Python3 COMPONENTS Interpreter Development)

include_directories(${STF_BASE}/stf-inc)

add_compile_options(-Wno-sign-conversion -Wno-deprecated-declarations -Wno-deprecated-copy -Wno-implicit-int-conversion -Wno-missing-field-initializers)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wno-ignored-optimization-argument -Wno-unknown-warning-option)
    add_link_options(-Wno-unused-command-line-argument -Wno-ignored-optimization-argument)
endif()

get_directory_property(include_dirs INCLUDE_DIRECTORIES)
get_directory_property(compile_opts COMPILE_OPTIONS)
get_directory_property(link_opts LINK_OPTIONS)

set(extra_lib_dirs $<TARGET_FILE_DIR:stf>)
string(REPLACE ";" ":" include_dirs "${include_dirs}")
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
                                  LD="${CMAKE_CXX_COMPILER}"
                                  CPPFLAGS="${compile_opts}"
                                  LDFLAGS="${link_opts}"
                                  STFPY_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}"
                                  ${Python3_EXECUTABLE} setup.py build_ext -f
                                                                           ${CYTHON_PARALLEL_FLAG}
                                                                           --include-dirs "${include_dirs}"
                                                                           --libraries "${stf_libraries}"
                                                                           --library-dirs "${extra_lib_dirs}"
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
