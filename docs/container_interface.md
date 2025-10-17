# Graph Container Interface

This document summarizes the Graph Container Interface (GCI) from P1709/D3130. The GCI defines the primitive concepts, traits, types, and functions used to define and access adjacency lists and edgelists, independent of their internal design and organization.

**Reference:** This document follows the conventions defined in [common_graph_guidelines.md](common_graph_guidelines.md).

## Overview

The Graph Container Interface provides:
- **Flexibility**: Supports various graph representations (vector of lists, CSR, adjacency matrix, etc.)
- **Customization**: All functions are customization point objects (CPOs) that can be overridden
- **Generality**: Designed to support future graph data models beyond current requirements
- **Default Implementations**: Reasonable defaults minimize work needed to adapt existing data structures

### Design Principles

1. **Descriptors**: Opaque objects representing vertices and edges that abstract implementation details
   - For random-access containers: descriptors are integral indices
   - For bidirectional containers: descriptors are iterators
   - Reduces number of functions and concepts needed

2. **Customization Points**: All interface functions can be overridden via CPO mechanism

3. **Range-of-Ranges**: Adjacency lists treated as ranges of vertices, where each vertex is a range of edges

4. **Partitions**: Support for unipartite (1), bipartite (2), and multipartite (n) graphs

## Adjacency List Interface

### Concepts

**Qualifiers used in concept names:**
- **`index`**: Vertex range is random-access and vertex ID is integral
- **`sourced`**: Edge has a source ID

#### Edge Concepts

| Concept | Description |
|---------|-------------|
| `basic_edge<G, E>` | Edge `E` with `target_id(g,uv)` available |
| `adjacency_edge<G, E>` | Edge `E` with `target(g,uv)` available |
| `sourced_edge<G, E>` | Edge `E` with `source_id(g,uv)` and `source(g,uv)` available |

#### Edge Range Concepts

| Concept | Description |
|---------|-------------|
| `targeted_edge_range<G, ER>` | Range `ER` of edges with `target_id(g,uv)` |

#### Vertex Concepts

| Concept | Description |
|---------|-------------|
| `vertex_range<G, VR>` | General vertex range concept |
| `index_vertex_range<G, VR>` | Vertex range with random-access and integral vertex IDs (high-performance graphs) |

#### Adjacency List Concepts

| Concept | Description |
|---------|-------------|
| `adjacency_list<G>` | General adjacency list (allows associative containers for vertices) |
| `index_adjacency_list<G>` | High-performance adjacency list (vertices in random-access containers, integral IDs) |

**Note:** All algorithms initially proposed use `index_adjacency_list<G>`. Future proposals may use the more general `adjacency_list<G>`.

### Type Traits

| Trait | Type | Description |
|-------|------|-------------|
| `has_degree<G>` | concept | Is `degree(g,u)` available? |
| `has_find_vertex<G>` | concept | Are `find_vertex(g,_)` functions available? |
| `has_find_vertex_edge<G>` | concept | Are `find_vertex_edge(g,_)` functions available? |
| `has_contains_edge<G>` | concept | Is `contains_edge(g,uid,vid)` available? |
| `define_unordered_edge<G>` | struct (false_type) | Specialize to derive from `true_type` for unordered edges |
| `unordered_edge<G>` | concept | Graph has unordered edges |
| `ordered_edge<G>` | concept | Graph has ordered edges |
| `define_adjacency_matrix<G>` | struct (false_type) | Specialize to derive from `true_type` for adjacency matrix |
| `is_adjacency_matrix<G>` | struct | Type trait for adjacency matrix |
| `is_adjacency_matrix_v<G>` | bool | Variable template for adjacency matrix |
| `adjacency_matrix<G>` | concept | Graph is an adjacency matrix |

### Type Aliases

Type aliases follow the `_t<G>` convention where `G` is the graph type.

#### Graph Types

