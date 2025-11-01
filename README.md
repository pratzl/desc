# Graph Library

A modern C++20 graph library providing a comprehensive, customizable interface for graph data structures, algorithms, and views.

## Overview

This library provides the foundation for a complete graph library following the P1709 Graph Library proposal. It includes:

- **Descriptor System**: Type-safe, efficient descriptors for vertices and edges that abstract storage strategies
- **Graph Container Interface**: Standardized concepts, traits, and functions for adjacency lists and edgelists
- **Customization Point Objects (CPOs)**: All graph operations can be customized for existing data structures
- **Range-Based Design**: Graphs as ranges of vertices, where each vertex is a range of edges
- **Documentation**: Comprehensive documentation following P1709 conventions

**Current Status**: Phase 4 complete - All core CPOs, value access CPOs, and optional partitioning/sourced edge CPOs implemented with comprehensive tests. Ready for adjacency list concepts and traits definition.

## Features

### Core Descriptor System
- **Type-safe descriptors**: Vertex and edge descriptors are distinct types, preventing accidental misuse
- **Zero-cost abstraction**: No runtime overhead compared to raw indices/iterators
- **Storage flexibility**: Works with random-access containers (vector, deque) and associative containers (map, unordered_map)
- **Descriptor views**: `vertex_descriptor_view` and `edge_descriptor_view` provide range-based iteration
- **Automatic pattern support**: `vertices(g)` and `edges(g, u)` automatically return descriptor views for containers following inner value and edge value patterns
- **Hash support**: Built-in std::hash specializations for use in unordered containers

### Graph Container Interface (GCI)
- **Standardized concepts**: Edge, vertex, and adjacency list concepts for type checking
- **Flexible implementations**: Supports vector-of-lists, CSR, adjacency matrix, and custom graph types
- **CPO-based architecture**: All functions are customization point objects following MSVC style
- **Default implementations**: Reasonable defaults minimize work to adapt existing data structures
- **Pattern recognition**: Automatic support for common edge patterns (tuples, edge_info)
- **Partition support**: Unipartite, bipartite, and multipartite graphs
- **Edgelist interface**: Separate interface for edge lists with source/target ID access

### Documentation
- **Complete naming conventions**: All template parameters, type aliases, and variable names from P1709
- **Container interface specification**: Comprehensive summary of concepts, traits, types, and functions
- **CPO implementation guide**: Detailed patterns for creating customization point objects
- **Common guidelines**: Architectural commitments and project structure requirements

## Requirements

- C++20 compliant compiler (GCC 10+, Clang 10+, MSVC 2019+)
- CMake 3.20 or later
- Ninja (optional, recommended)

## Building

```bash
# Configure
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure
```

## Quick Start

### Automatic Graph Support (No Custom Code Needed!)

```cpp
#include <graph/graph.hpp>
#include <vector>
#include <map>

// Simple adjacency list: vector of vectors of ints
std::vector<std::vector<int>> adj_list = {
    {1, 2},      // Vertex 0 -> edges to 1, 2
    {2, 3},      // Vertex 1 -> edges to 2, 3
    {3},         // Vertex 2 -> edge to 3
    {}           // Vertex 3 -> no edges
};

// Works automatically with vertices(g) and edges(g, u)!
for (auto u : graph::vertices(adj_list)) {
    std::cout << "Vertex " << u.vertex_id() << " connects to: ";
    for (auto e : graph::edges(adj_list, u)) {
        std::cout << e.target_id(adj_list) << " ";
    }
    std::cout << "\n";
}

// Map-based graph also works automatically
std::map<int, std::vector<int>> map_graph = {
    {100, {200, 300}},
    {200, {300}},
    {300, {}}
};

for (auto u : graph::vertices(map_graph)) {
    std::cout << "Vertex " << u.vertex_id() << " connects to: ";
    for (auto e : graph::edges(map_graph, u)) {
        std::cout << e.target_id(map_graph) << " ";
    }
    std::cout << "\n";
}
```

### Vertex Descriptors with Vector Storage

```cpp
#include <graph/vertex_descriptor.hpp>
#include <graph/vertex_descriptor_view.hpp>
#include <vector>

std::vector<int> vertices = {10, 20, 30, 40, 50};

using VectorIter = std::vector<int>::iterator;
using VD = graph::vertex_descriptor<VectorIter>;

// Create descriptor for vertex at index 2
VD vd{2};
std::cout << "Vertex ID: " << vd.vertex_id() << "\n";  // Output: 2

// Iterate over all vertices
graph::vertex_descriptor_view view{vertices};
for (auto desc : view) {
    std::cout << "Vertex " << desc.vertex_id() << "\n";
}
```

