#include <catch2/catch_test_macros.hpp>
#include "graph/container/compressed_graph.hpp"
#include <string>
#include <vector>
#include <numeric>  // for std::accumulate

using namespace std;
using namespace graph;
using namespace graph::container;

// =============================================================================
// Category 1: Basic Value Type Tests
// =============================================================================

TEST_CASE("compressed_graph default constructor", "[constructors]") {
    SECTION("void/void/void") {
        compressed_graph<void, void, void> g;
        REQUIRE(g.empty());
        REQUIRE(g.size() == 0);
    }
    
    SECTION("int/int/int") {
        compressed_graph<int, int, int> g(42);
        REQUIRE(g.empty());
        REQUIRE(g.value() == 42);
    }
    
    SECTION("string/string/string") {
        compressed_graph<string, string, string> g(string("test"));
        REQUIRE(g.empty());
        REQUIRE(g.value() == "test");
    }
}

TEST_CASE("compressed_graph load_edges with void edge values", "[api][edges]") {
    compressed_graph<void, int, void> g;
    
    vector<copyable_edge_t<int, void>> edges = {{0, 1}, {0, 2}, {1, 2}};
    g.load_edges(edges);
    
    REQUIRE_FALSE(g.empty());
    REQUIRE(g.size() >= 2); // At least 2 vertices
}

TEST_CASE("compressed_graph load_edges with int edge values", "[api][edges]") {
    compressed_graph<int, int, void> g;
    
    vector<copyable_edge_t<int, int>> edges = {{0, 1, 10}, {0, 2, 20}, {1, 2, 30}};
    g.load_edges(edges);
    
    REQUIRE_FALSE(g.empty());
    REQUIRE(g.size() >= 2);
}

TEST_CASE("compressed_graph load_edges with string edge values", "[api][edges]") {
    compressed_graph<string, int, void> g;
    
    vector<copyable_edge_t<int, string>> edges = {
        {0, 1, string("a")}, 
        {0, 2, string("b")}, 
        {1, 2, string("c")}
    };
    g.load_edges(edges);
    
    REQUIRE_FALSE(g.empty());
}

TEST_CASE("compressed_graph load_vertices with void vertex values", "[api][vertices]") {
    compressed_graph<void, void, void> g;
    
    vector<copyable_vertex_t<int, void>> vertices = {{0}, {1}, {2}};
    g.load_vertices(vertices);
    
    REQUIRE(g.size() == 3);
}

TEST_CASE("compressed_graph load_vertices with int vertex values", "[api][vertices]") {
    compressed_graph<void, int, void> g;
    
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}, {2, 300}};
    g.load_vertices(vertices);
    
    REQUIRE(g.size() == 3);
}

TEST_CASE("compressed_graph load_vertices with string vertex values", "[api][vertices]") {
    compressed_graph<void, string, void> g;
    
    vector<copyable_vertex_t<int, string>> vertices = {
        {0, string("v0")}, 
        {1, string("v1")}, 
        {2, string("v2")}
    };
    g.load_vertices(vertices);
    
    REQUIRE(g.size() == 3);
}

// =============================================================================
// Issue #4 Fix Tests: load_vertices with void vertex values
// =============================================================================

TEST_CASE("load_vertices with void VV after load_edges", "[issue4][load_vertices][void]") {
    compressed_graph<int, void, void> g;
    
    // Load edges first - creates vertices implicitly
    vector<copyable_edge_t<int, int>> edges = {{0, 1, 10}, {0, 2, 20}, {1, 2, 30}};
    g.load_edges(edges);
    
    // Now load void vertices - should be no-op but not crash
    vector<copyable_vertex_t<int, void>> vertices = {{0}, {1}, {2}};
    g.load_vertices(vertices);
    
    REQUIRE(g.size() == 3);
}

TEST_CASE("load_vertices with void VV before load_edges", "[issue4][load_vertices][void]") {
    compressed_graph<int, void, void> g;
    
    // Load edges first to create graph structure
    vector<copyable_edge_t<int, int>> edges = {{0, 1, 10}, {1, 2, 20}};
    g.load_edges(edges);
    
    REQUIRE(g.size() == 3);
    
    // Load void vertices - no-op since VV is void
    vector<copyable_vertex_t<int, void>> vertices = {{0}, {1}, {2}};
    g.load_vertices(vertices);
    
    REQUIRE(g.size() == 3);
}

