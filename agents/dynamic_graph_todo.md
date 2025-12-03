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
  4. `vector` vertices + `deque` edges (vod_graph_traits - stable edge iterators)
  5. `deque` vertices + `forward_list` edges (dofl_graph_traits - stable vertex iterators)
  6. `deque` vertices + `list` edges (dol_graph_traits - bidirectional with stability)
  7. `deque` vertices + `vector` edges (dov_graph_traits - random access edges)
  8. `deque` vertices + `deque` edges (dod_graph_traits - all stable iterators)

**Test Files Organization (Optimized with TEMPLATE_TEST_CASE):**
```
tests/
  # Phase 1: Sequential container unified tests (vector/deque vertices)
  test_dynamic_graph_common.cpp         # ~2000 lines, runs 8x via TEMPLATE_TEST_CASE
                                        # Uses uint64_t vertex IDs, tests auto-extension
                                        # Covers: vofl, vol, vov, vod, dofl, dol, dov, dod
  
  # Phase 1: Container-specific behavior tests
  test_dynamic_graph_vector_traits.cpp  # ~800 lines (vofl, vol, vov, vod specifics)
  test_dynamic_graph_deque_traits.cpp   # ~1000 lines (dofl, dol, dov, dod specifics + CPO)
  test_dynamic_graph_edge_containers.cpp # ~600 lines (edge container variations)
                                        # Tests forward_list, list, vector, deque edge behaviors
  
  # Phase 3: Associative container unified tests (map/unordered_map vertices)
  test_dynamic_graph_associative_common.cpp  # ~1600 lines, runs 3-6x
                                             # Uses std::string vertex IDs, key-based lookup
  
  # Phase 3: Map-specific behavior tests
  test_dynamic_graph_map_traits.cpp          # ~700 lines (ordered, comparators)
  test_dynamic_graph_unordered_map_traits.cpp # ~700 lines (hash, buckets)
  
  # Legacy complete test files (Phase 1.1-1.3 already completed)
  test_dynamic_graph_vofl.cpp           # ~2673 lines ✅ COMPLETE
  test_dynamic_graph_vol.cpp            # ~2677 lines ✅ COMPLETE
  test_dynamic_graph_vov.cpp            # ~2677 lines ✅ COMPLETE
```

**Deque Edge Container Coverage:**
Deque edges provide stable iterators and random access, combining benefits of vector and list:
- `vod_graph_traits` (vector + deque): Contiguous vertices, stable edge iterators
- `dod_graph_traits` (deque + deque): All stable iterators, good for dynamic graphs
- Tests will verify: stable iterators during modifications, random access, bidirectional iteration
- CPO tests will validate deque-specific optimizations (e.g., efficient insert/erase)

**Note:** Phase 1.4+ uses optimized TEMPLATE_TEST_CASE for sequential containers. Phase 3 will need a separate associative common file due to fundamental differences in vertex ID types and construction semantics.

**Test Coverage per File (~200-300 tests each):**

1. **Construction Tests (40 tests)** ✅ COMPLETE (17 tests in Phase 1.1)
   - [x] Default constructor
   - [x] Constructor with allocator
   - [x] Constructor with edge range only (with/without vertex count)
   - [x] Constructor with edge + vertex ranges
   - [x] Constructor with initializer_list (16 new tests added)
   - [x] Constructor with graph value (copy/move)
   - [ ] Constructor with partitions (not implemented yet)
   - [x] Test all combinations of value types (void/int/string)
   - [x] Test sourced vs unsourced edges

2. **Load Operations Tests (30 tests)** ✅ COMPLETE (8 tests in Phase 1.1)
   - [x] `load_vertices()` with various projections
   - [x] `load_vertices()` with move semantics
   - [x] `load_edges()` with various projections
   - [x] `load_edges()` with move semantics
   - [x] `load_edges()` with vertex inference
   - [x] `load_edges()` with explicit vertex count
   - [x] Edge count hints
   - [x] Out-of-range vertex ID behavior (auto-extends)

