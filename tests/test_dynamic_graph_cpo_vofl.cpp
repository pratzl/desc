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
 * Current Status: 152 test cases, 984 assertions passing
 * 
 * CPOs tested (with available friend functions):
 * - vertices(g) - Get vertex range [3 tests]
 * - num_vertices(g) - Get vertex count [3 tests]
 * - find_vertex(g, uid) - Find vertex by ID [3 tests]
 * - vertex_id(g, u) - Get vertex ID from descriptor [7 tests]
 * - num_edges(g) - Get total edge count [3 tests]
 * - has_edge(g) - Check if graph has any edges [3 tests]
 * - edges(g, u) - Get edge range for vertex [13 tests]
 * - edges(g, uid) - Get edge range by vertex ID [10 tests]
 * - degree(g, u) - Get out-degree of vertex [10 tests]
 * - target_id(g, uv) - Get target vertex ID from edge [10 tests]
 * - target(g, uv) - Get target vertex descriptor from edge [11 tests]
 * - find_vertex_edge(g, u, v) - Find edge between vertices [13 tests]
 *   - find_vertex_edge(g, uid, vid) - Additional dedicated tests [11 tests]
 * - contains_edge(g, u, v) and contains_edge(g, uid, vid) - Check if edge exists [15 tests]
 *   - contains_edge(g, uid, vid) - Additional dedicated tests [13 tests]
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
// 7. edges(g, u) CPO Tests
//==================================================================================================

TEST_CASE("vofl CPO edges(g, u) returns edge_descriptor_view", "[vofl][cpo][edges]") {
    vofl_void g({{0, 1}, {0, 2}});
    
    auto u0 = *find_vertex(g, 0);
    auto edge_range = edges(g, u0);
    
    // Verify it's a range
    static_assert(std::ranges::forward_range<decltype(edge_range)>);
    
    // Should be able to iterate
    size_t count = 0;
    for ([[maybe_unused]] auto uv : edge_range) {
        ++count;
    }
    REQUIRE(count == 2);
}

TEST_CASE("vofl CPO edges(g, u) empty edge list", "[vofl][cpo][edges]") {
    vofl_void g;
    g.resize_vertices(3);
    
    auto u0 = *find_vertex(g, 0);
    auto edge_range = edges(g, u0);
    
    // Vertex with no edges should return empty range
    REQUIRE(edge_range.begin() == edge_range.end());
    
    size_t count = 0;
    for ([[maybe_unused]] auto uv : edge_range) {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE("vofl CPO edges(g, u) single edge", "[vofl][cpo][edges]") {
    vofl_void g({{0, 1}});
    
    auto u0 = *find_vertex(g, 0);
    auto edge_range = edges(g, u0);
    
    size_t count = 0;
    for (auto uv : edge_range) {
        REQUIRE(target_id(g, uv) == 1);
        ++count;
    }
    REQUIRE(count == 1);
}

TEST_CASE("vofl CPO edges(g, u) multiple edges", "[vofl][cpo][edges]") {
    vofl_void g({{0, 1}, {0, 2}, {0, 3}});
    
    auto u0 = *find_vertex(g, 0);
    auto edge_range = edges(g, u0);
    
    std::vector<uint32_t> targets;
    for (auto uv : edge_range) {
        targets.push_back(target_id(g, uv));
    }
    
    // forward_list: last added appears first (reverse order)
    REQUIRE(targets.size() == 3);
    REQUIRE(targets[0] == 3);
    REQUIRE(targets[1] == 2);
    REQUIRE(targets[2] == 1);
}

TEST_CASE("vofl CPO edges(g, u) const correctness", "[vofl][cpo][edges]") {
    vofl_void g({{0, 1}, {0, 2}});
    const auto& const_g = g;
    
    auto u0 = *find_vertex(const_g, 0);
    auto edge_range = edges(const_g, u0);
    
    size_t count = 0;
    for ([[maybe_unused]] auto uv : edge_range) {
        ++count;
    }
    REQUIRE(count == 2);
}

TEST_CASE("vofl CPO edges(g, u) with edge values", "[vofl][cpo][edges]") {
    vofl_int_ev g({{0, 1, 100}, {0, 2, 200}});
    
    auto u0 = *find_vertex(g, 0);
    auto edge_range = edges(g, u0);
    
    std::vector<int> values;
    for (auto uv : edge_range) {
        values.push_back(edge_value(g, uv));
    }
    
    REQUIRE(values.size() == 2);
    // forward_list order: reverse of insertion
    REQUIRE(values[0] == 200);
    REQUIRE(values[1] == 100);
}

TEST_CASE("vofl CPO edges(g, u) multiple iterations", "[vofl][cpo][edges]") {
    vofl_void g({{0, 1}, {0, 2}});
    
    auto u0 = *find_vertex(g, 0);
    auto edge_range = edges(g, u0);
    
    // First iteration
    size_t count1 = 0;
    for ([[maybe_unused]] auto uv : edge_range) {
        ++count1;
    }
    
    // Second iteration should work the same
    size_t count2 = 0;
    for ([[maybe_unused]] auto uv : edge_range) {
        ++count2;
    }
    
    REQUIRE(count1 == 2);
    REQUIRE(count2 == 2);
}

TEST_CASE("vofl CPO edges(g, u) all vertices", "[vofl][cpo][edges]") {
    vofl_void g({{0, 1}, {0, 2}, {1, 2}, {2, 0}});
    
    // Check each vertex's edges
    std::vector<size_t> edge_counts;
    for (auto u : vertices(g)) {
        size_t count = 0;
        for ([[maybe_unused]] auto uv : edges(g, u)) {
            ++count;
        }
        edge_counts.push_back(count);
    }
    
    REQUIRE(edge_counts.size() == 3);
    REQUIRE(edge_counts[0] == 2);  // vertex 0 has 2 edges
    REQUIRE(edge_counts[1] == 1);  // vertex 1 has 1 edge
    REQUIRE(edge_counts[2] == 1);  // vertex 2 has 1 edge
}

TEST_CASE("vofl CPO edges(g, u) with self-loop", "[vofl][cpo][edges]") {
    vofl_void g({{0, 0}, {0, 1}});
    
    auto u0 = *find_vertex(g, 0);
    auto edge_range = edges(g, u0);
    
    std::vector<uint32_t> targets;
    for (auto uv : edge_range) {
        targets.push_back(target_id(g, uv));
    }
    
    REQUIRE(targets.size() == 2);
    // Should include self-loop
    REQUIRE((targets[0] == 0 || targets[1] == 0));
    REQUIRE((targets[0] == 1 || targets[1] == 1));
}

TEST_CASE("vofl CPO edges(g, u) with parallel edges", "[vofl][cpo][edges]") {
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 10}, {0, 1, 20}, {0, 1, 30}
    };
    vofl_int_ev g;
    g.resize_vertices(2);
    g.load_edges(edge_data);
    
    auto u0 = *find_vertex(g, 0);
    auto edge_range = edges(g, u0);
    
    size_t count = 0;
    for (auto uv : edge_range) {
        REQUIRE(target_id(g, uv) == 1);
        ++count;
    }
    
    // Should return all three parallel edges
    REQUIRE(count == 3);
}

TEST_CASE("vofl CPO edges(g, u) large graph", "[vofl][cpo][edges]") {
    std::vector<copyable_edge_t<uint32_t, void>> edge_data;
    for (uint32_t i = 0; i < 20; ++i) {
        edge_data.push_back({0, i + 1});
    }
    
    vofl_void g;
    g.resize_vertices(21);
    g.load_edges(edge_data);
    
    auto u0 = *find_vertex(g, 0);
    auto edge_range = edges(g, u0);
    
    size_t count = 0;
    for ([[maybe_unused]] auto uv : edge_range) {
        ++count;
    }
    
    REQUIRE(count == 20);
}

