# Code formatting with clang-format
function(enable_format_check)
    find_program(CLANG_FORMAT clang-format)
    
    if(CLANG_FORMAT)
        file(GLOB_RECURSE ALL_SOURCE_FILES
            ${PROJECT_SOURCE_DIR}/include/*.hpp
            ${PROJECT_SOURCE_DIR}/tests/*.cpp
            ${PROJECT_SOURCE_DIR}/examples/*.cpp
            ${PROJECT_SOURCE_DIR}/benchmark/*.cpp
        )
        
        # Format target - applies formatting to all files
        add_custom_target(format
            COMMAND ${CLANG_FORMAT} -i ${ALL_SOURCE_FILES}
            COMMENT "Running clang-format on all source files"
            VERBATIM
        )
        
        # Format-check target - verifies formatting without modifying files
        add_custom_target(format-check
            COMMAND ${CLANG_FORMAT} --dry-run --Werror ${ALL_SOURCE_FILES}
            COMMENT "Checking code formatting (non-modifying)"
            VERBATIM
        )
        
        message(STATUS "Format targets available:")
        message(STATUS "  - format:       Apply clang-format to all source files")
        message(STATUS "  - format-check: Check formatting without modifying files")
    else()
        message(STATUS "clang-format not found - format targets unavailable")
    endif()
endfunction()
