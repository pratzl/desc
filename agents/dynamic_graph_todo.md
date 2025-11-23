# Dynamic Graph Testing and Enhancement Plan

## Overview
This plan outlines a phased approach to comprehensively test and enhance the `dynamic_graph` container, starting with existing functionality and progressively adding new container types and features. The goal is to achieve 95% test coverage while maintaining code stability.

## Conventions
Variable names must follow the following rules
- Variable names must follow the naming conventions in `common_graph_guidelines.md`
- Variable names must never be the same as a CPO function.
- The default variable name for a range/container of edges must be `edge_rng`
- The default variable name for a range/container of vertices must be `vertex_rng`


## Phase 1: Test Existing Functionality (Vector Vertices + List-based Edges)

### Phase 1.1: Core Container Combinations
Test all combinations of existing container types with various value types.

**Vertex Containers:** `vector`, `deque`  
**Edge Containers:** `vector`, `deque`, `list`, `forward_list`  
**Vertex ID Types:** `uint64_t`, `int`, `int8_t`  
**Edge Values:** `void`, `int`, `string`  
**Vertex Values:** `void`, `int`, `string`  
**Graph Values:** `void`, `int`, `string`  
**Sourced:** `true`, `false`

**Test Strategy:**
- Select representative combinations to achieve 95% coverage without exhaustive testing
- Priority combinations:
  1. `vector` vertices + `forward_list` edges (vofl_graph_traits - most lightweight)
  2. `vector` vertices + `list` edges (vol_graph_traits - bidirectional edges)
  3. `vector` vertices + `vector` edges (vov_graph_traits - best cache locality)
  4. `deque` vertices + `list` edges (future extension)

**Test Files Organization:**
```
tests/
  test_dynamic_graph_vofl.cpp      # vector + forward_list tests
  test_dynamic_graph_vol.cpp       # vector + list tests  
  test_dynamic_graph_vov.cpp       # vector + vector tests
  test_dynamic_graph_deque.cpp     # deque-based tests
```

**Test Coverage per File (~200-300 tests each):**

1. **Construction Tests (40 tests)** ✅ (17 foundation tests completed in Phase 1.1)
   - [x] Default constructor
   - [x] Constructor with allocator
   - [ ] Constructor with edge range only (with/without vertex count)
   - [ ] Constructor with edge + vertex ranges
   - [ ] Constructor with initializer_list
   - [x] Constructor with graph value (copy/move)
   - [ ] Constructor with partitions
   - [x] Test all combinations of value types (void/int/string)
   - [x] Test sourced vs unsourced edges

2. **Load Operations Tests (30 tests)** (Not yet started)
   - [ ] `load_vertices()` with various projections
   - [ ] `load_vertices()` with move semantics
   - [ ] `load_edges()` with various projections
   - [ ] `load_edges()` with move semantics
   - [ ] `load_edges()` with vertex inference
   - [ ] `load_edges()` with explicit vertex count
   - [ ] Edge count hints
   - [ ] Out-of-range vertex ID error handling

3. **Vertex Access Tests (25 tests)** ✅ (Partially complete - basic tests done)
   - [x] `operator[]` access
   - [x] `begin()/end()` iteration
   - [x] `size()` queries
   - [x] Const correctness
   - [x] Empty graph behavior
   - [ ] Single vertex graphs
   - [ ] Large graphs (1000+ vertices)

4. **Edge Access Tests (30 tests)** (Not yet started)
   - [ ] Access via vertex iterator
   - [ ] Access via vertex ID
   - [ ] Edge iteration for each vertex
   - [ ] Empty edge lists
   - [ ] Self-loops
   - [ ] Multiple edges to same target
   - [ ] Const correctness

5. **Value Access Tests (25 tests)** ✅ (Foundation tests completed)
   - [x] `vertex.value()` for non-void VV
   - [x] `edge.value()` for non-void EV
   - [x] Graph value access for non-void GV
   - [x] Compile-time errors for void types (static_assert checks)
   - [x] Value modification
   - [x] Move semantics

6. **Partition Tests (20 tests)** (Not yet started)
   - [ ] Single partition (default)
   - [ ] Multiple partitions
   - [ ] Partition ID lookup
   - [ ] Partition vertex ranges
   - [ ] Invalid partition access
   - [ ] Empty partitions
   - [ ] Partition boundary vertices

7. **Sourced Edge Tests (15 tests)** ✅ (Foundation tests completed)
   - [x] Source ID access when Sourced=true
   - [x] Compile-time error when Sourced=false
   - [ ] Source vertex lookup
   - [ ] Consistency checks (source matches vertex)

8. **Property Tests (20 tests)** ✅ (Basic tests completed)
   - [x] Vertex count queries
   - [ ] Edge count queries
   - [ ] Degree queries (per vertex)
   - [ ] `has_edge()` queries
   - [x] Empty graph properties
   - [ ] Graph with only vertices (no edges)

