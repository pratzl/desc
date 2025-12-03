# Dynamic Graph Traits Refactoring

## Problem
The `dynamic_graph.hpp` header included all STL container headers (`<vector>`, `<deque>`, `<list>`, `<forward_list>`), creating unnecessary compilation overhead for users who only need one container combination.

## Solution
Separated trait definitions into individual header files in `include/graph/container/traits/`. Each traits header only includes the containers it needs.

## New Structure

```
include/graph/container/traits/
├── vofl_graph_traits.hpp  // vector + forward_list (includes: vector, forward_list)
├── vol_graph_traits.hpp   // vector + list (includes: vector, list)
├── vov_graph_traits.hpp   // vector + vector (includes: vector only)
├── vod_graph_traits.hpp   // vector + deque (includes: vector, deque)
├── dofl_graph_traits.hpp  // deque + forward_list (includes: deque, forward_list)
├── dol_graph_traits.hpp   // deque + list (includes: deque, list)
├── dov_graph_traits.hpp   // deque + vector (includes: deque, vector)
└── dod_graph_traits.hpp   // deque + deque (includes: deque only)
```

## Usage

### Before (heavy includes):
```cpp
#include <graph/container/dynamic_graph.hpp>
using Graph = graph::container::dynamic_graph<int, int, void, uint32_t, false, 
                                               graph::container::vofl_graph_traits<int, int, void, uint32_t, false>>;
```

### After (lightweight, only what you need):
```cpp
#include <graph/container/traits/vofl_graph_traits.hpp>  // Only includes <vector> and <forward_list>
#include <graph/container/dynamic_graph.hpp>
using Graph = graph::container::dynamic_graph<int, int, void, uint32_t, false, 
                                               graph::container::vofl_graph_traits<int, int, void, uint32_t, false>>;
```

Or use the type alias:
```cpp
#include <graph/container/traits/vofl_graph_traits.hpp>
#include <graph/container/dynamic_graph.hpp>
using Graph = graph::container::dynamic_adjacency_graph<
                  graph::container::vofl_graph_traits<int, int, void, uint32_t, false>>;
```

## Migration Steps

**TODO**: Update `dynamic_graph.hpp` to remove trait struct definitions (lines 88-251) while keeping forward declarations. The trait definitions have been moved to individual headers.

### Files to Update:
1. `include/graph/container/dynamic_graph.hpp` - Remove lines 88-251 (trait definitions), keep forward declarations
2. All test files - Add appropriate `#include <graph/container/traits/*.hpp>` at the top
3. All example files - Add appropriate traits includes

### Test File Updates Required:
Each test file needs to include its specific traits header:
- `test_dynamic_graph_vofl.cpp` → `#include <graph/container/traits/vofl_graph_traits.hpp>`
- `test_dynamic_graph_vol.cpp` → `#include <graph/container/traits/vol_graph_traits.hpp>`
- `test_dynamic_graph_vov.cpp` → `#include <graph/container/traits/vov_graph_traits.hpp>`
- `test_dynamic_graph_vod.cpp` → `#include <graph/container/traits/vod_graph_traits.hpp>`
- `test_dynamic_graph_dofl.cpp` → `#include <graph/container/traits/dofl_graph_traits.hpp>`
- `test_dynamic_graph_dol.cpp` → `#include <graph/container/traits/dol_graph_traits.hpp>`
- `test_dynamic_graph_dov.cpp` → `#include <graph/container/traits/dov_graph_traits.hpp>`
- `test_dynamic_graph_dod.cpp` → `#include <graph/container/traits/dod_graph_traits.hpp>`
- (Same pattern for all CPO test files)

## Benefits
- **Faster compilation**: Only include containers you actually use
- **Clear dependencies**: Each traits file explicitly shows what containers it needs
- **Better modularity**: Traits can evolve independently
- **Future extensibility**: Easy to add new container combinations (e.g., map-based traits)

## Compilation Impact
- Before: Every file including `dynamic_graph.hpp` compiled `<vector>`, `<deque>`, `<list>`, `<forward_list>`
- After: Only compile the 1-2 container headers you actually need
- Expected: 20-40% faster compilation for files using dynamic_graph
