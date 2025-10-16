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

### 1. Underlying Data Representation

#### Vertex Representation
Vertices in the graph can be stored in different container types, which affects how descriptors reference them:

**Random Access Containers (e.g., `std::vector`):**
- Vertices MAY be stored as direct values in a contiguous container
- Each vertex is identified by its index position which is the Vertex ID
- The vertex itself can be any type (simple types, structs, classes)
- Examples: `std::vector<VertexData>` and `std::deque<VertexData>` where vertices are accessed by index

**Associative Containers (e.g., `std::map`, `std::unordered_map`):**
- Vertices MUST be stored as key-value pairs
- The key serves as the vertex ID (MUST be comparable and/or hashable)
- The value contains the vertex data/properties
- Example: `std::map<VertexId, VertexData>` where vertices are accessed by key
- The iterator's `value_type` is `std::pair<const Key, Value>`

#### Edge Representation
Edges in the graph can be stored in various ways depending on graph structure:

**Random Access Containers (e.g., `std::vector`):**
- Edges MAY be stored in contiguous containers
- Each edge is identified by its index position
- Two primary layouts are supported:
  - **Per-vertex adjacency storage** (RECOMMENDED): each vertex owns a random-access container of outgoing edges. The source vertex is implied by the owning container and MUST NOT be duplicated in the edge payload.
  - **Global edge storage**: all edges share a single random-access container. In this scenario the edge payload MUST include the source vertex identifier because the container position does not encode it.
- In both layouts the edge payload MUST include the target vertex identifier and MAY include additional properties.
- Examples:
  - `std::vector<std::vector<int>>` where each inner vector stores target vertex ids for a specific source vertex
  - `std::vector<Edge>` where `Edge` includes source and target vertex identifiers (for global storage)
  - `std::vector<std::pair<int,double>>` where the first element stores the target vertex id and the second stores an optional property
  - `std::vector<std::tuple<int32_t,double>>` where `int32_t` stores the target vertex id and `double` stores an optional property
  - Note: Tuple-like edge payloads MUST have 1 or more elements, and the first element MUST be the target vertex id

**Forward/Bidirectional Containers (e.g., `std::list`, `std::set`, `std::map`, adjacency lists):**
- Each edge is identified by an iterator to its position
- Edges MAY be stored in linked structures or per-vertex adjacency lists
- The same container value types as random access containers apply (simple types, pairs, tuples, or structs)
- The choice between per-vertex adjacency storage and a global container follows the same rules as above: omit the source vertex in per-vertex containers, include it when using a shared/global container.
- Examples:
  - `std::list<int>` where `int` stores the target vertex id
  - `std::list<std::pair<int,double>>` where first element is target vertex id, second is edge property
  - `std::set<int>` where `int` stores the target vertex id (for unweighted graphs)
  - `std::map<int,EdgeData>` where key is target vertex id and value contains edge properties
  - `std::list<Edge>` where `Edge` follows the same source-inclusion rules described above

**Edge Data Structure Requirements:**
- An edge representation SHOULD contain at minimum:
  - Target vertex identifier (as the first element if tuple-like, or as a simple value)
  - Optional edge weight or properties (if tuple-like or struct)
- Whether the edge payload stores the source vertex identifier depends on storage layout:
  - For per-vertex adjacency storage, the source vertex identifier MUST NOT be duplicated in the edge payload.
  - For global edge storage, the edge payload MUST include the source vertex identifier because container location does not encode it.
- Examples:
  - Per-vertex adjacency: `struct Edge { vertex_id target; EdgeData data; }`
  - Global storage: `struct Edge { vertex_id source; vertex_id target; EdgeData data; }`
- Note: The edge_descriptor will maintain both the edge location and the source vertex_descriptor, regardless of storage layout

### 2. Descriptor Base Concept/Interface
- Descriptors MUST be lightweight, copyable handles
- Descriptors MUST support comparison operations (==, !=, <, <=, >, >=)
- Descriptors MUST satisfy the C++20 `std::incrementable` concept (support pre/post increment semantics)
- Descriptors MUST be hashable for use in STL containers
- Descriptors MUST be default constructible and trivially copyable
- Descriptors SHOULD provide a null/invalid state representation
- Descriptors MUST be type-safe: vertex and edge descriptors SHALL be distinct types