3. **Vertex Access Tests (25 tests)** ✅ COMPLETE (10 tests in Phase 1.1)
   - [x] `operator[]` access
   - [x] `begin()/end()` iteration
   - [x] `size()` queries
   - [x] Const correctness
   - [x] Empty graph behavior
   - [x] Single vertex graphs
   - [x] Large graphs (1000+ vertices, 10k vertices)

4. **Edge Access Tests (30 tests)** ✅ COMPLETE (10 tests in Phase 1.1)
   - [x] Access via vertex iterator
   - [x] Access via vertex ID
   - [x] Edge iteration for each vertex
   - [x] Empty edge lists
   - [x] Self-loops (single and multiple)
   - [x] Multiple edges to same target (parallel edges)
   - [x] Const correctness

5. **Value Access Tests (25 tests)** ✅ COMPLETE (10 tests in Phase 1.1)
   - [x] `vertex.value()` for non-void VV
   - [x] `edge.value()` for non-void EV
   - [x] Graph value access for non-void GV
   - [x] Compile-time errors for void types (static_assert checks)
   - [x] Value modification
   - [x] Move semantics

6. **Partition Tests (20 tests)** ⏸️ DEFERRED
   - [ ] Single partition (default)
   - [ ] Multiple partitions
   - [ ] Partition ID lookup
   - [ ] Partition vertex ranges
   - [ ] Invalid partition access
   - [ ] Empty partitions
   - [ ] Partition boundary vertices
   - Note: Partition API not yet implemented in dynamic_graph

7. **Sourced Edge Tests (15 tests)** ✅ COMPLETE (6 tests in Phase 1.1)
   - [x] Source ID access when Sourced=true
   - [x] Compile-time error when Sourced=false
   - [x] Sourced edge construction
   - [x] Copy/move with sourced edges
   - Note: Source vertex lookup requires CPO implementation

8. **Property Tests (20 tests)** ✅ COMPLETE (7 tests in Phase 1.1)
   - [x] Vertex count queries
   - [x] Edge count queries (manual iteration)
   - [x] Degree queries (per vertex)
   - [x] Empty graph properties
   - [x] Graph with only vertices (no edges)
   - [x] Sink detection
   - [x] Max degree computation

9. **Memory & Performance Tests (15 tests)** ✅ COMPLETE (8 tests in Phase 1.1)
   - [x] Memory layout for void types (verify no storage)
   - [x] Large graph construction (10k vertices)
   - [x] Iterator stability after modifications
   - [x] Dense graphs (many edges per vertex)
   - [x] Sparse graphs (few edges)
   - [x] Performance characteristics
   - Note: Reserve/resize not applicable to forward_list

10. **Edge Cases & Error Handling (25 tests)** ✅ COMPLETE (11 tests in Phase 1.1)
    - [x] Null/empty ranges
    - [x] Invalid vertex IDs (auto-extends)
    - [x] Boundary values (zero, negative, large)
    - [x] Exception safety
    - [x] Duplicate edges behavior
    - [x] Incremental building
    - [x] Bidirectional edges

11. **Iterator & Ranges Tests (15 tests)** ✅ COMPLETE (3 tests in Phase 1.1)
    - [x] Iterator stability
    - [x] Nested iteration
    - [x] std::ranges integration (count_if, find_if, transform, filter)
    - [x] Algorithm compatibility (accumulate, all_of, any_of, none_of)

12. **Workflow & Integration Tests (10 tests)** ✅ COMPLETE (3 tests in Phase 1.1)
    - [x] Build/query/modify workflows
    - [x] Social network simulation
    - [x] Dependency graph
    - [x] Complex graph structures (triangle, star, complete, cycle, tree, bipartite)

