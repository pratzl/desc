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
 * Current Status: 41 test cases, 92 assertions passing
 * 
 * CPOs tested (with available friend functions):
 * - vertices(g) - Get vertex range
 * - num_vertices(g) - Get vertex count
 * - find_vertex(g, uid) - Find vertex by ID
 * - vertex_id(g, u) - Get vertex ID from descriptor [7 tests]
 * - num_edges(g) - Get total edge count
 * - has_edge(g) - Check if graph has any edges
 * - vertex_value(g, u) - Access vertex value (when VV != void) [6 tests]
 * - edge_value(g, uv) - Access edge value (when EV != void) [6 tests + 3 integration tests]
 * 
 * Friend functions implemented and tested:
 * - vertex_value(g,u) in dynamic_graph_base (lines 1345-1348)
 * - edge_value(g,uv) in dynamic_vertex_base (lines 665-676)
 * - edges(g,u) in dynamic_vertex_base (lines 678-679)
 * 
 * Note: forward_list uses push_front() for edge insertion, so edges appear in
 * reverse order of loading. Tests account for this behavior.
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
// 7. Integration Tests - Multiple CPOs Working Together
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
// 8. vertex_value(g, u) CPO Tests
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
// 9. edge_value(g, uv) CPO Tests
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
    int idx = 0;
    
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
// 10. Integration Tests - vertex_value and edge_value Together
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
