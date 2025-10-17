# Migration Plan: Descriptor Library → Graph Library

## Executive Summary

This document outlines the transformation of the current descriptor-focused library into a comprehensive, high-performance generic graph library while maintaining alignment with `common_graph_guidelines.md`.

## Current State Analysis

### Current Project Structure
```
desc/
├── include/desc/          # Descriptor headers in 'desc' namespace
│   ├── descriptor.hpp
│   ├── descriptor_traits.hpp
│   ├── edge_descriptor.hpp
│   ├── edge_descriptor_view.hpp
│   ├── vertex_descriptor.hpp
│   └── vertex_descriptor_view.hpp
├── tests/                 # 75 passing unit tests
├── examples/              # Basic usage examples
├── docs/                  # CPO guide, design docs
└── CMakeLists.txt         # Basic build configuration
```

### What We Have
✅ **Core descriptors**: Vertex and edge descriptor abstractions  
✅ **Storage flexibility**: Supports vector (direct) and map (keyed) patterns  
✅ **C++20 concepts**: Strong type constraints  
✅ **Descriptor views**: Forward iteration over descriptors  
✅ **Hash support**: std::hash specializations  
✅ **Comprehensive CPO guide**: MSVC-style CPO patterns documented  
✅ **Working tests**: 75 tests covering descriptor functionality  
✅ **Namespace**: Uses `graph::` namespace (changed from `desc`)  

### What We Don't Have
❌ Graph containers (adjacency list, adjacency matrix, etc.)  
❌ Graph algorithms (BFS, DFS, shortest path, etc.)  
❌ Graph views (filtered, transformed, etc.)  
❌ CPO implementations for graph operations  
❌ Comprehensive type aliases (`vertex_t<G>`, `edge_t<G>`, etc.)  
❌ Property map support  
❌ Edge list utilities  
❌ Benchmark infrastructure  
❌ Full project structure per guidelines  

## Conflicts & Alignment Issues

### 1. ✅ **Namespace** - ALIGNED
- **Current**: Uses `graph::` namespace
- **Required**: Uses `graph::` namespace
- **Action**: ✅ None needed

### 2. ⚠️ **Directory Structure** - PARTIAL ALIGNMENT
- **Current**: `include/desc/` with descriptor headers
- **Required**: `include/graph/` with algorithm/, container/, views/ subdirectories
- **Conflict**: Headers in `desc/` subdirectory instead of `graph/`
- **Action**: 🔧 Rename `include/desc/` → `include/graph/`

### 3. ⚠️ **Missing Directories** - GAP
- **Current**: No algorithm/, container/, views/, benchmark/ directories
- **Required**: Full structure per guidelines
- **Action**: 🔧 Create missing directories

### 4. ✅ **CPO Style** - ALIGNED
- **Current**: Comprehensive CPO guide with MSVC-style patterns
- **Required**: "MUST follow the style used by the MSVC standard library implementation"
- **Action**: ✅ Already documented and ready to use

### 5. ⚠️ **Type Aliases** - PARTIAL
- **Current**: No parameterized type aliases (`vertex_t<G>`, `edge_t<G>`)
- **Required**: `vertex_t<G>`, `edge_t<G>`, `vertex_iterator_t<G>`, etc.
- **Action**: 🔧 Implement type alias system

### 6. ⚠️ **Naming Conventions** - NEEDS REVIEW
- **Current**: Descriptors use internal naming
- **Required**: Specific variable names (G/g, V/u/v/w, E/uv, etc.)
- **Action**: 🔧 Review and align descriptor interfaces

### 7. ❌ **Graph Containers** - MISSING
- **Current**: Only descriptors, no actual graph implementations
- **Required**: Graph containers in `include/graph/container/`
- **Action**: 🔧 Implement core graph containers

### 8. ❌ **Algorithms** - MISSING
- **Current**: No algorithms
- **Required**: Algorithms in `include/graph/algorithm/`
- **Action**: 🔧 Implement graph algorithms

### 9. ❌ **Views** - MISSING
- **Current**: Only descriptor views
- **Required**: Graph views in `include/graph/views/`
- **Action**: 🔧 Implement graph views