**Phase 1.1 Status:** ✅ **COMPLETE**
- File: `test_dynamic_graph_vofl.cpp` 
- Tests Added: 123 new tests (10 construction + 14 load operations + 24 vertex/edge access + 29 error handling + 17 comprehensive + 10 initializer_list + 19 test sections)
- Total Tests: 968 (845 existing + 123 new)
- Coverage: ~98% of vofl_graph_traits functionality
- All tests passing ✓
- Member functions uncommented: `target_id()`, `source_id()` (prerequisite for Phase 2 CPO)

**Phase 1.2 Status:** ✅ **COMPLETE**
- File: `test_dynamic_graph_vol.cpp`
- Tests Added: 115 tests (113 CTests, 628 assertions)
- Total Tests: 1075 (960 existing + 115 new)
- Coverage: ~98% of vol_graph_traits functionality
- All tests passing ✓
- Key features tested: Bidirectional edge iteration, std::list behavior
- Validated: Forward/backward iteration, reverse value verification
- File size: 2677 lines (matching vofl comprehensiveness)

**Phase 1.3 Status:** ✅ **COMPLETE**
- File: `test_dynamic_graph_vov.cpp`
- Tests Added: 115 tests (113 CTests, 628 assertions)
- Total Tests: 1190 (1075 existing + 115 new)
- Coverage: ~98% of vov_graph_traits functionality
- All tests passing ✓
- Key features tested: Random access, cache locality, std::vector behavior
- Validated: Random access iterators, index-based access
- File size: 2677 lines (matching vol and vofl comprehensiveness)

**Phase 1.4a: Vector Vertices + Deque Edges (vod_graph_traits) - COMPLETE** ✅
- Tests Added: 50 tests (44 CTests)
- File: `test_dynamic_graph_vod.cpp`
- Coverage: ~98% of vod_graph_traits functionality
- All tests passing ✓
- Key features tested: Stable edge iterators, random access, bidirectional iteration
- Validated: Deque edge container benefits (stable iterators + random access)
- File size: 2649 lines

**Implementation Approach:**
- [x] Create one test file at a time
- [x] Start with `test_dynamic_graph_vofl.cpp` (most common use case) - **COMPLETE**
- [x] Continue with `test_dynamic_graph_vol.cpp` (bidirectional edges) - **COMPLETE**
- [x] Next: `test_dynamic_graph_vov.cpp` (best cache locality) - **COMPLETE**
- [ ] Phase 1.4: Create optimized test structure using TEMPLATE_TEST_CASE
  - [ ] `test_dynamic_graph_common.cpp` - unified tests for 8 sequential traits (~2000 lines)
        Traits: vofl, vol, vov, vod, dofl, dol, dov, dod
  - [ ] `test_dynamic_graph_vector_traits.cpp` - vector vertex specifics (~800 lines)
        Covers: vofl, vol, vov, vod (contiguous vertex storage)
  - [ ] `test_dynamic_graph_deque_traits.cpp` - deque vertex specifics + CPO (~1000 lines)
        Covers: dofl, dol, dov, dod (non-contiguous vertex storage)
  - [ ] `test_dynamic_graph_edge_containers.cpp` - all edge container variations (~600 lines)
        Covers: forward_list, list, vector, deque edge behaviors
- [ ] Phase 3: Associative container tests (separate from Phase 1)
  - [ ] `test_dynamic_graph_associative_common.cpp` - unified tests for map traits (~1600 lines)
  - [ ] `test_dynamic_graph_map_traits.cpp` - map-specific behavior (~700 lines)
  - [ ] `test_dynamic_graph_unordered_map_traits.cpp` - unordered_map-specific (~700 lines)
- [x] Use Catch2 test framework with SECTION organization
- [x] Use Catch2 TEMPLATE_TEST_CASE for cross-container unified tests
- [x] Employ test generators for value type combinations where appropriate
- [x] Include both positive tests (correct usage) and negative tests (error detection)