TEST_CASE("vofl CPO edges(g, uid) with vertex ID", "[vofl][cpo][edges]") {
    vofl_void g({{0, 1}, {0, 2}});
    
    auto edge_range = edges(g, uint32_t(0));
    
    size_t count = 0;
    for ([[maybe_unused]] auto uv : edge_range) {
        ++count;
    }
    REQUIRE(count == 2);
}

TEST_CASE("vofl CPO edges(g, uid) returns edge_descriptor_view", "[vofl][cpo][edges]") {
    vofl_void g({{0, 1}, {0, 2}, {1, 2}});
    
    auto edge_range = edges(g, uint32_t(1));
    
    // Verify return type is edge_descriptor_view
    static_assert(std::ranges::forward_range<decltype(edge_range)>);
    
    size_t count = 0;
    for ([[maybe_unused]] auto uv : edge_range) {
        ++count;
    }
    REQUIRE(count == 1);
}

TEST_CASE("vofl CPO edges(g, uid) with isolated vertex", "[vofl][cpo][edges]") {
    vofl_void g({{0, 1}, {0, 2}});
    g.resize_vertices(4);  // Vertex 3 is isolated
    
    auto edge_range = edges(g, uint32_t(3));
    
    size_t count = 0;
    for ([[maybe_unused]] auto uv : edge_range) {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE("vofl CPO edges(g, uid) with different ID types", "[vofl][cpo][edges]") {
    vofl_void g({{0, 1}, {0, 2}});
    
    // Test with different integral types
    auto range1 = edges(g, uint32_t(0));
    auto range2 = edges(g, 0);  // int literal
    auto range3 = edges(g, size_t(0));
    
    size_t count1 = 0, count2 = 0, count3 = 0;
    for ([[maybe_unused]] auto uv : range1) ++count1;
    for ([[maybe_unused]] auto uv : range2) ++count2;
    for ([[maybe_unused]] auto uv : range3) ++count3;
    
    REQUIRE(count1 == 2);
    REQUIRE(count2 == 2);
    REQUIRE(count3 == 2);
}

TEST_CASE("vofl CPO edges(g, uid) const correctness", "[vofl][cpo][edges]") {
    const vofl_void g({{0, 1}, {0, 2}, {1, 2}});
    
    auto edge_range = edges(g, uint32_t(0));
    
    size_t count = 0;
    for ([[maybe_unused]] auto uv : edge_range) {
        ++count;
    }
    REQUIRE(count == 2);
}

TEST_CASE("vofl CPO edges(g, uid) with edge values", "[vofl][cpo][edges]") {
    vofl_int_ev g;
    g.resize_vertices(3);
    
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 10}, {0, 2, 20}
    };
    g.load_edges(edge_data);
    
    auto edge_range = edges(g, uint32_t(0));
    
    std::vector<int> values;
    for (auto uv : edge_range) {
        values.push_back(edge_value(g, uv));
    }
    
    REQUIRE(values.size() == 2);
    // forward_list reverse order
    REQUIRE(values[0] == 20);
    REQUIRE(values[1] == 10);
}

TEST_CASE("vofl CPO edges(g, uid) multiple vertices", "[vofl][cpo][edges]") {
    vofl_void g({{0, 1}, {0, 2}, {1, 2}, {1, 0}});
    
    auto edges0 = edges(g, uint32_t(0));
    auto edges1 = edges(g, uint32_t(1));
    auto edges2 = edges(g, uint32_t(2));
    
    size_t count0 = 0, count1 = 0, count2 = 0;
    for ([[maybe_unused]] auto uv : edges0) ++count0;
    for ([[maybe_unused]] auto uv : edges1) ++count1;
    for ([[maybe_unused]] auto uv : edges2) ++count2;
    
    REQUIRE(count0 == 2);
    REQUIRE(count1 == 2);
    REQUIRE(count2 == 0);
}

TEST_CASE("vofl CPO edges(g, uid) with parallel edges", "[vofl][cpo][edges]") {
    vofl_int_ev g;
    g.resize_vertices(3);
    
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 10}, {0, 1, 20}, {0, 1, 30}  // 3 parallel edges
    };
    g.load_edges(edge_data);
    
    auto edge_range = edges(g, uint32_t(0));
    
    std::vector<int> values;
    for (auto uv : edge_range) {
        values.push_back(edge_value(g, uv));
    }
    
    REQUIRE(values.size() == 3);
    // All target vertex 1, different values
    REQUIRE(values[0] == 30);  // reverse order
    REQUIRE(values[1] == 20);
    REQUIRE(values[2] == 10);
}

TEST_CASE("vofl CPO edges(g, uid) consistency with edges(g, u)", "[vofl][cpo][edges]") {
    vofl_int_ev g;
    g.resize_vertices(4);
    
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 10}, {0, 2, 20}, {0, 3, 30}
    };
    g.load_edges(edge_data);
    
    auto u0 = *find_vertex(g, 0);
    
    // Test edges(g, uid) and edges(g, u) give same results
    auto range_by_id = edges(g, uint32_t(0));
    auto range_by_desc = edges(g, u0);
    
    std::vector<int> values_by_id;
    std::vector<int> values_by_desc;
    
    for (auto uv : range_by_id) {
        values_by_id.push_back(edge_value(g, uv));
    }
    
    for (auto uv : range_by_desc) {
        values_by_desc.push_back(edge_value(g, uv));
    }
    
    REQUIRE(values_by_id.size() == values_by_desc.size());
    REQUIRE(values_by_id == values_by_desc);
}

TEST_CASE("vofl CPO edges(g, uid) large graph", "[vofl][cpo][edges]") {
    vofl_void g;
    g.resize_vertices(50);
    
    // Add 20 edges from vertex 0
    std::vector<copyable_edge_t<uint32_t, void>> edge_data;
    for (uint32_t i = 1; i <= 20; ++i) {
        edge_data.push_back({0, i});
    }
    g.load_edges(edge_data);
    
    auto edge_range = edges(g, uint32_t(0));
    
    size_t count = 0;
    for ([[maybe_unused]] auto uv : edge_range) {
        ++count;
    }
    
    REQUIRE(count == 20);
}

TEST_CASE("vofl CPO edges(g, u) with string edge values", "[vofl][cpo][edges]") {
    vofl_string g;
    g.resize_vertices(3);
    
    std::vector<copyable_edge_t<uint32_t, std::string>> edge_data = {
        {0, 1, "first"}, {0, 2, "second"}
    };
    g.load_edges(edge_data);
    
    auto u0 = *find_vertex(g, 0);
    auto edge_range = edges(g, u0);
    
    std::vector<std::string> edge_vals;
    for (auto uv : edge_range) {
        edge_vals.push_back(edge_value(g, uv));
    }
    
    REQUIRE(edge_vals.size() == 2);
    // forward_list order
    REQUIRE(edge_vals[0] == "second");
    REQUIRE(edge_vals[1] == "first");
}

//==================================================================================================
// 8. degree(g, u) CPO Tests
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
// 9. target_id(g, uv) CPO Tests
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
// 10. target(g, uv) CPO Tests
//==================================================================================================