TEST_CASE("load_vertices with void VV on empty graph", "[issue4][load_vertices][void]") {
    compressed_graph<void, void, void> g;
    
    // Load void vertices on empty graph - no-op since VV is void
    vector<copyable_vertex_t<int, void>> vertices = {{0}, {1}, {2}, {3}, {4}};
    g.load_vertices(vertices);
    
    REQUIRE(g.size() == 5);
}

TEST_CASE("load_vertices void VV with rvalue reference", "[issue4][load_vertices][void]") {
    compressed_graph<int, void, void> g;
    
    // Test rvalue reference overload of load_row_values
    g.load_vertices(vector<copyable_vertex_t<int, void>>{{0}, {1}, {2}});
    
    REQUIRE(g.size() == 3);
}

TEST_CASE("load_vertices void VV with custom projection", "[issue4][load_vertices][void]") {
    compressed_graph<void, void, void> g;
    
    // Test with custom projection function
    struct VertexData { int id; };
    vector<VertexData> data = {{0}, {1}, {2}};
    
    auto projection = [](const VertexData& vd) {
        return copyable_vertex_t<int, void>{vd.id};
    };
    
    g.load_vertices(data, projection);
    
    REQUIRE(g.size() == 3);
}

TEST_CASE("load_vertices void VV combined with non-void edges", "[issue4][load_vertices][void]") {
    compressed_graph<string, void, void> g;
    
    // Load edges with string values
    vector<copyable_edge_t<int, string>> edges = {
        {0, 1, string("edge1")},
        {1, 2, string("edge2")},
        {2, 3, string("edge3")}
    };
    g.load_edges(edges);
    
    // Load void vertices
    vector<copyable_vertex_t<int, void>> vertices = {{0}, {1}, {2}, {3}};
    g.load_vertices(vertices);
    
    REQUIRE(g.size() == 4);
}

TEST_CASE("load_vertices void VV with explicit vertex count", "[issue4][load_vertices][void]") {
    compressed_graph<void, void, void> g;
    
    vector<copyable_vertex_t<int, void>> vertices = {{0}, {1}};
    
    // Provide explicit vertex count larger than range size
    g.load_vertices(vertices, identity(), 5);
    
    // Should use explicit vertex count (5) as it's larger than max_id + 1 (2)
    REQUIRE(g.size() == 5);
}

// =============================================================================
// Category 2: VId and EIndex Type Variations
// =============================================================================

TEST_CASE("compressed_graph with uint32_t VId", "[types][vid]") {
    compressed_graph<void, void, void, uint32_t, uint32_t> g;
    
    vector<copyable_edge_t<uint32_t, void>> edges = {{0, 1}, {1, 2}};
    g.load_edges(edges);
    
    REQUIRE_FALSE(g.empty());
}

TEST_CASE("compressed_graph with int32_t VId", "[types][vid]") {
    compressed_graph<void, void, void, int32_t, int32_t> g;
    
    vector<copyable_edge_t<int32_t, void>> edges = {{0, 1}, {1, 2}};
    g.load_edges(edges);
    
    REQUIRE_FALSE(g.empty());
}

TEST_CASE("compressed_graph with uint64_t VId", "[types][vid]") {
    compressed_graph<void, void, void, uint64_t, uint64_t> g;
    
    vector<copyable_edge_t<uint64_t, void>> edges = {{0, 1}, {1, 2}};
    g.load_edges(edges);
    
    REQUIRE_FALSE(g.empty());
}

TEST_CASE("compressed_graph with int8_t VId", "[types][vid]") {
    compressed_graph<void, void, void, int8_t, int8_t> g;
    
    vector<copyable_edge_t<int8_t, void>> edges = {{0, 1}, {1, 2}};
    g.load_edges(edges);
    
    REQUIRE_FALSE(g.empty());
}

TEST_CASE("compressed_graph with mixed signed/unsigned types", "[types][vid]") {
    compressed_graph<void, void, void, int32_t, uint32_t> g;
    
    vector<copyable_edge_t<int32_t, void>> edges = {{0, 1}, {1, 2}};
    g.load_edges(edges);
    
    REQUIRE_FALSE(g.empty());
}

// =============================================================================
// Category 3: Constructor Tests
// =============================================================================