**Status:** Phase 1.1, 1.2, 1.3, 1.4a completed; Phase 2 substantially complete for existing instances
- ✅ test_dynamic_graph_vofl.cpp created with 123 tests (2673 lines)
- ✅ test_dynamic_graph_vol.cpp expanded to 115 tests (2677 lines)
- ✅ test_dynamic_graph_vov.cpp created with 115 tests (2677 lines)
- ✅ test_dynamic_graph_vod.cpp created with 50 tests (2649 lines)
- ✅ test_dynamic_graph_cpo_vofl.cpp created with 196 tests (3414 lines)
- ✅ test_dynamic_graph_cpo_vol.cpp created with 196 tests (3413 lines)
- ✅ test_dynamic_graph_cpo_vov.cpp created with 196 tests (3486 lines)
- ⏳ Phase 1.4: Optimized test structure for sequential containers (deque traits)
- ⏳ Phase 2: Continue CPO tests for each new graph instance
- ⏳ Phase 3: Separate optimized structure for associative containers

**Expected Line Count (Optimized Strategy):**
- Legacy complete files (Phase 1.1-1.3): 8,027 lines ✅
- Phase 1.4a (vod): 2,649 lines ✅
- CPO tests for Phase 1.1-1.3: 10,313 lines ✅
- New optimized files Phase 1.4 (sequential with deque edges): ~4,400 lines
  * test_dynamic_graph_common.cpp: ~2,000 lines (8 traits)
  * test_dynamic_graph_vector_traits.cpp: ~800 lines (4 traits: vofl, vol, vov, vod)
  * test_dynamic_graph_deque_traits.cpp: ~1,000 lines (4 traits: dofl, dol, dov, dod)
  * test_dynamic_graph_edge_containers.cpp: ~600 lines (4 edge types)
- CPO tests for Phase 1.4 (sequential): ~13,600 lines
  * test_dynamic_graph_cpo_vod.cpp: ~3,400 lines (pending)
  * test_dynamic_graph_cpo_dofl.cpp: ~3,400 lines
  * test_dynamic_graph_cpo_dol.cpp: ~3,400 lines
  * test_dynamic_graph_cpo_dov.cpp: ~3,400 lines
  * test_dynamic_graph_cpo_dod.cpp: ~3,400 lines (deque + deque - all stable iterators)
- New optimized files Phase 3 (associative): ~3,000 lines
- CPO tests for Phase 3 (associative): ~9,000 lines
- **Total Phase 1 + Phase 2 + Phase 3: ~51,000 lines** (comprehensive coverage)
- **Note:** Phase 2 (CPO tests) run in parallel with Phase 1, not sequentially

---

## Phase 2: Add CPO Support (Integrated with Phase 1)

For each graph instance tested in Phase 1, create comprehensive CPO tests. This phase runs in parallel with Phase 1, not sequentially - each new graph type gets both basic tests and CPO tests.

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

**Test Files Completed:**
- `tests/test_dynamic_graph_cpo_vofl.cpp` (3,414 lines, 196 test cases) ✅
- `tests/test_dynamic_graph_cpo_vol.cpp` (3,413 lines, 196 test cases) ✅
- `tests/test_dynamic_graph_cpo_vov.cpp` (3,486 lines, 196 test cases) ✅

**Total CPO Tests:** 10,313 lines, 588 test cases, 5,580 assertions

**Test Files Pending (Phase 1.4 with deque edge coverage):**
- `tests/test_dynamic_graph_cpo_vod.cpp` (~3,400 lines) - vector + deque edges
- `tests/test_dynamic_graph_cpo_dofl.cpp` (~3,400 lines) - deque + forward_list edges
- `tests/test_dynamic_graph_cpo_dol.cpp` (~3,400 lines) - deque + list edges
- `tests/test_dynamic_graph_cpo_dov.cpp` (~3,400 lines) - deque + vector edges
- `tests/test_dynamic_graph_cpo_dod.cpp` (~3,400 lines) - deque + deque edges (all stable)
- Plus future associative container CPO tests