| Type Alias | Definition | Comment |
|------------|------------|---------|
| `graph_reference_t<G>` | `add_lvalue_reference<G>` | |
| `graph_value_t<G>` | `decltype(graph_value(g))` | Optional |

#### Vertex Types

| Type Alias | Definition | Comment |
|------------|------------|---------|
| `vertex_range_t<G>` | `decltype(vertices(g))` | Range of vertices |
| `vertex_iterator_t<G>` | `iterator_t<vertex_range_t<G>>` | Iterator over vertices |
| `vertex_t<G>` | `range_value_t<vertex_range_t<G>>` | Vertex descriptor type |
| `vertex_reference_t<G>` | `range_reference_t<vertex_range_t<G>>` | Reference to vertex |
| `vertex_id_t<G>` | `decltype(vertex_id(g,u))` | Vertex identifier type |
| `vertex_value_t<G>` | `decltype(vertex_value(g,u))` | Optional vertex value |

#### Edge Types

| Type Alias | Definition | Comment |
|------------|------------|---------|
| `vertex_edge_range_t<G>` | `decltype(edges(g,u))` | Range of outgoing edges from vertex |
| `vertex_edge_iterator_t<G>` | `iterator_t<vertex_edge_range_t<G>>` | Iterator over edges |
| `edge_t<G>` | `range_value_t<vertex_edge_range_t<G>>` | Edge descriptor type |
| `edge_reference_t<G>` | `range_reference_t<vertex_edge_range_t<G>>` | Reference to edge |
| `edge_value_t<G>` | `decltype(edge_value(g,uv))` | Optional edge value |

#### Partition Types

| Type Alias | Definition | Comment |
|------------|------------|---------|
| `partition_id_t<G>` | `decltype(partition_id(g,u))` | Optional partition identifier |
| `partition_vertex_range_t<G>` | `decltype(vertices(g,pid))` | Optional partition vertex range |

### Functions

#### Graph Functions

| Function | Return Type | Complexity | Default Implementation |
|----------|-------------|------------|------------------------|
| `graph_value(g)` | `graph_value_t<G>` | constant | n/a, optional |
| `vertices(g)` | `vertex_range_t<G>` | constant | `g` if `random_access_range<G>`, n/a otherwise |
| `num_vertices(g)` | integral | constant | `size(vertices(g))` |
| `num_edges(g)` | integral | \|E\| | `n=0; for(u: vertices(g)) n+=distance(edges(g,u)); return n;` |
| `has_edge(g)` | bool | \|V\| | `for(u: vertices(g)) if !empty(edges(g,u)) return true; return false;` |
| `num_partitions(g)` | integral | constant | 1 |
| `vertices(g,pid)` | `partition_vertex_range_t<G>` | constant | `vertices(g)` |
| `num_vertices(g,pid)` | integral | constant | `size(vertices(g))` |

**Note:** Complexity shown for `num_edges(g)` and `has_edge(g)` is for the default implementation. Specific implementations may have better characteristics.

#### Vertex Functions

| Function | Return Type | Complexity | Default Implementation |
|----------|-------------|------------|------------------------|
| `find_vertex(g,uid)` | `vertex_iterator_t<G>` | constant | `begin(vertices(g)) + uid` if random-access |
| `vertex_id(g,u)` | `vertex_id_t<G>` | constant | (see Determining vertex_id below) |
| `vertex_value(g,u)` | `vertex_value_t<G>` | constant | n/a, optional |
| `vertex_value(g,uid)` | `vertex_value_t<G>` | constant | `vertex_value(g,*find_vertex(g,uid))`, optional |
| `degree(g,u)` | integral | constant | `size(edges(g,u))` if sized_range |
| `degree(g,uid)` | integral | constant | `degree(g,*find_vertex(g,uid))` if sized_range |
| `edges(g,u)` | `vertex_edge_range_t<G>` | constant | `u` if `forward_range<vertex_t<G>>`, n/a otherwise |
| `edges(g,uid)` | `vertex_edge_range_t<G>` | constant | `edges(g,*find_vertex(g,uid))` |
| `partition_id(g,u)` | `partition_id_t<G>` | constant | Optional |
| `partition_id(g,uid)` | `partition_id_t<G>` | constant | Optional |

