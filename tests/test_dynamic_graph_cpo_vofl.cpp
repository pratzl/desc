/**
 * @file test_dynamic_graph_cpo_vofl.cpp
 * @brief Phase 2 CPO tests for dynamic_graph with vofl_graph_traits
 * 
 * Tests CPO (Customization Point Object) integration with dynamic_graph.
 * These tests verify that CPOs work correctly with the default implementations
 * and friend function overrides in dynamic_graph.
 * 
 * Container: vector<vertex> + forward_list<edge>
 * 
 * Current Status: 67 test cases, 528 assertions passing
 * 
 * CPOs tested (with available friend functions):
 * - vertices(g) - Get vertex range [3 tests]
 * - num_vertices(g) - Get vertex count [3 tests]
 * - find_vertex(g, uid) - Find vertex by ID [3 tests]
 * - vertex_id(g, u) - Get vertex ID from descriptor [7 tests]
 * - num_edges(g) - Get total edge count [3 tests]
 * - has_edge(g) - Check if graph has any edges [3 tests]
 * - degree(g, u) - Get out-degree of vertex [10 tests]
 * - target_id(g, uv) - Get target vertex ID from edge [10 tests]
 * - vertex_value(g, u) - Access vertex value (when VV != void) [6 tests]
 * - edge_value(g, uv) - Access edge value (when EV != void) [6 tests]
 * - graph_value(g) - Access graph value (when GV != void) [6 tests]
 * 
 * Friend functions implemented and tested:
 * - vertex_value(g,u) in dynamic_graph_base (lines 1345-1348)
 * - edge_value(g,uv) in dynamic_vertex_base (lines 665-676)
 * - edges(g,u) in dynamic_vertex_base (lines 678-679)
 * 
 * Note: forward_list uses push_front() for edge insertion, so edges appear in
 * reverse order of loading. Tests account for this behavior.
 * Note: degree(g,u) uses the CPO default implementation with std::ranges::distance.
 */

#include <catch2/catch_test_macros.hpp>
#include <graph/container/dynamic_graph.hpp>
#include <graph/detail/graph_cpo.hpp>
#include <string>
#include <vector>
#include <algorithm>

using namespace graph;
using namespace graph::container;

// Type aliases for test configurations
using vofl_void     = dynamic_graph<void, void, void, uint32_t, false, vofl_graph_traits<void, void, void, uint32_t, false>>;
using vofl_int_ev   = dynamic_graph<int, void, void, uint32_t, false, vofl_graph_traits<int, void, void, uint32_t, false>>;
using vofl_int_vv   = dynamic_graph<void, int, void, uint32_t, false, vofl_graph_traits<void, int, void, uint32_t, false>>;
using vofl_all_int  = dynamic_graph<int, int, int, uint32_t, false, vofl_graph_traits<int, int, int, uint32_t, false>>;
using vofl_string   = dynamic_graph<std::string, std::string, std::string, uint32_t, false, 
                                     vofl_graph_traits<std::string, std::string, std::string, uint32_t, false>>;

//==================================================================================================
// 1. vertices(g) CPO Tests
//==================================================================================================

TEST_CASE("vofl CPO vertices(g) returns vertex_descriptor_view", "[vofl][cpo][vertices]") {
    vofl_void g;
    g.resize_vertices(5);
    
    auto v_range = vertices(g);
    
    // Should be a sized range
    REQUIRE(std::ranges::size(v_range) == 5);
    
    // Should be iterable
    size_t count = 0;
    for ([[maybe_unused]] auto v : v_range) {
        ++count;
    }
    REQUIRE(count == 5);
}

TEST_CASE("vofl CPO vertices(g) const correctness", "[vofl][cpo][vertices]") {
    const vofl_void g;
    
    auto v_range = vertices(g);
    REQUIRE(std::ranges::size(v_range) == 0);
}

TEST_CASE("vofl CPO vertices(g) with values", "[vofl][cpo][vertices]") {
    vofl_int_vv g;
    g.resize_vertices(3);
    
    auto v_range = vertices(g);
    REQUIRE(std::ranges::size(v_range) == 3);
}

//==================================================================================================
// 2. num_vertices(g) CPO Tests
//==================================================================================================

TEST_CASE("vofl CPO num_vertices(g) empty graph", "[vofl][cpo][num_vertices]") {
    vofl_void g;
    
    REQUIRE(num_vertices(g) == 0);
}

TEST_CASE("vofl CPO num_vertices(g) non-empty", "[vofl][cpo][num_vertices]") {
    vofl_void g;
    g.resize_vertices(10);
    
    REQUIRE(num_vertices(g) == 10);
}

TEST_CASE("vofl CPO num_vertices(g) matches vertices size", "[vofl][cpo][num_vertices]") {
    vofl_int_vv g;
    g.resize_vertices(7);
    
    REQUIRE(num_vertices(g) == std::ranges::size(vertices(g)));
}

//==================================================================================================
// 3. find_vertex(g, uid) CPO Tests
//==================================================================================================