9. **Memory & Performance Tests (15 tests)** ✅ (Foundation tests completed)
   - [ ] Reserve operations
   - [ ] Resize operations
   - [x] Memory layout for void types (verify no storage)
   - [ ] Large graph construction (10k vertices)
   - [x] Iterator stability after modifications

10. **Edge Cases & Error Handling (25 tests)** ✅ (Foundation tests completed)
    - [x] Null/empty ranges
    - [ ] Invalid vertex IDs
    - [ ] Overflow detection
    - [ ] Exception safety
    - [x] Duplicate edges behavior
    - [ ] Malformed partition data
    - [ ] max_vertex_id boundary conditions

**Implementation Approach:**
- [x] Create one test file at a time
- [x] Start with `test_dynamic_graph_vofl.cpp` (most common use case) - **79 foundation tests completed**
- [x] Use Catch2 test framework with SECTION organization
- [x] Employ test generators for value type combinations where appropriate
- [x] Include both positive tests (correct usage) and negative tests (error detection)

**Status:** Phase 1.1 foundation tests completed (79/200+ tests)
- ✅ test_dynamic_graph_vofl.cpp created with 79 tests
- ⏳ Remaining: Load operations, vertex/edge access with data, partitions

**Expected Line Count:** ~5000-7000 lines total for Phase 1

---

## Phase 2: Add Minimal CPO Support

After Phase 1 tests pass, add CPO function support using default implementations.

**CPOs to Implement (in priority order):**

1. **vertices(g)** - Return vertex range
   - Default: return g (graph is already a range)
   - Test: iteration, range algorithms

2. **edges(g, u)** - Return edge range for vertex u
   - Default: return u.edges()
   - Test: per-vertex iteration, const correctness

3. **vertex_id(g, u)** - Get vertex ID from vertex iterator/descriptor
   - Implementation: iterator subtraction from begin()
   - Test: all vertices, random access

4. **target_id(g, uv)** - Get target vertex ID from edge
   - Member function on edge (already exists in design)
   - Test: all edges, consistency

5. **find_vertex(g, uid)** - Find vertex by ID
   - Default: vertices(g).begin() + uid (for random access)
   - Test: all vertex IDs, out of range

6. **num_vertices(g)** - Count vertices
   - Default: g.size()
   - Test: empty, single, many

7. **num_edges(g)** - Count total edges
   - Implementation: track in member variable (already exists)
   - Test: empty, various counts

8. **degree(g, u)** - Get out-degree of vertex
   - Default: ranges::distance(edges(g, u))
   - Test: isolated vertices, various degrees

9. **source_id(g, uv)** - Get source ID (when Sourced=true)
   - Member function on edge
   - Test: sourced graphs only

10. **source(g, uv)** - Get source vertex (when Sourced=true)
    - Default: find_vertex(g, source_id(g, uv))
    - Test: sourced graphs only

11. **target(g, uv)** - Get target vertex
    - Default: find_vertex(g, target_id(g, uv))
    - Test: all edges

12. **vertex_value(g, u)** - Access vertex value
    - Default: u.value()
    - Test: non-void VV only

13. **edge_value(g, uv)** - Access edge value
    - Default: uv.value()
    - Test: non-void EV only

14. **graph_value(g)** - Access graph value
    - Member function
    - Test: non-void GV only

15. **partition_id(g, uid)** - Get partition for vertex
    - Implementation: binary search (already outlined)
    - Test: all partitions

16. **num_partitions(g)** - Count partitions
    - Implementation: partition_.size() - 1
    - Test: single and multiple

**Test File:**
```
tests/test_dynamic_graph_cpo.cpp  (~1000-1500 lines)
```

**CPO Implementation Strategy:**
- Uncomment and complete existing friend functions in dynamic_graph.hpp
- Use default implementations via CPO machinery from graph_cpo.hpp
- Leverage existing descriptor infrastructure where applicable
- Test each CPO with all container combinations from Phase 1

---

## Phase 3: Extend Vertex Containers (Associative)

Add support for map-based vertex containers.

### Phase 3.1: Map Vertex Containers

**New Traits Structures:**
```cpp
template <class EV, class VV, class GV, class VId, bool Sourced>
struct mofl_graph_traits {  // map + forward_list
  using vertices_type = std::map<VId, vertex_type>;
  using edges_type = std::forward_list<edge_type>;
};

template <class EV, class VV, class GV, class VId, bool Sourced>
struct mol_graph_traits {  // map + list
  using vertices_type = std::map<VId, vertex_type>;
  using edges_type = std::list<edge_type>;
};

template <class EV, class VV, class GV, class VId, bool Sourced>
struct umofl_graph_traits {  // unordered_map + forward_list
  using vertices_type = std::unordered_map<VId, vertex_type>;
  using edges_type = std::forward_list<edge_type>;
};
```

**Additional Vertex ID Types for Map:**
- `std::pair<int, int>` (coordinate pairs)
- `std::tuple<int, int, int>` (3D coordinates)
- `std::string` (named vertices)
- Custom struct `USAAddress` with operator<, hash support:
  ```cpp
  struct USAAddress {
    std::string street;
    std::string city;
    std::string state;
    std::string zip;
    auto operator<=>(const USAAddress&) const = default;
  };
  ```