TEST_CASE("compressed_graph default constructor creates empty graph", "[constructors]") {
    compressed_graph<void, void, void> g;
    
    REQUIRE(g.empty());
    REQUIRE(g.size() == 0);
}

TEST_CASE("compressed_graph constructor with graph value", "[constructors][graph_value]") {
    SECTION("int graph value") {
        compressed_graph<void, void, int> g(42);
        REQUIRE(g.value() == 42);
        REQUIRE(g.empty());
    }
    
    SECTION("string graph value") {
        compressed_graph<void, void, string> g(string("graph1"));
        REQUIRE(g.value() == "graph1");
        REQUIRE(g.empty());
    }
}

TEST_CASE("compressed_graph copy constructor", "[constructors][copy]") {
    compressed_graph<int, int, int> g1(100);
    vector<copyable_edge_t<int, int>> edges = {{0, 1, 10}, {1, 2, 20}};
    g1.load_edges(edges);
    
    compressed_graph<int, int, int> g2(g1);
    
    REQUIRE(g2.value() == 100);
    REQUIRE(g2.size() == g1.size());
    REQUIRE_FALSE(g2.empty());
}

TEST_CASE("compressed_graph move constructor", "[constructors][move]") {
    compressed_graph<int, int, int> g1(100);
    vector<copyable_edge_t<int, int>> edges = {{0, 1, 10}, {1, 2, 20}};
    g1.load_edges(edges);
    
    auto original_size = g1.size();
    compressed_graph<int, int, int> g2(std::move(g1));
    
    REQUIRE(g2.value() == 100);
    REQUIRE(g2.size() == original_size);
}

// =============================================================================
// Category 4: Public API Tests
// =============================================================================

TEST_CASE("compressed_graph reserve allocates space", "[api][reserve]") {
    compressed_graph<void, void, void> g;
    g.reserve(200, 100);
    
    REQUIRE(g.empty());
    REQUIRE(g.size() == 0);
}

TEST_CASE("compressed_graph find_vertex locates vertices", "[api][find_vertex]") {
    compressed_graph<void, int, void> g;
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {5, 500}, {10, 1000}};
    g.load_vertices(vertices);
    
    auto it = g.find_vertex(5);
    REQUIRE(it != g.vertices().end());
    // Verify we can dereference the iterator (it points to a valid vertex)
    [[maybe_unused]] auto& vertex = *it;
}

TEST_CASE("compressed_graph find_vertex returns end for missing vertices", "[api][find_vertex]") {
    compressed_graph<void, int, void> g;
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {5, 500}};
    g.load_vertices(vertices);
    
    auto it = g.find_vertex(999);
    REQUIRE(it == g.vertices().end());
}

TEST_CASE("compressed_graph index_of returns correct index", "[api][index_of]") {
    compressed_graph<void, int, void> g;
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {5, 500}, {10, 1000}};
    g.load_vertices(vertices);
    
    // index_of takes a reference to a vertex/row object, not an ID
    auto& vertex_row = g[5];
    auto idx = g.index_of(vertex_row);
    REQUIRE(idx == 5);
}

TEST_CASE("compressed_graph operator subscript accesses vertices by index", "[api][operator_subscript]") {
    compressed_graph<void, int, void> g;
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {5, 500}, {10, 1000}};
    g.load_vertices(vertices);
    
    // operator[] with vertex id 5 gives us the vertex at position 5
    [[maybe_unused]] auto& v = g[5];
    // csr_row doesn't have id() - the id is implicit from the array position
    REQUIRE(g.size() > 5);
}

// =============================================================================
// Category 5: Boundary Condition Tests
// =============================================================================

TEST_CASE("compressed_graph handles empty graph", "[boundary][empty]") {
    compressed_graph<void, void, void> g;
    
    REQUIRE(g.empty());
    REQUIRE(g.size() == 0);
    
    // Test vertices() on empty graph
    size_t count = 0;
    for ([[maybe_unused]] auto& v : g.vertices()) {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE("compressed_graph handles single vertex", "[boundary][single]") {
    compressed_graph<void, int, void> g;
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}};
    g.load_vertices(vertices);
    
    REQUIRE(g.size() == 1);
    REQUIRE_FALSE(g.empty());
}