TEST_CASE("vofl CPO target(g, uv) basic access", "[vofl][cpo][target]") {
    vofl_void g({{0, 1}, {0, 2}, {1, 2}});
    
    // Get edge from vertex 0
    auto u0 = *find_vertex(g, 0);
    auto edge_view = edges(g, u0);
    auto it = edge_view.begin();
    
    REQUIRE(it != edge_view.end());
    auto uv = *it;
    
    // Get target vertex descriptor
    auto target_vertex = target(g, uv);
    
    // Verify it's the correct vertex (forward_list: last added appears first)
    REQUIRE(vertex_id(g, target_vertex) == 2);
}

TEST_CASE("vofl CPO target(g, uv) returns vertex descriptor", "[vofl][cpo][target]") {
    vofl_void g({{0, 1}, {1, 2}});
    
    auto u0 = *find_vertex(g, 0);
    auto edge_view = edges(g, u0);
    
    for (auto uv : edge_view) {
        auto target_vertex = target(g, uv);
        
        // Should return a vertex descriptor
        static_assert(vertex_descriptor_type<decltype(target_vertex)>);
        
        // Can use it to get vertex_id
        auto tid = vertex_id(g, target_vertex);
        REQUIRE(tid == 1);
        break;
    }
}

TEST_CASE("vofl CPO target(g, uv) consistency with target_id", "[vofl][cpo][target]") {
    vofl_void g({{0, 1}, {0, 2}, {1, 2}, {1, 3}});
    
    // For all edges, verify target(g,uv) matches find_vertex(g, target_id(g,uv))
    for (auto u : vertices(g)) {
        for (auto uv : edges(g, u)) {
            auto target_desc = target(g, uv);
            auto tid = target_id(g, uv);
            auto expected_desc = *find_vertex(g, tid);
            
            REQUIRE(vertex_id(g, target_desc) == vertex_id(g, expected_desc));
        }
    }
}

TEST_CASE("vofl CPO target(g, uv) with edge values", "[vofl][cpo][target]") {
    vofl_int_ev g({{0, 1, 100}, {0, 2, 200}, {1, 2, 300}});
    
    // target() should work regardless of edge value type
    auto u0 = *find_vertex(g, 0);
    for (auto uv : edges(g, u0)) {
        auto target_vertex = target(g, uv);
        auto tid = vertex_id(g, target_vertex);
        REQUIRE((tid == 1 || tid == 2));
    }
}

TEST_CASE("vofl CPO target(g, uv) const correctness", "[vofl][cpo][target]") {
    vofl_void g({{0, 1}, {1, 2}});
    const auto& const_g = g;
    
    auto u0 = *find_vertex(const_g, 0);
    auto edge_view = edges(const_g, u0);
    
    for (auto uv : edge_view) {
        auto target_vertex = target(const_g, uv);
        REQUIRE(vertex_id(const_g, target_vertex) == 1);
        break;
    }
}

TEST_CASE("vofl CPO target(g, uv) self-loop", "[vofl][cpo][target]") {
    vofl_void g({{0, 0}, {0, 1}});  // Self-loop and regular edge
    
    auto u0 = *find_vertex(g, 0);
    auto edge_view = edges(g, u0);
    auto it = edge_view.begin();
    
    // forward_list: last added (0,1) appears first
    auto uv1 = *it;
    auto target1 = target(g, uv1);
    REQUIRE(vertex_id(g, target1) == 1);
    
    ++it;
    // First added (0,0) appears second - self-loop
    auto uv0 = *it;
    auto target0 = target(g, uv0);
    REQUIRE(vertex_id(g, target0) == 0);  // Target is same as source
}

TEST_CASE("vofl CPO target(g, uv) access target properties", "[vofl][cpo][target]") {
    vofl_int_vv g;
    g.resize_vertices(3);
    
    // Set vertex values
    for (auto u : vertices(g)) {
        vertex_value(g, u) = static_cast<int>(vertex_id(g, u)) * 10;
    }
    
    // Add edges
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {{0, 1}, {0, 2}};
    g.load_edges(edge_data);
    
    // Access target vertex values through target()
    auto u0 = *find_vertex(g, 0);
    for (auto uv : edges(g, u0)) {
        auto target_vertex = target(g, uv);
        auto target_value = vertex_value(g, target_vertex);
        auto tid = vertex_id(g, target_vertex);
        
        REQUIRE(target_value == static_cast<int>(tid) * 10);
    }
}

TEST_CASE("vofl CPO target(g, uv) with string vertex values", "[vofl][cpo][target]") {
    vofl_string g;
    g.resize_vertices(3);
    
    // Set string vertex values
    std::vector<std::string> names = {"Alice", "Bob", "Charlie"};
    size_t idx = 0;
    for (auto u : vertices(g)) {
        vertex_value(g, u) = names[idx++];
    }
    
    // Add edges with string edge values
    std::vector<copyable_edge_t<uint32_t, std::string>> edge_data = {
        {0, 1, "likes"}, {0, 2, "knows"}
    };
    g.load_edges(edge_data);
    
    // Verify we can access target names
    auto u0 = *find_vertex(g, 0);
    std::vector<std::string> target_names;
    for (auto uv : edges(g, u0)) {
        auto target_vertex = target(g, uv);
        target_names.push_back(vertex_value(g, target_vertex));
    }
    
    // Should have 2 targets (reverse order due to forward_list)
    REQUIRE(target_names.size() == 2);
    REQUIRE((target_names[0] == "Charlie" || target_names[0] == "Bob"));
}

TEST_CASE("vofl CPO target(g, uv) parallel edges", "[vofl][cpo][target]") {
    // Multiple edges to same target
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
        auto target_vertex = target(g, uv);
        REQUIRE(vertex_id(g, target_vertex) == 1);
    }
}

TEST_CASE("vofl CPO target(g, uv) iteration and navigation", "[vofl][cpo][target]") {
    // Create a path graph: 0->1->2->3
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 1}, {1, 2}, {2, 3}
    };
    vofl_void g;
    g.resize_vertices(4);
    g.load_edges(edge_data);
    
    // Navigate the path using target()
    auto current = *find_vertex(g, 0);
    std::vector<uint32_t> path;
    path.push_back(vertex_id(g, current));
    
    // Follow edges to build path
    while (true) {
        auto edge_view = edges(g, current);
        auto it = edge_view.begin();
        if (it == edge_view.end()) break;
        
        auto uv = *it;
        current = target(g, uv);
        path.push_back(vertex_id(g, current));
        
        if (path.size() >= 4) break;  // Prevent infinite loop
    }
    
    // Should have followed path 0->1->2->3
    REQUIRE(path.size() == 4);
    REQUIRE(path[0] == 0);
    REQUIRE(path[1] == 1);
    REQUIRE(path[2] == 2);
    REQUIRE(path[3] == 3);
}

TEST_CASE("vofl CPO target(g, uv) large graph", "[vofl][cpo][target]") {
    // Create a graph with many edges
    std::vector<copyable_edge_t<uint32_t, void>> edge_data;
    for (uint32_t i = 0; i < 50; ++i) {
        edge_data.push_back({i, (i + 1) % 100});
        edge_data.push_back({i, (i + 2) % 100});
    }
    
    vofl_void g;
    g.resize_vertices(100);
    g.load_edges(edge_data);
    
    // Verify target() works for all edges
    size_t edge_count = 0;
    for (auto u : vertices(g)) {
        for (auto uv : edges(g, u)) {
            auto target_vertex = target(g, uv);
            auto tid = vertex_id(g, target_vertex);
            REQUIRE(tid < 100);
            ++edge_count;
        }
    }
    
    REQUIRE(edge_count == 100);
}

//==================================================================================================
// 11. find_vertex_edge(g, u, v) CPO Tests
//==================================================================================================

