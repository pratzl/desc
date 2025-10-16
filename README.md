# Graph Descriptors

A modern C++20 library providing type-safe, efficient descriptors for vertices and edges in graph data structures.

## Overview

This library implements descriptors as lightweight, copyable handles for graph elements. Descriptors abstract away the underlying storage strategy, allowing graphs to use various container types (vectors, maps, custom containers) while maintaining a consistent interface.

## Features

- **Type-safe descriptors**: Vertex and edge descriptors are distinct types, preventing accidental misuse
- **Zero-cost abstraction**: No runtime overhead compared to raw indices/iterators
- **Storage flexibility**: Works with random-access containers (vector, deque) and associative containers (map, unordered_map)
- **C++20 concepts**: Strong compile-time guarantees using concepts
- **STL compatibility**: Descriptors work seamlessly with standard containers and algorithms
- **Forward-only views**: Descriptor views provide range-based iteration over graph elements

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

### Vertex Descriptors with Vector Storage

```cpp
#include <desc/vertex_descriptor.hpp>
#include <desc/vertex_descriptor_view.hpp>
#include <vector>

std::vector<int> vertices = {10, 20, 30, 40, 50};

using VectorIter = std::vector<int>::iterator;
using VD = desc::vertex_descriptor<VectorIter>;

// Create descriptor for vertex at index 2
VD vd{2};
std::cout << "Vertex ID: " << vd.vertex_id() << "\n";  // Output: 2

// Iterate over all vertices
desc::vertex_descriptor_view view{vertices};
for (auto desc : view) {
    std::cout << "Vertex " << desc.vertex_id() << "\n";
}
```

### Vertex Descriptors with Map Storage

```cpp
#include <desc/vertex_descriptor.hpp>
#include <desc/vertex_descriptor_view.hpp>
#include <map>

std::map<int, std::string> vertex_map = {
    {100, "Node A"},
    {200, "Node B"},
    {300, "Node C"}
};

using MapIter = std::map<int, std::string>::iterator;
using VD = desc::vertex_descriptor<MapIter>;

// Create descriptor from iterator
auto it = vertex_map.find(200);
VD vd{it};
std::cout << "Vertex ID: " << vd.vertex_id() << "\n";  // Output: 200

// Iterate over all vertices
desc::vertex_descriptor_view map_view{vertex_map};
for (auto desc : map_view) {
    std::cout << "Vertex " << desc.vertex_id() << "\n";
}
```

## Implementation Status

- [x] **Phase 1: Foundation** (COMPLETE)
  - CMake build configuration
  - Catch2 integration
  - Descriptor concepts
  - Vertex descriptor template
  - Vertex descriptor view
  - Comprehensive unit tests

- [ ] **Phase 2: Edge Descriptors** (TODO)
  - Edge descriptor template
  - Edge descriptor view
  - Unit tests

- [ ] **Phase 3: Advanced Features** (TODO)
  - Descriptor traits
  - Property maps
  - Performance benchmarks

## Design Principles

### Zero-Cost Abstraction
Descriptors compile down to simple index or iterator operations with no runtime overhead.

### Type Safety
Different descriptor types cannot be accidentally mixed:
```cpp
using VectorDesc = vertex_descriptor<std::vector<int>::iterator>;
using MapDesc = vertex_descriptor<std::map<int,int>::iterator>;
// These are distinct types - cannot be assigned to each other
```

### Forward-Only Views
Descriptor views provide forward iteration only because descriptors are synthesized on-the-fly. This design:
- Works uniformly across all storage strategies
- Prevents invalid references to temporary descriptors
- Maintains compatibility with range-based algorithms

## Documentation

See [descriptor.md](descriptor.md) for the complete specification following RFC 2119 requirements.

## License

[License information to be added]

## Contributing

[Contributing guidelines to be added]