### Vertex Descriptors with Map Storage

```cpp
#include <graph/vertex_descriptor.hpp>
#include <graph/vertex_descriptor_view.hpp>
#include <map>

std::map<int, std::string> vertex_map = {
    {100, "Node A"},
    {200, "Node B"},
    {300, "Node C"}
};

using MapIter = std::map<int, std::string>::iterator;
using VD = graph::vertex_descriptor<MapIter>;

// Create descriptor from iterator
auto it = vertex_map.find(200);
VD vd{it};
std::cout << "Vertex ID: " << vd.vertex_id() << "\n";  // Output: 200

// Iterate over all vertices
graph::vertex_descriptor_view map_view{vertex_map};
for (auto desc : map_view) {
    std::cout << "Vertex " << desc.vertex_id() << "\n";
}
```

## Implementation Status

### ✅ Phase 1: Foundation (COMPLETE)
- [x] **Directory Structure**: Standard graph library layout (algorithm/, container/, views/, detail/)
- [x] **Descriptor System**:
  - Core descriptor concepts and traits
  - Vertex descriptors (keyed and direct storage)
  - Edge descriptors with source vertex tracking
  - Descriptor views for range-based access
  - Comprehensive unit tests (75 tests passing for descriptors)
- [x] **Infrastructure**:
  - CPO framework with _Choice_t pattern
  - Common std library imports (detail/graph_using.hpp)
  - Graph information structures (graph_info.hpp)
  - Edge list container (edgelist.hpp)
  - CMake build system with presets
  - Catch2 test framework integration
- [x] **Documentation**:
  - Complete naming conventions from P1709
  - Graph Container Interface specification
  - CPO implementation guide
  - Common graph guidelines
  - Migration documentation

### 🔄 Phase 2: Graph Utility CPOs (IN PROGRESS)
Implement core graph operation CPOs in `graph_cpo.hpp` following the canonical order:

**Phase 1: Core Foundation (Essential)**
- [x] `vertices(g)` - Get vertex range (returns `vertex_descriptor_view`) ✅ **COMPLETE** - 18 tests passing
- [x] Type aliases: `vertex_range_t<G>`, `vertex_iterator_t<G>`, `vertex_t<G>` ✅ **COMPLETE** - 5 tests passing
- [x] `vertex_id(g, u)` - Get vertex ID from descriptor ✅ **COMPLETE** - 15 tests passing
  - Resolution order (inner_value first):
    1. `u.inner_value(g).vertex_id(g)` - Inner value member (highest priority)
    2. `vertex_id(g, u.inner_value(g))` - ADL with inner_value
    3. `vertex_id(g, u)` - ADL with descriptor
    4. `u.vertex_id()` - Descriptor default (lowest priority)
- [x] Type alias: `vertex_id_t<G>` ✅ **COMPLETE** - 2 tests passing
- [x] `find_vertex(g, uid)` - Find vertex by ID ✅ **COMPLETE** - 11 tests passing
  - Resolution order:
    1. `g.find_vertex(uid)` - Member function (highest priority)
    2. `find_vertex(g, uid)` - ADL (medium priority)
    3. `std::ranges::next(std::ranges::begin(vertices(g)), uid)` - Random access default (lowest priority)
  - **Note**: Default implementation requires `sized_range` (works with vector/deque, maps need custom implementation)
- [x] `edges(g, u)` - Get outgoing edges from vertex (returns `edge_descriptor_view`) ✅ **COMPLETE** - 14 tests passing
  - Resolution order:
    1. `g.edges(u)` - Member function (highest priority)
    2. `edges(g, u)` - ADL (medium priority)
    3. `edge_descriptor_view(u.inner_value(g), u)` - Edge value pattern default (lowest priority)
  - **Automatic support** for containers following edge value patterns: simple (int), pair, tuple, custom
- [x] Type aliases: `vertex_edge_range_t<G>`, `vertex_edge_iterator_t<G>`, `edge_t<G>` ✅ **COMPLETE**
- [x] `target_id(g, uv)` - Get target vertex ID from edge ✅ **COMPLETE** - 20 tests passing
  - Resolution order:
    1. `target_id(g, uv)` - ADL (highest priority)
    2. `uv.target_id(edges)` - Edge descriptor default (lowest priority)
  - **Automatic extraction** for edge patterns: simple (int → itself), pair (→ .first), tuple (→ get<0>), custom via ADL