TEST_CASE("vofl CPO find_vertex(g, uid) with uint32_t", "[vofl][cpo][find_vertex]") {
    vofl_void g;
    g.resize_vertices(5);
    
    auto v = find_vertex(g, uint32_t{2});
    
    REQUIRE(v != vertices(g).end());
}

TEST_CASE("vofl CPO find_vertex(g, uid) with int", "[vofl][cpo][find_vertex]") {
    vofl_void g;
    g.resize_vertices(5);
    
    // Should handle int -> uint32_t conversion
    auto v = find_vertex(g, 3);
    
    REQUIRE(v != vertices(g).end());
}

TEST_CASE("vofl CPO find_vertex(g, uid) bounds check", "[vofl][cpo][find_vertex]") {
    vofl_void g;
    g.resize_vertices(3);
    
    auto v0 = find_vertex(g, 0);
    auto v2 = find_vertex(g, 2);
    
    REQUIRE(v0 != vertices(g).end());
    REQUIRE(v2 != vertices(g).end());
}

//==================================================================================================
// 4. vertex_id(g, u) CPO Tests
//==================================================================================================

TEST_CASE("vofl CPO vertex_id(g, u) basic access", "[vofl][cpo][vertex_id]") {
    vofl_void g;
    g.resize_vertices(5);
    
    auto v_range = vertices(g);
    auto v_it = v_range.begin();
    auto v_desc = *v_it;
    
    auto id = vertex_id(g, v_desc);
    REQUIRE(id == 0);
}

TEST_CASE("vofl CPO vertex_id(g, u) all vertices", "[vofl][cpo][vertex_id]") {
    vofl_void g;
    g.resize_vertices(10);
    
    size_t expected_id = 0;
    for (auto v : vertices(g)) {
        REQUIRE(vertex_id(g, v) == expected_id);
        ++expected_id;
    }
}

TEST_CASE("vofl CPO vertex_id(g, u) const correctness", "[vofl][cpo][vertex_id]") {
    const vofl_void g;
    
    // Empty graph - should compile even though no vertices to iterate
    for (auto v : vertices(g)) {
        [[maybe_unused]] auto id = vertex_id(g, v);
    }
    REQUIRE(num_vertices(g) == 0);
}

TEST_CASE("vofl CPO vertex_id(g, u) with vertex values", "[vofl][cpo][vertex_id]") {
    vofl_int_vv g;
    g.resize_vertices(5);
    
    // Initialize vertex values to their IDs
    for (auto v : vertices(g)) {
        auto id = vertex_id(g, v);
        vertex_value(g, v) = static_cast<int>(id) * 10;
    }
    
    // Verify IDs match expected values
    for (auto v : vertices(g)) {
        auto id = vertex_id(g, v);
        REQUIRE(vertex_value(g, v) == static_cast<int>(id) * 10);
    }
}

TEST_CASE("vofl CPO vertex_id(g, u) with find_vertex", "[vofl][cpo][vertex_id]") {
    vofl_void g;
    g.resize_vertices(8);
    
    // Find vertex by ID and verify round-trip
    for (uint32_t expected_id = 0; expected_id < 8; ++expected_id) {
        auto v_it = find_vertex(g, expected_id);
        REQUIRE(v_it != vertices(g).end());
        
        auto v_desc = *v_it;
        auto actual_id = vertex_id(g, v_desc);
        REQUIRE(actual_id == expected_id);
    }
}

TEST_CASE("vofl CPO vertex_id(g, u) sequential iteration", "[vofl][cpo][vertex_id]") {
    vofl_void g;
    g.resize_vertices(100);
    
    // Verify IDs are sequential
    auto v_range = vertices(g);
    auto it = v_range.begin();
    for (size_t expected = 0; expected < 100; ++expected) {
        REQUIRE(it != v_range.end());
        auto v = *it;
        REQUIRE(vertex_id(g, v) == expected);
        ++it;
    }
}

TEST_CASE("vofl CPO vertex_id(g, u) consistency across calls", "[vofl][cpo][vertex_id]") {
    vofl_void g;
    g.resize_vertices(5);
    
    auto v_range = vertices(g);
    auto v_it = v_range.begin();
    auto v_desc = *v_it;
    
    // Call vertex_id multiple times - should be stable
    auto id1 = vertex_id(g, v_desc);
    auto id2 = vertex_id(g, v_desc);
    auto id3 = vertex_id(g, v_desc);
    
    REQUIRE(id1 == id2);
    REQUIRE(id2 == id3);
}

//==================================================================================================
// 5. num_edges(g) CPO Tests
//==================================================================================================

TEST_CASE("vofl CPO num_edges(g) empty graph", "[vofl][cpo][num_edges]") {
    vofl_void g;
    
    REQUIRE(num_edges(g) == 0);
}

TEST_CASE("vofl CPO num_edges(g) with edges", "[vofl][cpo][num_edges]") {
    vofl_void g({{0, 1}, {1, 2}, {2, 0}});
    
    REQUIRE(num_edges(g) == 3);
}