TEST_CASE("compressed_graph handles single edge", "[boundary][single]") {
    compressed_graph<void, int, void> g;
    vector<copyable_edge_t<int, void>> edges = {{0, 1}};
    g.load_edges(edges);
    
    REQUIRE_FALSE(g.empty());
    REQUIRE(g.size() >= 2);
}

TEST_CASE("compressed_graph handles vertices with no edges", "[boundary][isolated]") {
    compressed_graph<void, int, void> g;
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}, {2, 300}};
    g.load_vertices(vertices);
    
    REQUIRE(g.size() == 3);
}

TEST_CASE("compressed_graph handles large vertex IDs", "[boundary][large_ids]") {
    compressed_graph<void, void, void, uint64_t, uint64_t> g;
    vector<copyable_edge_t<uint64_t, void>> edges = {
        {0, 1000000}, 
        {1000000, 2000000}
    };
    g.load_edges(edges);
    
    REQUIRE_FALSE(g.empty());
}

// =============================================================================
// Category 6: Const-Correctness Tests
// =============================================================================

TEST_CASE("compressed_graph const methods work correctly", "[const][api]") {
    compressed_graph<void, int, void> g;
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}};
    g.load_vertices(vertices);
    
    const auto& cg = g;
    
    REQUIRE_FALSE(cg.empty());
    REQUIRE(cg.size() == 2);
    
    // Test const vertices()
    size_t count = 0;
    for ([[maybe_unused]] const auto& v : cg.vertices()) {
        ++count;
    }
    REQUIRE(count == 2);
}

TEST_CASE("compressed_graph const find_vertex", "[const][find_vertex]") {
    compressed_graph<void, int, void> g;
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {5, 500}};
    g.load_vertices(vertices);
    
    const auto& cg = g;
    auto it = cg.find_vertex(5);
    
    REQUIRE(it != cg.vertices().end());
    // Verify we can dereference the iterator
    [[maybe_unused]] const auto& vertex = *it;
}

TEST_CASE("compressed_graph const operator subscript", "[const][operator_subscript]") {
    compressed_graph<void, int, void> g;
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {5, 500}};
    g.load_vertices(vertices);
    
    const auto& cg = g;
    [[maybe_unused]] const auto& v = cg[5];
    
    // Verify the graph has the expected size
    REQUIRE(cg.size() > 5);
}

// =============================================================================
// Category 7: Additional Tests
// =============================================================================

TEST_CASE("compressed_graph copy assignment", "[copy][assignment]") {
    compressed_graph<int, int, int> g1(100);
    vector<copyable_edge_t<int, int>> edges = {{0, 1, 10}};
    g1.load_edges(edges);
    
    compressed_graph<int, int, int> g2(200);
    g2 = g1;
    
    REQUIRE(g2.value() == 100);
    REQUIRE(g2.size() == g1.size());
}

TEST_CASE("compressed_graph move assignment", "[move][assignment]") {
    compressed_graph<int, int, int> g1(100);
    vector<copyable_edge_t<int, int>> edges = {{0, 1, 10}};
    g1.load_edges(edges);
    
    auto original_size = g1.size();
    compressed_graph<int, int, int> g2(200);
    g2 = std::move(g1);
    
    REQUIRE(g2.value() == 100);
    REQUIRE(g2.size() == original_size);
}

TEST_CASE("compressed_graph handles self-loops", "[edges][self_loop]") {
    compressed_graph<void, void, void> g;
    vector<copyable_edge_t<int, void>> edges = {{0, 0}, {1, 1}};
    g.load_edges(edges);
    
    REQUIRE_FALSE(g.empty());
}

TEST_CASE("compressed_graph handles duplicate edges", "[edges][duplicates]") {
    compressed_graph<int, void, void> g;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, 
        {0, 1, 20}, 
        {0, 1, 30}
    };
    g.load_edges(edges);
    
    REQUIRE_FALSE(g.empty());
}

TEST_CASE("compressed_graph load_edges and load_vertices together", "[api][combined]") {
    compressed_graph<int, int, void> g;
    
    // Load edges first (this creates the CSR structure)
    vector<copyable_edge_t<int, int>> edges = {{0, 1, 10}, {1, 2, 20}};
    g.load_edges(edges);
    
    // Then load vertices (this populates vertex values)
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}, {2, 300}};
    g.load_vertices(vertices);
    
    REQUIRE(g.size() == 3);
    REQUIRE_FALSE(g.empty());
}