- [x] `target(g, uv)` - Get target vertex descriptor from edge ✅ **COMPLETE** - 28 tests passing
  - Resolution order:
    1. `g.target(uv)` - Member function (highest priority)
    2. `target(g, uv)` - ADL (medium priority)
    3. `*find_vertex(g, target_id(g, uv))` - Default (lowest priority)
  - **Return type flexibility**: Custom implementations (member/ADL) can return either:
    - `vertex_descriptor` (vertex_t<G>) - used as-is
    - `vertex_iterator` (iterator to vertices) - automatically converted to descriptor
  - **Smart type conversion**: Internal `_to_vertex_descriptor` helper properly handles both direct descriptors and iterators from `vertices(g)`
  - **Default implementation**: Combines `find_vertex` and `target_id` for convenience
  - **Performance**: O(1) for random-access graphs, O(log n) or O(1) average for associative graphs

**Phase 2: Query Functions (High Priority)**
- [x] `num_vertices(g)` - Count vertices in graph ✅ **COMPLETE** - 24 tests passing
  - Resolution order:
    1. `g.num_vertices()` - Member function (highest priority)
    2. `num_vertices(g)` - ADL (medium priority)
    3. `std::ranges::size(g)` - Ranges default (lowest priority)
  - **Automatic support** for any sized_range (vector, deque, map, unordered_map, etc.)
- [x] `num_edges(g)` - Count total edges in graph ✅ **COMPLETE** - 31 tests passing
  - Resolution order:
    1. `g.num_edges()` - Member function (highest priority)
    2. `num_edges(g)` - ADL (medium priority)
    3. Iterate vertices and sum edge counts - Default (lowest priority)
  - **Default implementation**: Iterates through all vertices using `vertices(g)`, for each vertex calls `edges(g, u)`, uses `std::ranges::size()` for sized ranges or `std::ranges::distance()` for non-sized ranges
  - **Note**: For directed graphs counts each edge once; for undirected graphs counts each edge twice (once per endpoint)
- [x] `edges(g, uid)` - Get outgoing edges from vertex by ID (convenience wrapper) ✅ **COMPLETE** - 19 tests passing
  - Resolution order:
    1. `g.edges(uid)` - Member function (highest priority)
    2. `edges(g, uid)` - ADL (medium priority)
    3. `edges(g, *find_vertex(g, uid))` - Default using find_vertex (lowest priority)
  - **Note**: Convenience function that combines find_vertex + edges(g,u) to avoid boilerplate
  - **Return type**: Returns `edge_descriptor_view` (same as edges(g,u))
  - **Dual overload pattern**: Uses `requires (!vertex_descriptor_type<VId>)` constraint to differentiate from edges(g,u)
- [x] `degree(g, u)` - Get out-degree of vertex (descriptor version) ✅ **COMPLETE** - 17 tests passing
  - Resolution order:
    1. `g.degree(u)` - Member function (highest priority)
    2. `degree(g, u)` - ADL (medium priority)
    3. `std::ranges::size(edges(g, u))` - Default using edges (lowest priority)
  - **Return type**: Integral type (size_t or similar)
  - **Default implementation**: Uses `std::ranges::size()` on result of `edges(g, u)` when available
- [x] `degree(g, uid)` - Get out-degree of vertex by ID (convenience wrapper) ✅ **COMPLETE** - Included in 17 tests
  - Resolution order:
    1. `g.degree(uid)` - Member function (highest priority)
    2. `degree(g, uid)` - ADL (medium priority)
    3. `degree(g, *find_vertex(g, uid))` - Default using find_vertex (lowest priority)
  - **Note**: Convenience function that combines find_vertex + degree(g,u) to avoid boilerplate
  - **Dual overload pattern**: Uses `requires (!vertex_descriptor_type<VId>)` constraint to differentiate from degree(g,u)

**Phase 3: Edge Queries (Medium Priority)**
- [x] `find_vertex_edge(g, u, v)` - Find edge from u to v ✅ **COMPLETE** - Included in 17 tests
  - Resolution order:
    1. `g.find_vertex_edge(u, v)` - Member function (highest priority)
    2. `find_vertex_edge(g, u, v)` - ADL (medium priority)
    3. Iterates `edges(g, u)` comparing `target_id(g, e)` (lowest priority)
  - **Return type**: Edge descriptor (like iterator), returns end sentinel if not found
  - **Triple overload pattern**: Three versions for different parameter combinations
- [x] `find_vertex_edge(g, u, vid)` - Find edge from u to vid ✅ **COMPLETE** - Included in 17 tests
  - Resolution order:
    1. `g.find_vertex_edge(u, vid)` - Member function (highest priority)
    2. `find_vertex_edge(g, u, vid)` - ADL (medium priority)
    3. Iterates `edges(g, u)` comparing `target_id(g, e) == vid` (lowest priority)
  - **Constraint**: Uses `requires (!vertex_descriptor_type<VId>)` to differentiate from (g,u,v) overload
