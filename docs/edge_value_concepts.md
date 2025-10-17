# Edge Value Type Concepts and Pattern Detection

## Overview

This document describes the concepts and type traits added to `descriptor.hpp` for detecting and categorizing edge value types used in graph descriptors.

## Concepts

### Edge Value Type Patterns

The library defines four mutually exclusive concepts for edge value patterns:

#### 1. `simple_edge_type<T>`
Identifies simple integral edge types where the value itself is the target vertex ID.

**Example:**
```cpp
std::vector<int> edges = {1, 2, 3, 4};  // Each int is a target vertex ID
static_assert(simple_edge_type<int>);
```

#### 2. `pair_edge_type<T>`
Identifies pair-like edge types with `.first` (target ID) and `.second` (properties).

**Example:**
```cpp
std::vector<std::pair<int, double>> edges = {
    {1, 1.5},  // Target: 1, Weight: 1.5
    {2, 2.5}   // Target: 2, Weight: 2.5
};
static_assert(pair_edge_type<std::pair<int, double>>);
```

#### 3. `tuple_edge_type<T>`
Identifies tuple-like edge types where the first element is the target ID.

**Example:**
```cpp
std::vector<std::tuple<int, double, std::string>> edges = {
    {1, 1.5, "road"},  // Target: 1, Weight: 1.5, Type: "road"
    {2, 2.5, "rail"}   // Target: 2, Weight: 2.5, Type: "rail"
};
static_assert(tuple_edge_type<std::tuple<int, double, std::string>>);
```

#### 4. `custom_edge_type<T>`
Identifies custom struct/class edge types (fallback pattern).

**Example:**
```cpp
struct CustomEdge {
    int target;
    double weight;
    std::string label;
};
static_assert(custom_edge_type<CustomEdge>);
```

### Comprehensive Concept

#### `edge_value_type<T>`
Accepts any valid edge value type (matches at least one of the above patterns).

**Example:**
```cpp
static_assert(edge_value_type<int>);
static_assert(edge_value_type<std::pair<int, double>>);
static_assert(edge_value_type<std::tuple<int, double>>);
static_assert(edge_value_type<CustomEdge>);
```

## Type Traits

### Pattern Detection Struct

#### `edge_value_pattern<T>`
Provides boolean flags for each pattern:

```cpp
template<typename T>
struct edge_value_pattern {
    static constexpr bool is_simple;
    static constexpr bool is_pair;
    static constexpr bool is_tuple;
    static constexpr bool is_custom;
};
```

**Usage:**
```cpp
using Pattern = edge_value_pattern<std::pair<int, double>>;
static_assert(Pattern::is_pair);
static_assert(!Pattern::is_simple);
```

### Pattern Detection Variable Template

#### `edge_value_pattern_v<T>`
Convenient variable template for pattern detection:

```cpp
auto pattern = edge_value_pattern_v<std::tuple<int, double>>;
if (pattern.is_tuple) {
    // Handle tuple edge type
}
```

### Pattern Enum

#### `edge_pattern` Enumeration
Represents the four edge patterns:

```cpp
enum class edge_pattern {
    simple,   ///< Simple integral type
    pair,     ///< Pair-like with .first/.second
    tuple,    ///< Tuple-like with std::get<N>
    custom    ///< Custom struct/class
};
```

#### `edge_pattern_type_v<T>`
Returns the pattern enum value for a type:

```cpp
static_assert(edge_pattern_type_v<int> == edge_pattern::simple);
static_assert(edge_pattern_type_v<std::pair<int, double>> == edge_pattern::pair);
static_assert(edge_pattern_type_v<std::tuple<int, double>> == edge_pattern::tuple);
static_assert(edge_pattern_type_v<CustomEdge> == edge_pattern::custom);
```

## Pattern Matching in target_id()

The `target_id()` function in `edge_descriptor` uses these patterns to extract the target vertex ID:

```cpp
template<typename EdgeContainer>
constexpr auto target_id(const EdgeContainer& edges) const noexcept {
    using edge_value_type = typename std::iterator_traits<EdgeIter>::value_type;
    
    const auto& edge_value = /* get edge from container */;
    
    if constexpr (std::integral<edge_value_type>) {
        return edge_value;  // Simple: value is target ID
    }
    else if constexpr (requires { edge_value.first; }) {
        return edge_value.first;  // Pair: .first is target ID
    }
    else if constexpr (requires { std::get<0>(edge_value); }) {
        return std::get<0>(edge_value);  // Tuple: first element is target ID
    }
    else {
        return edge_value;  // Custom: return whole value
    }
}
```

## Use Cases

### 1. Compile-Time Type Checking

```cpp
template<typename EdgeType>
void process_edges() {
    static_assert(edge_value_type<EdgeType>, "Invalid edge type");
    
    if constexpr (simple_edge_type<EdgeType>) {
        // Handle simple integer edges
    } else if constexpr (pair_edge_type<EdgeType>) {
        // Handle pair edges with properties
    } else if constexpr (tuple_edge_type<EdgeType>) {
        // Handle tuple edges with multiple properties
    } else {
        // Handle custom edge types
    }
}
```

### 2. Generic Edge Processing

```cpp
template<typename EdgeIter>
void analyze_edge_structure() {
    using EdgeType = typename std::iterator_traits<EdgeIter>::value_type;
    
    constexpr auto pattern = edge_pattern_type_v<EdgeType>;
    
    std::cout << "Edge pattern: ";
    switch (pattern) {
        case edge_pattern::simple:
            std::cout << "simple (target ID only)\n";
            break;
        case edge_pattern::pair:
            std::cout << "pair (target + property)\n";
            break;
        case edge_pattern::tuple:
            std::cout << "tuple (target + multiple properties)\n";
            break;
        case edge_pattern::custom:
            std::cout << "custom struct/class\n";
            break;
    }
}
```

### 3. Constraining Function Templates

```cpp
// Function that only accepts pair-like edges
template<typename EdgeType>
    requires pair_edge_type<EdgeType>
auto get_edge_weight(const EdgeType& edge) {
    return edge.second;
}

// Function that only accepts edges with properties
template<typename EdgeType>
    requires (!simple_edge_type<EdgeType>)
void process_weighted_edge(const EdgeType& edge) {
    // Edge has properties beyond just target ID
}
```

## Design Rationale

1. **Mutual Exclusivity**: Each edge type matches exactly one pattern, preventing ambiguity.

2. **Priority Order**: The pattern matching follows a specific order:
   - Simple (integral) ? Pair (.first/.second) ? Tuple (std::get) ? Custom (fallback)

3. **Compile-Time Detection**: All concepts and traits are evaluated at compile-time, maintaining zero-cost abstraction.

4. **Extensibility**: The pattern system can be extended with new patterns without breaking existing code.

## Testing

Comprehensive tests are provided in `test_edge_concepts.cpp`:
- ? 64 tests covering all concepts and traits
- ? Pattern detection for all edge types
- ? Mutual exclusivity verification
- ? Runtime pattern queries
- ? Type safety guarantees

## Summary

These concepts and traits provide:
- **Type Safety**: Compile-time guarantees about edge value types
- **Clear Intent**: Explicit naming of edge patterns
- **Easy Detection**: Simple APIs for pattern matching
- **Zero Cost**: All checks happen at compile-time
- **Future Proof**: Extensible design for new patterns
