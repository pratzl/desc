# Vertex Storage Type Concepts and Pattern Detection

## Overview

This document describes the concepts and type traits added to `descriptor.hpp` for detecting and categorizing vertex storage patterns used in graph descriptors.

## Concepts

### Vertex Storage Patterns

The library defines two mutually exclusive concepts for vertex storage patterns:

#### 1. `direct_vertex_type<Iter>`
Identifies random-access iterators used for direct/index-based vertex storage.

**Example:**
```cpp
std::vector<VertexData> vertices;  // Index is vertex ID
static_assert(direct_vertex_type<std::vector<VertexData>::iterator>);
static_assert(direct_vertex_type<std::deque<int>::iterator>);
```

**Characteristics:**
- Uses `size_t` index as vertex ID
- Storage type: `size_t`
- Access pattern: `vertices[index]`
- Supports random access operations

#### 2. `keyed_vertex_type<Iter>`
Identifies bidirectional iterators with pair-like value types used for key-value vertex storage.

**Example:**
```cpp
std::map<int, VertexData> vertex_map;  // Key is vertex ID
static_assert(keyed_vertex_type<std::map<int, VertexData>::iterator>);
static_assert(keyed_vertex_type<std::unordered_map<std::string, int>::iterator>);
```

**Characteristics:**
- Uses key (first element of pair) as vertex ID
- Storage type: Iterator
- Access pattern: `(*iterator).first` for ID, `(*iterator).second` for data
- Requires bidirectional iteration (not random access)
- Value type must be pair-like (`std::pair` or tuple-like with at least 2 elements)

### Comprehensive Concept

#### `vertex_iterator<Iter>`
Accepts any valid vertex iterator (either direct or keyed).

**Example:**
```cpp
static_assert(vertex_iterator<std::vector<int>::iterator>);           // Direct
static_assert(vertex_iterator<std::map<int, std::string>::iterator>); // Keyed
```

## Type Traits

### Pattern Detection Struct

#### `vertex_storage_pattern<Iter>`
Provides boolean flags for each pattern:

```cpp
template<typename Iter>
struct vertex_storage_pattern {
    static constexpr bool is_direct;
    static constexpr bool is_keyed;
};
```

**Usage:**
```cpp
using VecIter = std::vector<int>::iterator;
using Pattern = vertex_storage_pattern<VecIter>;
static_assert(Pattern::is_direct);
static_assert(!Pattern::is_keyed);
```

### Pattern Detection Variable Template

#### `vertex_storage_pattern_v<Iter>`
Convenient variable template for pattern detection:

```cpp
auto pattern = vertex_storage_pattern_v<std::map<int, std::string>::iterator>;
if (pattern.is_keyed) {
    // Handle keyed storage
}
```

### Pattern Enum

#### `vertex_pattern` Enumeration
Represents the two vertex storage patterns:

```cpp
enum class vertex_pattern {
    direct,   ///< Random-access/direct storage (index-based ID)
    keyed     ///< Key-value storage (key-based ID)
};
```

#### `vertex_pattern_type_v<Iter>`
Returns the pattern enum value for an iterator:

```cpp
static_assert(vertex_pattern_type_v<std::vector<int>::iterator> == vertex_pattern::direct);
static_assert(vertex_pattern_type_v<std::map<int, int>::iterator> == vertex_pattern::keyed);
```

### Vertex ID Type Extraction

#### `vertex_id_type_t<Iter>`
Extracts the type used for vertex IDs:

```cpp
// Direct storage: ID is size_t (the index)
using VecIter = std::vector<int>::iterator;
using IDType1 = vertex_id_type_t<VecIter>;
static_assert(std::same_as<IDType1, std::size_t>);

// Keyed storage: ID is the key type
using MapIter = std::map<std::string, double>::iterator;
using IDType2 = vertex_id_type_t<MapIter>;
static_assert(std::same_as<IDType2, std::string>);
```

## Pattern Matching in vertex_id()

The `vertex_id()` function in `vertex_descriptor` uses these patterns to extract the vertex ID:

```cpp
constexpr auto vertex_id() const noexcept {
    if constexpr (std::random_access_iterator<VertexIter>) {
        return storage_;  // Direct: return index
    } else {
        // Keyed: extract key from pair-like value_type
        return std::get<0>(*storage_);
    }
}
```

## Use Cases

### 1. Compile-Time Storage Pattern Detection

```cpp
template<typename VertexIter>
void process_vertices() {
    static_assert(vertex_iterator<VertexIter>, "Invalid vertex iterator");
    
    if constexpr (direct_vertex_type<VertexIter>) {
        // Optimized path for vector-like storage
        // Can use index arithmetic
    } else if constexpr (keyed_vertex_type<VertexIter>) {
        // Path for map-like storage
        // Must use iterator dereferencing
    }
}
```