TEST_CASE("vofl CPO num_edges(g) after multiple edge additions", "[vofl][cpo][num_edges]") {
    vofl_void g;
    g.resize_vertices(4);
    
    std::vector<copyable_edge_t<uint32_t, void>> edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, {0, 2}
    };
    g.load_edges(edges, std::identity{}, 4, 0);
    
    REQUIRE(num_edges(g) == 5);
}

//==================================================================================================
// 6. has_edge(g) CPO Tests
//==================================================================================================

TEST_CASE("vofl CPO has_edge(g) empty graph", "[vofl][cpo][has_edge]") {
    vofl_void g;
    
    REQUIRE(!has_edge(g));
}

TEST_CASE("vofl CPO has_edge(g) with edges", "[vofl][cpo][has_edge]") {
    vofl_void g({{0, 1}});
    
    REQUIRE(has_edge(g));
}

TEST_CASE("vofl CPO has_edge(g) matches num_edges", "[vofl][cpo][has_edge]") {
    vofl_void g1;
    vofl_void g2({{0, 1}});
    
    REQUIRE(has_edge(g1) == (num_edges(g1) > 0));
    REQUIRE(has_edge(g2) == (num_edges(g2) > 0));
}

//==================================================================================================
// 7. degree(g, u) CPO Tests
//==================================================================================================

TEST_CASE("vofl CPO degree(g, u) isolated vertex", "[vofl][cpo][degree]") {
    vofl_void g;
    g.resize_vertices(3);
    
    // Vertices with no edges should have degree 0
    for (auto u : vertices(g)) {
        REQUIRE(degree(g, u) == 0);
    }
}

TEST_CASE("vofl CPO degree(g, u) single edge", "[vofl][cpo][degree]") {
    vofl_void g({{0, 1}});
    
    auto v_range = vertices(g);
    auto v0 = *v_range.begin();
    
    REQUIRE(degree(g, v0) == 1);
}

TEST_CASE("vofl CPO degree(g, u) multiple edges from vertex", "[vofl][cpo][degree]") {
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 1}, {0, 2}, {0, 3}, {1, 2}
    };
    vofl_void g;
    g.resize_vertices(4);
    g.load_edges(edge_data);
    
    // Vertex 0 has 3 outgoing edges
    auto v0 = *vertices(g).begin();
    REQUIRE(degree(g, v0) == 3);
    
    // Vertex 1 has 1 outgoing edge
    auto v1 = *std::next(vertices(g).begin(), 1);
    REQUIRE(degree(g, v1) == 1);
    
    // Vertices 2 and 3 have no outgoing edges
    auto v2 = *std::next(vertices(g).begin(), 2);
    auto v3 = *std::next(vertices(g).begin(), 3);
    REQUIRE(degree(g, v2) == 0);
    REQUIRE(degree(g, v3) == 0);
}

TEST_CASE("vofl CPO degree(g, u) all vertices", "[vofl][cpo][degree]") {
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 1}, {0, 2},
        {1, 2}, {1, 3},
        {2, 3},
        {3, 0}
    };
    vofl_void g;
    g.resize_vertices(4);
    g.load_edges(edge_data);
    
    // Expected degrees: v0=2, v1=2, v2=1, v3=1
    size_t expected_degrees[] = {2, 2, 1, 1};
    size_t idx = 0;
    
    for (auto u : vertices(g)) {
        REQUIRE(degree(g, u) == expected_degrees[idx]);
        ++idx;
    }
}

TEST_CASE("vofl CPO degree(g, u) const correctness", "[vofl][cpo][degree]") {
    vofl_void g({{0, 1}, {0, 2}});
    
    const vofl_void& const_g = g;
    
    auto v0 = *vertices(const_g).begin();
    REQUIRE(degree(const_g, v0) == 2);
}

TEST_CASE("vofl CPO degree(g, uid) by vertex ID", "[vofl][cpo][degree]") {
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 1}, {0, 2}, {0, 3}
    };
    vofl_void g;
    g.resize_vertices(4);
    g.load_edges(edge_data);
    
    // Access degree by vertex ID
    REQUIRE(degree(g, uint32_t{0}) == 3);
    REQUIRE(degree(g, uint32_t{1}) == 0);
    REQUIRE(degree(g, uint32_t{2}) == 0);
    REQUIRE(degree(g, uint32_t{3}) == 0);
}

TEST_CASE("vofl CPO degree(g, u) matches manual count", "[vofl][cpo][degree]") {
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 1}, {0, 2}, {0, 3},
        {1, 0}, {1, 2},
        {2, 1}
    };
    vofl_void g;
    g.resize_vertices(4);
    g.load_edges(edge_data);
    
    for (auto u : vertices(g)) {
        auto deg = degree(g, u);
        
        // Manual count
        size_t manual_count = 0;
        for ([[maybe_unused]] auto e : edges(g, u)) {
            ++manual_count;
        }
        
        REQUIRE(deg == manual_count);
    }
}