TEST_CASE("compressed_graph clear empties the graph", "[api][clear]") {
    compressed_graph<void, int, void> g;
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}};
    g.load_vertices(vertices);
    
    REQUIRE_FALSE(g.empty());
    
    g.clear();
    
    REQUIRE(g.empty());
    REQUIRE(g.size() == 0);
}

TEST_CASE("compressed_graph iteration over vertices", "[api][iteration]") {
    compressed_graph<void, int, void> g;
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}, {2, 300}};
    g.load_vertices(vertices);
    
    size_t count = 0;
    for ([[maybe_unused]] auto& v : g.vertices()) {
        ++count;
    }
    
    REQUIRE(count == 3);
}

TEST_CASE("compressed_graph range-based for loop", "[api][iteration][range]") {
    compressed_graph<void, int, void> g;
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}, {2, 300}};
    g.load_vertices(vertices);
    
    size_t count = 0;
    for ([[maybe_unused]] const auto& v : g.vertices()) {
        ++count;
    }
    
    REQUIRE(count == 3);
}

// =============================================================================
// Category: size(), empty(), and clear() Tests (Issue #1 Fix)
// =============================================================================

TEST_CASE("compressed_graph size() returns vertex count", "[api][size][issue1]") {
    SECTION("empty graph has size 0") {
        compressed_graph<void, void, void> g;
        REQUIRE(g.size() == 0);
    }
    
    SECTION("graph with vertices from load_vertices") {
        compressed_graph<void, int, void> g;
        vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}, {2, 300}, {3, 400}};
        g.load_vertices(vertices);
        REQUIRE(g.size() == 4);
    }
    
    SECTION("graph with vertices from load_edges") {
        compressed_graph<void, int, void> g;
        vector<copyable_edge_t<int, void>> edges = {
            {0, 1}, {0, 2}, {1, 3}, {2, 3}, {3, 4}
        };
        g.load_edges(edges);
        REQUIRE(g.size() == 5); // vertices 0-4
    }
    
    SECTION("graph with mixed edge and vertex loading") {
        compressed_graph<int, int, void> g;
        vector<copyable_edge_t<int, int>> edges = {{0, 1, 10}, {1, 2, 20}};
        g.load_edges(edges);
        
        vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}, {2, 300}};
        g.load_vertices(vertices);
        
        REQUIRE(g.size() == 3);
    }
    
    SECTION("graph with large number of vertices") {
        compressed_graph<void, void, void> g;
        vector<copyable_edge_t<int, void>> edges;
        for (int i = 0; i < 100; ++i) {
            edges.push_back({i, i + 1});
        }
        g.load_edges(edges);
        REQUIRE(g.size() == 101); // 0-100
    }
}

TEST_CASE("compressed_graph empty() checks if graph has vertices", "[api][empty][issue1]") {
    SECTION("newly constructed graph is empty") {
        compressed_graph<void, void, void> g;
        REQUIRE(g.empty());
    }
    
    SECTION("graph with graph value but no vertices is empty") {
        compressed_graph<void, void, int> g(42);
        REQUIRE(g.empty());
    }
    
    SECTION("graph after loading vertices is not empty") {
        compressed_graph<void, int, void> g;
        vector<copyable_vertex_t<int, int>> vertices = {{0, 100}};
        g.load_vertices(vertices);
        REQUIRE_FALSE(g.empty());
    }
    
    SECTION("graph after loading edges is not empty") {
        compressed_graph<void, void, void> g;
        vector<copyable_edge_t<int, void>> edges = {{0, 1}};
        g.load_edges(edges);
        REQUIRE_FALSE(g.empty());
    }
    
    SECTION("graph becomes empty after clear") {
        compressed_graph<void, int, void> g;
        vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}};
        g.load_vertices(vertices);
        
        REQUIRE_FALSE(g.empty());
        g.clear();
        REQUIRE(g.empty());
    }
}

