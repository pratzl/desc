# Static analysis tool integration
function(enable_clang_tidy target_name)
    option(ENABLE_CLANG_TIDY "Enable clang-tidy analysis" OFF)

    if(ENABLE_CLANG_TIDY)
        find_program(CLANGTIDY clang-tidy)
        if(CLANGTIDY)
            set_target_properties(${target_name} PROPERTIES
                CXX_CLANG_TIDY "${CLANGTIDY}"
            )
            message(STATUS "clang-tidy enabled for target: ${target_name}")
        else()
            message(WARNING "clang-tidy requested but not found")
        endif()
    endif()
endfunction()

function(enable_cppcheck target_name)
    option(ENABLE_CPPCHECK "Enable cppcheck analysis" OFF)

    if(ENABLE_CPPCHECK)
        find_program(CPPCHECK cppcheck)
        if(CPPCHECK)
            set(CMAKE_CXX_CPPCHECK
                "${CPPCHECK}"
                "--enable=warning,style,performance,portability"
                "--inline-suppr"
                "--suppress=*:${CMAKE_SOURCE_DIR}/build/*"
                "--suppress=*:${CMAKE_SOURCE_DIR}/tests/Catch2/*"
            )
            set_target_properties(${target_name} PROPERTIES
                CXX_CPPCHECK "${CMAKE_CXX_CPPCHECK}"
            )
            message(STATUS "cppcheck enabled for target: ${target_name}")
        else()
            message(WARNING "cppcheck requested but not found")
        endif()
    endif()
endfunction()
