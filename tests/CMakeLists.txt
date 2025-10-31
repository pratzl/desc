cmake_minimum_required(VERSION 3.20)

include(FetchContent)

# Fetch Catch2 v3
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.0
)
FetchContent_MakeAvailable(Catch2)

# Test executable
add_executable(descriptor_tests
    test_main.cpp
    test_vertex_descriptor.cpp
    test_edge_descriptor.cpp
    test_descriptor_traits.cpp
    test_edge_concepts.cpp
    test_vertex_concepts.cpp
    test_vertices_cpo.cpp
    test_vertex_id_cpo.cpp
    test_type_aliases.cpp
)

target_link_libraries(descriptor_tests
    PRIVATE
        descriptors
        Catch2::Catch2WithMain
)

# Register tests with CTest
include(CTest)
include(Catch)
catch_discover_tests(descriptor_tests)
