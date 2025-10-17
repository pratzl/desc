# Common Graph Guidelines

These requirements apply to every graph-related instruction set, including implementations, views, algorithms, and the data model.

## Core Architectural Commitments
1. The interface MUST be able to adapt to existing graph data structures.
2. The interface focuses on outgoing edges. If the underlying graph exposes incoming edges, a separate view will present them through this outgoing-edge interface for consistency.
3. Graph, vertex, and edge functions are customization point objects (CPOs) so they can be overridden for a graph data structure.

## Implementation Requirements
- MUST use C++20.
- MUST support Windows, Linux, and macOS.
- MUST support the primary compiler for each OS; MAY support additional compilers per OS.

## Naming Conventions

| Type Name | Variable Name       | Description |
|-----------|---------------------|-------------|
| `G`       | `g`                 | A graph object. |
| `GV`      | `gval`              | Optional graph value. |
| `V`       | `u`, `v`, `w`       | A vertex descriptor. `u` is the source (or only) vertex. `v` is the target vertex. `w` may be used when another vertex may be involved in a function or other context. |
| `VId`     | `uid`, `vid`, `wid` | Vertex Id. `uid` is the source (or only) vertex id. `vid` is the target vertex id. `wid` may be used when another vertex may be involved in a function or other context. |
| `VV`      | `uval`, `vval`      | Optional vertex value. |
| `VVF`     | `vvf`               | A Vertex Value Function. It is a function object that returns a `VV` which MAY be a value type or a reference type. |
| `E`       | `uv`, `vw`, `uw`    | An edge descriptor. Examples: `uv` represents an edge from vertex `u` to vertex `v`. |
| `EV`      | `eval`              | Optional edge value. |
| `EVF`     | `evf`               | An Edge Value Function. It is a function object that returns a `EV` which MAY be a value type or a reference type. |

## Parameterized Type Aliases
Types use the `_t<G>` suffix to indicate they are parameterized by graph type `G`:
- `vertex_t<G>` - Vertex descriptor type (corresponds to `V`)
- `edge_t<G>` - Edge descriptor type (corresponds to `E`)
- `vertex_id_t<G>`, `vertex_value_t<G>`, `edge_value_t<G>`, etc. - Other derived types
- `vertex_iterator_t<G>` - Iterator type for traversing vertices
- `vertex_range_t<G>` - Range type that enumerates vertices
- `vertex_edge_range_t<G>` - Range type for the outgoing edges of a vertex
- `edge_iterator_t<G>` - Iterator type for traversing edges

## Public Interface Requirements
All public functions for the graph data model have the following requirements:
1. All implementations MUST be customization point objects (CPO) and MUST follow the style used by the MSVC standard library implementation.
2. The return types of the functions MUST be used to define parameterized types.
3. Default implementations MUST be provided whenever possible in the CPO.
4. Each CPO function MAY be overridden for each graph type, vertex type and edge type.

When creating a new instruction document, reference this file near the top and only document the requirements unique to that artifact.

## Project Structure

The implementation MUST follow this directory structure (based on graph-v2):

```
graph-v3/
├── benchmark/              # Performance benchmarks
│   └── CMakeLists.txt
├── cmake/                  # CMake modules and scripts
├── data/                   # Test data files
├── doc/                    # Documentation (Doxygen output)
├── docs/                   # Documentation source
│   └── sphinx/
├── example/                # Example programs
│   ├── CMakeLists.txt
│   ├── AdaptingThirdPartyGraph/
│   ├── CppCon2021/
│   ├── CppCon2022/
│   └── PageRank/
├── include/                # Public header files
│   └── graph/
│       ├── algorithm/      # Graph algorithms
│       ├── container/      # Graph container implementations
│       ├── detail/         # Implementation details (private)
│       ├── views/          # Graph views
│       ├── edgelist.hpp    # Edge list functionality
│       ├── graph.hpp       # Main graph header
│       ├── graph_info.hpp  # Graph information structures
│       └── graph_utility.hpp # Utility functions
├── scripts/                # Build and maintenance scripts
│   └── format.sh
├── tests/                  # Unit tests
│   ├── CMakeLists.txt
│   ├── catch_main.cpp      # Catch2 main
│   ├── *_tests.cpp         # Test files for each component
│   ├── csv.hpp             # Test utilities
│   ├── csv_routes.cpp
│   ├── csv_routes.hpp
│   ├── examples.cpp
│   └── tests.cpp
├── .clang-format           # Code formatting rules
├── .clang-tidy             # Static analysis rules
├── .cmake-format.yaml      # CMake formatting rules
├── .gitignore              # Git ignore patterns
├── CMakeLists.txt          # Root CMake configuration
├── CMakePresets.json       # CMake presets for build configurations
├── CODE_OF_CONDUCT.md      # Code of conduct
├── CONTRIBUTING.md         # Contribution guidelines
├── LICENSE                 # License file
├── README.md               # Project readme
├── Notes.md                # Development notes
└── ToDo.md                 # TODO list
```

**Key Directories**:
- `include/graph/`: All public headers implementing the graph data model interface
- `tests/`: Comprehensive unit tests for all functionality
- `example/`: Example programs demonstrating usage patterns
- `benchmark/`: Performance benchmarks and comparison tests
- `cmake/`: Modular CMake configuration for dependencies and build settings
- `data/`: Test data in various formats (Matrix Market, CSV)

**Note**: The `out/` directory (generated build artifacts) is excluded from version control and documentation.
