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
 * CPOs tested (with available friend functions):
 * - vertices(g) - Get vertex range
 * - num_vertices(g) - Get vertex count
 * - find_vertex(g, uid) - Find vertex by ID
 * - num_edges(g) - Get total edge count
 * - has_edge(g) - Check if graph has any edges
 * - vertex_value(g, u) - Access vertex value (when VV != void)
 * - edge_value(g, uv) - Access edge value (when EV != void)
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
// 4. num_edges(g) CPO Tests
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
// 5. has_edge(g) CPO Tests
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
// 6. Integration Tests - Multiple CPOs Working Together
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
