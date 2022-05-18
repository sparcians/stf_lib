project(stfpy)

find_package(PythonInterp 3.7 REQUIRED)
find_package(PythonLibs 3.7 REQUIRED)

include_directories(${STF_BASE}/stf-inc)

add_compile_options(-Wno-sign-conversion -Wno-deprecated-declarations)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_link_options(-Wno-unused-command-line-argument)
endif()

get_directory_property(include_dirs INCLUDE_DIRECTORIES)
get_directory_property(compile_opts COMPILE_OPTIONS)
get_directory_property(link_opts LINK_OPTIONS)

set(extra_lib_dirs $<TARGET_FILE_DIR:stf> $<TARGET_FILE_DIR:mavis_json_files>)
string(REPLACE ";" ":" include_dirs "${include_dirs}")
string(REPLACE ";" ":" extra_lib_dirs "${extra_lib_dirs}")
set(stf_libraries "stf pthread zstd mavis_json_files")

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
                                  LD="${CMAKE_CXX_LINK_EXECUTABLE}"
                                  CPPFLAGS="${compile_opts}"
                                  LDFLAGS="${link_opts}"
                                  STFPY_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}"
                                  ${PYTHON_EXECUTABLE} setup.py build_ext -f
                                                                          ${CYTHON_PARALLEL_FLAG}
                                                                          --include-dirs "${include_dirs}"
                                                                          --libraries "${stf_libraries}"
                                                                          --library-dirs "${extra_lib_dirs}"
  COMMAND ${CMAKE_COMMAND} -E env STFPY_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}"
                                  ${PYTHON_EXECUTABLE} setup.py bdist_wheel --bdist-dir "${CMAKE_CURRENT_BINARY_DIR}/bdist_wheel"
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

if(STF_INSTALL_DIR)
    file(GLOB whl_file "${CMAKE_CURRENT_BINARY_DIR}/stfpy*.whl")
    install(CODE "execute_process(COMMAND pip install --prefix ${STF_INSTALL_DIR} ${whl_file})")
    install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/stfpython DESTINATION ${STF_INSTALL_DIR})
endif()