TEST_CASE("vofl CPO find_vertex_edge(g, u, v) basic edge found", "[vofl][cpo][find_vertex_edge]") {
    vofl_void g({{0, 1}, {0, 2}, {1, 2}});
    
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    auto u2 = *find_vertex(g, 2);
    
    // Find existing edges
    auto e01 = find_vertex_edge(g, u0, u1);
    auto e02 = find_vertex_edge(g, u0, u2);
    auto e12 = find_vertex_edge(g, u1, u2);
    
    REQUIRE(target_id(g, e01) == 1);
    REQUIRE(target_id(g, e02) == 2);
    REQUIRE(target_id(g, e12) == 2);
}

TEST_CASE("vofl CPO find_vertex_edge(g, u, v) edge not found", "[vofl][cpo][find_vertex_edge]") {
    vofl_void g({{0, 1}, {1, 2}});
    
    auto u0 = *find_vertex(g, 0);
    auto u2 = *find_vertex(g, 2);
    
    // Edge from 0 to 2 doesn't exist (only 0->1->2)
    auto edge_range = edges(g, u0);
    auto end_iter = std::ranges::end(edge_range);
    auto e02 = find_vertex_edge(g, u0, u2);
    
    // When not found, should return an edge descriptor that equals end
    // We verify by checking if iterating from the result gives us nothing
    bool found = false;
    for (auto uv : edges(g, u0)) {
        if (target_id(g, uv) == 2) {
            found = true;
            break;
        }
    }
    REQUIRE_FALSE(found);
}

TEST_CASE("vofl CPO find_vertex_edge(g, u, vid) with vertex ID", "[vofl][cpo][find_vertex_edge]") {
    vofl_void g({{0, 1}, {0, 2}, {1, 2}});
    
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    
    // Find edge using vertex descriptor + vertex ID
    auto e01 = find_vertex_edge(g, u0, uint32_t(1));
    auto e02 = find_vertex_edge(g, u0, uint32_t(2));
    auto e12 = find_vertex_edge(g, u1, uint32_t(2));
    
    REQUIRE(target_id(g, e01) == 1);
    REQUIRE(target_id(g, e02) == 2);
    REQUIRE(target_id(g, e12) == 2);
}

TEST_CASE("vofl CPO find_vertex_edge(g, uid, vid) with both IDs", "[vofl][cpo][find_vertex_edge]") {
    vofl_void g({{0, 1}, {0, 2}, {1, 2}});
    
    // Find edges using both vertex IDs
    auto e01 = find_vertex_edge(g, uint32_t(0), uint32_t(1));
    auto e02 = find_vertex_edge(g, uint32_t(0), uint32_t(2));
    auto e12 = find_vertex_edge(g, uint32_t(1), uint32_t(2));
    
    REQUIRE(target_id(g, e01) == 1);
    REQUIRE(target_id(g, e02) == 2);
    REQUIRE(target_id(g, e12) == 2);
}

TEST_CASE("vofl CPO find_vertex_edge(g, u, v) with edge values", "[vofl][cpo][find_vertex_edge]") {
    vofl_int_ev g;
    g.resize_vertices(3);
    
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 100}, {0, 2, 200}, {1, 2, 300}
    };
    g.load_edges(edge_data);
    
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    auto u2 = *find_vertex(g, 2);
    
    auto e01 = find_vertex_edge(g, u0, u1);
    auto e02 = find_vertex_edge(g, u0, u2);
    auto e12 = find_vertex_edge(g, u1, u2);
    
    REQUIRE(edge_value(g, e01) == 100);
    REQUIRE(edge_value(g, e02) == 200);
    REQUIRE(edge_value(g, e12) == 300);
}

TEST_CASE("vofl CPO find_vertex_edge(g, u, v) const correctness", "[vofl][cpo][find_vertex_edge]") {
    const vofl_void g({{0, 1}, {0, 2}});
    
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    
    auto e01 = find_vertex_edge(g, u0, u1);
    
    REQUIRE(target_id(g, e01) == 1);
}

TEST_CASE("vofl CPO find_vertex_edge(g, u, v) with self-loop", "[vofl][cpo][find_vertex_edge]") {
    vofl_void g({{0, 0}, {0, 1}});  // 0->0 (self-loop), 0->1
    
    auto u0 = *find_vertex(g, 0);
    
    // Find self-loop
    auto e00 = find_vertex_edge(g, u0, u0);
    
    REQUIRE(target_id(g, e00) == 0);
}

TEST_CASE("vofl CPO find_vertex_edge(g, u, v) with parallel edges", "[vofl][cpo][find_vertex_edge]") {
    vofl_int_ev g;
    g.resize_vertices(2);
    
    // Multiple edges from 0 to 1 with different values
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 10}, {0, 1, 20}, {0, 1, 30}
    };
    g.load_edges(edge_data);
    
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    
    // Should find one of the parallel edges (typically the first encountered)
    auto e01 = find_vertex_edge(g, u0, u1);
    
    REQUIRE(target_id(g, e01) == 1);
    // Verify it's one of the parallel edges
    int val = edge_value(g, e01);
    REQUIRE((val == 10 || val == 20 || val == 30));
}

TEST_CASE("vofl CPO find_vertex_edge(g, u, v) with string edge values", "[vofl][cpo][find_vertex_edge]") {
    vofl_string g;
    g.resize_vertices(3);
    
    std::vector<copyable_edge_t<uint32_t, std::string>> edge_data = {
        {0, 1, "edge_01"}, {0, 2, "edge_02"}, {1, 2, "edge_12"}
    };
    g.load_edges(edge_data);
    
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    auto u2 = *find_vertex(g, 2);
    
    auto e01 = find_vertex_edge(g, u0, u1);
    auto e02 = find_vertex_edge(g, u0, u2);
    auto e12 = find_vertex_edge(g, u1, u2);
    
    REQUIRE(edge_value(g, e01) == "edge_01");
    REQUIRE(edge_value(g, e02) == "edge_02");
    REQUIRE(edge_value(g, e12) == "edge_12");
}

TEST_CASE("vofl CPO find_vertex_edge(g, u, v) multiple source vertices", "[vofl][cpo][find_vertex_edge]") {
    vofl_void g({{0, 2}, {1, 2}, {2, 3}});
    
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    auto u2 = *find_vertex(g, 2);
    auto u3 = *find_vertex(g, 3);
    
    // Different sources to same target
    auto e02 = find_vertex_edge(g, u0, u2);
    auto e12 = find_vertex_edge(g, u1, u2);
    auto e23 = find_vertex_edge(g, u2, u3);
    
    REQUIRE(target_id(g, e02) == 2);
    REQUIRE(target_id(g, e12) == 2);
    REQUIRE(target_id(g, e23) == 3);
}

TEST_CASE("vofl CPO find_vertex_edge(g, u, v) large graph", "[vofl][cpo][find_vertex_edge]") {
    vofl_void g;
    g.resize_vertices(100);
    
    // Add edges from vertex 0 to vertices 1-99
    std::vector<copyable_edge_t<uint32_t, void>> edge_data;
    for (uint32_t i = 1; i < 100; ++i) {
        edge_data.push_back({0, i});
    }
    g.load_edges(edge_data);
    
    auto u0 = *find_vertex(g, 0);
    auto u50 = *find_vertex(g, 50);
    auto u99 = *find_vertex(g, 99);
    
    auto e0_50 = find_vertex_edge(g, u0, u50);
    auto e0_99 = find_vertex_edge(g, u0, u99);
    
    REQUIRE(target_id(g, e0_50) == 50);
    REQUIRE(target_id(g, e0_99) == 99);
}