TEST_CASE("compressed_graph clear() removes all data", "[api][clear][issue1]") {
    SECTION("clear on empty graph is safe") {
        compressed_graph<void, void, void> g;
        REQUIRE_NOTHROW(g.clear());
        REQUIRE(g.empty());
        REQUIRE(g.size() == 0);
    }
    
    SECTION("clear removes vertices") {
        compressed_graph<void, int, void> g;
        vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}, {2, 300}};
        g.load_vertices(vertices);
        
        REQUIRE(g.size() == 3);
        g.clear();
        REQUIRE(g.size() == 0);
        REQUIRE(g.empty());
    }
    
    SECTION("clear removes edges") {
        compressed_graph<int, void, void> g;
        vector<copyable_edge_t<int, int>> edges = {
            {0, 1, 10}, {0, 2, 20}, {1, 2, 30}
        };
        g.load_edges(edges);
        
        REQUIRE_FALSE(g.empty());
        g.clear();
        REQUIRE(g.empty());
    }
    
    SECTION("clear removes both edges and vertices") {
        compressed_graph<int, int, void> g;
        vector<copyable_edge_t<int, int>> edges = {{0, 1, 10}, {1, 2, 20}};
        g.load_edges(edges);
        
        vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}, {2, 300}};
        g.load_vertices(vertices);
        
        REQUIRE(g.size() == 3);
        g.clear();
        REQUIRE(g.size() == 0);
        REQUIRE(g.empty());
    }
    
    SECTION("graph can be reused after clear") {
        compressed_graph<void, int, void> g;
        
        // First load
        vector<copyable_vertex_t<int, int>> vertices1 = {{0, 100}, {1, 200}};
        g.load_vertices(vertices1);
        REQUIRE(g.size() == 2);
        
        // Clear and reload
        g.clear();
        vector<copyable_vertex_t<int, int>> vertices2 = {{0, 300}, {1, 400}, {2, 500}};
        g.load_vertices(vertices2);
        REQUIRE(g.size() == 3);
    }
    
    SECTION("clear with graph value") {
        compressed_graph<void, void, int> g(42);
        vector<copyable_edge_t<int, void>> edges = {{0, 1}, {1, 2}};
        g.load_edges(edges);
        
        g.clear();
        REQUIRE(g.empty());
        REQUIRE(g.size() == 0);
        // Note: graph value is not cleared by clear()
    }
}

TEST_CASE("compressed_graph size/empty/clear work with all value types", "[api][types][issue1]") {
    SECTION("void/void/void") {
        compressed_graph<void, void, void> g;
        REQUIRE(g.empty());
        REQUIRE(g.size() == 0);
        
        vector<copyable_edge_t<int, void>> edges = {{0, 1}, {1, 2}};
        g.load_edges(edges);
        REQUIRE_FALSE(g.empty());
        REQUIRE(g.size() == 3);
        
        g.clear();
        REQUIRE(g.empty());
        REQUIRE(g.size() == 0);
    }
    
    SECTION("int/int/int") {
        compressed_graph<int, int, int> g(999);
        REQUIRE(g.empty());
        
        vector<copyable_edge_t<int, int>> edges = {{0, 1, 10}};
        g.load_edges(edges);
        vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}};
        g.load_vertices(vertices);
        
        REQUIRE_FALSE(g.empty());
        REQUIRE(g.size() == 2);
        
        g.clear();
        REQUIRE(g.empty());
    }
    
    SECTION("string/string/void") {
        compressed_graph<string, string, void> g;
        vector<copyable_edge_t<int, string>> edges = {
            {0, 1, string("edge1")},
            {1, 2, string("edge2")}
        };
        g.load_edges(edges);
        
        vector<copyable_vertex_t<int, string>> vertices = {
            {0, string("v0")},
            {1, string("v1")},
            {2, string("v2")}
        };
        g.load_vertices(vertices);
        
        REQUIRE(g.size() == 3);
        g.clear();
        REQUIRE(g.size() == 0);
    }
}

TEST_CASE("compressed_graph size/empty are consistent", "[api][size][empty][issue1]") {
    compressed_graph<void, int, void> g;
    
    SECTION("size == 0 implies empty") {
        REQUIRE(g.size() == 0);
        REQUIRE(g.empty());
    }
    
    SECTION("size > 0 implies not empty") {
        vector<copyable_vertex_t<int, int>> vertices = {{0, 100}};
        g.load_vertices(vertices);
        REQUIRE(g.size() > 0);
        REQUIRE_FALSE(g.empty());
    }
    
    SECTION("after clear, size == 0 and empty") {
        vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}};
        g.load_vertices(vertices);
        g.clear();
        REQUIRE(g.size() == 0);
        REQUIRE(g.empty());
    }
}