- [x] `find_vertex_edge(g, uid, vid)` - Find edge from uid to vid ✅ **COMPLETE** - Included in 17 tests
  - Resolution order:
    1. `g.find_vertex_edge(uid, vid)` - Member function (highest priority)
    2. `find_vertex_edge(g, uid, vid)` - ADL (medium priority)
    3. `find_vertex_edge(g, *find_vertex(g, uid), vid)` - Default using find_vertex (lowest priority)
  - **Note**: Convenience function for ID-based edge queries
- [x] `contains_edge(g, u, v)` - Check if edge exists ✅ **COMPLETE** - Included in 21 tests
  - Resolution order:
    1. `g.contains_edge(u, v)` - Member function (highest priority)
    2. `contains_edge(g, u, v)` - ADL (medium priority)
    3. Iterates `edges(g, u)` comparing `target_id(g, e)` with `vertex_id(g, v)` (lowest priority)
  - **Return type**: `bool` - true if edge exists, false otherwise
  - **Note**: More efficient than find_vertex_edge for existence checks
- [x] `contains_edge(g, uid, vid)` - Check if edge exists ✅ **COMPLETE** - Included in 21 tests
  - Resolution order:
    1. `g.contains_edge(uid, vid)` - Member function (highest priority)
    2. `contains_edge(g, uid, vid)` - ADL (medium priority)
    3. `find_vertex(g, uid)` then iterate `edges(g, u)` comparing `target_id(g, e) == vid` (lowest priority)
  - **Dual overload pattern**: Uses `requires (!vertex_descriptor_type<UId> && !vertex_descriptor_type<VId>)` to differentiate
- [x] `has_edge(g)` - Check if graph has any edges ✅ **COMPLETE** - 33 tests passing
  - Resolution order:
    1. `g.has_edge()` - Member function (highest priority)
    2. `has_edge(g)` - ADL (medium priority)
    3. Iterate `vertices(g)` checking if any vertex has non-empty `edges(g, u)` (lowest priority)
  - **Return type**: `bool` - true if graph has at least one edge, false otherwise
  - **Default implementation**: Uses `std::ranges::find_if` to find first vertex with non-empty edge range (short-circuits on first match)
  - **Note**: Efficient early termination - stops at first vertex with edges

**Phase 4: Value Access (Optional)**
- [x] `vertex_value(g, u)` - Get user-defined vertex value ✅ **COMPLETE** - 21 tests passing
  - Resolution order:
    1. `g.vertex_value(u)` - Member function (highest priority)
    2. `vertex_value(g, u)` - ADL (medium priority)
    3. `u.inner_value(g)` - Default using descriptor's inner_value (lowest priority)
  - **Return type**: `decltype(auto)` - Perfect forwarding of references
  - **Default implementation**: Delegates to `u.inner_value(g)` which handles different container types:
    * Random-access (vector): returns `container[index]`
    * Associative (map): returns `.second` value (not the key)
    * Bidirectional: returns dereferenced value
  - **Note**: Provides access to user-defined vertex properties/data stored in the graph
- [x] `edge_value(g, uv)` - Get user-defined edge value ✅ **COMPLETE** - 33 tests passing
  - Resolution order:
    1. `g.edge_value(uv)` - Member function (highest priority)
    2. `edge_value(g, uv)` - ADL (medium priority)
    3. `uv.inner_value(edges)` - Default using edge descriptor's inner_value (lowest priority)
  - **Return type**: `decltype(auto)` - Perfect forwarding of references (supports both by-value and by-reference returns)
  - **Default implementation**: Delegates to `uv.inner_value(uv.source().inner_value(g))` which extracts properties based on edge pattern:
    * Simple (int): returns the value itself (target ID only, no separate property)
    * Pair (target, weight): returns `.second` (the weight/property)
    * Tuple (target, prop1, prop2, ...): returns tuple of elements [1, N) (all properties except target)
    * Custom struct: returns the whole value (user manages property access)
  - **Note**: Honors const-correctness - returns const reference when graph is const, mutable reference when graph is mutable
- [x] `graph_value(g)` - Get user-defined graph value ✅ **COMPLETE** - 27 tests passing
  - Resolution order:
    1. `g.graph_value()` - Member function (highest priority)
    2. `graph_value(g)` - ADL (lowest priority)
  - **Return type**: `decltype(auto)` - Perfect forwarding of references (supports both by-value and by-reference returns)
  - **No default implementation**: Compile-time error if neither member nor ADL is found
  - **Note**: Provides access to user-defined graph-level properties/metadata (e.g., name, version, statistics)
  - **Use cases**: Graph metadata, version tracking, global parameters (weight multipliers), statistics

