# Vertex Inner Value Pattern Concepts

This document describes the concepts and type traits added to support the different type patterns used in `vertex_descriptor::inner_value()`.

## Overview

The `inner_value()` method returns the vertex data (excluding the vertex ID/key) and behaves differently based on the container type. Three distinct patterns are supported:

**IMPORTANT:** These patterns enable the default implementation of `vertices(g)`. When a graph container `g` follows one of these inner value patterns, `vertices(g)` can automatically return `vertex_descriptor_view(g)` without requiring a custom `vertices()` member or ADL override.

## 1. Random Access Vertex Pattern

**Concept:** `random_access_vertex_pattern<Iter>`

- **Container Type:** Random-access containers (e.g., `std::vector`, `std::deque`)
- **Behavior:** Returns the entire element from `container[index]`
- **Use Case:** When vertex ID is implicitly the index, and the entire value is the vertex data

```cpp
// Example: std::vector<VertexData>
std::vector<VertexData> vertices;
// inner_value returns VertexData&

static_assert(random_access_vertex_pattern<decltype(vertices.begin())>);
```

## 2. Pair Value Vertex Pattern

**Concept:** `pair_value_vertex_pattern<Iter>`

- **Container Type:** Bidirectional containers with pair-like values (e.g., `std::map`, `std::unordered_map`)
- **Behavior:** Returns `.second` from the pair (the data part, excluding the key)
- **Use Case:** When vertex ID is the key, and only the mapped value is the vertex data

```cpp
// Example: std::map<int, VertexData>
std::map<int, VertexData> vertices;
// inner_value returns VertexData& (the .second part)

static_assert(pair_value_vertex_pattern<decltype(vertices.begin())>);
```

## 3. Whole Value Vertex Pattern

**Concept:** `whole_value_vertex_pattern<Iter>`

- **Container Type:** Bidirectional containers with non-pair values
- **Behavior:** Returns the entire dereferenced iterator value
- **Use Case:** Custom bidirectional containers where the value is not structured as a pair

```cpp
// Example: Custom bidirectional container with simple values
CustomBidirectionalContainer<VertexData> vertices;
// inner_value returns VertexData&

static_assert(whole_value_vertex_pattern<decltype(vertices.begin())>);
```

## Comprehensive Concept

**Concept:** `has_inner_value_pattern<Iter>`

This concept ensures that an iterator matches at least one of the three supported patterns. Any valid `vertex_iterator` should satisfy this concept.

```cpp
template<typename Iter>
concept has_inner_value_pattern = 
    random_access_vertex_pattern<Iter> || 
    pair_value_vertex_pattern<Iter> || 
    whole_value_vertex_pattern<Iter>;
```

## Type Traits and Enumerations

### Pattern Detection Trait

```cpp
template<typename Iter>
struct vertex_inner_value_pattern {
    static constexpr bool is_random_access = random_access_vertex_pattern<Iter>;
    static constexpr bool is_pair_value = pair_value_vertex_pattern<Iter>;
    static constexpr bool is_whole_value = whole_value_vertex_pattern<Iter>;
};

// Helper variable template
template<typename Iter>
inline constexpr auto vertex_inner_value_pattern_v = vertex_inner_value_pattern<Iter>{};
```

### Pattern Enumeration

```cpp
enum class vertex_inner_pattern {
    random_access,  ///< Random-access container, returns container[index]
    pair_value,     ///< Pair-like value, returns .second (data without key)
    whole_value     ///< Non-pair value, returns entire dereferenced iterator
};
```

### Pattern Type Trait

```cpp
template<typename Iter>
struct vertex_inner_pattern_type {
    static constexpr vertex_inner_pattern value = 
        random_access_vertex_pattern<Iter> ? vertex_inner_pattern::random_access :
        pair_value_vertex_pattern<Iter> ? vertex_inner_pattern::pair_value :
        vertex_inner_pattern::whole_value;
};

// Helper variable template
template<typename Iter>
inline constexpr vertex_inner_pattern vertex_inner_pattern_type_v = 
    vertex_inner_pattern_type<Iter>::value;
```

## Usage Examples

### Compile-Time Pattern Detection

