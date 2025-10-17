# Graph Library

A modern C++20 graph library providing a comprehensive, customizable interface for graph data structures, algorithms, and views.

## Overview

This library provides the foundation for a complete graph library following the P1709 Graph Library proposal. It includes:

- **Descriptor System**: Type-safe, efficient descriptors for vertices and edges that abstract storage strategies
- **Graph Container Interface**: Standardized concepts, traits, and functions for adjacency lists and edgelists
- **Customization Point Objects (CPOs)**: All graph operations can be customized for existing data structures
- **Range-Based Design**: Graphs as ranges of vertices, where each vertex is a range of edges
- **Documentation**: Comprehensive documentation following P1709 conventions

**Current Status**: Phase 1 complete - Core infrastructure and documentation in place, ready for container and algorithm implementation.

## Features

### Core Descriptor System
- **Type-safe descriptors**: Vertex and edge descriptors are distinct types, preventing accidental misuse
- **Zero-cost abstraction**: No runtime overhead compared to raw indices/iterators
- **Storage flexibility**: Works with random-access containers (vector, deque) and associative containers (map, unordered_map)
- **Forward-only views**: Descriptor views provide range-based iteration over graph elements
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
  - Comprehensive unit tests (75 tests passing)
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

### 📋 Phase 2: Graph Utility CPOs (PLANNED)
- [ ] Implement core graph operation CPOs in `graph_utility.hpp`:
  - `vertex_id(g, v)` - Get vertex ID from descriptor
  - `num_vertices(g)` - Get vertex count
  - `num_edges(g)` - Get edge count
  - `vertices(g)` - Get vertex range
  - `edges(g)` - Get all edges range
  - `out_edges(g, v)` - Get outgoing edges from vertex
- [ ] Unit tests for each CPO
- [ ] Documentation for CPO usage

### 📋 Phase 3: First Container Implementation (PLANNED)
- [ ] `adjacency_list.hpp` in `include/graph/container/`:
  - Dynamic adjacency list representation
  - Uses descriptors for vertices and edges
  - CPO customizations
  - Comprehensive unit tests
  - Usage examples

### 📋 Phase 4: Basic Algorithms (PLANNED)
- [ ] Foundational algorithms in `include/graph/algorithm/`:
  - `breadth_first_search.hpp` - BFS traversal
  - `depth_first_search.hpp` - DFS traversal
  - `dijkstra_shortest_paths.hpp` - Single-source shortest paths
  - `topological_sort.hpp` - Topological ordering

### 📋 Phase 5: Views and Adaptors (PLANNED)
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
├── tests/                  # Unit tests (75 tests, all passing)
│   ├── test_descriptor_traits.cpp
│   ├── test_edge_concepts.cpp
│   ├── test_edge_descriptor.cpp
│   ├── test_vertex_concepts.cpp
│   └── test_vertex_descriptor.cpp
├── CMakeLists.txt         # Root CMake configuration
├── CMakePresets.json      # CMake build presets
├── MIGRATION_PHASE1.md    # Phase 1 migration details
├── PHASE1_COMPLETE.md     # Phase 1 completion report
└── README.md              # This file
```

## Design Principles

### 1. CPO-Based Interface
All graph operations are customization point objects (CPOs) following MSVC standard library style:
- Member functions (highest priority)
- ADL-findable free functions (medium priority)  
- Default implementations (lowest priority)

This allows adaptation of existing graph data structures without modification.

### 2. Outgoing Edge Focus
The interface focuses on outgoing edges from vertices. If a graph exposes incoming edges, a separate view presents them through the same outgoing-edge interface for consistency.

### 3. Range-of-Ranges Model
Adjacency lists are treated as ranges of vertices, where each vertex is a range of edges. This enables use of standard algorithms and range adaptors.

### 4. Descriptor Abstraction
Descriptors are opaque objects that abstract implementation details:
- Random-access containers: descriptors are integral indices
- Bidirectional containers: descriptors are iterators
- Provides uniform interface regardless of storage strategy

### 5. Zero-Cost Abstraction
Descriptors compile down to simple index or iterator operations with no runtime overhead.

### 6. Type Safety
Different descriptor types cannot be accidentally mixed:
```cpp
using VectorDesc = graph::vertex_descriptor<std::vector<int>::iterator>;
using MapDesc = graph::vertex_descriptor<std::map<int,int>::iterator>;
// These are distinct types - cannot be assigned to each other
```

## Documentation

### Main Documentation Files
- **[docs/common_graph_guidelines.md](docs/common_graph_guidelines.md)**: Complete naming conventions, architectural commitments, and project structure requirements
- **[docs/container_interface.md](docs/container_interface.md)**: Comprehensive summary of the Graph Container Interface (adjacency lists and edgelists)
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

The project includes 75 unit tests covering all descriptor functionality:

```bash
# Run all tests
ctest --test-dir build/linux-gcc-debug --output-on-failure

# Run specific test
ctest --test-dir build/linux-gcc-debug -R vertex_descriptor

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

**Status**: Phase 1 Complete ✅ | 75/75 Tests Passing ✅ | Ready for Phase 2 Development 🚀