### 10. ⚠️ **Documentation** - PARTIAL
- **Current**: Excellent CPO guide, basic README
- **Required**: Comprehensive documentation with examples
- **Action**: 🔧 Expand documentation

## Migration Strategy

### Phase 1: Foundation Restructuring (Week 1)

#### 1.1 Directory Restructure
```bash
# Rename header directory
mv include/desc include/graph

# Create new subdirectories
mkdir -p include/graph/algorithm
mkdir -p include/graph/container
mkdir -p include/graph/views
mkdir -p include/graph/detail
mkdir -p benchmark
mkdir -p data
mkdir -p docs/sphinx
mkdir -p example/AdaptingThirdPartyGraph
```

#### 1.2 Update Include Paths
- Change all `#include <desc/...>` → `#include <graph/...>`
- Update CMakeLists.txt to reference `include/graph/`
- Update test includes
- Update example includes

#### 1.3 Create Core Type Aliases Header
Create `include/graph/graph_types.hpp`:
```cpp
namespace graph {

// Parameterized type aliases
template<typename G>
using vertex_t = typename G::vertex_type;

template<typename G>
using edge_t = typename G::edge_type;

template<typename G>
using vertex_id_t = typename G::vertex_id_type;

template<typename G>
using vertex_iterator_t = typename G::vertex_iterator;

template<typename G>
using edge_iterator_t = typename G::edge_iterator;

template<typename G>
using vertex_range_t = typename G::vertex_range;

template<typename G>
using vertex_edge_range_t = typename G::vertex_edge_range;

// Optional value types
template<typename G>
using vertex_value_t = typename G::vertex_value_type;

template<typename G>
using edge_value_t = typename G::edge_value_type;

} // namespace graph
```

### Phase 2: Graph Container Foundations (Weeks 2-3)

#### 2.1 Define Graph Concepts
Create `include/graph/graph_concepts.hpp`:
```cpp
namespace graph {

// Core graph concept
template<typename G>
concept graph = requires(G g) {
    typename vertex_t<G>;
    typename edge_t<G>;
    typename vertex_id_t<G>;
    typename vertex_iterator_t<G>;
    typename edge_iterator_t<G>;
    { vertices(g) } -> std::ranges::range;
};

// Adjacency graph concept
template<typename G>
concept adjacency_graph = graph<G> && requires(G g, vertex_t<G> u) {
    { edges(g, u) } -> std::ranges::range;
};

// Other concepts...
} // namespace graph
```

#### 2.2 Implement CPO Infrastructure
Create `include/graph/detail/cpo_base.hpp`:
```cpp
namespace graph::_cpo {

// Shared _Choice_t struct (already documented in guide)
template<typename _Ty>
struct _Choice_t {
    _Ty _Strategy = _Ty{};
    bool _No_throw = false;
};

} // namespace graph::_cpo
```

#### 2.3 Implement Core CPOs
Following the MSVC-style pattern from `docs/cpo.md`:

**`include/graph/vertices.hpp`**: CPO for getting vertex range
**`include/graph/edges.hpp`**: CPO for getting edge range
**`include/graph/num_vertices.hpp`**: CPO for vertex count
**`include/graph/num_edges.hpp`**: CPO for edge count
**`include/graph/vertex_id.hpp`**: CPO for vertex ID (already documented)
**`include/graph/source.hpp`**: CPO for edge source vertex
**`include/graph/target.hpp`**: CPO for edge target vertex

#### 2.4 First Graph Container
Create `include/graph/container/adjacency_list.hpp`:
```cpp
namespace graph {

template<typename VV = void, typename EV = void, 
         typename Container = std::vector<...>>
class adjacency_list {
public:
    using vertex_type = vertex_descriptor<...>;
    using edge_type = edge_descriptor<...>;
    using vertex_id_type = std::size_t;
    using vertex_iterator = ...;
    using edge_iterator = ...;
    
    // Interface following CPO requirements
    auto vertices() const { return vertex_range{...}; }
    auto edges(vertex_type u) const { return edge_range{...}; }
    // ...
};

} // namespace graph
```

### Phase 3: Algorithm Infrastructure (Weeks 4-5)