**Implementation Changes Needed:**
1. Update `vertex_id()` CPO for bidirectional iterators (map)
2. Update `find_vertex()` to use map's find() member
3. Adjust construction logic for non-contiguous vertex IDs
4. Handle vertex insertion vs. assignment semantics

**Test Files:**
```
tests/test_dynamic_graph_map.cpp           (~1500 lines)
tests/test_dynamic_graph_unordered_map.cpp (~1500 lines)
```

**Test Coverage:**
- All tests from Phase 1, adapted for map semantics
- Non-contiguous vertex ID sequences
- Custom hash functions (for unordered_map)
- Key comparison semantics (for map)
- Iterator invalidation rules
- Compound key types

---

## Phase 4: Extend Edge Containers (Associative/Set)

Add support for set/map-based edge containers.

### Phase 4.1: Set-Based Edge Containers

**New Traits Structures:**
```cpp
template <class EV, class VV, class GV, class VId, bool Sourced>
struct vos_graph_traits {  // vector + set
  using vertices_type = std::vector<vertex_type>;
  using edges_type = std::set<edge_type>;  // requires edge operator<
};

template <class EV, class VV, class GV, class VId, bool Sourced>
struct vous_graph_traits {  // vector + unordered_set
  using vertices_type = std::vector<vertex_type>;
  using edges_type = std::unordered_set<edge_type>;  // requires edge hash
};
```

**Implementation Changes Needed:**
1. Add operator< to dynamic_edge for set support
2. Add hash specialization for dynamic_edge for unordered_set
3. Update edge insertion logic (insert vs push_back)
4. Handle duplicate edge detection in sets

**Test Files:**
```
tests/test_dynamic_graph_set.cpp           (~1000 lines)
tests/test_dynamic_graph_unordered_set.cpp (~1000 lines)
```

### Phase 4.2: Map-Based Edge Containers

**New Traits Structures:**
```cpp
template <class EV, class VV, class GV, class VId, bool Sourced>
struct voem_graph_traits {  // vector + edge map
  using vertices_type = std::vector<vertex_type>;
  using edges_type = std::map<VId, edge_type>;  // keyed by target_id
};
```

**Implementation Changes Needed:**
1. Edge access by target ID key
2. Update find_vertex_edge() CPO
3. Handle edge insertion semantics (replace vs. error on duplicate)

**Test File:**
```
tests/test_dynamic_graph_edge_map.cpp  (~1000 lines)
```

---

## Phase 5: Non-Integral Vertex IDs

Extend support beyond integral types for vertex IDs.

**New VId Types:**
- `double` (floating-point IDs)
- `std::string` (string IDs)
- Compound types: `struct Name { std::string first, last; }`

**Requirements:**
- Must support map/unordered_map as vertex containers (Phase 3 dependency)
- Must provide operator<, hash, operator== as appropriate
- Update vertex_id() CPO for non-integral contexts

**Test File:**
```
tests/test_dynamic_graph_non_integral_id.cpp  (~800 lines)
```

**Test Coverage:**
- Construction with non-integral IDs
- Lookup and iteration
- Partition support (may need custom partition function)
- CPO compatibility

---

## Phase 6: Comprehensive Integration Tests

After all phases, create integration tests for complex scenarios.

**Test File:**
```
tests/test_dynamic_graph_integration.cpp  (~1200 lines)
```

### Phase 6: Integration Test Scenarios

**Test Coverage:**
- Mixed container types in same codebase
- Large-scale graphs (100k+ vertices, 1M+ edges)
- Interoperability with compressed_graph
- Conversion between graph types
- STL algorithm compatibility
- Range adaptor compatibility

**Specific Tests:**
1. **Graph Conversion** (50 tests)
   - dynamic_graph → compressed_graph
   - compressed_graph → dynamic_graph
   - Preserve vertex/edge values
   - Preserve partition information
   - Handle large graphs efficiently

2. **Cross-Container Operations** (40 tests)
   - Copy between different traits types
   - Transform graph structure
   - Merge multiple graphs
   - Extract subgraphs

3. **STL Algorithm Integration** (50 tests)
   - `std::ranges::for_each` on vertices/edges
   - `std::ranges::find_if` for search
   - `std::ranges::sort` on edge containers
   - `std::ranges::transform` on values
   - Pipeline operations with views
   - `std::ranges::count_if` for filtering
   - `std::ranges::accumulate` for aggregation

4. **Performance Validation** (40 tests)
   - Benchmark construction time
   - Benchmark iteration time
   - Memory usage measurement
   - Compare container choices
   - Identify performance regressions
   - Cache locality impact

5. **CPO Interoperability** (60 tests)
   - Use CPOs across different graph types
   - Generic functions using CPOs
   - Verify CPO consistency
   - Test CPO with all container combinations

**Note:** Graph algorithms (BFS, DFS, shortest path, etc.) will be implemented and tested separately using the CPO functions once the CPO layer is complete and stable. This ensures algorithms are generic and work across all graph types.