**Note:** Functions with `uid` parameter are convenience functions that call `find_vertex(g,uid)` first.

#### Edge Functions

| Function | Return Type | Complexity | Default Implementation |
|----------|-------------|------------|------------------------|
| `target_id(g,uv)` | `vertex_id_t<G>` | constant | (see pattern matching below) |
| `target(g,uv)` | `vertex_t<G>` | constant | `*(begin(vertices(g)) + target_id(g,uv))` if random-access & integral |
| `edge_value(g,uv)` | `edge_value_t<G>` | constant | `uv` if forward_range, n/a otherwise, optional |
| `find_vertex_edge(g,u,vid)` | `vertex_edge_iterator_t<G>` | linear | `find(edges(g,u), [](uv) { return target_id(g,uv)==vid; })` |
| `find_vertex_edge(g,uid,vid)` | `vertex_edge_iterator_t<G>` | linear | `find_vertex_edge(g, *find_vertex(g,uid), vid)` |
| `contains_edge(g,uid,vid)` | bool | constant (matrix) / linear | Adjacency matrix: `uid < size(vertices(g)) && vid < size(vertices(g))`<br>Otherwise: `find_vertex_edge(g,uid,vid) != end(edges(g,uid))` |

**Sourced Edge Functions** (only when `source_id(g,uv)` is defined):

| Function | Return Type | Complexity | Default Implementation |
|----------|-------------|------------|------------------------|
| `source_id(g,uv)` | `vertex_id_t<G>` | constant | n/a, optional |
| `source(g,uv)` | `vertex_t<G>` | constant | `*(begin(vertices(g)) + source_id(g,uv))` if random-access & integral |

### Determining vertex_id and its Type

The type for `vertex_id_t<G>` is determined in the following order:

1. Use the type returned by `vertex_id(g,u)` when overridden for a graph
2. When the graph matches pattern `random_access_range<forward_range<integral>>` or `random_access_range<forward_range<tuple<integral,...>>>`, use the integral type
3. Use `size_t` in all other cases

The value of `vertex_id(g,u)` for a descriptor is determined by:

1. Use the value returned by `vertex_id(g,u)` when overridden for a graph
2. Use the index value on the descriptor

**Recommendation:** Override `vertex_id(g,u)` to use a smaller type (e.g., `int32_t` or `int16_t`) for space and performance advantages, ensuring it's large enough for the number of vertices and edges.

### Descriptor Views

Descriptors are opaque, abstract objects representing vertices and edges. They provide:

- **Equality comparison**: `==`, `!=`
- **Ordering**: `<`, `<=`, `>`, `>=` (if supported by underlying container)
- **Copy and assignment**: Standard semantics
- **Default construction**: Valid but may not represent valid vertex/edge
- **`inner_value()`**: Member function returning reference to underlying container value (for CPO customization)

**Implementation:**
- Random-access containers: descriptor is an integral index
- Bidirectional containers: descriptor is an iterator

**Views:**
- `descriptor_range_view<I>`: View over a range with descriptors
- `descriptor_subrange_view<I>`: View for edges from multiple vertices in single container (CSR, adjacency matrix)

### Partition Support

- **Unipartite**: `num_partitions(g) == 1` (default)
- **Bipartite**: `num_partitions(g) == 2`
- **Multipartite**: `num_partitions(g) >= 2`

**Vertex Value Types:**
Multiple partition types can be handled using:
- `std::variant`: Lambda returns appropriate variant based on partition
- Base class pointer: Call member function based on partition
- `void*`: Cast to concrete type based on partition

**Edge Filtering:**
- `edges(g,uid,pid)` / `edges(g,u,pid)`: Filter edges where target is in partition `pid`