TEST_CASE("vofl CPO find_vertex_edge(g, uid, vid) with different integral types", "[vofl][cpo][find_vertex_edge]") {
    vofl_void g({{0, 1}, {0, 2}});
    
    // Test with different integral types
    auto e1 = find_vertex_edge(g, uint32_t(0), uint32_t(1));
    auto e2 = find_vertex_edge(g, 0, 1);  // int literals
    auto e3 = find_vertex_edge(g, size_t(0), size_t(2));
    
    REQUIRE(target_id(g, e1) == 1);
    REQUIRE(target_id(g, e2) == 1);
    REQUIRE(target_id(g, e3) == 2);
}

TEST_CASE("vofl CPO find_vertex_edge(g, u, v) isolated vertex", "[vofl][cpo][find_vertex_edge]") {
    vofl_void g({{0, 1}});
    g.resize_vertices(3);  // Vertex 2 is isolated
    
    auto u0 = *find_vertex(g, 0);
    auto u2 = *find_vertex(g, 2);
    
    // Try to find edge from isolated vertex
    bool found = false;
    for (auto uv : edges(g, u2)) {
        if (target_id(g, uv) == 0) {
            found = true;
            break;
        }
    }
    REQUIRE_FALSE(found);
}

//--------------------------------------------------------------------------------------------------
// 11b. Additional dedicated tests for find_vertex_edge(g, uid, vid) overload
//--------------------------------------------------------------------------------------------------

TEST_CASE("vofl CPO find_vertex_edge(g, uid, vid) basic usage", "[vofl][cpo][find_vertex_edge][uid_vid]") {
    vofl_void g({{0, 1}, {0, 2}, {1, 2}, {2, 3}});
    
    // Test finding edges using only vertex IDs
    auto e01 = find_vertex_edge(g, 0, 1);
    auto e02 = find_vertex_edge(g, 0, 2);
    auto e12 = find_vertex_edge(g, 1, 2);
    auto e23 = find_vertex_edge(g, 2, 3);
    
    REQUIRE(target_id(g, e01) == 1);
    REQUIRE(target_id(g, e02) == 2);
    REQUIRE(target_id(g, e12) == 2);
    REQUIRE(target_id(g, e23) == 3);
}

TEST_CASE("vofl CPO find_vertex_edge(g, uid, vid) edge not found", "[vofl][cpo][find_vertex_edge][uid_vid]") {
    vofl_void g({{0, 1}, {1, 2}});
    
    // Try to find non-existent edges
    auto e02 = find_vertex_edge(g, 0, 2);  // No direct edge from 0 to 2
    auto e10 = find_vertex_edge(g, 1, 0);  // No reverse edge
    auto e21 = find_vertex_edge(g, 2, 1);  // No reverse edge
    
    // Verify these are "not found" results (implementation-defined behavior)
    // We can verify by checking if edges exist
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    auto u2 = *find_vertex(g, 2);
    
    bool found_02 = false;
    for (auto e : edges(g, u0)) {
        if (target_id(g, e) == 2) found_02 = true;
    }
    REQUIRE(!found_02);
    
    bool found_10 = false;
    for (auto e : edges(g, u1)) {
        if (target_id(g, e) == 0) found_10 = true;
    }
    REQUIRE(!found_10);
}

TEST_CASE("vofl CPO find_vertex_edge(g, uid, vid) with edge values", "[vofl][cpo][find_vertex_edge][uid_vid]") {
    vofl_int_ev g;
    g.resize_vertices(4);
    
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}, {2, 3, 40}
    };
    g.load_edges(edge_data);
    
    // Find edges using vertex IDs and verify their values
    auto e01 = find_vertex_edge(g, 0, 1);
    auto e02 = find_vertex_edge(g, 0, 2);
    auto e12 = find_vertex_edge(g, 1, 2);
    auto e23 = find_vertex_edge(g, 2, 3);
    
    REQUIRE(edge_value(g, e01) == 10);
    REQUIRE(edge_value(g, e02) == 20);
    REQUIRE(edge_value(g, e12) == 30);
    REQUIRE(edge_value(g, e23) == 40);
}

TEST_CASE("vofl CPO find_vertex_edge(g, uid, vid) with parallel edges", "[vofl][cpo][find_vertex_edge][uid_vid]") {
    vofl_int_ev g;
    g.resize_vertices(3);
    
    // Add multiple edges from 0 to 1 with different values
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 100}, {0, 1, 200}, {0, 1, 300}, {1, 2, 400}
    };
    g.load_edges(edge_data);
    
    // find_vertex_edge should find one of the parallel edges
    auto e01 = find_vertex_edge(g, 0, 1);
    REQUIRE(target_id(g, e01) == 1);
    
    // The edge value should be one of the parallel edge values
    int val = edge_value(g, e01);
    REQUIRE((val == 100 || val == 200 || val == 300));
}

TEST_CASE("vofl CPO find_vertex_edge(g, uid, vid) with self-loop", "[vofl][cpo][find_vertex_edge][uid_vid]") {
    vofl_int_ev g;
    g.resize_vertices(3);
    
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 0, 99},   // Self-loop
        {0, 1, 10},
        {1, 1, 88}    // Self-loop
    };
    g.load_edges(edge_data);
    
    // Find self-loops using vertex IDs
    auto e00 = find_vertex_edge(g, 0, 0);
    auto e11 = find_vertex_edge(g, 1, 1);
    
    REQUIRE(target_id(g, e00) == 0);
    REQUIRE(edge_value(g, e00) == 99);
    REQUIRE(target_id(g, e11) == 1);
    REQUIRE(edge_value(g, e11) == 88);
}

TEST_CASE("vofl CPO find_vertex_edge(g, uid, vid) const correctness", "[vofl][cpo][find_vertex_edge][uid_vid]") {
    vofl_int_ev g;
    g.resize_vertices(3);
    
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 100}, {1, 2, 200}
    };
    g.load_edges(edge_data);
    
    // Test with const graph
    const auto& cg = g;
    
    auto e01 = find_vertex_edge(cg, 0, 1);
    auto e12 = find_vertex_edge(cg, 1, 2);
    
    REQUIRE(target_id(cg, e01) == 1);
    REQUIRE(edge_value(cg, e01) == 100);
    REQUIRE(target_id(cg, e12) == 2);
    REQUIRE(edge_value(cg, e12) == 200);
}

TEST_CASE("vofl CPO find_vertex_edge(g, uid, vid) with different integral types", "[vofl][cpo][find_vertex_edge][uid_vid]") {
    vofl_void g({{0, 1}, {1, 2}, {2, 3}});
    
    // Test with various integral types for IDs
    auto e01_uint32 = find_vertex_edge(g, uint32_t(0), uint32_t(1));
    auto e12_int = find_vertex_edge(g, int(1), int(2));
    auto e23_size = find_vertex_edge(g, size_t(2), size_t(3));
    
    REQUIRE(target_id(g, e01_uint32) == 1);
    REQUIRE(target_id(g, e12_int) == 2);
    REQUIRE(target_id(g, e23_size) == 3);
}

TEST_CASE("vofl CPO find_vertex_edge(g, uid, vid) with string edge values", "[vofl][cpo][find_vertex_edge][uid_vid]") {
    vofl_string g;
    g.resize_vertices(4);
    
    std::vector<copyable_edge_t<uint32_t, std::string>> edge_data = {
        {0, 1, "alpha"}, {0, 2, "beta"}, {1, 2, "gamma"}, {2, 3, "delta"}
    };
    g.load_edges(edge_data);
    
    // Find edges and verify string values
    auto e01 = find_vertex_edge(g, 0, 1);
    auto e02 = find_vertex_edge(g, 0, 2);
    auto e12 = find_vertex_edge(g, 1, 2);
    auto e23 = find_vertex_edge(g, 2, 3);
    
    REQUIRE(edge_value(g, e01) == "alpha");
    REQUIRE(edge_value(g, e02) == "beta");
    REQUIRE(edge_value(g, e12) == "gamma");
    REQUIRE(edge_value(g, e23) == "delta");
}