---

## Phase 7: Advanced Features and Edge Cases

Test advanced functionality and stress scenarios.

### Phase 7.1: Graph Mutation Operations

**Test File:** `test_dynamic_graph_mutation.cpp` (~1200 lines)

**Features to Test:**

1. **Edge Mutation** (100 tests)
   - `add_edge(g, uid, vid)` - Add single edge
   - `add_edge(g, uid, vid, value)` - Add edge with value
   - `remove_edge(g, uid, vid)` - Remove specific edge
   - `remove_first_edge(g, uid, vid)` - Remove first of parallel edges
   - `remove_all_edges(g, uid, vid)` - Remove all parallel edges
   - `clear_edges(g, uid)` - Remove all edges from vertex
   - `update_edge_value(g, uid, vid, value)` - Modify edge value

2. **Vertex Mutation** (80 tests)
   - `add_vertex(g)` - Add vertex with auto-ID
   - `add_vertex(g, uid)` - Add vertex with specific ID
   - `add_vertex(g, uid, value)` - Add vertex with value
   - `remove_vertex(g, uid)` - Remove vertex and incident edges
   - `update_vertex_value(g, uid, value)` - Modify vertex value

3. **Iterator Stability** (60 tests)
   - Vertex iterator invalidation rules
   - Edge iterator invalidation rules
   - Reference validity after mutations
   - Safe mutation during iteration

4. **Exception Safety** (40 tests)
   - Strong guarantee for add operations
   - Basic guarantee for bulk operations
   - Rollback on exception
   - Resource cleanup

5. **Concurrent Modification** (20 tests)
   - Multiple readers, single writer patterns
   - Document thread-safety guarantees
   - Race condition detection (if applicable)

### Phase 7.2: Stress and Performance Tests

**Test File:** `test_dynamic_graph_stress.cpp` (~800 lines)

**Stress Scenarios:**

1. **Scale Tests** (60 tests)
   - 1M vertices, sparse edges
   - 10k vertices, dense edges (near-complete)
   - 10M edges with 100k vertices
   - Memory limit testing
   - Construction time benchmarks

2. **Special Graph Structures** (50 tests)
   - Complete graphs K_n (all-to-all)
   - Star graphs (one hub, many spokes)
   - Chain graphs (linear path)
   - Binary trees (perfect balance)
   - Random graphs (Erdős–Rényi)
   - Scale-free graphs (power-law)
   - Small-world graphs (Watts-Strogatz)

3. **Extreme Degrees** (30 tests)
   - Vertex with 1M+ out-edges
   - Graph with all isolated vertices
   - Graph with maximum degree variance
   - Self-loop dominated graphs

4. **Parallel Edges** (25 tests)
   - 1000+ parallel edges between two vertices
   - Many vertex pairs with parallel edges
   - Edge count accuracy with duplicates
   - Performance with parallel edges

5. **Value Type Stress** (35 tests)
   - Large string values (1MB+ each)
   - Complex nested structures
   - Move-only types at scale
   - Custom allocators under load

### Phase 7.3: Edge Cases and Corner Cases

**Test File:** `test_dynamic_graph_edge_cases.cpp` (~1000 lines)

**Edge Case Categories:**

1. **Empty and Single-Element** (60 tests)
   - Empty graph (no vertices, no edges)
   - Single vertex, no edges (isolated)
   - Single vertex, self-loop only
   - Single edge graph
   - All possible void combinations

2. **Boundary Values** (70 tests)
   - VId = 0, VId = max
   - Negative VId (for signed types)
   - VId near overflow boundary
   - Size = 0, Size = max
   - Partition boundaries

3. **Self-Loops** (50 tests)
   - Single self-loop per vertex
   - Multiple self-loops per vertex
   - Graph with only self-loops
   - Degree calculation with self-loops
   - Iteration behavior with self-loops
   - Remove self-loops

4. **Duplicate Edges** (50 tests)
   - Duplicate detection in set containers
   - Duplicate preservation in list containers
   - Edge count with duplicates
   - Remove one vs. remove all duplicates
   - Find with multiple matches

5. **Bidirectional Simulation** (40 tests)
   - Add reverse edges manually
   - In-degree calculation (counting incoming)
   - Predecessor lookup
   - Bidirectional traversal
   - Symmetric edge verification

6. **Malformed Input** (60 tests)
   - Null ranges
   - Empty ranges
   - Invalid projection functions
   - Inconsistent vertex IDs
   - Target ID exceeds vertex count
   - Negative IDs (when invalid)

7. **Numeric Edge Cases** (50 tests)
   - Integer overflow detection
   - Wraparound behavior
   - Signed/unsigned mismatch
   - Size_t limits
   - Vertex count at capacity

8. **Value Semantics Edge Cases** (40 tests)
   - Non-copyable values (move-only)
   - Non-movable values (copy-only)
   - Throwing copy constructors
   - Throwing move constructors
   - Throwing destructors
   - Deleted assignment operators

