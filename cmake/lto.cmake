# Allows modifying check_ipo_supported behavior via CMAKE_CXX_FLAGS
cmake_policy(SET CMP0138 NEW)

function(_check_lto_supported)
    include(CheckIPOSupported)

    # Check if defaults work fine before doing anything else
    check_ipo_supported(RESULT result OUTPUT output LANGUAGES CXX)

    if(result)
        message("-- Default toolchain supports LTO")
    else()
        # The defaults didn't work - see if we can find a config that will work
        if(DEFINED ENV{LD})
          find_program(LD $ENV{LD})
        else()
          find_program(LD "ld")
        endif()

        execute_process(COMMAND ${LD} -v
                        OUTPUT_VARIABLE LD_VERSION)
        string(STRIP ${LD_VERSION} LD_VERSION)
        string(REGEX MATCH "[^ \t\r\n]+$" LD_VERSION ${LD_VERSION})
        message("-- ld version is ${LD_VERSION}")

        find_program(GOLD "ld.gold")
        find_program(LLD "ld.lld")

        set(chosen_linker ld)

        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
          if(LLD)
            set(chosen_linker lld)
          elseif(GOLD)
            set(chosen_linker gold)
          elseif(${LD_VERSION} VERSION_LESS "2.21")
            message(FATAL_ERROR "Either ld.lld or ld.gold are required when compiling with clang")
          endif()
        else()
          if(GOLD)
            set(chosen_linker gold)
          elseif(${LD_VERSION} VERSION_LESS "2.21")
            message(FATAL_ERROR "ld.gold is required when compiling with gcc")
          endif()
        endif()

        message("-- Using ${chosen_linker} for LTO linking")
        if(NOT chosen_linker STREQUAL "ld")
            set(extra_lto_link_flags -fuse-ld=${chosen_linker})
        endif()

        # Try again with the new linker
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${extra_lto_link_flags}")
        check_ipo_supported(RESULT result OUTPUT output LANGUAGES CXX)
        set(_LTO_LINK_FLAGS ${extra_lto_link_flags} PARENT_SCOPE)
    endif()

    set(_LTO_CHECK_RESULT ${result} PARENT_SCOPE)
    set(_LTO_CHECK_OUTPUT ${output} PARENT_SCOPE)
endfunction()

function(target_enable_lto target)
  if(NOT NO_STF_LTO)
    _check_lto_supported()

    if(_LTO_CHECK_RESULT)
      set_property(TARGET ${target} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
      target_link_options(${target} PUBLIC ${_LTO_LINK_FLAGS})
    else()
      message(WARNING "IPO is not supported for target ${target}: ${_LTO_CHECK_OUTPUT}")
    endif()
  endif()
endfunction()

# This needs to be a macro since it has to operate in the caller's scope
# Recommend using target_enable_lto instead wherever possible
macro(enable_lto)
  if(NOT NO_STF_LTO)
    _check_lto_supported()

    if(_LTO_CHECK_RESULT)
      set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
      add_link_options(${_LTO_LINK_FLAGS})
    else()
      message(WARNING "IPO is not supported: ${_LTO_CHECK_OUTPUT}")
    endif()
  endif()
  unset(_LTO_LINK_FLAGS)
  unset(_LTO_CHECK_RESULT)
  unset(_LTO_CHECK_OUTPUT)
endmacro()