### 3. Vertex Descriptor
- MUST represent a handle to a vertex in the graph
- MUST be a template with a single parameter for the underlying container's iterator type
- Iterator category constraints:
  - **Random Access Iterator** (e.g., vector, deque): Used for index-based vertex storage
  - **Bidirectional Iterator** (e.g., map, unordered_map): Used for key-value based vertex storage
    - When bidirectional: the iterator's `value_type` MUST satisfy a pair-like concept where:
      - The type MUST have at least 2 members (accessible via tuple protocol or pair interface)
      - The first element serves as the vertex ID (key)
      - This can be checked using `std::tuple_size<value_type>::value >= 2` or by requiring `.first` and `.second` members
- MUST have a single member variable that:
  - MUST be `size_t index` when the iterator is a random access iterator
  - MUST be the iterator type itself when the iterator is a bidirectional iterator (non-random access)
- MUST provide a `vertex_id()` member function that returns the vertex's unique identifier:
  - When the vertex iterator is random access: MUST return the `size_t` member value (the index)
  - When the vertex iterator is bidirectional: MUST return the key (first element) from the pair-like `value_type`
  - Return type SHOULD be deduced appropriately based on iterator category
- MUST provide a public `value()` member function that returns the underlying storage handle:
  - When the vertex iterator is random access: `value()` MUST return the stored `size_t` index
  - When the vertex iterator is bidirectional: `value()` MUST return the stored iterator
  - Return type SHOULD be the exact type of the underlying member (copy by value)
- MUST provide pre-increment and post-increment operators (`operator++` / `operator++(int)`) whose behavior mirrors the underlying storage:
  - For random access iterators: increment operations MUST advance the `size_t` index by one
  - For bidirectional iterators: increment operations MUST advance the stored iterator
- MUST be efficiently passable by value
- SHOULD support conversion to/from underlying index type (for random access case)
- MUST integrate with std::hash for unordered containers

### 4. Edge Descriptor
- MUST represent a handle to an edge in the graph
- MUST be a template with two parameters:
  - First parameter: the underlying edge container's iterator type
  - Second parameter: the vertex iterator type
- MUST have two member variables:
  - First member: MUST be `size_t index` (for edge index) when the edge iterator is a random access iterator, or the edge iterator type itself when the edge iterator is a forward/bidirectional iterator (non-random access)
  - Second member: MUST be a `vertex_descriptor` (instantiated with the vertex iterator type from the second template parameter) representing the source vertex
- The edge descriptor identifies both WHERE the edge is stored (via first member) and WHICH vertex it originates from (via second member)
- MUST provide a public `value()` member function that returns the underlying edge storage handle:
  - When the edge iterator is random access: `value()` MUST return the stored `size_t` index
  - When the edge iterator is forward/bidirectional: `value()` MUST return the stored edge iterator
  - Return type SHOULD be the exact type of the underlying first member (copy by value)
- MUST provide pre-increment and post-increment operators whose behavior mirrors the underlying edge storage:
  - For random access iterators: increment operations MUST advance the `size_t` index by one while leaving the source `vertex_descriptor` unchanged
  - For forward/bidirectional iterators: increment operations MUST advance the stored edge iterator while leaving the source `vertex_descriptor` unchanged
- Directed/undirected semantics are determined by the graph structure, not by the descriptor itself
- MUST be efficiently passable by value
- MUST integrate with std::hash for unordered containers

### 5. Descriptor Views

***IMPORTANT LIMITATION***: Descriptor views are restricted to forward iteration only. This is because the descriptor is synthesized by the iterator during traversal (the descriptor contains the iterator itself), making it incompatible with random access operations. Specifically, operator[] would need to return a reference to a descriptor, but since the descriptor is created on-the-fly by the iterator, there is no stable object to reference. This design enables the interface to work uniformly across different storage strategies (vectors, maps, custom containers) while maintaining descriptor-based abstraction.

#### Vertex Descriptor View
- MUST provide a view over the underlying vertex storage that yields `vertex_descriptor` values
- MUST model `std::ranges::view` (lightweight, copyable, reference semantics)
- View iterator requirements:
  - Iterator value_type MUST be `vertex_descriptor`
  - Iterator MUST satisfy the `std::forward_iterator` concept. Random-access operations (e.g., `operator[]`, `operator+=`) MUST NOT be provided because descriptors are synthesized on the fly during traversal.
  - Increment operations MUST advance the underlying index/iterator consistent with the corresponding descriptor semantics
