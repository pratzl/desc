# Compiler cache configuration for faster rebuilds
# Automatically detects and enables ccache or sccache if available

function(enable_compiler_cache)
    option(ENABLE_CACHE "Enable compiler cache (ccache/sccache)" ON)

    if(ENABLE_CACHE)
        # Try to find ccache
        find_program(CCACHE_PROGRAM ccache)
        if(CCACHE_PROGRAM)
            message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
            set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" PARENT_SCOPE)
            set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" PARENT_SCOPE)
            
            # Configure ccache for optimal performance
            execute_process(COMMAND ${CCACHE_PROGRAM} --set-config=max_size=5G)
            execute_process(COMMAND ${CCACHE_PROGRAM} --set-config=compression=true)
            
            return()
        endif()
        
        # Try to find sccache as fallback
        find_program(SCCACHE_PROGRAM sccache)
        if(SCCACHE_PROGRAM)
            message(STATUS "Found sccache: ${SCCACHE_PROGRAM}")
            set(CMAKE_CXX_COMPILER_LAUNCHER "${SCCACHE_PROGRAM}" PARENT_SCOPE)
            set(CMAKE_C_COMPILER_LAUNCHER "${SCCACHE_PROGRAM}" PARENT_SCOPE)
            
            return()
        endif()
        
        # Neither found
        message(STATUS "No compiler cache found (ccache or sccache). Install one for faster rebuilds.")
    else()
        message(STATUS "Compiler cache disabled by ENABLE_CACHE=OFF")
    endif()
endfunction()