**CPO Implementation Strategy:**
- ✅ Uncommented and completed friend functions in dynamic_graph.hpp
- ✅ Used default implementations via CPO machinery from graph_cpo.hpp
- ✅ Leveraged existing descriptor infrastructure
- ✅ Testing each CPO with multiple configurations (EV, VV, GV, Sourced, Partitions)
- ⏳ Continue pattern for each new graph instance type

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
struct mov_graph_traits {  // map + vector
  using vertices_type = std::map<VId, vertex_type>;
  using edges_type = std::vector<edge_type>;
};

template <class EV, class VV, class GV, class VId, bool Sourced>
struct umofl_graph_traits {  // unordered_map + forward_list
  using vertices_type = std::unordered_map<VId, vertex_type>;
  using edges_type = std::forward_list<edge_type>;
};

template <class EV, class VV, class GV, class VId, bool Sourced>
struct umol_graph_traits {  // unordered_map + list
  using vertices_type = std::unordered_map<VId, vertex_type>;
  using edges_type = std::list<edge_type>;
};

template <class EV, class VV, class GV, class VId, bool Sourced>
struct umov_graph_traits {  // unordered_map + vector
  using vertices_type = std::unordered_map<VId, vertex_type>;
  using edges_type = std::vector<edge_type>;
};
```

**Required Vertex ID Types for Associative Containers:**
- `std::string` (named vertices - primary test type)
- `std::pair<int, int>` (coordinate pairs)
- `std::tuple<int, int, int>` (3D coordinates)
- Custom struct `USAAddress` with operator<, hash support

**Critical Differences from Sequential Containers:**
1. **No auto-extension** - only creates vertices explicitly referenced in edges
2. **Key-based lookup** - `g["alice"]` not `g[0]`
3. **Bidirectional iterators only** - no random access, no iterator arithmetic
4. **Different CPO implementations** - vertex_id() returns key, find_vertex() uses find()
5. **Different construction semantics** - sparse vertex IDs by design

**Implementation Changes Needed:**
1. Update `vertex_id()` CPO for bidirectional iterators (map) - returns key not index
2. Update `find_vertex()` to use map's find() member instead of begin() + offset
3. Adjust construction logic for non-contiguous vertex IDs
4. Handle vertex insertion vs. assignment semantics
5. No auto-extension - vertices must be explicitly created

**Test Files:**
```
tests/test_dynamic_graph_associative_common.cpp  # ~1600 lines (unified for 3-6 traits)
tests/test_dynamic_graph_map_traits.cpp          # ~700 lines (map-specific)
tests/test_dynamic_graph_unordered_map_traits.cpp # ~700 lines (unordered_map-specific)
```

**Test Coverage (Associative Common File):**
- Construction with string vertex IDs
- Key-based vertex lookup (not index-based)
- No auto-extension behavior
- Sparse vertex ID sequences
- Bidirectional iteration only (no random access)
- All edge access patterns (same as sequential)
- Value access (same as sequential where applicable)
- CPO implementations specific to associative containers

**Test Coverage (Map-Specific):**
- Ordered iteration (sorted by key)
- Custom comparators
- Key ordering requirements
- Iterator invalidation rules specific to std::map
- Lower/upper bound operations

**Test Coverage (Unordered_Map-Specific):**
- Custom hash functions (for unordered_map)
- Hash collision handling
- Bucket management
- Load factor behavior
- Hash consistency across copies
- Iterator invalidation rules specific to std::unordered_map

**Why Separate Common File is Required:**
- Sequential containers use `uint64_t` with auto-extension: `g[5]` creates vertices 0-5
- Associative containers use `std::string` without auto-extension: `g["alice"]` only creates "alice"
- Test assertions are fundamentally different: `REQUIRE(g[0].id() == 0)` vs `REQUIRE(g["alice"].id() == "alice")`
- CPO behavior differs: `vertex_id()` returns index vs key, `find_vertex()` uses arithmetic vs find()
- Cannot share TEMPLATE_TEST_CASE between integral and non-integral vertex ID types

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
  # Phase 1: Sequential containers (Optimized with TEMPLATE_TEST_CASE)
  test_dynamic_graph_vofl.cpp           # ~2673 lines ✅ COMPLETE
  test_dynamic_graph_vol.cpp            # ~2677 lines ✅ COMPLETE
  test_dynamic_graph_vov.cpp            # ~2677 lines ✅ COMPLETE
  test_dynamic_graph_common.cpp         # ~1800 lines (6 sequential traits, uint64_t IDs)
  test_dynamic_graph_vector_traits.cpp  # ~600 lines (vector-specific)
  test_dynamic_graph_deque_traits.cpp   # ~800 lines (deque-specific + CPO)
  test_dynamic_graph_edge_containers.cpp # ~400 lines (edge container deltas)
  
  # Phase 2: CPO support
  test_dynamic_graph_cpo.cpp       # ~2000 lines
  
  # Phase 3: Associative vertex containers (Separate TEMPLATE_TEST_CASE)
  test_dynamic_graph_associative_common.cpp  # ~1600 lines (3-6 assoc traits, string IDs)
  test_dynamic_graph_map_traits.cpp          # ~700 lines (map-specific: ordering, comparators)
  test_dynamic_graph_unordered_map_traits.cpp # ~700 lines (unordered_map-specific: hashing)
  
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

**Total Estimated Lines (Optimized):** ~21,800 lines of test code
**Total Estimated Tests:** ~4,000-4,700 test cases
**Test Executions:** ~14,000+ (TEMPLATE_TEST_CASE multiplying tests across 6 sequential + 3-6 associative traits)
**Code Reduction:** ~7,400 lines saved (25% overall reduction)

**Optimization Strategy:**
- **Two separate "common" files**: One for sequential containers (uint64_t IDs), one for associative (string IDs)
- Use Catch2 TEMPLATE_TEST_CASE to run identical tests across container types within each category
- Separate container-specific behavior tests (iterator invalidation, memory layout, CPO differences)
- Cannot unify sequential and associative due to fundamental differences in vertex ID types and semantics
- Maintain full coverage with significantly less code duplication
- Clear separation between common behavior and container-specific quirks

**Why Two Common Files Are Required:**
- **Sequential (vector/deque)**: Auto-extends to accommodate any integral vertex ID, random/bidirectional access
- **Associative (map/unordered_map)**: Only creates explicitly referenced vertices, bidirectional access only, key-based lookup
- Different vertex ID types: `uint64_t` vs `std::string` (cannot template over both)
- Different CPO implementations: `vertex_id()` returns index vs key
- Different test assertions: `g[5]` vs `g["alice"]`

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
   - ✅ **Use Catch2 TEMPLATE_TEST_CASE** for cross-container unified tests (Phase 1.4+)
   - Use Catch2 GENERATE for cartesian products of parameters
   - TEMPLATE_TEST_CASE_SIG for complex type parameter combinations
   - Separate container-specific tests from common behavior tests
   - Macros for repetitive assertion patterns (minimize use)
   - Property-based testing for algorithm validation
   
   **TEMPLATE_TEST_CASE Example:**
   ```cpp
   TEMPLATE_TEST_CASE("construction works", "[construction]",
       vofl_graph_traits, vol_graph_traits, vov_graph_traits,
       dofl_graph_traits, dol_graph_traits, dov_graph_traits) {
       using Graph = dynamic_graph<int, int, void, uint64_t, false, TestType>;
       Graph g;
       REQUIRE(g.size() == 0);
       // Test body runs 6 times (once per traits type)
   }
   ```

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