#### 3.1 Algorithm Utilities
Create `include/graph/algorithm/` with:
- `visitor.hpp` - Visitor concepts and base classes
- `colors.hpp` - Vertex coloring for algorithms
- `queue_helpers.hpp` - Queue utilities for traversal

#### 3.2 First Algorithms
- `bfs.hpp` - Breadth-first search
- `dfs.hpp` - Depth-first search
- `dijkstra.hpp` - Shortest path

#### 3.3 Algorithm CPOs
Each algorithm as a CPO following MSVC style

### Phase 4: Views & Utilities (Week 6)

#### 4.1 Graph Views
Create `include/graph/views/`:
- `filtered_graph.hpp` - Filter vertices/edges
- `reverse_graph.hpp` - Reverse edge directions
- `subgraph.hpp` - Subgraph view

#### 4.2 Utilities
- `include/graph/edgelist.hpp` - Edge list functionality
- `include/graph/graph_utility.hpp` - Helper functions
- `include/graph/graph_info.hpp` - Graph information structures

### Phase 5: Testing & Examples (Week 7)

#### 5.1 Expand Test Suite
- Container tests
- CPO tests
- Algorithm tests
- View tests
- Integration tests

#### 5.2 Create Examples
- `example/AdaptingThirdPartyGraph/` - Show CPO customization
- `example/BasicUsage/` - Simple graph usage
- `example/Algorithms/` - Algorithm demonstrations
- `example/PageRank/` - Real-world algorithm

### Phase 6: Benchmarks & Documentation (Week 8)

#### 6.1 Benchmarks
Create `benchmark/`:
- Container performance comparisons
- Algorithm performance tests
- Comparison with Boost.Graph, other libraries

#### 6.2 Documentation
- Update README.md with full library scope
- Create getting started guide
- Document all CPOs
- API reference (Doxygen)
- Tutorial examples

## Detailed File Changes

### Rename Operations
```bash
# Headers
include/desc/descriptor.hpp         → include/graph/descriptor.hpp
include/desc/descriptor_traits.hpp  → include/graph/descriptor_traits.hpp
include/desc/vertex_descriptor.hpp  → include/graph/vertex_descriptor.hpp
include/desc/edge_descriptor.hpp    → include/graph/edge_descriptor.hpp
include/desc/vertex_descriptor_view.hpp → include/graph/vertex_descriptor_view.hpp
include/desc/edge_descriptor_view.hpp   → include/graph/edge_descriptor_view.hpp

# Tests
tests/test_descriptor_traits.cpp
tests/test_edge_concepts.cpp
tests/test_edge_descriptor.cpp
tests/test_vertex_concepts.cpp
tests/test_vertex_descriptor.cpp
# All need: #include <desc/...> → #include <graph/...>
```

### New Files to Create

#### Core Infrastructure
```
include/graph/graph.hpp              # Main include header
include/graph/graph_types.hpp        # Type aliases
include/graph/graph_concepts.hpp     # Graph concepts
include/graph/detail/cpo_base.hpp    # CPO infrastructure
```

#### CPOs
```
include/graph/vertices.hpp           # vertices(g) CPO
include/graph/edges.hpp              # edges(g, u) CPO
include/graph/num_vertices.hpp       # num_vertices(g) CPO
include/graph/num_edges.hpp          # num_edges(g, u) CPO
include/graph/vertex_id.hpp          # vertex_id(v) CPO
include/graph/source.hpp             # source(e) CPO
include/graph/target.hpp             # target(e) CPO
include/graph/edge_value.hpp         # edge_value(e) CPO (if supported)
include/graph/vertex_value.hpp       # vertex_value(v) CPO (if supported)
```

#### Containers
```
include/graph/container/adjacency_list.hpp
include/graph/container/adjacency_matrix.hpp
include/graph/container/compressed_sparse_row.hpp
```

#### Algorithms
```
include/graph/algorithm/breadth_first_search.hpp
include/graph/algorithm/depth_first_search.hpp
include/graph/algorithm/dijkstra_shortest_path.hpp
include/graph/algorithm/topological_sort.hpp
```

#### Views
```
include/graph/views/filtered_graph.hpp
include/graph/views/reverse_graph.hpp
include/graph/views/subgraph.hpp
```