- MUST expose `begin()`/`end()` (const and non-const) returning forward iterators that satisfy the above requirements
- MUST NOT copy vertex storage; underlying containers MUST be referenced via pointer/reference semantics
- MAY provide `size()`/`empty()` helpers when the underlying container supports them
- Implementations SHOULD derive from `std::ranges::view_interface` unless a compelling reason prevents it
- Rationale: constraining the iterator to forward traversal ensures the view works uniformly across storage strategies (vectors, maps, custom containers) while maintaining descriptor-based abstraction and avoiding invalid references

#### Edge Descriptor View
- MUST provide a view over the underlying edge storage that yields `edge_descriptor` values
- MUST model `std::ranges::view`
- View iterator requirements:
  - Iterator value_type MUST be `edge_descriptor`
  - Iterator MUST satisfy the `std::forward_iterator` concept. Random-access operations MUST NOT be provided for the same reasons as the vertex descriptor view (descriptors are synthesized per step).
  - Increment operations MUST advance the underlying edge index/iterator and preserve the source `vertex_descriptor`
- MUST support both per-vertex adjacency storage and global edge storage configurations
- MUST expose `begin()`/`end()` (const and non-const) returning forward iterators that satisfy the above requirements
- MUST NOT copy edge storage; underlying containers MUST be referenced via pointer/reference semantics
- MAY provide helpers (e.g., `size()`, `empty()`) when supported by the underlying storage
- Implementations SHOULD derive from `std::ranges::view_interface` unless a compelling reason prevents it
- Rationale: restricting to forward iteration guarantees compatibility with all supported storage layouts while keeping descriptors ephemeral and preventing invalid references

### 6. Design Principles
- **Zero-cost abstraction**: Implementation MUST NOT introduce runtime overhead compared to raw indices
- **Type safety**: Implementation MUST prevent mixing vertex and edge descriptors
- **STL compatibility**: Descriptors MUST work seamlessly with standard containers
- **Const-correctness**: Implementation MUST maintain proper const semantics throughout
- **Modern C++20**: Implementation SHOULD utilize concepts, requires clauses, and other C++20 features where appropriate

## Implementation Guidelines

### Phase 1: Foundation
1. Create CMake build configuration with C++20 support
2. Set up Catch2 integration for testing
3. Define descriptor concept using C++20 concepts, including:
   - Pair-like concept for bidirectional iterator value_type (using `std::tuple_size >= 2` or `.first`/`.second` members)
   - Random access and bidirectional iterator concepts
4. Implement basic vertex descriptor template with:
   - Template parameter for container iterator type
   - Concept constraints: random access OR (bidirectional AND pair-like value_type)
   - Conditional member type based on iterator category (size_t for random access, iterator for bidirectional)
   - `vertex_id()` member function with conditional implementation based on iterator category
  - `value()` member function returning the underlying index or iterator
  - Pre/post increment operators that forward to index/iterator increments
5. Implement `vertex_descriptor_view` that wraps the underlying vertex storage, yields descriptors, models `std::ranges::view`, exposes forward-only iterators, and (SHOULD) derives from `std::ranges::view_interface`
6. Write comprehensive unit tests for vertex descriptor and vertex descriptor view with both random access and bidirectional iterators (including map-based containers), covering `vertex_id()`, `value()`, increment semantics, and verifying the view satisfies `std::forward_range`

### Phase 2: Edge Descriptors
1. Implement edge descriptor template with:
   - Two template parameters (edge container iterator and vertex iterator)
   - First member variable: conditional based on edge iterator category (size_t for random access, iterator for forward/bidirectional)
   - Second member variable: vertex_descriptor instantiated with the vertex iterator type (represents source vertex)
   - Proper std::random_access_iterator and std::forward_iterator concept constraints
  - Pre/post increment operators consistent with underlying storage semantics
  - `value()` member function returning the underlying edge index or iterator
2. Implement `edge_descriptor_view` that adapts both per-vertex adjacency storage and global edge storage while yielding descriptors, modelling a forward-only view, and (SHOULD) deriving from `std::ranges::view_interface`
3. Write comprehensive unit tests for edge descriptor and edge descriptor view with both random access and forward iterators
4. Test `value()`/increment behavior, confirm the view satisfies `std::forward_range`, and cover various edge data types (simple integers, pairs, tuples, structs)
5. Ensure proper comparison and hashing

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
- Increment operations (pre/post increment semantics)
- `value()` accessors returning the correct underlying handle types
- Hash function consistency
- Edge cases (null/invalid descriptors)
- Container usage (vector, set, unordered_map)
- Descriptor views (vertex and edge): forward-range compliance, iterator value_type requirements, absence of random-access operations, and behavior of `view_interface` helpers when used
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