TEST_CASE("vofl CPO degree(g, u) with edge values", "[vofl][cpo][degree]") {
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}
    };
    vofl_int_ev g;
    g.resize_vertices(3);
    g.load_edges(edge_data);
    
    auto v0 = *vertices(g).begin();
    auto v1 = *std::next(vertices(g).begin(), 1);
    auto v2 = *std::next(vertices(g).begin(), 2);
    
    REQUIRE(degree(g, v0) == 2);
    REQUIRE(degree(g, v1) == 1);
    REQUIRE(degree(g, v2) == 0);
}

TEST_CASE("vofl CPO degree(g, u) self-loop", "[vofl][cpo][degree]") {
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 0}, {0, 1}  // Self-loop plus normal edge
    };
    vofl_void g;
    g.resize_vertices(2);
    g.load_edges(edge_data);
    
    auto v0 = *vertices(g).begin();
    REQUIRE(degree(g, v0) == 2);  // Self-loop counts as one edge
}

TEST_CASE("vofl CPO degree(g, u) large graph", "[vofl][cpo][degree]") {
    vofl_void g;
    g.resize_vertices(100);
    
    // Create a star graph: vertex 0 connects to all others
    std::vector<copyable_edge_t<uint32_t, void>> edge_data;
    for (uint32_t i = 1; i < 100; ++i) {
        edge_data.push_back({0, i});
    }
    g.load_edges(edge_data);
    
    auto v0 = *vertices(g).begin();
    REQUIRE(degree(g, v0) == 99);
    
    // All other vertices have degree 0
    size_t idx = 0;
    for (auto u : vertices(g)) {
        if (idx > 0) {
            REQUIRE(degree(g, u) == 0);
        }
        ++idx;
    }
}

//==================================================================================================
// 8. target_id(g, uv) CPO Tests
//==================================================================================================

TEST_CASE("vofl CPO target_id(g, uv) basic access", "[vofl][cpo][target_id]") {
    vofl_void g({{0, 1}, {0, 2}, {1, 2}});
    
    // Get edges from vertex 0
    auto u0 = *find_vertex(g, 0);
    auto edge_view = edges(g, u0);
    auto it = edge_view.begin();
    
    REQUIRE(it != edge_view.end());
    auto uv0 = *it;
    REQUIRE(target_id(g, uv0) == 2);  // forward_list: last added appears first
    
    ++it;
    REQUIRE(it != edge_view.end());
    auto uv1 = *it;
    REQUIRE(target_id(g, uv1) == 1);  // forward_list: first added appears second
}

TEST_CASE("vofl CPO target_id(g, uv) all edges", "[vofl][cpo][target_id]") {
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 1}, {0, 2}, {1, 2}, {1, 3}, {2, 3}
    };
    vofl_void g;
    g.resize_vertices(4);
    g.load_edges(edge_data);
    
    // Collect all target IDs
    std::vector<uint32_t> targets;
    for (auto u : vertices(g)) {
        for (auto uv : edges(g, u)) {
            targets.push_back(target_id(g, uv));
        }
    }
    
    // Should have 5 edges total
    REQUIRE(targets.size() == 5);
    
    // Verify all target IDs are valid vertex IDs
    for (auto tid : targets) {
        REQUIRE(tid < num_vertices(g));
    }
}

TEST_CASE("vofl CPO target_id(g, uv) with edge values", "[vofl][cpo][target_id]") {
    vofl_int_ev g({{0, 1, 100}, {0, 2, 200}, {1, 2, 300}});
    
    // Verify target_id works with edge values present
    for (auto u : vertices(g)) {
        for (auto uv : edges(g, u)) {
            auto tid = target_id(g, uv);
            REQUIRE(tid < num_vertices(g));
        }
    }
}

TEST_CASE("vofl CPO target_id(g, uv) const correctness", "[vofl][cpo][target_id]") {
    vofl_void g({{0, 1}, {1, 2}});
    const auto& const_g = g;
    
    auto u0 = *find_vertex(const_g, 0);
    auto edge_view = edges(const_g, u0);
    
    for (auto uv : edge_view) {
        auto tid = target_id(const_g, uv);
        REQUIRE(tid == 1);
        break;
    }
}

TEST_CASE("vofl CPO target_id(g, uv) self-loop", "[vofl][cpo][target_id]") {
    vofl_void g({{0, 0}, {0, 1}});  // Self-loop and regular edge
    
    auto u0 = *find_vertex(g, 0);
    auto edge_view = edges(g, u0);
    auto it = edge_view.begin();
    
    // forward_list: last added (0,1) appears first
    REQUIRE(target_id(g, *it) == 1);
    ++it;
    // First added (0,0) appears second
    REQUIRE(target_id(g, *it) == 0);  // Self-loop target is source
}