## Edgelist Interface

An edgelist is a range of values with `source_id`, `target_id`, and optional `edge_value`. It's similar to edges in adjacency lists but is a distinct, separate range.

**Namespace:** `std::graph::edgelist` (to avoid conflicts with adjacency list)

### Concepts

| Concept | Description |
|---------|-------------|
| `basic_sourced_edge<EL, E>` | Edge with `source_id(e)` and `target_id(e)` |
| `basic_sourced_edgelist<EL>` | Range of basic sourced edges |
| `sourced_edge<EL, E>` | Edge with source/target descriptors and optional value |
| `sourced_edgelist<EL>` | Range of sourced edges |
| `index_sourced_edgelist<EL>` | Edgelist with integral vertex IDs |

### Type Traits

| Trait | Type | Description |
|-------|------|-------------|
| `is_directed<EL>` | struct (false_type) | Specialize to derive from `true_type` for directed graphs |
| `is_directed_v<EL>` | bool | Variable template for directed edgelist |

**Usage:** During graph construction, may add a second edge with reversed source_id and target_id when true.

### Type Aliases

| Type Alias | Definition | Comment |
|------------|------------|---------|
| `edge_range_t<EL>` | `EL` | |
| `edge_iterator_t<EL>` | `iterator_t<edge_range_t<EL>>` | |
| `edge_t<EL>` | `range_value_t<edge_range_t<EL>>` | |
| `edge_reference_t<EL>` | `range_reference_t<edge_range_t<EL>>` | |
| `edge_value_t<EL>` | `decltype(edge_value(e))` | Optional |
| `vertex_id_t<EL>` | `decltype(target_id(e))` | |

### Functions

| Function | Return Type | Complexity | Default Implementation |
|----------|-------------|------------|------------------------|
| `target_id(e)` | `vertex_id_t<EL>` | constant | (see pattern matching below) |
| `source_id(e)` | `vertex_id_t<EL>` | constant | (see pattern matching below) |
| `edge_value(e)` | `edge_value_t<EL>` | constant | Optional, see pattern matching |
| `contains_edge(el,uid,vid)` | bool | linear | `find_if(el, [](e) { return source_id(e)==uid && target_id(e)==vid; })` |
| `num_edges(el)` | integral | constant | `size(el)` |
| `has_edge(el)` | bool | constant | `num_edges(el) > 0` |

### Pattern Matching for Edge Types

The interface automatically recognizes these patterns:

#### Tuple Patterns

| Pattern | Provides |
|---------|----------|
| `tuple<integral, integral>` | `source_id(e)`, `target_id(e)` |
| `tuple<integral, integral, scalar>` | `source_id(e)`, `target_id(e)`, `edge_value(e)` |

#### edge_info Patterns

| Pattern | Provides |
|---------|----------|
| `edge_info<VId, true, void, void>` | `source_id(e)`, `target_id(e)` |
| `edge_info<VId, true, void, EV>` | `source_id(e)`, `target_id(e)`, `edge_value(e)` |

**Note:** For other edge types, override the functions.

## Exception Handling

**`graph_error`**: Exception class inherited from `runtime_error`. May be used by any function but primarily anticipated for `load` functions.

## Using Existing Data Structures

The GCI provides reasonable defaults to minimize adaptation work:

- **Default implementations**: Leverage standard library types and containers
- **Override capability**: All functions are CPOs that can be customized
- **Pattern recognition**: Automatic support for common patterns (tuples, edge_info)
- **Minimal boilerplate**: Most adaptations require overriding only a few key functions

**Recommendation:** See the Graph Library Containers paper for detailed examples of adapting external data structures.

---

**Document Status:** For exposition only (concepts marked as such pending consensus)

**Related Documents:**
- [common_graph_guidelines.md](common_graph_guidelines.md) - Naming conventions and architectural requirements
- P1709/D3130 - Full specification with implementation details