TEST_CASE("compressed_graph clear preserves graph invariants", "[api][clear][issue1]") {
    compressed_graph<int, int, void> g;
    
    // Load some data
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}, {2, 3, 40}
    };
    g.load_edges(edges);
    
    vector<copyable_vertex_t<int, int>> vertices = {
        {0, 100}, {1, 200}, {2, 300}, {3, 400}
    };
    g.load_vertices(vertices);
    
    size_t original_size = g.size();
    REQUIRE(original_size == 4);
    
    // Clear the graph
    g.clear();
    
    // Verify all data is gone
    REQUIRE(g.size() == 0);
    REQUIRE(g.empty());
    
    // Verify we can reload new data
    vector<copyable_edge_t<int, int>> new_edges = {{0, 1, 50}, {1, 2, 60}};
    g.load_edges(new_edges);
    
    REQUIRE(g.size() == 3);
    REQUIRE_FALSE(g.empty());
}

TEST_CASE("compressed_graph const-correctness of size/empty", "[api][const][issue1]") {
    compressed_graph<void, int, void> g;
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}};
    g.load_vertices(vertices);
    
    const auto& cg = g;
    
    REQUIRE(cg.size() == 2);
    REQUIRE_FALSE(cg.empty());
}

// =============================================================================
// Category: vertices() and vertex_ids() Tests (Issue #2 Fix)
// =============================================================================

TEST_CASE("compressed_graph vertices() returns correct range", "[api][vertices][issue2]") {
    SECTION("empty graph") {
        compressed_graph<void, void, void> g;
        
        size_t count = 0;
        for ([[maybe_unused]] auto& v : g.vertices()) {
            ++count;
        }
        
        REQUIRE(count == 0);
        REQUIRE(g.vertices().empty());
    }
    
    SECTION("graph with vertices from edges") {
        compressed_graph<void, void, void> g;
        vector<copyable_edge_t<int, void>> edges = {{0, 1}, {0, 2}, {1, 2}, {1, 3}};
        g.load_edges(edges);
        
        size_t count = 0;
        for ([[maybe_unused]] auto& v : g.vertices()) {
            ++count;
        }
        
        REQUIRE(count == g.size());
        REQUIRE(count == 4); // vertices 0, 1, 2, 3
    }
    
    SECTION("graph with explicit vertex values") {
        compressed_graph<void, int, void> g;
        vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}, {2, 300}};
        g.load_vertices(vertices);
        
        size_t count = 0;
        for ([[maybe_unused]] auto& v : g.vertices()) {
            ++count;
        }
        
        REQUIRE(count == 3);
        REQUIRE(count == g.size());
    }
}

TEST_CASE("compressed_graph vertices() const version", "[api][vertices][const][issue2]") {
    compressed_graph<void, int, void> g;
    vector<copyable_edge_t<int, void>> edges = {{0, 1}, {1, 2}, {2, 3}};
    g.load_edges(edges);
    
    const auto& cg = g;
    
    size_t count = 0;
    for ([[maybe_unused]] const auto& v : cg.vertices()) {
        ++count;
    }
    
    REQUIRE(count == cg.size());
    REQUIRE(count == 4);
}

TEST_CASE("compressed_graph vertex_ids() returns correct range", "[api][vertex_ids][issue2]") {
    SECTION("empty graph") {
        compressed_graph<void, void, void> g;
        
        size_t count = 0;
        for ([[maybe_unused]] auto id : g.vertex_ids()) {
            ++count;
        }
        
        REQUIRE(count == 0);
    }
    
    SECTION("graph with 5 vertices") {
        compressed_graph<void, void, void> g;
        vector<copyable_edge_t<int, void>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 4}};
        g.load_edges(edges);
        
        REQUIRE(g.size() == 5);
        
        vector<int> ids;
        for (auto id : g.vertex_ids()) {
            ids.push_back(id);
        }
        
        REQUIRE(ids.size() == 5);
        REQUIRE(ids == vector<int>{0, 1, 2, 3, 4});
    }
    
    SECTION("vertex_ids match vertices count") {
        compressed_graph<void, int, void> g;
        vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}, {2, 300}, {3, 400}};
        g.load_vertices(vertices);
        
        size_t id_count = 0;
        for ([[maybe_unused]] auto id : g.vertex_ids()) {
            ++id_count;
        }
        
        size_t vertex_count = 0;
        for ([[maybe_unused]] auto& v : g.vertices()) {
            ++vertex_count;
        }
        
        REQUIRE(id_count == vertex_count);
        REQUIRE(id_count == g.size());
    }
}