### 2. Generic Vertex ID Extraction

```cpp
template<typename VertexIter>
void print_vertex_id_type() {
    using IDType = vertex_id_type_t<VertexIter>;
    
    std::cout << "Vertex ID type: " << typeid(IDType).name() << "\n";
    std::cout << "Storage pattern: ";
    
    if constexpr (direct_vertex_type<VertexIter>) {
        std::cout << "direct (index-based)\n";
    } else {
        std::cout << "keyed (map-based)\n";
    }
}
```

### 3. Constraining Function Templates

```cpp
// Function that only accepts direct (vector-like) vertices
template<typename VertexIter>
    requires direct_vertex_type<VertexIter>
auto get_vertex_by_index(std::size_t index) {
    return vertex_descriptor<VertexIter>{index};
}

// Function that only accepts keyed (map-like) vertices
template<typename VertexIter, typename KeyType>
    requires keyed_vertex_type<VertexIter>
auto find_vertex_by_key(const KeyType& key, /* container */) {
    // Implementation for map-based lookup
}
```

### 4. Runtime Pattern Switching

```cpp
template<typename VertexIter>
void analyze_vertex_storage() {
    constexpr auto pattern = vertex_pattern_type_v<VertexIter>;
    
    switch (pattern) {
        case vertex_pattern::direct:
            std::cout << "Using direct/index-based storage\n";
            std::cout << "Vertex ID type: size_t\n";
            std::cout << "Supports: O(1) random access\n";
            break;
        case vertex_pattern::keyed:
            using IDType = vertex_id_type_t<VertexIter>;
            std::cout << "Using keyed/map-based storage\n";
            std::cout << "Vertex ID type: " << typeid(IDType).name() << "\n";
            std::cout << "Supports: Arbitrary key types\n";
            break;
    }
}
```

## Design Rationale

1. **Mutual Exclusivity**: Each vertex iterator matches exactly one pattern, preventing ambiguity.

2. **Clear Semantics**:
   - **Direct**: Index-based, O(1) access, size_t IDs
   - **Keyed**: Map-based, flexible keys, O(log n) or O(1) access

3. **ID Type Extraction**: Automatically determines whether vertex IDs are `size_t` (direct) or the key type (keyed).

4. **Compile-Time Detection**: All concepts and traits are evaluated at compile-time, maintaining zero-cost abstraction.

5. **Extensibility**: The pattern system can accommodate future storage strategies without breaking existing code.

## Comparison with Edge Concepts

| Aspect | Vertex Concepts | Edge Concepts |
|--------|-----------------|---------------|
| **Patterns** | 2 (direct, keyed) | 4 (simple, pair, tuple, custom) |
| **What's Categorized** | Storage strategy | Value type structure |
| **ID Extraction** | Index or key | First element or whole value |
| **Primary Use** | Container choice | Edge property layout |

## Testing

Comprehensive tests are provided in `test_vertex_concepts.cpp`:
- ? 11 new tests covering all concepts and traits
- ? Pattern detection for vector and map iterators
- ? Mutual exclusivity verification
- ? Vertex ID type extraction
- ? Runtime pattern queries
- ? Const iterator support
- ? Integration with vertex_descriptor

## Example: Dual-Storage Graph

```cpp
template<typename VertexIter>
class Graph {
    // Vertex storage automatically adapts based on iterator type
    
    void add_vertex(/* args */) {
        if constexpr (direct_vertex_type<VertexIter>) {
            // Vector storage: just push_back
            vertices.push_back(vertex_data);
        } else if constexpr (keyed_vertex_type<VertexIter>) {
            // Map storage: insert with key
            vertices.insert({vertex_id, vertex_data});
        }
    }
    
    auto get_vertex_id(vertex_descriptor<VertexIter> vd) const {
        // vertex_id() automatically extracts index or key
        return vd.vertex_id();  // Type: vertex_id_type_t<VertexIter>
    }
};

// Usage:
Graph<std::vector<int>::iterator> vec_graph;      // Direct storage
Graph<std::map<int, int>::iterator> map_graph;    // Keyed storage
```

## Summary

These concepts and traits provide:
- **Storage Abstraction**: Uniform interface for vector and map storage
- **Type Safety**: Compile-time guarantees about vertex storage patterns
- **ID Type Deduction**: Automatic determination of vertex ID types
- **Clear Semantics**: Explicit naming distinguishes storage strategies
- **Zero Cost**: All checks happen at compile-time
- **Integration**: Works seamlessly with `vertex_descriptor` and `vertex_descriptor_view`