TEST_CASE("vofl CPO target_id(g, uv) parallel edges", "[vofl][cpo][target_id]") {
    // Multiple edges between same vertices
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 10}, {0, 1, 20}, {0, 1, 30}
    };
    vofl_int_ev g;
    g.resize_vertices(2);
    g.load_edges(edge_data);
    
    auto u0 = *find_vertex(g, 0);
    auto edge_view = edges(g, u0);
    
    // All parallel edges should have same target
    for (auto uv : edge_view) {
        REQUIRE(target_id(g, uv) == 1);
    }
}

TEST_CASE("vofl CPO target_id(g, uv) consistency with vertex_id", "[vofl][cpo][target_id]") {
    vofl_void g({{0, 1}, {0, 2}, {1, 2}});
    
    for (auto u : vertices(g)) {
        for (auto uv : edges(g, u)) {
            auto tid = target_id(g, uv);
            
            // Find target vertex and verify its ID matches
            auto target_vertex = *find_vertex(g, tid);
            REQUIRE(vertex_id(g, target_vertex) == tid);
        }
    }
}

TEST_CASE("vofl CPO target_id(g, uv) large graph", "[vofl][cpo][target_id]") {
    // Create a graph with many edges
    std::vector<copyable_edge_t<uint32_t, void>> edge_data;
    for (uint32_t i = 0; i < 50; ++i) {
        edge_data.push_back({i, (i + 1) % 100});
        edge_data.push_back({i, (i + 2) % 100});
    }
    
    vofl_void g;
    g.resize_vertices(100);
    g.load_edges(edge_data);
    
    // Verify all target IDs are valid
    for (auto u : vertices(g)) {
        for (auto uv : edges(g, u)) {
            auto tid = target_id(g, uv);
            REQUIRE(tid < 100);
        }
    }
}

TEST_CASE("vofl CPO target_id(g, uv) with string edge values", "[vofl][cpo][target_id]") {
    using vofl_string_ev = dynamic_graph<std::string, void, void, uint32_t, false, 
                                         vofl_graph_traits<std::string, void, void, uint32_t, false>>;
    
    std::vector<copyable_edge_t<uint32_t, std::string>> edge_data = {
        {0, 1, "edge01"}, {0, 2, "edge02"}, {1, 2, "edge12"}
    };
    vofl_string_ev g;
    g.resize_vertices(3);
    g.load_edges(edge_data);
    
    // target_id should work regardless of edge value type
    auto u0 = *find_vertex(g, 0);
    for (auto uv : edges(g, u0)) {
        auto tid = target_id(g, uv);
        REQUIRE((tid == 1 || tid == 2));
    }
}

TEST_CASE("vofl CPO target_id(g, uv) iteration order", "[vofl][cpo][target_id]") {
    // Verify target_id works correctly with forward_list reverse insertion order
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 1}, {0, 2}, {0, 3}
    };
    vofl_void g;
    g.resize_vertices(4);
    g.load_edges(edge_data);
    
    auto u0 = *find_vertex(g, 0);
    auto edge_view = edges(g, u0);
    auto it = edge_view.begin();
    
    // forward_list uses push_front: last loaded appears first
    std::vector<uint32_t> expected_targets = {3, 2, 1};
    size_t idx = 0;
    
    for (auto uv : edge_view) {
        REQUIRE(target_id(g, uv) == expected_targets[idx]);
        ++idx;
    }
    REQUIRE(idx == 3);
}

//==================================================================================================
// 9. Integration Tests - Multiple CPOs Working Together
//==================================================================================================

TEST_CASE("vofl CPO integration: graph construction and traversal", "[vofl][cpo][integration]") {
    vofl_void g({{0, 1}, {1, 2}});
    
    // Verify through CPOs
    REQUIRE(num_vertices(g) == 3);
    REQUIRE(num_edges(g) == 2);
    REQUIRE(has_edge(g));
}

TEST_CASE("vofl CPO integration: empty graph properties", "[vofl][cpo][integration]") {
    vofl_void g;
    
    REQUIRE(num_vertices(g) == 0);
    REQUIRE(num_edges(g) == 0);
    REQUIRE(!has_edge(g));
    REQUIRE(std::ranges::size(vertices(g)) == 0);
}

TEST_CASE("vofl CPO integration: find vertex by id", "[vofl][cpo][integration]") {
    vofl_void g;
    g.resize_vertices(5);
    
    // Find each vertex by ID
    for (uint32_t i = 0; i < 5; ++i) {
        auto v = find_vertex(g, i);
        REQUIRE(v != vertices(g).end());
    }
}

TEST_CASE("vofl CPO integration: vertices and num_vertices consistency", "[vofl][cpo][integration]") {
    vofl_void g;
    g.resize_vertices(10);
    
    REQUIRE(num_vertices(g) == 10);
    
    size_t count = 0;
    for ([[maybe_unused]] auto v : vertices(g)) {
        ++count;
    }
    REQUIRE(count == num_vertices(g));
}