9. **Container-Specific Edge Cases** (40 tests)
   - Map with non-default comparator
   - Unordered_map with poor hash function
   - Set ordering preservation
   - Forward_list limitations (no size, no backward)
   - Deque iterator invalidation

10. **Allocator Edge Cases** (40 tests)
    - Stateful allocators
    - Allocator propagation on copy/move
    - Allocator mismatch detection
    - Custom allocators with different allocation strategies
    - Memory pool exhaustion

---

## Additional Features for Future Consideration

### Feature Set 1: Bidirectional Graph Support

**Implementation:**
```cpp
template <class EV, class VV, class GV, class VId, class Traits>
struct bidirectional_graph_traits : public Traits {
  using in_edges_type = typename Traits::edges_type;
};

// Add to dynamic_vertex:
in_edges_type in_edges_;  // Store incoming edges
```

**New CPOs:**
- `in_edges(g, u)` - Get incoming edges
- `in_degree(g, u)` - Count incoming edges
- `predecessors(g, u)` - Get predecessor vertices

**Test File:** `test_dynamic_graph_bidirectional.cpp` (~800 lines)

### Feature Set 2: Graph Views and Adaptors

**View Types:**
- `vertex_filter_view` - Filter vertices by predicate
- `edge_filter_view` - Filter edges by predicate
- `reverse_graph_view` - Swap source/target semantics
- `subgraph_view` - View subset of vertices

**Test File:** `test_dynamic_graph_views.cpp` (~600 lines)

### Feature Set 3: Property Maps

**External Property Storage:**
```cpp
template <typename G, typename T>
class vertex_property_map {
  std::vector<T> properties_;
  // or std::map<vertex_id_type, T> for sparse
};
```

**Use Cases:**
- Vertex colors for graph coloring
- Distances for shortest path
- Discovery/finish times for DFS
- Avoid modifying graph structure for temporary data

**Test File:** `test_dynamic_graph_property_maps.cpp` (~500 lines)

### Feature Set 4: Serialization

**Formats to Support:**
- Edge list (CSV, TSV)
- Adjacency list
- GraphML (XML-based)
- JSON
- Binary (custom format)

**Operations:**
- `save_to_edge_list(g, ostream)`
- `load_from_edge_list(istream)`
- `save_to_json(g, ostream)`
- `load_from_json(istream)`

**Test File:** `test_dynamic_graph_serialization.cpp` (~700 lines)

### Feature Set 5: Graph Generators

**Generator Functions:**
- `make_complete_graph(n)` - Complete graph K_n
- `make_cycle_graph(n)` - Cycle graph C_n
- `make_path_graph(n)` - Path graph P_n
- `make_star_graph(n)` - Star graph S_n
- `make_grid_graph(rows, cols)` - 2D grid
- `make_random_graph(n, p)` - Erdős–Rényi G(n,p)
- `make_barabasi_albert_graph(n, m)` - Scale-free
- `make_watts_strogatz_graph(n, k, p)` - Small-world

**Test File:** `test_dynamic_graph_generators.cpp` (~600 lines)

### Feature Set 6: Graph Metrics

**Statistical Functions:**
- `density(g)` - Edge density (E / V*(V-1))
- `avg_degree(g)` - Mean degree
- `degree_distribution(g)` - Histogram
- `clustering_coefficient(g, u)` - Local clustering
- `diameter(g)` - Maximum shortest path
- `radius(g)` - Minimum eccentricity
- `center_vertices(g)` - Vertices with minimum eccentricity

**Test File:** `test_dynamic_graph_metrics.cpp` (~500 lines)

---

## Enhanced Test Scenarios by Phase

### Phase 1 Enhancements:

Add to each existing test file:

**11. Value Type Advanced Tests** (40 tests per file)
   - Move-only types (unique_ptr, movable_struct)
   - Non-movable types (copy-only)
   - Throwing constructors/destructors
   - Large value types (>1KB)
   - Polymorphic types with virtual functions
   - Aggregate types
   - Types with const members

**12. Numeric Limits Tests** (30 tests per file)
   - VId at type limits (min, max)
   - VId overflow prevention
   - Size calculations near limits
   - Signed vs unsigned VId behavior
   - Negative VId handling (if applicable)

**13. Memory and Resource Tests** (25 tests per file)
   - Custom allocators with tracking
   - Allocator propagation semantics
   - Memory leak detection
   - RAII verification
   - Exception-induced leaks

**14. Self-Loop Tests** (20 tests per file)
   - Add self-loops during construction
   - Iterate self-loops
   - Count self-loops in degree
   - Remove self-loops
   - Multiple self-loops per vertex

**15. Parallel Edge Tests** (20 tests per file)
   - Multiple edges same endpoints
   - Count parallel edges
   - Iterate all parallel edges
   - Remove specific parallel edge
   - Edge container behavior (set vs list)

### Phase 2 CPO Enhancements:

Add to `test_dynamic_graph_cpo.cpp`:

**Additional CPOs to Implement:**