**Phase 5: Optional Features (Sourced Edges & Partitioning)**
- [x] `source_id(g, uv)` - Get source vertex ID (for sourced edges) ✅ **COMPLETE** - No tests (CPO only)
  - Resolution order:
    1. `g.source_id(uv)` - Member function (highest priority)
    2. `source_id(g, uv)` - ADL (lowest priority)
  - **No default implementation**: Unlike target_id which works for standard adjacency lists, source requires explicit implementation
  - **Use cases**: Bidirectional graphs, edge lists, graphs where edges explicitly track their source vertex
  - **Return type**: Vertex ID (by value)
- [x] `source(g, uv)` - Get source vertex descriptor (for sourced edges) ✅ **COMPLETE** - No tests (CPO only)
  - Resolution order:
    1. `g.source(uv)` - Member function (highest priority)
    2. `source(g, uv)` - ADL (lowest priority)
  - **No default implementation**: Unlike target which has default via find_vertex, source requires explicit implementation
  - **Return type flexibility**: Custom implementations can return either:
    - `vertex_descriptor` (vertex_t<G>) - used as-is
    - `vertex_iterator` (iterator to vertices) - automatically converted to descriptor
  - **Smart type conversion**: Internal `_to_vertex_descriptor` helper properly handles both cases
  - **Use cases**: Bidirectional graphs, edge list graphs where edges are stored independently
- [x] `partition_id(g, u)` - Get partition ID for a vertex ✅ **COMPLETE** - 16 tests passing
  - Resolution order:
    1. `g.partition_id(u)` - Member function (highest priority)
    2. `partition_id(g, u)` - ADL (medium priority)
    3. Default: returns 0 (lowest priority)
  - **Default implementation**: Assumes single partition where all vertices belong to partition 0
  - **Use cases**: Multi-partition distributed graphs, graph coloring, NUMA-aware partitioning, load balancing
  - **Return type**: Integral type (default 0)
  - **Noexcept**: Default implementation is noexcept
- [x] `num_partitions(g)` - Get number of partitions in the graph ✅ **COMPLETE** - 27 tests passing
  - Resolution order:
    1. `g.num_partitions()` - Member function (highest priority)
    2. `num_partitions(g)` - ADL (medium priority)
    3. Default: returns 1 (lowest priority)
  - **Default implementation**: Assumes single partition (all vertices in partition 0)
  - **Relationship to partition_id**: partition_id values should be in range [0, num_partitions)
  - **Use cases**: Distributed graphs, dynamic partition counting, graph coloring results, NUMA-aware partition counts
  - **Return type**: Integral type (default 1)
  - **Noexcept**: Default implementation is noexcept

Unit tests and documentation for each CPO added.

### 📋 Phase 6: Adjacency List Concepts & Traits (PLANNED)
- [ ] Define adjacency list concepts in `graph/adjacency_list_concepts.hpp`:
  - Edge concepts:
    * `targeted_edge` - Target and Target IDs only
    * `sourced_edge` - Source and Source IDs only
    * `sourced_targeted_edge` - Target IDs and Source IDs
    * `targeted_edge_range` - Forward range of targeted_edges
    * `sourced_targeted_edge_range` - Forward range of edges that support sourced_edge and targeted_edge concepts
  - Vertex concepts:
    * `vertex_range` - Sized, forward range of vertices with Vertex ID
    * `index_vertex_range` - Random access range of vertices with Vertex ID
  - Adjacency list concepts:
    - `adjacency_list` - Concept for graphs with adjacency list structure using vertex_range and targeted_edge_range.
    - `index_adjacency_list` - Concept for graphs with adjacency list structure using index_vertex_range and targeted_edge_range.
    - `sourced_adjacency_list` - Concept for graphs with adjacency list structure using vertex_range and sourced_targeted_edge_range.
    - `index_sourced_adjacency_list` - Concept for graphs with adjacency list structure using index_vertex_range and sourced_targeted_edge_range.
  - Requirements:
    * Container type traits (vertex/edge container types)
    * Iterator requirements
    * Size/capacity operations
    * Modification operations (add/remove vertices/edges)