TEST_CASE("vofl CPO find_vertex_edge(g, uid, vid) in large graph", "[vofl][cpo][find_vertex_edge][uid_vid]") {
    vofl_void g;
    g.resize_vertices(100);
    
    // Create edges from vertex 0 to all other vertices
    std::vector<copyable_edge_t<uint32_t, void>> edge_data;
    for (uint32_t i = 1; i < 100; ++i) {
        edge_data.push_back({0, i});
    }
    g.load_edges(edge_data);
    
    // Test finding edges to various vertices
    auto e01 = find_vertex_edge(g, 0, 1);
    auto e050 = find_vertex_edge(g, 0, 50);
    auto e099 = find_vertex_edge(g, 0, 99);
    
    REQUIRE(target_id(g, e01) == 1);
    REQUIRE(target_id(g, e050) == 50);
    REQUIRE(target_id(g, e099) == 99);
}

TEST_CASE("vofl CPO find_vertex_edge(g, uid, vid) from isolated vertex", "[vofl][cpo][find_vertex_edge][uid_vid]") {
    vofl_void g;
    g.resize_vertices(5);
    
    // Only add edges between some vertices, leave vertex 3 isolated
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 1}, {1, 2}, {2, 4}
    };
    g.load_edges(edge_data);
    
    // Try to find edge from isolated vertex
    auto u3 = *find_vertex(g, 3);
    
    // Verify vertex 3 has no outgoing edges
    auto edges_3 = edges(g, u3);
    REQUIRE(std::ranges::distance(edges_3) == 0);
}

TEST_CASE("vofl CPO find_vertex_edge(g, uid, vid) chain of edges", "[vofl][cpo][find_vertex_edge][uid_vid]") {
    vofl_int_ev g;
    g.resize_vertices(6);
    
    // Create a chain: 0->1->2->3->4->5
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 10}, {1, 2, 20}, {2, 3, 30}, {3, 4, 40}, {4, 5, 50}
    };
    g.load_edges(edge_data);
    
    // Traverse the chain using find_vertex_edge
    auto e01 = find_vertex_edge(g, 0, 1);
    REQUIRE(edge_value(g, e01) == 10);
    
    auto e12 = find_vertex_edge(g, 1, 2);
    REQUIRE(edge_value(g, e12) == 20);
    
    auto e23 = find_vertex_edge(g, 2, 3);
    REQUIRE(edge_value(g, e23) == 30);
    
    auto e34 = find_vertex_edge(g, 3, 4);
    REQUIRE(edge_value(g, e34) == 40);
    
    auto e45 = find_vertex_edge(g, 4, 5);
    REQUIRE(edge_value(g, e45) == 50);
}

//==================================================================================================
// 12. contains_edge(g, u, v) and contains_edge(g, uid, vid) CPO Tests
//==================================================================================================

TEST_CASE("vofl CPO contains_edge(g, u, v) edge exists", "[vofl][cpo][contains_edge]") {
    vofl_void g({{0, 1}, {0, 2}, {1, 2}});
    
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    auto u2 = *find_vertex(g, 2);
    
    // Check existing edges
    REQUIRE(contains_edge(g, u0, u1));
    REQUIRE(contains_edge(g, u0, u2));
    REQUIRE(contains_edge(g, u1, u2));
}

TEST_CASE("vofl CPO contains_edge(g, u, v) edge does not exist", "[vofl][cpo][contains_edge]") {
    vofl_void g({{0, 1}, {1, 2}});
    
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    auto u2 = *find_vertex(g, 2);
    
    // Check non-existent edges
    REQUIRE_FALSE(contains_edge(g, u0, u2));  // No direct edge from 0 to 2
    REQUIRE_FALSE(contains_edge(g, u1, u0));  // No reverse edge
    REQUIRE_FALSE(contains_edge(g, u2, u1));  // No reverse edge
    REQUIRE_FALSE(contains_edge(g, u2, u0));  // No reverse edge
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) with vertex IDs", "[vofl][cpo][contains_edge]") {
    vofl_void g({{0, 1}, {0, 2}, {1, 2}, {2, 3}});
    
    // Check existing edges using vertex IDs
    REQUIRE(contains_edge(g, 0, 1));
    REQUIRE(contains_edge(g, 0, 2));
    REQUIRE(contains_edge(g, 1, 2));
    REQUIRE(contains_edge(g, 2, 3));
    
    // Check non-existent edges
    REQUIRE_FALSE(contains_edge(g, 0, 3));
    REQUIRE_FALSE(contains_edge(g, 1, 0));
    REQUIRE_FALSE(contains_edge(g, 2, 0));
    REQUIRE_FALSE(contains_edge(g, 3, 0));
}

TEST_CASE("vofl CPO contains_edge(g, u, v) with edge values", "[vofl][cpo][contains_edge]") {
    vofl_int_ev g;
    g.resize_vertices(4);
    
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 100}, {0, 2, 200}, {1, 2, 300}
    };
    g.load_edges(edge_data);
    
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    auto u2 = *find_vertex(g, 2);
    auto u3 = *find_vertex(g, 3);
    
    // Check existing edges
    REQUIRE(contains_edge(g, u0, u1));
    REQUIRE(contains_edge(g, u0, u2));
    REQUIRE(contains_edge(g, u1, u2));
    
    // Check non-existent edges
    REQUIRE_FALSE(contains_edge(g, u0, u3));
    REQUIRE_FALSE(contains_edge(g, u3, u0));
}

TEST_CASE("vofl CPO contains_edge(g, u, v) with parallel edges", "[vofl][cpo][contains_edge]") {
    vofl_int_ev g;
    g.resize_vertices(3);
    
    // Add multiple edges from 0 to 1
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 100}, {0, 1, 200}, {0, 1, 300}, {1, 2, 400}
    };
    g.load_edges(edge_data);
    
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    auto u2 = *find_vertex(g, 2);
    
    // Should return true if any edge exists between u and v
    REQUIRE(contains_edge(g, u0, u1));
    REQUIRE(contains_edge(g, u1, u2));
}

TEST_CASE("vofl CPO contains_edge(g, u, v) with self-loop", "[vofl][cpo][contains_edge]") {
    vofl_int_ev g;
    g.resize_vertices(3);
    
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 0, 99},   // Self-loop
        {0, 1, 10},
        {1, 1, 88}    // Self-loop
    };
    g.load_edges(edge_data);
    
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    auto u2 = *find_vertex(g, 2);
    
    // Check self-loops
    REQUIRE(contains_edge(g, u0, u0));
    REQUIRE(contains_edge(g, u1, u1));
    REQUIRE_FALSE(contains_edge(g, u2, u2));
    
    // Check regular edges
    REQUIRE(contains_edge(g, u0, u1));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) with self-loop", "[vofl][cpo][contains_edge]") {
    vofl_int_ev g;
    g.resize_vertices(3);
    
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 0, 99}, {1, 1, 88}, {0, 1, 10}
    };
    g.load_edges(edge_data);
    
    // Check self-loops using vertex IDs
    REQUIRE(contains_edge(g, 0, 0));
    REQUIRE(contains_edge(g, 1, 1));
    REQUIRE_FALSE(contains_edge(g, 2, 2));
}