#### Utilities
```
include/graph/edgelist.hpp
include/graph/graph_utility.hpp
include/graph/graph_info.hpp
```

## CMake Updates

### Updated Root CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.20)
project(graph VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_BENCHMARKS "Build benchmarks" OFF)

# Library target (header-only)
add_library(graph INTERFACE)
target_include_directories(graph INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(graph INTERFACE cxx_std_20)

# Subdirectories
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif()
```

## Documentation Updates

### Update README.md
Transform from "Graph Descriptors" to comprehensive graph library:
- Update title and description
- Expand feature list to include containers, algorithms, views
- Add architecture overview
- Update examples to show full graph usage
- Add performance characteristics
- Explain CPO-based design

### Create New Documentation
- `docs/getting_started.md` - Quick start guide
- `docs/architecture.md` - Library architecture
- `docs/customization.md` - How to adapt existing graph structures
- `docs/algorithm_guide.md` - Algorithm usage patterns
- Update `docs/cpo.md` with graph-specific CPO examples

## Testing Strategy

### Maintain Existing Tests
✅ Keep all 75 descriptor tests passing

### Add New Test Categories
- **Container tests**: Test each graph container implementation
- **CPO tests**: Test each CPO with multiple customization paths
- **Algorithm tests**: Correctness tests for all algorithms
- **View tests**: Test graph views and compositions
- **Integration tests**: End-to-end scenarios
- **Concept tests**: Verify concept requirements

## Risk Mitigation

### Backward Compatibility
- Keep descriptor headers functional during transition
- Use deprecated headers that forward to new locations
- Provide migration guide for existing users

### Incremental Approach
- Each phase delivers working, testable functionality
- Can release after each phase
- Maintain test coverage throughout

### Quality Assurance
- All tests must pass before merging changes
- Add new tests for new functionality
- Maintain or improve code coverage
- Cross-compiler testing (MSVC, GCC, Clang)

## Success Criteria

### Phase 1 Complete
✅ Directory structure matches guidelines  
✅ All existing tests still pass  
✅ Type alias system in place  
✅ CPO infrastructure ready  

### Phase 2 Complete
✅ At least one graph container working  
✅ Core CPOs implemented (vertices, edges, etc.)  
✅ Container tests passing  

### Phase 3 Complete
✅ BFS and DFS algorithms working  
✅ Algorithm tests passing  
✅ Example programs demonstrate algorithms  

### Phase 4 Complete
✅ Basic graph views implemented  
✅ View tests passing  
✅ Utilities in place  

### Phase 5 Complete
✅ Comprehensive test coverage  
✅ Multiple working examples  
✅ Example programs in required directories  

### Phase 6 Complete
✅ Benchmark suite in place  
✅ Documentation complete  
✅ Ready for external review  

## Timeline Summary

- **Week 1**: Foundation restructuring
- **Weeks 2-3**: Graph containers and CPOs
- **Weeks 4-5**: Algorithms
- **Week 6**: Views and utilities
- **Week 7**: Testing and examples
- **Week 8**: Benchmarks and documentation

**Total Estimated Effort**: 8 weeks for full transformation

## Next Steps

### Immediate Actions
1. ✅ Review this migration plan
2. 🔧 Create feature branch for restructuring
3. 🔧 Execute Phase 1 (directory restructure)
4. 🔧 Update and verify all tests pass
5. 🔧 Commit Phase 1 changes
6. 🔧 Begin Phase 2 (graph containers)

### Decision Points
- [ ] Approve directory structure changes
- [ ] Confirm type alias naming conventions
- [ ] Select first graph container to implement
- [ ] Choose first algorithms to implement
- [ ] Determine property map integration strategy

## Conclusion

The current descriptor library provides an excellent foundation for a comprehensive graph library. The main work ahead is:

1. **Structural**: Reorganize to match guidelines (low risk)
2. **Additive**: Build containers, algorithms, views on top of descriptors (incremental)
3. **Integration**: Tie everything together with CPOs (clear pattern established)

The descriptors, concepts, and CPO patterns are already well-designed and tested. The transformation is primarily about expanding scope while maintaining the quality and design principles already in place.
