# Graph Descriptor Project - Implementation Instructions

## Project Overview
This is a C++20 project that implements descriptors as abstractions for vertices and edges in a graph data structure. The descriptors provide type-safe, efficient handles for graph elements.

## RFC 2119 Key Words
The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in this document are to be interpreted as described in RFC 2119.

## Project Configuration

### Build System
- **Build Tool**: CMake (minimum version 3.20)
- **Generator**: Ninja
- **C++ Standard**: C++20
- **Testing Framework**: Catch2 (v3.x recommended)

### Directory Structure
```
desc/
├── CMakeLists.txt           # Root CMake configuration
├── include/
│   └── desc/
│       ├── descriptor.hpp   # Core descriptor interface
│       ├── vertex_descriptor.hpp
│       └── edge_descriptor.hpp
├── src/
│   └── (implementation files if needed)
├── tests/
│   ├── CMakeLists.txt
│   ├── test_main.cpp        # Catch2 main
│   ├── test_vertex_descriptor.cpp
│   └── test_edge_descriptor.cpp
├── examples/
│   └── basic_usage.cpp
└── README.md
```

## Core Requirements

### 1. Descriptor Base Concept/Interface
- Descriptors MUST be lightweight, copyable handles
- Descriptors MUST support comparison operations (==, !=, <, <=, >, >=)
- Descriptors MUST be hashable for use in STL containers
- Descriptors MUST be default constructible and trivially copyable
- Descriptors SHOULD provide a null/invalid state representation
- Descriptors MUST be type-safe: vertex and edge descriptors SHALL be distinct types

### 2. Vertex Descriptor
- MUST represent a handle to a vertex in the graph
- SHOULD contain minimal data (typically an index or ID)
- MUST be efficiently passable by value
- SHOULD support conversion to/from underlying index type
- MUST integrate with std::hash for unordered containers

### 3. Edge Descriptor
- MUST represent a handle to an edge in the graph
- SHOULD contain source vertex, target vertex, and edge identifier
- MUST support directed and undirected edge semantics
- MUST be efficiently passable by value
- MUST integrate with std::hash for unordered containers

### 4. Design Principles
- **Zero-cost abstraction**: Implementation MUST NOT introduce runtime overhead compared to raw indices
- **Type safety**: Implementation MUST prevent mixing vertex and edge descriptors
- **STL compatibility**: Descriptors MUST work seamlessly with standard containers
- **Const-correctness**: Implementation MUST maintain proper const semantics throughout
- **Modern C++20**: Implementation SHOULD utilize concepts, requires clauses, and other C++20 features where appropriate

## Implementation Guidelines

### Phase 1: Foundation
1. Create CMake build configuration with C++20 support
2. Set up Catch2 integration for testing
3. Define descriptor concept using C++20 concepts
4. Implement basic vertex descriptor
5. Write comprehensive unit tests for vertex descriptor

### Phase 2: Edge Descriptors
1. Implement edge descriptor with source/target vertices
2. Add support for both directed and undirected semantics
3. Write comprehensive unit tests for edge descriptor
4. Ensure proper comparison and hashing

### Phase 3: Advanced Features
1. Add descriptor traits and type utilities
2. Implement descriptor property maps (optional)
3. Add range support and iterators if applicable
4. Performance benchmarks
5. Documentation and examples

## Testing Requirements

### Unit Tests (Catch2)
Each component MUST have comprehensive tests covering:
- Default construction and initialization
- Copy and move semantics
- Comparison operations (all six comparison operators)
- Hash function consistency
- Edge cases (null/invalid descriptors)
- Container usage (vector, set, unordered_map)
- Type safety (compilation failures for invalid operations)

### Test Organization
- Tests MUST use Catch2 TEST_CASE and SECTION macros
- Tests SHOULD group related tests logically
- Tests MUST include both positive and negative test cases
- Tests SHOULD test compile-time constraints where applicable

## CMake Configuration Template

```cmake
cmake_minimum_required(VERSION 3.20)
project(graph_descriptors VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

# Library target (header-only or compiled)
add_library(descriptors INTERFACE)
target_include_directories(descriptors INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(descriptors INTERFACE cxx_std_20)

# Testing
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
```

## Code Style Guidelines

- Implementation MUST use snake_case for functions and variables
- Implementation MUST use PascalCase for types and classes
- Implementation SHOULD prefer `constexpr` where possible
- Implementation SHOULD use `[[nodiscard]]` for functions that return important values
- Public APIs MUST be documented with Doxygen-style comments
- Header files MUST be self-contained (include all dependencies)

## Build Commands

```bash
# Configure
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure

# Or using ninja directly
cd build && ninja && ninja test
```

## Incremental Update Process

When updating this project:
1. Agent MUST review existing tests to understand current behavior
2. Agent MUST add new tests for new features BEFORE implementation (TDD)
3. Agent MUST implement features to make tests pass
4. Agent MUST run full test suite to ensure no regressions
5. Agent SHOULD update documentation and examples
6. Agent SHOULD update this descriptor.md file with any new requirements or design decisions

## Success Criteria

A successful implementation:
- ✓ MUST compile with C++20 compliant compilers (GCC 10+, Clang 10+, MSVC 2019+)
- ✓ MUST pass all unit tests with 100% success rate
- ✓ MUST provide zero runtime overhead compared to raw index usage
- ✓ MUST prevent common programming errors through type safety
- ✓ MUST integrate seamlessly with STL containers
- ✓ MUST have clear, well-documented public APIs
- ✓ SHOULD include working examples demonstrating usage

## Future Considerations

- Integration with graph adjacency structures
- Property maps for associating data with descriptors
- Descriptor ranges and views
- Custom allocator support
- Serialization support
- Thread-safety considerations for concurrent graph operations

## Notes for the Agent

- Agent MUST run tests after making changes
- Agent SHOULD keep the implementation header-only unless there's a compelling reason not to
- Agent SHOULD prioritize simplicity and clarity over premature optimization
- Agent MUST favor stronger type safety when in doubt
- Agent MUST document design decisions and trade-offs in code comments
- Agent SHOULD update this file when requirements evolve or design decisions are made

---

**Last Updated**: October 16, 2025
**Version**: 0.1.0
**Status**: Initial specification