- [ ] Define adjacency list traits in `graph/adjacency_list_traits.hpp`:
  - `has_degree<G>` - A graph supports the degree(g,u) and degree(g,uid) functions
  - `has_find_vertex<G>` - A graph supports find_vertex(g,uid)
  - `has_find_vertex_edge<G>` - A graph supports find_vertex_edge(g,u,v), find_vertex_edge(g,u,vid) and find_vertex_edge(g,uid,vid)
  - `has_contains_edge<G, V>` - A graph supports contains_edge(g,u,v) and contains_edge(g,uid,vid)
  - `define_unordered_edge<G>` - Overridable trait on a graph that defaults to false. When true, logic in views must guarantee that the target vertex or vertex id is not the same where it came from (the source) when traversing the graph.
  - Helper metafunctions for compile-time queries
- [ ] Unit tests for concepts and traits:
  - Concept satisfaction with various container types
  - Trait extraction and correctness
  - SFINAE-friendly design verification
  - Integration with existing descriptor framework

### 📋 Phase 7: First Container Implementation (PLANNED)
### 📋 Phase 8: Basic Algorithms (PLANNED)
- [ ] Foundational algorithms in `include/graph/algorithm/`:
  - `breadth_first_search.hpp` - BFS traversal
  - `depth_first_search.hpp` - DFS traversal
  - `dijkstra_shortest_paths.hpp` - Single-source shortest paths
  - `topological_sort.hpp` - Topological ordering

### 📋 Phase 9: Views and Adaptors (PLANNED)
- [ ] Graph views in `include/graph/views/`:
  - `vertices.hpp` - Vertex range views
  - `edges.hpp` - Edge range views
  - `filtered_graph.hpp` - Filtered graph views
  - `reverse_graph.hpp` - Reversed graph views

## Project Structure

```
desc/
├── benchmark/              # Performance benchmarks (future)
├── docs/                   # Documentation
│   ├── common_graph_guidelines.md    # Naming conventions and requirements
│   ├── container_interface.md        # GCI specification summary
│   ├── cpo.md                         # CPO implementation guide
│   └── vertex_storage_concepts.md    # Storage pattern concepts
├── examples/               # Example programs
│   └── basic_usage.cpp    # Basic descriptor usage
├── include/                # Public headers
│   └── graph/
│       ├── algorithm/     # Graph algorithms (future)
│       ├── container/     # Graph containers (future)
│       ├── detail/        # Implementation details
│       │   ├── graph_cpo.hpp      # CPO framework
│       │   └── graph_using.hpp    # Common std imports
│       ├── views/         # Graph views (future)
│       ├── descriptor.hpp           # Core descriptor concepts
│       ├── descriptor_traits.hpp    # Descriptor type traits
│       ├── vertex_descriptor.hpp    # Vertex descriptor
│       ├── vertex_descriptor_view.hpp
│       ├── edge_descriptor.hpp      # Edge descriptor
│       ├── edge_descriptor_view.hpp
│       ├── edgelist.hpp            # Edge list container
│       ├── graph.hpp               # Main library header
│       ├── graph_info.hpp          # Graph information structures
│       └── graph_utility.hpp       # Utility CPOs (stub)
├── scripts/                # Build and maintenance scripts
│   └── format.sh          # Code formatting script
├── tests/                  # Unit tests (490 tests, all passing)
│   ├── test_contains_edge_cpo.cpp
│   ├── test_degree_cpo.cpp
│   ├── test_descriptor_traits.cpp
│   ├── test_edge_concepts.cpp
│   ├── test_edge_descriptor.cpp
│   ├── test_edge_value_cpo.cpp
│   ├── test_edges_cpo.cpp
│   ├── test_edges_uid_cpo.cpp
│   ├── test_find_vertex_cpo.cpp
│   ├── test_find_vertex_edge_cpo.cpp
│   ├── test_graph_value_cpo.cpp
│   ├── test_has_edge_cpo.cpp
│   ├── test_num_edges_cpo.cpp
│   ├── test_num_partitions_cpo.cpp
│   ├── test_num_vertices_cpo.cpp
│   ├── test_partition_id_cpo.cpp
│   ├── test_target_cpo.cpp
│   ├── test_target_id_cpo.cpp
│   ├── test_type_aliases.cpp
│   ├── test_vertex_concepts.cpp
│   ├── test_vertex_descriptor.cpp
│   ├── test_vertex_id_cpo.cpp
│   ├── test_vertex_value_cpo.cpp
│   └── test_vertices_cpo.cpp
├── CMakeLists.txt         # Root CMake configuration
├── CMakePresets.json      # CMake build presets
├── MIGRATION_PHASE1.md    # Phase 1 migration details
├── PHASE1_COMPLETE.md     # Phase 1 completion report
└── README.md              # This file
```

## Design Principles

