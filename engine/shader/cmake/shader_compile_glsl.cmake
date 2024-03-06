function(compile_shader SHADERS TARGET_NAME SHADER_INCLUDE_FOLDER GENERATED_DIR)

    set(WORKING_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

    set(ALL_GENERATED_CPP_FILES "")

    foreach(SHADER ${SHADERS})
    # Prepare a header name and a global variable for this shader
        get_filename_component(SHADER_NAME ${SHADER} NAME)
        string(REPLACE "." "_" HEADER_NAME ${SHADER_NAME})
        string(TOUPPER ${HEADER_NAME} GLOBAL_SHADER_VAR)

        set(CPP_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_DIR}/cpp/${HEADER_NAME}.h")
        set(CPP_TARGET "${GLOBAL_SHADER_VAR}_TARGET")

        add_custom_target(
            ${CPP_TARGET}
            COMMAND ${CMAKE_COMMAND} -DPATH=${SHADER} -DHEADER=${CPP_FILE}
                -DGLOBAL=${GLOBAL_SHADER_VAR} -P "${WORKING_DIR}/cmake/generate_shader_cpp_file.cmake"
            WORKING_DIRECTORY "${WORKING_DIR}")

        list(APPEND ALL_GENERATED_CPP_FILES ${CPP_TARGET})

    endforeach()

    add_custom_target(${TARGET_NAME}
        DEPENDS ${ALL_GENERATED_CPP_FILES} SOURCES ${SHADERS})

endfunction()