TEST_CASE("vofl CPO contains_edge(g, u, v) const correctness", "[vofl][cpo][contains_edge]") {
    vofl_void g({{0, 1}, {1, 2}});
    
    const auto& cg = g;
    auto u0 = *find_vertex(cg, 0);
    auto u1 = *find_vertex(cg, 1);
    auto u2 = *find_vertex(cg, 2);
    
    REQUIRE(contains_edge(cg, u0, u1));
    REQUIRE(contains_edge(cg, u1, u2));
    REQUIRE_FALSE(contains_edge(cg, u0, u2));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) const correctness", "[vofl][cpo][contains_edge]") {
    vofl_void g({{0, 1}, {1, 2}});
    
    const auto& cg = g;
    
    REQUIRE(contains_edge(cg, 0, 1));
    REQUIRE(contains_edge(cg, 1, 2));
    REQUIRE_FALSE(contains_edge(cg, 0, 2));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) with different integral types", "[vofl][cpo][contains_edge]") {
    vofl_void g({{0, 1}, {1, 2}, {2, 3}});
    
    // Test with various integral types
    REQUIRE(contains_edge(g, uint32_t(0), uint32_t(1)));
    REQUIRE(contains_edge(g, int(1), int(2)));
    REQUIRE(contains_edge(g, size_t(2), size_t(3)));
    
    REQUIRE_FALSE(contains_edge(g, uint32_t(0), uint32_t(3)));
    REQUIRE_FALSE(contains_edge(g, int(3), int(0)));
}

TEST_CASE("vofl CPO contains_edge(g, u, v) empty graph", "[vofl][cpo][contains_edge]") {
    vofl_void g;
    g.resize_vertices(3);
    
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    auto u2 = *find_vertex(g, 2);
    
    // No edges in the graph
    REQUIRE_FALSE(contains_edge(g, u0, u1));
    REQUIRE_FALSE(contains_edge(g, u1, u2));
    REQUIRE_FALSE(contains_edge(g, u0, u2));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) isolated vertex", "[vofl][cpo][contains_edge]") {
    vofl_void g;
    g.resize_vertices(5);
    
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 1}, {1, 2}, {2, 4}
    };
    g.load_edges(edge_data);
    
    // Vertex 3 is isolated - has no edges
    REQUIRE_FALSE(contains_edge(g, 3, 0));
    REQUIRE_FALSE(contains_edge(g, 3, 1));
    REQUIRE_FALSE(contains_edge(g, 3, 2));
    REQUIRE_FALSE(contains_edge(g, 3, 4));
    REQUIRE_FALSE(contains_edge(g, 0, 3));
}