### 1. Descriptor View Architecture
**IMPORTANT**: All range-returning CPOs must return descriptor views:
- `vertices(g)` MUST return `vertex_descriptor_view` 
- `edges(g, u)` MUST return `edge_descriptor_view`

This ensures consistent descriptor semantics across all graph types. The CPOs use a three-tier resolution:
1. Custom override via member function (e.g., `g.vertices()`)
2. Custom override via ADL (e.g., `vertices(g)`)
3. Default implementation using pattern recognition:
   - `vertices(g)` → `vertex_descriptor_view(g)` if g follows inner value patterns
   - `edges(g, u)` → `edge_descriptor_view(u.inner_value(), u)` if u.inner_value() follows edge value patterns

### 2. CPO-Based Interface
All graph operations are customization point objects (CPOs) following MSVC standard library style:
- Member functions (highest priority)
- ADL-findable free functions (medium priority)  
- Default implementations (lowest priority)

This allows adaptation of existing graph data structures without modification.

### 3. Outgoing Edge Focus
The interface focuses on outgoing edges from vertices. If a graph exposes incoming edges, a separate view presents them through the same outgoing-edge interface for consistency.

### 4. Range-of-Ranges Model
Adjacency lists are treated as ranges of vertices, where each vertex is a range of edges. This enables use of standard algorithms and range adaptors.

### 5. Descriptor Abstraction
Descriptors are opaque objects that abstract implementation details:
- Random-access containers: descriptors are integral indices
- Bidirectional containers: descriptors are iterators
- Provides uniform interface regardless of storage strategy

### 6. Zero-Cost Abstraction
Descriptors compile down to simple index or iterator operations with no runtime overhead.

### 7. Type Safety
Different descriptor types cannot be accidentally mixed:
```cpp
using VectorDesc = graph::vertex_descriptor<std::vector<int>::iterator>;
using MapDesc = graph::vertex_descriptor<std::map<int,int>::iterator>;
// These are distinct types - cannot be assigned to each other
```

### 8. Automatic Pattern Recognition
The library automatically recognizes common container patterns:

**Inner Value Patterns** (for `vertices(g)`):
- Random-access containers (vector, deque) - index-based vertex IDs
- Associative containers (map, unordered_map) - key-based vertex IDs
- Bidirectional containers - iterator-based vertex IDs

**Edge Value Patterns** (for `edges(g, u)`):
- Simple edges: `std::vector<int>` - integers as target IDs
- Pair edges: `std::vector<std::pair<int, Weight>>` - .first as target ID
- Tuple edges: `std::vector<std::tuple<int, ...>>` - first element as target ID
- Custom edges: structs/classes with custom target ID extraction

This means simple graph containers like `std::vector<std::vector<int>>` or `std::map<int, std::vector<int>>` work automatically without any customization!

## Documentation

### Main Documentation Files
- **[docs/common_graph_guidelines.md](docs/common_graph_guidelines.md)**: Complete naming conventions, architectural commitments, and project structure requirements
- **[docs/container_interface.md](docs/container_interface.md)**: Comprehensive summary of the Graph Container Interface (adjacency lists and edgelists)
- **[docs/graph_cpo_order.md](docs/graph_cpo_order.md)**: Canonical CPO implementation order with 19 CPOs organized by phases
- **[docs/graph_cpo_implementation.md](docs/graph_cpo_implementation.md)**: Complete CPO implementation guide with full code examples (11 CPOs with MSVC-style _Choice_t pattern)
- **[docs/vertex_inner_value_patterns.md](docs/vertex_inner_value_patterns.md)**: Inner value pattern concepts for automatic `vertices(g)` support
- **[docs/edge_value_concepts.md](docs/edge_value_concepts.md)**: Edge value pattern concepts for automatic `edges(g, u)` support
- **[docs/cpo.md](docs/cpo.md)**: Detailed guide for implementing customization point objects
- **[PHASE1_COMPLETE.md](PHASE1_COMPLETE.md)**: Phase 1 completion report with verification results
- **[MIGRATION_PHASE1.md](MIGRATION_PHASE1.md)**: Detailed record of Phase 1 migration

### Reference
This library follows the P1709 Graph Library proposal specifications:
- P1709/D3130 - Container Interface specification
- P1709 conventions.tex - Naming conventions

## Getting Started

### 1. Clone and Build

```bash
git clone https://github.com/pratzl/desc.git
cd desc

# Configure with preset
cmake --preset linux-gcc-debug

# Build
cmake --build build/linux-gcc-debug

# Run tests (75 tests)
ctest --test-dir build/linux-gcc-debug --output-on-failure
```

### 2. Include in Your Project