17. `contains_vertex(g, uid)` - Check vertex existence (40 tests)
18. `contains_edge(g, uid, vid)` - Check edge existence (40 tests)
19. `find_vertex_edge(g, uid, vid)` - Find specific edge (40 tests)
20. `in_edges(g, u)` - Incoming edges (if bidirectional) (30 tests)
21. `in_degree(g, u)` - Incoming degree (if bidirectional) (30 tests)
22. `is_adjacent(g, u, v)` - Check adjacency (30 tests)
23. `is_isolated(g, u)` - Check if vertex has no edges (25 tests)
24. `isolated_vertices(g)` - Range of isolated vertices (25 tests)

**CPO Testing Enhancements:**
- Customization point testing (ADL, member, default)
- Noexcept specifications
- Forwarding references
- Perfect forwarding
- SFINAE behavior
- Concept constraints

### Phase 3-5 Enhancements:

**Complex Key Types to Add:**

For Phase 3 (map vertices):
- `std::variant<int, string>` - Variant keys
- `std::optional<int>` - Optional keys
- Pointer types (identity comparison)
- Custom types with complex comparison logic
- Types with expensive comparison operators

For Phase 5 (non-integral):
- `double` with epsilon comparison
- `std::complex<double>` - Complex numbers
- Multi-field compound types
- Hierarchical key types

**Hash Function Tests:**
- Custom hash with collisions
- Perfect hash functions
- Poor hash functions (performance)
- Hash consistency across copies

---

## Coverage Goals (Revised)

**Target Coverage by Phase:**
- Phase 1: 75% of dynamic_graph.hpp
- Phase 2: 88% of dynamic_graph.hpp + CPO functions  
- Phase 3: 92% overall
- Phase 4: 94% overall
- Phase 5: 96% overall
- Phase 6: 97% with algorithm validation
- Phase 7: 98%+ with stress and edge cases

**Coverage Measurement Tools:**
- Use `gcov` or `llvm-cov` for line coverage
- Track branch coverage for all conditionals
- Monitor template instantiation coverage
- Focus on exception handling paths
- Measure cyclomatic complexity coverage

---

## Implementation Timeline (Revised)

### Recommended Order:
1. **Phase 1.1** - Core vofl tests (2-3 days)
2. **Phase 1.2** - Core vol tests (2-3 days)
3. **Phase 1.3** - Core vov tests (2-3 days)
4. **Phase 2** - CPO implementation and tests (3-4 days)
5. **Phase 1.4** - Deque vertex tests (1-2 days)
6. **Phase 7.3** - Edge cases (ongoing, 2-3 days)
7. **Phase 3** - Map vertex containers (3-4 days)
8. **Phase 4** - Set/map edge containers (3-4 days)
9. **Phase 5** - Non-integral IDs (2-3 days)
10. **Phase 6** - Integration tests (2-3 days)
11. **Phase 7.1** - Mutation operations (2-3 days)
12. **Phase 7.2** - Stress tests (2-3 days)

**Total Estimated Time:** 5-7 weeks for comprehensive coverage

**Note:** Graph algorithms will be implemented in a future phase using CPO functions, ensuring generic implementations that work across all graph container types (dynamic_graph, compressed_graph, etc.).

### Safety Considerations (Enhanced):
- After each phase, run **all** existing tests to ensure no regressions
- Use **feature branches** for each phase with PR review
- Commit after each test file passes completely
- Maintain **backward compatibility** throughout all phases
- Document **any breaking changes** with migration guide
- Use **continuous integration** to catch issues early
- Perform **static analysis** (clang-tidy, cppcheck) regularly
- Run **sanitizers** (ASan, UBSan, TSan, MSan) on all tests
- Conduct **code reviews** focusing on edge cases
- Maintain **changelog** for each phase completion

---

## Summary of Test Organization

```
tests/
  # Phase 1: Existing functionality
  test_dynamic_graph_vofl.cpp      # ~1800 lines
  test_dynamic_graph_vol.cpp       # ~1800 lines
  test_dynamic_graph_vov.cpp       # ~1800 lines
  test_dynamic_graph_deque.cpp     # ~1200 lines
  
  # Phase 2: CPO support
  test_dynamic_graph_cpo.cpp       # ~2000 lines
  
  # Phase 3: Associative vertex containers
  test_dynamic_graph_map.cpp       # ~1800 lines
  test_dynamic_graph_unordered_map.cpp  # ~1800 lines
  
  # Phase 4: Set/map edge containers
  test_dynamic_graph_set.cpp       # ~1200 lines
  test_dynamic_graph_unordered_set.cpp  # ~1200 lines
  test_dynamic_graph_edge_map.cpp  # ~1200 lines
  
  # Phase 5: Non-integral IDs
  test_dynamic_graph_non_integral_id.cpp  # ~1000 lines
  
  # Phase 6: Integration
  test_dynamic_graph_integration.cpp  # ~1200 lines
  
  # Phase 7: Advanced features
  test_dynamic_graph_mutation.cpp      # ~1200 lines
  test_dynamic_graph_stress.cpp        # ~800 lines
  test_dynamic_graph_edge_cases.cpp    # ~1000 lines
```