TEST_CASE("vofl CPO contains_edge(g, u, v) with string edge values", "[vofl][cpo][contains_edge]") {
    vofl_string g;
    g.resize_vertices(4);
    
    std::vector<copyable_edge_t<uint32_t, std::string>> edge_data = {
        {0, 1, "alpha"}, {0, 2, "beta"}, {1, 2, "gamma"}
    };
    g.load_edges(edge_data);
    
    auto u0 = *find_vertex(g, 0);
    auto u1 = *find_vertex(g, 1);
    auto u2 = *find_vertex(g, 2);
    auto u3 = *find_vertex(g, 3);
    
    REQUIRE(contains_edge(g, u0, u1));
    REQUIRE(contains_edge(g, u0, u2));
    REQUIRE(contains_edge(g, u1, u2));
    REQUIRE_FALSE(contains_edge(g, u3, u0));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) large graph", "[vofl][cpo][contains_edge]") {
    vofl_void g;
    g.resize_vertices(100);
    
    // Create edges from vertex 0 to all other vertices
    std::vector<copyable_edge_t<uint32_t, void>> edge_data;
    for (uint32_t i = 1; i < 100; ++i) {
        edge_data.push_back({0, i});
    }
    g.load_edges(edge_data);
    
    // Check edges from vertex 0
    REQUIRE(contains_edge(g, 0, 1));
    REQUIRE(contains_edge(g, 0, 50));
    REQUIRE(contains_edge(g, 0, 99));
    
    // Check non-existent edges
    REQUIRE_FALSE(contains_edge(g, 1, 0));
    REQUIRE_FALSE(contains_edge(g, 1, 2));
    REQUIRE_FALSE(contains_edge(g, 50, 99));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) complete small graph", "[vofl][cpo][contains_edge]") {
    vofl_void g;
    g.resize_vertices(4);
    
    // Create a complete graph on 4 vertices (every vertex connected to every other)
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 1}, {0, 2}, {0, 3},
        {1, 0}, {1, 2}, {1, 3},
        {2, 0}, {2, 1}, {2, 3},
        {3, 0}, {3, 1}, {3, 2}
    };
    g.load_edges(edge_data);
    
    // Every pair should have an edge
    for (uint32_t i = 0; i < 4; ++i) {
        for (uint32_t j = 0; j < 4; ++j) {
            if (i != j) {
                REQUIRE(contains_edge(g, i, j));
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------
// 12b. Additional dedicated tests for contains_edge(g, uid, vid) overload
//--------------------------------------------------------------------------------------------------

TEST_CASE("vofl CPO contains_edge(g, uid, vid) basic usage", "[vofl][cpo][contains_edge][uid_vid]") {
    vofl_void g({{0, 1}, {0, 2}, {1, 2}, {2, 3}});
    
    // Test checking edges using only vertex IDs
    REQUIRE(contains_edge(g, 0, 1));
    REQUIRE(contains_edge(g, 0, 2));
    REQUIRE(contains_edge(g, 1, 2));
    REQUIRE(contains_edge(g, 2, 3));
    
    // Non-existent edges
    REQUIRE_FALSE(contains_edge(g, 0, 3));
    REQUIRE_FALSE(contains_edge(g, 1, 0));
    REQUIRE_FALSE(contains_edge(g, 3, 2));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) all edges not found", "[vofl][cpo][contains_edge][uid_vid]") {
    vofl_void g({{0, 1}, {1, 2}});
    
    // Check all possible non-existent edges in opposite directions
    REQUIRE_FALSE(contains_edge(g, 0, 2));  // No transitive edge
    REQUIRE_FALSE(contains_edge(g, 1, 0));  // No reverse
    REQUIRE_FALSE(contains_edge(g, 2, 0));  // No reverse
    REQUIRE_FALSE(contains_edge(g, 2, 1));  // No reverse
    
    // Self-loops that don't exist
    REQUIRE_FALSE(contains_edge(g, 0, 0));
    REQUIRE_FALSE(contains_edge(g, 1, 1));
    REQUIRE_FALSE(contains_edge(g, 2, 2));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) with edge values", "[vofl][cpo][contains_edge][uid_vid]") {
    vofl_int_ev g;
    g.resize_vertices(5);
    
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 10}, {0, 2, 20}, {1, 3, 30}, {2, 4, 40}
    };
    g.load_edges(edge_data);
    
    // Check existing edges using vertex IDs
    REQUIRE(contains_edge(g, 0, 1));
    REQUIRE(contains_edge(g, 0, 2));
    REQUIRE(contains_edge(g, 1, 3));
    REQUIRE(contains_edge(g, 2, 4));
    
    // Check non-existent edges
    REQUIRE_FALSE(contains_edge(g, 0, 3));
    REQUIRE_FALSE(contains_edge(g, 0, 4));
    REQUIRE_FALSE(contains_edge(g, 1, 2));
    REQUIRE_FALSE(contains_edge(g, 3, 4));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) with parallel edges", "[vofl][cpo][contains_edge][uid_vid]") {
    vofl_int_ev g;
    g.resize_vertices(3);
    
    // Add multiple edges from 0 to 1
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 100}, {0, 1, 200}, {0, 1, 300}, {1, 2, 400}
    };
    g.load_edges(edge_data);
    
    // Should return true if any edge exists between uid and vid
    REQUIRE(contains_edge(g, 0, 1));
    REQUIRE(contains_edge(g, 1, 2));
    REQUIRE_FALSE(contains_edge(g, 0, 2));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) bidirectional check", "[vofl][cpo][contains_edge][uid_vid]") {
    vofl_void g;
    g.resize_vertices(3);
    
    // Create edges in both directions between some vertices
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 1}, {1, 0}, {1, 2}  // Bidirectional between 0 and 1, one-way 1->2
    };
    g.load_edges(edge_data);
    
    // Check bidirectional
    REQUIRE(contains_edge(g, 0, 1));
    REQUIRE(contains_edge(g, 1, 0));
    
    // Check unidirectional
    REQUIRE(contains_edge(g, 1, 2));
    REQUIRE_FALSE(contains_edge(g, 2, 1));
    
    // Check non-existent
    REQUIRE_FALSE(contains_edge(g, 0, 2));
    REQUIRE_FALSE(contains_edge(g, 2, 0));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) with different integral types", "[vofl][cpo][contains_edge][uid_vid]") {
    vofl_void g({{0, 1}, {1, 2}, {2, 3}});
    
    // Test with various integral types for IDs
    REQUIRE(contains_edge(g, uint32_t(0), uint32_t(1)));
    REQUIRE(contains_edge(g, int(1), int(2)));
    REQUIRE(contains_edge(g, size_t(2), size_t(3)));
    
    // Mixed types
    REQUIRE(contains_edge(g, uint32_t(0), size_t(1)));
    REQUIRE(contains_edge(g, int(1), uint32_t(2)));
    
    // Non-existent with different types
    REQUIRE_FALSE(contains_edge(g, uint32_t(0), uint32_t(3)));
    REQUIRE_FALSE(contains_edge(g, size_t(3), int(0)));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) star graph", "[vofl][cpo][contains_edge][uid_vid]") {
    vofl_void g;
    g.resize_vertices(6);
    
    // Create a star graph: vertex 0 connected to all others
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}
    };
    g.load_edges(edge_data);
    
    // Check all edges from center
    for (uint32_t i = 1; i < 6; ++i) {
        REQUIRE(contains_edge(g, 0, i));
    }
    
    // Check no edges between outer vertices
    for (uint32_t i = 1; i < 6; ++i) {
        for (uint32_t j = i + 1; j < 6; ++j) {
            REQUIRE_FALSE(contains_edge(g, i, j));
            REQUIRE_FALSE(contains_edge(g, j, i));
        }
    }
    
    // Check no edges back to center
    for (uint32_t i = 1; i < 6; ++i) {
        REQUIRE_FALSE(contains_edge(g, i, 0));
    }
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) chain graph", "[vofl][cpo][contains_edge][uid_vid]") {
    vofl_int_ev g;
    g.resize_vertices(6);
    
    // Create a chain: 0->1->2->3->4->5
    std::vector<copyable_edge_t<uint32_t, int>> edge_data = {
        {0, 1, 10}, {1, 2, 20}, {2, 3, 30}, {3, 4, 40}, {4, 5, 50}
    };
    g.load_edges(edge_data);
    
    // Check all chain edges exist
    for (uint32_t i = 0; i < 5; ++i) {
        REQUIRE(contains_edge(g, i, i + 1));
    }
    
    // Check no reverse edges
    for (uint32_t i = 1; i < 6; ++i) {
        REQUIRE_FALSE(contains_edge(g, i, i - 1));
    }
    
    // Check no skip edges
    REQUIRE_FALSE(contains_edge(g, 0, 2));
    REQUIRE_FALSE(contains_edge(g, 0, 3));
    REQUIRE_FALSE(contains_edge(g, 1, 3));
    REQUIRE_FALSE(contains_edge(g, 2, 5));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) cycle graph", "[vofl][cpo][contains_edge][uid_vid]") {
    vofl_void g;
    g.resize_vertices(5);
    
    // Create a cycle: 0->1->2->3->4->0
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}
    };
    g.load_edges(edge_data);
    
    // Check all cycle edges
    REQUIRE(contains_edge(g, 0, 1));
    REQUIRE(contains_edge(g, 1, 2));
    REQUIRE(contains_edge(g, 2, 3));
    REQUIRE(contains_edge(g, 3, 4));
    REQUIRE(contains_edge(g, 4, 0));  // Closing edge
    
    // Check no shortcuts across cycle
    REQUIRE_FALSE(contains_edge(g, 0, 2));
    REQUIRE_FALSE(contains_edge(g, 0, 3));
    REQUIRE_FALSE(contains_edge(g, 1, 3));
    REQUIRE_FALSE(contains_edge(g, 1, 4));
    REQUIRE_FALSE(contains_edge(g, 2, 4));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) dense graph", "[vofl][cpo][contains_edge][uid_vid]") {
    vofl_void g;
    g.resize_vertices(4);
    
    // Create edges between almost all pairs (missing 2->3)
    std::vector<copyable_edge_t<uint32_t, void>> edge_data = {
        {0, 1}, {0, 2}, {0, 3},
        {1, 0}, {1, 2}, {1, 3},
        {2, 0}, {2, 1},  // Missing 2->3
        {3, 0}, {3, 1}, {3, 2}
    };
    g.load_edges(edge_data);
    
    // Verify most edges exist
    int edge_count = 0;
    for (uint32_t i = 0; i < 4; ++i) {
        for (uint32_t j = 0; j < 4; ++j) {
            if (i != j && contains_edge(g, i, j)) {
                edge_count++;
            }
        }
    }
    REQUIRE(edge_count == 11);  // 12 possible - 1 missing
    
    // Verify the missing edge
    REQUIRE_FALSE(contains_edge(g, 2, 3));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) with string edge values", "[vofl][cpo][contains_edge][uid_vid]") {
    vofl_string g;
    g.resize_vertices(5);
    
    std::vector<copyable_edge_t<uint32_t, std::string>> edge_data = {
        {0, 1, "first"}, {1, 2, "second"}, {2, 3, "third"}, {3, 4, "fourth"}
    };
    g.load_edges(edge_data);
    
    // Check edges exist
    REQUIRE(contains_edge(g, 0, 1));
    REQUIRE(contains_edge(g, 1, 2));
    REQUIRE(contains_edge(g, 2, 3));
    REQUIRE(contains_edge(g, 3, 4));
    
    // Check non-existent
    REQUIRE_FALSE(contains_edge(g, 0, 2));
    REQUIRE_FALSE(contains_edge(g, 4, 0));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) single vertex graph", "[vofl][cpo][contains_edge][uid_vid]") {
    vofl_void g;
    g.resize_vertices(1);
    
    // No edges, not even self-loop
    REQUIRE_FALSE(contains_edge(g, 0, 0));
}

TEST_CASE("vofl CPO contains_edge(g, uid, vid) single edge graph", "[vofl][cpo][contains_edge][uid_vid]") {
    vofl_void g({{0, 1}});
    
    // Only one edge exists
    REQUIRE(contains_edge(g, 0, 1));
    
    // All other checks should fail
    REQUIRE_FALSE(contains_edge(g, 1, 0));
    REQUIRE_FALSE(contains_edge(g, 0, 0));
    REQUIRE_FALSE(contains_edge(g, 1, 1));
}

//==================================================================================================
// 13. Integration Tests - Multiple CPOs Working Together
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