TEST_CASE("vofl CPO integration: const graph access", "[vofl][cpo][integration]") {
    vofl_void g;
    g.resize_vertices(3);
    
    const vofl_void& const_g = g;
    
    REQUIRE(num_vertices(const_g) == 3);
    REQUIRE(num_edges(const_g) == 0);
    REQUIRE(!has_edge(const_g));
    
    // Count vertices via iteration
    size_t vertex_count = 0;
    for ([[maybe_unused]] auto v : vertices(const_g)) {
        ++vertex_count;
    }
    REQUIRE(vertex_count == 3);
}

//==================================================================================================
// 9. vertex_value(g, u) CPO Tests
//==================================================================================================

TEST_CASE("vofl CPO vertex_value(g, u) basic access", "[vofl][cpo][vertex_value]") {
    vofl_int_vv g;
    g.resize_vertices(3);
    
    // vertices(g) returns vertex_descriptor_view which when iterated gives descriptors
    for (auto u : vertices(g)) {
        vertex_value(g, u) = 42;
        REQUIRE(vertex_value(g, u) == 42);
        break; // Just test the first one
    }
}

TEST_CASE("vofl CPO vertex_value(g, u) multiple vertices", "[vofl][cpo][vertex_value]") {
    vofl_int_vv g;
    g.resize_vertices(5);
    
    // Set values for all vertices
    int val = 0;
    for (auto u : vertices(g)) {
        vertex_value(g, u) = val;
        val += 100;
    }
    
    // Verify values
    val = 0;
    for (auto u : vertices(g)) {
        REQUIRE(vertex_value(g, u) == val);
        val += 100;
    }
}

TEST_CASE("vofl CPO vertex_value(g, u) const correctness", "[vofl][cpo][vertex_value]") {
    vofl_int_vv g;
    g.resize_vertices(3);
    
    for (auto u : vertices(g)) {
        vertex_value(g, u) = 999;
        break;
    }
    
    const vofl_int_vv& const_g = g;
    for (auto const_u : vertices(const_g)) {
        // Should be able to read from const graph
        REQUIRE(vertex_value(const_g, const_u) == 999);
        break;
    }
}

TEST_CASE("vofl CPO vertex_value(g, u) with string values", "[vofl][cpo][vertex_value]") {
    vofl_string g;
    g.resize_vertices(2);
    
    int idx = 0;
    std::string expected[] = {"first", "second"};
    for (auto u : vertices(g)) {
        vertex_value(g, u) = expected[idx++];
        if (idx >= 2) break;
    }
    
    idx = 0;
    for (auto u : vertices(g)) {
        REQUIRE(vertex_value(g, u) == expected[idx++]);
        if (idx >= 2) break;
    }
}

TEST_CASE("vofl CPO vertex_value(g, u) modification", "[vofl][cpo][vertex_value]") {
    vofl_all_int g;
    g.resize_vertices(3);
    
    for (auto u : vertices(g)) {
        vertex_value(g, u) = 10;
        REQUIRE(vertex_value(g, u) == 10);
        
        vertex_value(g, u) = 20;
        REQUIRE(vertex_value(g, u) == 20);
        
        // Modify through reference
        vertex_value(g, u) += 5;
        REQUIRE(vertex_value(g, u) == 25);
        break; // Just test first vertex
    }
}

//==================================================================================================
// 10. edge_value(g, uv) CPO Tests
//==================================================================================================

TEST_CASE("vofl CPO edge_value(g, uv) basic access", "[vofl][cpo][edge_value]") {
    vofl_int_ev g({{0, 1, 42}, {1, 2, 99}});
    
    for (auto u : vertices(g)) {
        auto& v = u.inner_value(g);
        auto& edge_range = v.edges();
        auto e_iter = edge_range.begin();
        if (e_iter != edge_range.end()) {
            using edge_iter_t = decltype(e_iter);
            using vertex_desc_t = decltype(u);
            auto uv = graph::edge_descriptor<edge_iter_t, typename vertex_desc_t::iterator_type>(e_iter, u);
            REQUIRE(edge_value(g, uv) == 42);
        }
        break;
    }
}

TEST_CASE("vofl CPO edge_value(g, uv) multiple edges", "[vofl][cpo][edge_value]") {
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 100},
        {0, 2, 200},
        {1, 2, 300}
    };
    vofl_int_ev g;
    g.resize_vertices(3);
    g.load_edges(edge_data);
    
    // Check first vertex's edges
    // Note: forward_list uses push_front, so edges are in reverse order of loading
    for (auto u : vertices(g)) {
        auto& v = u.inner_value(g);
        auto& edge_range = v.edges();
        auto e_iter = edge_range.begin();
        if (e_iter != edge_range.end()) {
            using edge_iter_t = decltype(e_iter);
            using vertex_desc_t = decltype(u);
            auto uv0 = graph::edge_descriptor<edge_iter_t, typename vertex_desc_t::iterator_type>(e_iter, u);
            REQUIRE(edge_value(g, uv0) == 200);  // loaded second, appears first in forward_list
            ++e_iter;
            if (e_iter != edge_range.end()) {
                auto uv1 = graph::edge_descriptor<edge_iter_t, typename vertex_desc_t::iterator_type>(e_iter, u);
                REQUIRE(edge_value(g, uv1) == 100);  // loaded first, appears second in forward_list
            }
        }
        break;
    }
}