**Total Estimated Lines:** ~18,800 lines of test code
**Total Estimated Tests:** ~2,800-3,500 test cases

**Note:** Graph algorithms (BFS, DFS, shortest path, MST, flow algorithms, etc.) will be implemented and tested in a separate phase after the CPO layer is complete and stable, ensuring they work generically across all graph types.

---

## Implementation Timeline

### Recommended Order:
1. **Phase 1.1** - Core vofl tests (most common use case)
2. **Phase 1.2** - Core vol tests (bidirectional support)
3. **Phase 1.3** - Core vov tests (performance-oriented)
4. **Phase 2** - CPO implementation (enables uniformity)
5. **Phase 1.4** - Deque vertex tests (completes Phase 1)
6. **Phase 3** - Map vertex containers (major feature)
7. **Phase 4** - Set/map edge containers (major feature)
8. **Phase 5** - Non-integral IDs (advanced feature)
9. **Phase 6** - Integration tests (validation)

### Safety Considerations:
- After each phase, run all existing tests to ensure no regressions
- Use feature branches for each phase
- Commit after each test file passes
- Maintain backward compatibility throughout
- Document any breaking changes

---

## Coverage Goals

**Target Coverage by Phase:**
- Phase 1: 70% of dynamic_graph.hpp
- Phase 2: 85% of dynamic_graph.hpp + CPO functions
- Phase 3: 90% overall
- Phase 4: 93% overall
- Phase 5: 95% overall
- Phase 6: 95%+ with edge case validation

**Coverage Measurement:**
- Use `gcov` or `llvm-cov` for line coverage
- Track branch coverage for conditionals
- Monitor template instantiation coverage
- Focus on untested error paths in later phases

---

## Notes

1. **Container Selection Strategy:**
   - Tests prioritize most common combinations first
   - Coverage matrix targets 95%+ with ~30% of total combinations
   - Combinations chosen based on: common usage, unique edge cases, performance characteristics

2. **Value Type Testing:**
   - Each test file covers void/int/string combinations
   - String tests stress move semantics and non-trivial destruction
   - Void tests verify zero-overhead abstraction
   - Add move-only and copy-only types for advanced scenarios

3. **Test Generation:**
   - Consider using Catch2 generators for cartesian products of parameters
   - Template test fixtures to reduce code duplication
   - Macros for repetitive assertion patterns
   - Property-based testing for algorithm validation

4. **Performance Benchmarks:**
   - Include micro-benchmarks in Phase 6 and 7.2
   - Compare container choices for typical graph operations
   - Document performance characteristics in comments
   - Use Google Benchmark or similar for consistent measurements
   - Track performance regressions across versions

5. **Future Extensions:**
   - **Phase 8**: Graph algorithms using CPOs (BFS, DFS, shortest path, MST, connectivity, topological sort, flow algorithms)
   - **Phase 9**: Bidirectional graph support (in-edges)
   - **Phase 9**: Multi-graph explicit support (parallel edges policy)
   - **Phase 10**: Property maps as separate entities
   - **Phase 10**: Graph views and adaptors
   - **Phase 11**: Serialization (GraphML, JSON, binary)
   - **Phase 11**: Graph generators (complete, random, scale-free)
   - **Phase 12**: Graph metrics (density, clustering, diameter)
   - **Future research**: Thread-safe variants with mutex protection

6. **Edge Case Philosophy:**
   - Test boundary conditions explicitly
   - Document undefined behavior clearly
   - Prefer compile-time errors over runtime errors
   - Use concepts/static_assert for type safety
   - Validate preconditions with assertions in debug builds

7. **Mutation Operations Design:**
   - Document iterator invalidation rules clearly
   - Strong exception guarantee for single-element operations
   - Basic exception guarantee for bulk operations
   - Consider copy-on-write for certain scenarios
   - Provide both checked and unchecked variants

8. **Memory Testing:**
   - Use custom allocators to track allocations
   - Verify no leaks with sanitizers
   - Test behavior under allocation failure
   - Measure memory overhead vs. theoretical minimum
   - Validate empty base optimization for void types

10. **Compatibility Testing:**
    - Ensure C++20 concepts are satisfied
    - Test with different standard library implementations
    - Verify ADL behavior is correct
    - Check for ODR violations
    - Ensure header-only compatibility if applicable

11. **Documentation Requirements:**
    - Document time complexity for all operations
    - Document space complexity and memory layout
    - Provide usage examples for each feature
    - Document design decisions and trade-offs
    - Include migration guides for API changes

12. **Continuous Improvement:**
    - Review coverage reports after each phase
    - Add tests for discovered bugs immediately
    - Refactor tests to reduce duplication
    - Update test plan based on implementation discoveries
    - Maintain test execution time under reasonable limits (~5 min total)

---

## Appendix A: Example Custom Types for Testing

