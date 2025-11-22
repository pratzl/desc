# Precompiled headers for faster compilation
# Configures commonly used standard library headers

function(enable_precompiled_headers target_name)
    option(ENABLE_PCH "Enable precompiled headers" ON)

    if(ENABLE_PCH)
        # For INTERFACE libraries, PCH doesn't make sense since they don't compile anything
        get_target_property(target_type ${target_name} TYPE)
        if(target_type STREQUAL "INTERFACE_LIBRARY")
            message(STATUS "Precompiled headers skipped for INTERFACE library: ${target_name}")
            return()
        endif()
        
        # Standard library headers commonly used in the project
        target_precompile_headers(${target_name} INTERFACE
            <algorithm>
            <concepts>
            <cstddef>
            <functional>
            <iostream>
            <iterator>
            <memory>
            <ranges>
            <string>
            <type_traits>
            <utility>
            <vector>
        )
        message(STATUS "Precompiled headers enabled for target: ${target_name}")
    else()
        message(STATUS "Precompiled headers disabled by ENABLE_PCH=OFF")
    endif()
endfunction()
