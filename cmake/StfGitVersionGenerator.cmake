set(STF_GIT_VERSION_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/scripts/gen_git_version.sh")

function(stf_git_version_generator input_file output_file var_name)
    get_filename_component(GIT_VERSION_TARGET ${output_file} NAME_WE)
    add_custom_target(
        ${GIT_VERSION_TARGET}
        "${STF_GIT_VERSION_SCRIPT}" "${CMAKE_CURRENT_SOURCE_DIR}" "${input_file}" "${output_file}" "${var_name}"
        DEPENDS "${STF_GIT_VERSION_SCRIPT}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        BYPRODUCTS ${output_file} ${CMAKE_CURRENT_BINARY_DIR}/.git_sha_cache
    )
endfunction()