```cpp
#include "desc/descriptor.hpp"
#include <vector>
#include <map>

using VectorIter = std::vector<int>::iterator;
using MapIter = std::map<int, double>::iterator;

// Check which pattern an iterator satisfies
static_assert(random_access_vertex_pattern<VectorIter>);
static_assert(pair_value_vertex_pattern<MapIter>);

// Get pattern as enum
static_assert(vertex_inner_pattern_type_v<VectorIter> == 
              vertex_inner_pattern::random_access);
static_assert(vertex_inner_pattern_type_v<MapIter> == 
              vertex_inner_pattern::pair_value);

// Use pattern detection trait
static_assert(vertex_inner_value_pattern_v<VectorIter>.is_random_access);
static_assert(vertex_inner_value_pattern_v<MapIter>.is_pair_value);
```

### Constraining Template Functions

```cpp
// Function only for random-access vertex patterns
template<typename Iter>
    requires random_access_vertex_pattern<Iter>
void process_indexed_vertices(Iter begin, Iter end) {
    // Implementation for vector-like containers
}

// Function only for pair-value vertex patterns
template<typename Iter>
    requires pair_value_vertex_pattern<Iter>
void process_keyed_vertices(Iter begin, Iter end) {
    // Implementation for map-like containers
}

// Function for any valid inner_value pattern
template<typename Iter>
    requires has_inner_value_pattern<Iter>
void process_any_vertices(Iter begin, Iter end) {
    // Implementation for all supported patterns
}
```

### Runtime Pattern Dispatch

```cpp
template<typename Iter>
void dispatch_by_pattern(Iter begin, Iter end) {
    if constexpr (random_access_vertex_pattern<Iter>) {
        // Handle random-access pattern
    } else if constexpr (pair_value_vertex_pattern<Iter>) {
        // Handle pair-value pattern
    } else if constexpr (whole_value_vertex_pattern<Iter>) {
        // Handle whole-value pattern
    }
}
```

## Relationship to Existing Concepts

These new inner_value pattern concepts complement the existing vertex storage concepts:

- `direct_vertex_type` / `keyed_vertex_type`: Determine how vertex IDs are stored/extracted
- `random_access_vertex_pattern` / `pair_value_vertex_pattern` / `whole_value_vertex_pattern`: Determine how vertex data is accessed via `inner_value()`

Both concept families work together to provide complete type safety for vertex descriptor operations.

## Integration with vertices(g) CPO

The inner value pattern concepts are crucial for the default implementation of `vertices(g)`:

### Default Implementation Strategy

`vertices(g)` MUST always return a `vertex_descriptor_view`. The CPO uses the following resolution order:

1. **Override with member function**: If `g.vertices()` exists, use it (must return `vertex_descriptor_view`)
2. **Override with ADL**: If ADL `vertices(g)` exists, use it (must return `vertex_descriptor_view`)
3. **Default to inner value pattern**: If `g` is a forward range with iterators satisfying `has_inner_value_pattern`, return `vertex_descriptor_view(g)`

### Why Inner Value Patterns Enable Default Implementation

When a container follows one of the inner value patterns, `vertex_descriptor_view` can automatically:
- Iterate over the container's elements
- Create `vertex_descriptor` instances that know how to extract vertex IDs and data
- Provide a uniform interface regardless of the underlying container type

### Example: Automatic vertices(g) Support

```cpp
// No custom vertices() needed for these containers:

// Random access pattern: vector as graph
std::vector<std::vector<int>> adj_list;
auto verts = vertices(adj_list);  // Returns vertex_descriptor_view(adj_list)
                                   // Descriptors use index as vertex ID

// Pair value pattern: map as graph
std::map<int, std::vector<int>> adj_map;
auto verts2 = vertices(adj_map);  // Returns vertex_descriptor_view(adj_map)
                                   // Descriptors use key as vertex ID

// Custom override when needed:
class MyGraph {
public:
    auto vertices() const { 
        return vertex_descriptor_view(internal_vertices_); 
    }
private:
    std::vector<VertexData> internal_vertices_;
};
```

This design means that most simple graph containers automatically support `vertices(g)` without any additional code, as long as they follow one of the three inner value patterns.