```cmake
# In your CMakeLists.txt
add_subdirectory(path/to/desc)
target_link_libraries(your_target PRIVATE graph::graph)
```

```cpp
// In your source files
#include <graph/graph.hpp>  // Main header includes all components
```

## Next Steps

### For Contributors
1. **Review Phase 1 completion**: See [PHASE1_COMPLETE.md](PHASE1_COMPLETE.md) for current status
2. **Implement Phase 2**: Graph utility CPOs following patterns in [docs/cpo.md](docs/cpo.md)
3. **Add containers**: Implement `adjacency_list` in `include/graph/container/`
4. **Follow conventions**: Use naming conventions from [docs/common_graph_guidelines.md](docs/common_graph_guidelines.md)

### For Users
Currently, the library provides:
- ✅ Descriptor abstractions for building custom graph types
- ✅ Complete documentation for the Graph Container Interface
- ✅ CPO framework for customization

Future phases will add:
- 🔜 Ready-to-use graph containers (adjacency_list, compressed_graph)
- 🔜 Graph algorithms (BFS, DFS, shortest paths, etc.)
- 🔜 Graph views (filtered, transformed, etc.)

## Build Configurations

The project includes CMake presets for common configurations:

```bash
# Linux GCC Debug
cmake --preset linux-gcc-debug
cmake --build build/linux-gcc-debug

# Linux GCC Release
cmake --preset linux-gcc-release
cmake --build build/linux-gcc-release

# Custom configuration
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++
cmake --build build
```

## Testing

The project includes 490 unit tests covering descriptor functionality, CPO implementations, partitioning, and type aliases:

```bash
# Run all tests
ctest --test-dir build/linux-gcc-debug --output-on-failure

# Run specific test
ctest --test-dir build/linux-gcc-debug -R vertex_descriptor

# Run vertices(g) CPO tests
ctest --test-dir build/linux-gcc-debug -R vertices

# Run vertex_id(g,u) CPO tests
ctest --test-dir build/linux-gcc-debug -R vertex_id

# Run find_vertex(g,uid) CPO tests
ctest --test-dir build/linux-gcc-debug -R find_vertex

# Run edges(g,u) CPO tests
ctest --test-dir build/linux-gcc-debug -R "edges.*cpo"

# Run edges(g,uid) CPO tests
ctest --test-dir build/linux-gcc-debug -R "edges.*uid"

# Run target_id(g,uv) CPO tests
ctest --test-dir build/linux-gcc-debug -R target_id

# Run target(g,uv) CPO tests
ctest --test-dir build/linux-gcc-debug -R "target.*cpo"

# Run num_vertices(g) CPO tests
ctest --test-dir build/linux-gcc-debug -R num_vertices

# Run num_edges(g) CPO tests
ctest --test-dir build/linux-gcc-debug -R num_edges

# Run degree(g,u) and degree(g,uid) CPO tests
ctest --test-dir build/linux-gcc-debug -R degree

# Run partition_id(g,u) CPO tests
ctest --test-dir build/linux-gcc-debug -R partition_id

# Run num_partitions(g) CPO tests
ctest --test-dir build/linux-gcc-debug -R num_partitions

# Run type alias tests
ctest --test-dir build/linux-gcc-debug -R "Type aliases"

# Verbose output
ctest --test-dir build/linux-gcc-debug -V
```

## Contributing

Contributions are welcome! Please:

1. Follow the naming conventions in [docs/common_graph_guidelines.md](docs/common_graph_guidelines.md)
2. Use CPO patterns from [docs/cpo.md](docs/cpo.md)
3. Add unit tests for new functionality
4. Update documentation as needed
5. Ensure all tests pass before submitting

## License

[License information to be determined]

## Acknowledgments

This library follows the design principles and specifications from:
- P1709 Graph Library Proposal
- C++ Standards Committee Graph Library discussions
- MSVC Standard Library CPO patterns

---

**Status**: Phase 1-5 Complete ✅ | 490/490 Tests Passing ✅ | All Core CPOs + Value Access + Sourced Edges + Partitioning Complete ✅

**Implemented CPOs**: vertices(g) • vertex_id(g,u) • find_vertex(g,uid) • edges(g,u) • edges(g,uid) • target_id(g,uv) • target(g,uv) • num_vertices(g) • num_edges(g) • degree(g,u) • degree(g,uid) • find_vertex_edge • contains_edge • has_edge(g) • vertex_value(g,u) • edge_value(g,uv) • graph_value(g) • source_id(g,uv) • source(g,uv) • partition_id(g,u) • num_partitions(g) • Type Aliases

````