### USAAddress (for map keys)
```cpp
struct USAAddress {
    std::string street;
    std::string city;
    std::string state;
    std::string zip;
    
    auto operator<=>(const USAAddress&) const = default;
    
    friend std::ostream& operator<<(std::ostream& os, const USAAddress& addr) {
        return os << addr.street << ", " << addr.city << ", " 
                  << addr.state << " " << addr.zip;
    }
};

namespace std {
    template<>
    struct hash<USAAddress> {
        size_t operator()(const USAAddress& addr) const {
            size_t h1 = hash<string>{}(addr.street);
            size_t h2 = hash<string>{}(addr.city);
            size_t h3 = hash<string>{}(addr.state);
            size_t h4 = hash<string>{}(addr.zip);
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };
}
```

### MoveOnlyValue (for testing move semantics)
```cpp
struct MoveOnlyValue {
    std::unique_ptr<int> data;
    
    MoveOnlyValue(int val = 0) : data(std::make_unique<int>(val)) {}
    MoveOnlyValue(const MoveOnlyValue&) = delete;
    MoveOnlyValue(MoveOnlyValue&&) = default;
    MoveOnlyValue& operator=(const MoveOnlyValue&) = delete;
    MoveOnlyValue& operator=(MoveOnlyValue&&) = default;
    
    int value() const { return *data; }
};
```

### ThrowingValue (for exception safety)
```cpp
struct ThrowingValue {
    int data;
    static int throw_countdown;
    
    ThrowingValue(int val = 0) : data(val) {
        if (throw_countdown > 0 && --throw_countdown == 0)
            throw std::runtime_error("Throwing constructor");
    }
    
    ThrowingValue(const ThrowingValue& other) : data(other.data) {
        if (throw_countdown > 0 && --throw_countdown == 0)
            throw std::runtime_error("Throwing copy constructor");
    }
    
    static void reset(int countdown = 0) { throw_countdown = countdown; }
};
```

### TrackingAllocator (for memory testing)
```cpp
template<typename T>
class TrackingAllocator {
    static inline size_t allocation_count = 0;
    static inline size_t deallocation_count = 0;
    static inline size_t bytes_allocated = 0;
    
public:
    using value_type = T;
    
    T* allocate(size_t n) {
        ++allocation_count;
        bytes_allocated += n * sizeof(T);
        return std::allocator<T>{}.allocate(n);
    }
    
    void deallocate(T* p, size_t n) {
        ++deallocation_count;
        std::allocator<T>{}.deallocate(p, n);
    }
    
    static void reset() {
        allocation_count = 0;
        deallocation_count = 0;
        bytes_allocated = 0;
    }
    
    static auto stats() {
        return std::tuple{allocation_count, deallocation_count, bytes_allocated};
    }
};
```

---

## Appendix B: Test Execution Guidelines

### Running Tests

```bash
# Run all tests
./graph3_tests

# Run specific phase
./graph3_tests "[vofl]"
./graph3_tests "[cpo]"

# Run with coverage
cmake --build build --target graph3_tests_coverage
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html

# Run with sanitizers
cmake -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined" ..
./graph3_tests

# Run performance benchmarks
./graph3_tests "[.benchmark]" --benchmark-samples 100
```

### Test Organization Best Practices

1. **Use SECTION for related tests**
   ```cpp
   TEST_CASE("load_edges behavior", "[load][edges]") {
       SECTION("empty range") { /* ... */ }
       SECTION("single edge") { /* ... */ }
       SECTION("multiple edges") { /* ... */ }
   }
   ```

2. **Use TEST_CASE_TEMPLATE for type parameterization**
   ```cpp
   TEMPLATE_TEST_CASE("works with various value types", "[values]",
                      int, std::string, double) {
       using Graph = dynamic_graph<TestType, void, void>;
       // ...
   }
   ```

3. **Use generators for combinations**
   ```cpp
   auto value = GENERATE(0, 42, -1, INT_MAX);
   auto sourced = GENERATE(true, false);
   ```

4. **Tag tests appropriately**
   - `[.]` - Hidden, must be explicitly selected
   - `[.benchmark]` - Performance tests
   - `[!throws]` - Expected to throw
   - `[!mayfail]` - May fail in some environments

---

## Appendix C: Implementation Checklist

### Before Starting Each Phase:
- [ ] Review previous phase coverage report
- [ ] Update test plan based on findings
- [ ] Create feature branch
- [ ] Set up CI pipeline for new tests

### During Implementation:
- [ ] Write tests before implementation (TDD)
- [ ] Run tests frequently during development
- [ ] Use compiler warnings (-Wall -Wextra -Wpedantic)
- [ ] Run static analyzers regularly
- [ ] Document complex test scenarios

### After Completing Each Phase:
- [ ] Achieve target coverage percentage
- [ ] Run all tests with sanitizers
- [ ] Generate and review coverage report
- [ ] Update documentation
- [ ] Code review
- [ ] Merge to main branch
- [ ] Tag release/milestone

### Quality Gates:
- [ ] Zero compiler warnings
- [ ] Zero sanitizer errors
- [ ] Zero static analyzer errors
- [ ] Coverage target met
- [ ] All tests passing
- [ ] Performance benchmarks within acceptable range
- [ ] Documentation updated
- [ ] Code review approved

---