TEST_CASE("vofl CPO edge_value(g, uv) modification", "[vofl][cpo][edge_value]") {
    vofl_all_int g({{0, 1, 50}});
    
    for (auto u : vertices(g)) {
        auto& v = u.inner_value(g);
        auto& edge_range = v.edges();
        auto e_iter = edge_range.begin();
        if (e_iter != edge_range.end()) {
            using edge_iter_t = decltype(e_iter);
            using vertex_desc_t = decltype(u);
            auto uv = graph::edge_descriptor<edge_iter_t, typename vertex_desc_t::iterator_type>(e_iter, u);
            
            REQUIRE(edge_value(g, uv) == 50);
            
            edge_value(g, uv) = 75;
            REQUIRE(edge_value(g, uv) == 75);
            
            // Modify through reference
            edge_value(g, uv) += 25;
            REQUIRE(edge_value(g, uv) == 100);
        }
        break;
    }
}

TEST_CASE("vofl CPO edge_value(g, uv) const correctness", "[vofl][cpo][edge_value]") {
    vofl_int_ev g({{0, 1, 42}});
    
    const vofl_int_ev& const_g = g;
    for (auto const_u : vertices(const_g)) {
        auto& const_v = const_u.inner_value(const_g);
        auto& const_edge_range = const_v.edges();
        auto const_e_iter = const_edge_range.begin();
        if (const_e_iter != const_edge_range.end()) {
            using const_edge_iter_t = decltype(const_e_iter);
            using const_vertex_desc_t = decltype(const_u);
            auto const_uv = graph::edge_descriptor<const_edge_iter_t, typename const_vertex_desc_t::iterator_type>(const_e_iter, const_u);
            REQUIRE(edge_value(const_g, const_uv) == 42);
        }
        break;
    }
}

TEST_CASE("vofl CPO edge_value(g, uv) with string values", "[vofl][cpo][edge_value]") {
    std::vector<copyable_edge_t<uint32_t, std::string>> edge_data = {
        {0, 1, "edge01"},
        {1, 2, "edge12"}
    };
    vofl_string g;
    g.resize_vertices(3);
    g.load_edges(edge_data);
    
    std::vector<std::string> expected = {"edge01", "edge12"};
    size_t idx = 0;
    
    for (auto u : vertices(g)) {
        auto& v = u.inner_value(g);
        auto& edge_range = v.edges();
        for (auto e_iter = edge_range.begin(); e_iter != edge_range.end(); ++e_iter) {
            if (idx < 2) {
                using edge_iter_t = decltype(e_iter);
                using vertex_desc_t = decltype(u);
                auto uv = graph::edge_descriptor<edge_iter_t, typename vertex_desc_t::iterator_type>(e_iter, u);
                REQUIRE(edge_value(g, uv) == expected[idx]);
                ++idx;
            }
        }
    }
}

TEST_CASE("vofl CPO edge_value(g, uv) iteration over all edges", "[vofl][cpo][edge_value]") {
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 10},
        {0, 2, 20},
        {1, 2, 30},
        {2, 0, 40}
    };
    vofl_int_ev g;
    g.resize_vertices(3);
    g.load_edges(edge_data);
    
    // Sum all edge values
    int sum = 0;
    for (auto u : vertices(g)) {
        auto& v = u.inner_value(g);
        auto& edge_range = v.edges();
        for (auto e_iter = edge_range.begin(); e_iter != edge_range.end(); ++e_iter) {
            using edge_iter_t = decltype(e_iter);
            using vertex_desc_t = decltype(u);
            auto uv = graph::edge_descriptor<edge_iter_t, typename vertex_desc_t::iterator_type>(e_iter, u);
            sum += edge_value(g, uv);
        }
    }
    
    REQUIRE(sum == 100);
}

//==================================================================================================
// 11. Integration Tests - vertex_value and edge_value Together
//==================================================================================================

TEST_CASE("vofl CPO integration: vertex values only", "[vofl][cpo][integration]") {
    vofl_all_int g;
    g.resize_vertices(5);
    
    // Set vertex values
    int val = 0;
    for (auto u : vertices(g)) {
        vertex_value(g, u) = val;
        val += 100;
    }
    
    // Verify vertex values
    val = 0;
    for (auto u : vertices(g)) {
        REQUIRE(vertex_value(g, u) == val);
        val += 100;
    }
}