TEST_CASE("compressed_graph vertex_ids() can access vertices", "[api][vertex_ids][issue2]") {
    compressed_graph<void, int, void> g;
    vector<copyable_vertex_t<int, int>> vertices = {{0, 100}, {1, 200}, {2, 300}};
    g.load_vertices(vertices);
    
    // Use vertex_ids() to iterate and access vertex values
    for (auto id : g.vertex_ids()) {
        auto it = g.find_vertex(id);
        REQUIRE(it != g.vertices().end()); // Not past end
    }
}

TEST_CASE("compressed_graph vertices() with different template parameters", "[api][vertices][types][issue2]") {
    SECTION("void/void/void") {
        compressed_graph<void, void, void> g;
        vector<copyable_edge_t<int, void>> edges = {{0, 1}, {1, 2}};
        g.load_edges(edges);
        
        size_t count = 0;
        for ([[maybe_unused]] auto& v : g.vertices()) {
            ++count;
        }
        REQUIRE(count == 3);
    }
    
    SECTION("int/int/void") {
        compressed_graph<int, int, void> g;
        vector<copyable_edge_t<int, int>> edges = {{0, 1, 10}, {1, 2, 20}};
        g.load_edges(edges);
        
        size_t count = 0;
        for ([[maybe_unused]] auto& v : g.vertices()) {
            ++count;
        }
        REQUIRE(count == 3);
    }
    
    SECTION("void/void/string (non-void GV)") {
        compressed_graph<void, void, string> g("test graph");
        vector<copyable_edge_t<int, void>> edges = {{0, 1}, {1, 2}};
        g.load_edges(edges);
        
        size_t count = 0;
        for ([[maybe_unused]] auto& v : g.vertices()) {
            ++count;
        }
        REQUIRE(count == 3);
        REQUIRE(g.value() == "test graph");
    }
}

TEST_CASE("compressed_graph vertices() excludes terminating row", "[api][vertices][internals][issue2]") {
    compressed_graph<void, void, void> g;
    vector<copyable_edge_t<int, void>> edges = {{0, 1}, {1, 2}, {2, 3}};
    g.load_edges(edges);
    
    // CSR stores n+1 rows (including terminating row)
    // vertices() should return exactly n vertices
    size_t vertex_count = 0;
    for ([[maybe_unused]] auto& v : g.vertices()) {
        ++vertex_count;
    }
    
    REQUIRE(vertex_count == g.size());
    REQUIRE(vertex_count == 4); // Not 5 (which would include terminator)
}

TEST_CASE("compressed_graph vertices() can be used with STL algorithms", "[api][vertices][algorithms][issue2]") {
    compressed_graph<void, void, void> g;
    vector<copyable_edge_t<int, void>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 4}};
    g.load_edges(edges);
    
    auto verts = g.vertices();
    
    // Test std::distance
    size_t dist = std::distance(verts.begin(), verts.end());
    REQUIRE(dist == g.size());
    REQUIRE(dist == 5);
    
    // Test std::count_if (count all vertices - trivial but tests compatibility)
    size_t count = std::count_if(verts.begin(), verts.end(), 
                                  [](const auto&) { return true; });
    REQUIRE(count == 5);
}

TEST_CASE("compressed_graph vertex_ids() can be used with STL algorithms", "[api][vertex_ids][algorithms][issue2]") {
    compressed_graph<void, void, void> g;
    vector<copyable_edge_t<int, void>> edges = {{0, 2}, {2, 4}, {4, 6}};
    g.load_edges(edges);
    
    auto ids = g.vertex_ids();
    
    // Test std::accumulate (sum of IDs)
    int sum = std::accumulate(ids.begin(), ids.end(), 0);
    // IDs are 0, 1, 2, 3, 4, 5, 6 = 21
    REQUIRE(sum == 21);
    
    // Test std::any_of
    bool has_zero = std::any_of(ids.begin(), ids.end(), [](auto id) { return id == 0; });
    REQUIRE(has_zero);
}