TEST_CASE("vofl CPO integration: vertex and edge values", "[vofl][cpo][integration]") {
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 5},
        {1, 2, 10}
    };
    vofl_all_int g;
    g.resize_vertices(3);
    g.load_edges(edge_data);
    
    // Set vertex values
    int val = 0;
    for (auto u : vertices(g)) {
        vertex_value(g, u) = val;
        val += 100;
    }
    
    // Verify vertex values
    val = 0;
    for (auto u : vertices(g)) {
        REQUIRE(vertex_value(g, u) == val);
        val += 100;
    }
    
    // Verify edge values
    for (auto u : vertices(g)) {
        auto& v = u.inner_value(g);
        auto& edge_range = v.edges();
        auto e_iter = edge_range.begin();
        if (e_iter != edge_range.end()) {
            using edge_iter_t = decltype(e_iter);
            using vertex_desc_t = decltype(u);
            auto uv = graph::edge_descriptor<edge_iter_t, typename vertex_desc_t::iterator_type>(e_iter, u);
            int expected = (u.vertex_id() == 0) ? 5 : 10;
            REQUIRE(edge_value(g, uv) == expected);
        }
        if (u.vertex_id() >= 1) break; // Only check first 2 vertices
    }
}

TEST_CASE("vofl CPO graph_value(g) basic access", "[vofl][cpo][graph_value]") {
    vofl_all_int g({{0, 1, 1}});
    
    // Set graph value
    graph_value(g) = 42;
    
    REQUIRE(graph_value(g) == 42);
}

TEST_CASE("vofl CPO graph_value(g) default initialization", "[vofl][cpo][graph_value]") {
    vofl_all_int g;
    
    // Default constructed int should be 0
    REQUIRE(graph_value(g) == 0);
}

TEST_CASE("vofl CPO graph_value(g) const correctness", "[vofl][cpo][graph_value]") {
    vofl_all_int g({{0, 1, 1}});
    graph_value(g) = 99;
    
    const auto& const_g = g;
    
    // Should be able to read from const graph
    REQUIRE(graph_value(const_g) == 99);
    
    // Verify type is const-qualified
    static_assert(std::is_const_v<std::remove_reference_t<decltype(graph_value(const_g))>>);
}

TEST_CASE("vofl CPO graph_value(g) with string values", "[vofl][cpo][graph_value]") {
    vofl_string g;
    
    // Set string value
    graph_value(g) = "graph metadata";
    
    REQUIRE(graph_value(g) == "graph metadata");
    
    // Modify through reference
    graph_value(g) += " updated";
    
    REQUIRE(graph_value(g) == "graph metadata updated");
}

TEST_CASE("vofl CPO graph_value(g) modification", "[vofl][cpo][graph_value]") {
    vofl_all_int g({{0, 1, 1}, {1, 2, 2}});
    
    // Initialize
    graph_value(g) = 0;
    REQUIRE(graph_value(g) == 0);
    
    // Increment
    graph_value(g) += 10;
    REQUIRE(graph_value(g) == 10);
    
    // Multiply
    graph_value(g) *= 3;
    REQUIRE(graph_value(g) == 30);
}

TEST_CASE("vofl CPO graph_value(g) independent of vertices/edges", "[vofl][cpo][graph_value]") {
    vofl_all_int g({{0, 1, 1}});
    graph_value(g) = 100;
    
    // Modify vertex values
    for (auto u : vertices(g)) {
        vertex_value(g, u) = 50;
    }
    
    // Graph value should be unchanged
    REQUIRE(graph_value(g) == 100);
    
    // Modify edge values
    for (auto u : vertices(g)) {
        auto& v = u.inner_value(g);
        auto& edge_range = v.edges();
        for (auto e_iter = edge_range.begin(); e_iter != edge_range.end(); ++e_iter) {
            using edge_iter_t = decltype(e_iter);
            using vertex_desc_t = decltype(u);
            auto uv = graph::edge_descriptor<edge_iter_t, typename vertex_desc_t::iterator_type>(e_iter, u);
            edge_value(g, uv) = 75;
        }
    }
    
    // Graph value should still be unchanged
    REQUIRE(graph_value(g) == 100);
}

TEST_CASE("vofl CPO integration: modify vertex and edge values", "[vofl][cpo][integration]") {
    vofl_all_int g({{0, 1, 1}, {1, 2, 2}});
    
    // Initialize vertex values
    for (auto u : vertices(g)) {
        vertex_value(g, u) = 0;
    }
    
    // Accumulate edge values into source vertices
    for (auto u : vertices(g)) {
        auto& v = u.inner_value(g);
        auto& edge_range = v.edges();
        for (auto e_iter = edge_range.begin(); e_iter != edge_range.end(); ++e_iter) {
            using edge_iter_t = decltype(e_iter);
            using vertex_desc_t = decltype(u);
            auto uv = graph::edge_descriptor<edge_iter_t, typename vertex_desc_t::iterator_type>(e_iter, u);
            vertex_value(g, u) += edge_value(g, uv);
        }
    }
    
    // Verify accumulated values
    int expected_values[] = {1, 2, 0};
    int idx = 0;
    for (auto u : vertices(g)) {
        REQUIRE(vertex_value(g, u) == expected_values[idx]);
        ++idx;
        if (idx >= 3) break;
    }
}
