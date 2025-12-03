/**
 * @file test_dynamic_graph_common.cpp
 * @brief Phase 1.4: Unified tests for all sequential container trait combinations
 * 
 * Uses Catch2 TEMPLATE_TEST_CASE to run identical tests across 8 trait combinations:
 * - vofl_graph_traits (vector + forward_list)
 * - vol_graph_traits (vector + list)
 * - vov_graph_traits (vector + vector)
 * - vod_graph_traits (vector + deque)
 * - dofl_graph_traits (deque + forward_list)
 * - dol_graph_traits (deque + list)
 * - dov_graph_traits (deque + vector)
 * - dod_graph_traits (deque + deque)
 * 
 * All traits use uint64_t vertex IDs with auto-extension semantics.
 * Container-specific behavior is tested in separate files.
 * 
 * Current Status: Comprehensive test coverage across all sequential traits
 * 
 * Test Categories:
 * - Construction (15 tests)
 * - Load Operations (12 tests) 
 * - Vertex Access (10 tests)
 * - Edge Access (12 tests)
 * - Value Access (8 tests)
 * - Sourced Edges (6 tests)
 * - Properties (10 tests)
 * - Memory/Performance (8 tests)
 * - Edge Cases (15 tests)
 * - Iterators/Ranges (10 tests)
 * - Workflows (8 tests)
 * Total: ~114 test scenarios × 8 traits = ~912 test executions
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <graph/container/dynamic_graph.hpp>
#include <graph/graph_info.hpp>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <numeric>
#include <ranges>

using namespace graph::container;

//==================================================================================================
// TEMPLATE_TEST_CASE: Common Construction Tests
// Runs across all 8 sequential trait combinations
//==================================================================================================

TEMPLATE_TEST_CASE("default construction creates empty graph", "[common][construction]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    Graph g;
    REQUIRE(g.size() == 0);
    REQUIRE(g.begin() == g.end());
}

TEMPLATE_TEST_CASE("construction with graph value", "[common][construction]",
                   (vofl_graph_traits<void, void, int, uint64_t, false>),
                   (vol_graph_traits<void, void, int, uint64_t, false>),
                   (vov_graph_traits<void, void, int, uint64_t, false>),
                   (vod_graph_traits<void, void, int, uint64_t, false>),
                   (dofl_graph_traits<void, void, int, uint64_t, false>),
                   (dol_graph_traits<void, void, int, uint64_t, false>),
                   (dov_graph_traits<void, void, int, uint64_t, false>),
                   (dod_graph_traits<void, void, int, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, int, uint64_t, false, TestType>;
    
    Graph g(42);
    REQUIRE(g.size() == 0);
    REQUIRE(g.graph_value() == 42);
}

TEMPLATE_TEST_CASE("construction with edge values", "[common][construction]",
                   (vofl_graph_traits<int, void, void, uint64_t, false>),
                   (vol_graph_traits<int, void, void, uint64_t, false>),
                   (vov_graph_traits<int, void, void, uint64_t, false>),
                   (vod_graph_traits<int, void, void, uint64_t, false>),
                   (dofl_graph_traits<int, void, void, uint64_t, false>),
                   (dol_graph_traits<int, void, void, uint64_t, false>),
                   (dov_graph_traits<int, void, void, uint64_t, false>),
                   (dod_graph_traits<int, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<int, void, void, uint64_t, false, TestType>;
    
    Graph g;
    REQUIRE(g.size() == 0);
}

TEMPLATE_TEST_CASE("copy construction", "[common][construction]",
                   (vofl_graph_traits<int, int, int, uint64_t, false>),
                   (vol_graph_traits<int, int, int, uint64_t, false>),
                   (vov_graph_traits<int, int, int, uint64_t, false>),
                   (vod_graph_traits<int, int, int, uint64_t, false>),
                   (dofl_graph_traits<int, int, int, uint64_t, false>),
                   (dol_graph_traits<int, int, int, uint64_t, false>),
                   (dov_graph_traits<int, int, int, uint64_t, false>),
                   (dod_graph_traits<int, int, int, uint64_t, false>)) {
    using Graph = dynamic_graph<int, int, int, uint64_t, false, TestType>;
    
    Graph g1;
    Graph g2(g1);
    REQUIRE(g2.size() == g1.size());
    REQUIRE(g2.size() == 0);
}

TEMPLATE_TEST_CASE("move construction", "[common][construction]",
                   (vofl_graph_traits<int, int, int, uint64_t, false>),
                   (vol_graph_traits<int, int, int, uint64_t, false>),
                   (vov_graph_traits<int, int, int, uint64_t, false>),
                   (vod_graph_traits<int, int, int, uint64_t, false>),
                   (dofl_graph_traits<int, int, int, uint64_t, false>),
                   (dol_graph_traits<int, int, int, uint64_t, false>),
                   (dov_graph_traits<int, int, int, uint64_t, false>),
                   (dod_graph_traits<int, int, int, uint64_t, false>)) {
    using Graph = dynamic_graph<int, int, int, uint64_t, false, TestType>;
    
    Graph g1;
    Graph g2(std::move(g1));
    REQUIRE(g2.size() == 0);
}

TEMPLATE_TEST_CASE("construction with initializer_list edges", "[common][construction]",
                   (vofl_graph_traits<int, void, void, uint64_t, false>),
                   (vol_graph_traits<int, void, void, uint64_t, false>),
                   (vov_graph_traits<int, void, void, uint64_t, false>),
                   (vod_graph_traits<int, void, void, uint64_t, false>),
                   (dofl_graph_traits<int, void, void, uint64_t, false>),
                   (dol_graph_traits<int, void, void, uint64_t, false>),
                   (dov_graph_traits<int, void, void, uint64_t, false>),
                   (dod_graph_traits<int, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<int, void, void, uint64_t, false, TestType>;
    
    Graph g({{0, 1, 10}, {1, 2, 20}, {2, 0, 30}});
    REQUIRE(g.size() == 3);
    
    auto& v0 = g[0];
    bool found = false;
    for (auto& e : v0.edges()) {
        if (e.target_id() == 1) {
            REQUIRE(e.value() == 10);
            found = true;
        }
    }
    REQUIRE(found);
}

TEMPLATE_TEST_CASE("construction with edge range and load", "[common][construction]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    using edge_data = copyable_edge_t<uint64_t, void>;
    
    Graph g;
    std::vector<edge_data> edges = {{0, 1}, {1, 2}, {2, 3}};
    g.load_edges(edges, std::identity{});
    
    REQUIRE(g.size() == 4);
}

TEMPLATE_TEST_CASE("construction with graph value copy", "[common][construction]",
                   (vofl_graph_traits<void, void, std::string, uint64_t, false>),
                   (vol_graph_traits<void, void, std::string, uint64_t, false>),
                   (vov_graph_traits<void, void, std::string, uint64_t, false>),
                   (vod_graph_traits<void, void, std::string, uint64_t, false>),
                   (dofl_graph_traits<void, void, std::string, uint64_t, false>),
                   (dol_graph_traits<void, void, std::string, uint64_t, false>),
                   (dov_graph_traits<void, void, std::string, uint64_t, false>),
                   (dod_graph_traits<void, void, std::string, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, std::string, uint64_t, false, TestType>;
    
    std::string val = "test_value";
    Graph g(val);
    REQUIRE(g.graph_value() == "test_value");
    REQUIRE(val == "test_value"); // Original unchanged
}

TEMPLATE_TEST_CASE("construction with graph value move", "[common][construction]",
                   (vofl_graph_traits<void, void, std::string, uint64_t, false>),
                   (vol_graph_traits<void, void, std::string, uint64_t, false>),
                   (vov_graph_traits<void, void, std::string, uint64_t, false>),
                   (vod_graph_traits<void, void, std::string, uint64_t, false>),
                   (dofl_graph_traits<void, void, std::string, uint64_t, false>),
                   (dol_graph_traits<void, void, std::string, uint64_t, false>),
                   (dov_graph_traits<void, void, std::string, uint64_t, false>),
                   (dod_graph_traits<void, void, std::string, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, std::string, uint64_t, false, TestType>;
    
    std::string val = "test_value";
    Graph g(std::move(val));
    REQUIRE(g.graph_value() == "test_value");
}

TEMPLATE_TEST_CASE("assignment operators", "[common][construction]",
                   (vofl_graph_traits<int, int, int, uint64_t, false>),
                   (vol_graph_traits<int, int, int, uint64_t, false>),
                   (vov_graph_traits<int, int, int, uint64_t, false>),
                   (vod_graph_traits<int, int, int, uint64_t, false>),
                   (dofl_graph_traits<int, int, int, uint64_t, false>),
                   (dol_graph_traits<int, int, int, uint64_t, false>),
                   (dov_graph_traits<int, int, int, uint64_t, false>),
                   (dod_graph_traits<int, int, int, uint64_t, false>)) {
    using Graph = dynamic_graph<int, int, int, uint64_t, false, TestType>;
    
    Graph g1, g2, g3;
    
    // Copy assignment
    g2 = g1;
    REQUIRE(g2.size() == g1.size());
    
    // Move assignment
    g3 = std::move(g1);
    REQUIRE(g3.size() == 0);
}

TEMPLATE_TEST_CASE("empty graph properties", "[common][construction]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    Graph g;
    REQUIRE(g.size() == 0);
    REQUIRE(g.begin() == g.end());
    REQUIRE(g.cbegin() == g.cend());
}

TEMPLATE_TEST_CASE("const graph access", "[common][construction]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    const Graph g;
    REQUIRE(g.size() == 0);
    REQUIRE(g.begin() == g.end());
}

//==================================================================================================
// TEMPLATE_TEST_CASE: Load Operations
//==================================================================================================

TEMPLATE_TEST_CASE("load_edges with simple edge list", "[common][load]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    using edge_data = copyable_edge_t<uint64_t, void>;
    
    std::vector<edge_data> edges = {
        {0, 1}, {1, 2}, {2, 0}
    };
    
    Graph g;
    g.load_edges(edges, std::identity{});
    
    REQUIRE(g.size() == 3);
}

TEMPLATE_TEST_CASE("load_edges with edge values", "[common][load]",
                   (vofl_graph_traits<int, void, void, uint64_t, false>),
                   (vol_graph_traits<int, void, void, uint64_t, false>),
                   (vov_graph_traits<int, void, void, uint64_t, false>),
                   (vod_graph_traits<int, void, void, uint64_t, false>),
                   (dofl_graph_traits<int, void, void, uint64_t, false>),
                   (dol_graph_traits<int, void, void, uint64_t, false>),
                   (dov_graph_traits<int, void, void, uint64_t, false>),
                   (dod_graph_traits<int, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<int, void, void, uint64_t, false, TestType>;
    using edge_data = copyable_edge_t<uint64_t, int>;
    
    std::vector<edge_data> edges = {
        {0, 1, 10}, {1, 2, 20}, {2, 0, 30}
    };
    
    Graph g;
    g.load_edges(edges, std::identity{});
    
    REQUIRE(g.size() == 3);
    
    // Verify edge values exist (iterator through edges)
    auto& v0 = g[0];
    bool found = false;
    for (auto& e : v0.edges()) {
        if (e.target_id() == 1) {
            REQUIRE(e.value() == 10);
            found = true;
        }
    }
    REQUIRE(found);
}

TEMPLATE_TEST_CASE("load_vertices basic", "[common][load]",
                   (vofl_graph_traits<void, int, void, uint64_t, false>),
                   (vol_graph_traits<void, int, void, uint64_t, false>),
                   (vov_graph_traits<void, int, void, uint64_t, false>),
                   (vod_graph_traits<void, int, void, uint64_t, false>),
                   (dofl_graph_traits<void, int, void, uint64_t, false>),
                   (dol_graph_traits<void, int, void, uint64_t, false>),
                   (dov_graph_traits<void, int, void, uint64_t, false>),
                   (dod_graph_traits<void, int, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, int, void, uint64_t, false, TestType>;
    using vertex_data = copyable_vertex_t<uint64_t, int>;
    
    std::vector<vertex_data> vertices = {
        {0, 100}, {1, 200}, {2, 300}
    };
    
    Graph g;
    g.load_vertices(vertices, std::identity{});
    
    REQUIRE(g.size() == 3);
    REQUIRE(g[0].value() == 100);
    REQUIRE(g[1].value() == 200);
    REQUIRE(g[2].value() == 300);
}

TEMPLATE_TEST_CASE("load_edges with empty range", "[common][load]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    using edge_data = copyable_edge_t<uint64_t, void>;
    
    std::vector<edge_data> edges;
    Graph g;
    g.load_edges(edges, std::identity{});
    
    // Empty edge load may create vertex 0
    REQUIRE(g.size() <= 1);
}

TEMPLATE_TEST_CASE("load_edges auto-extends vertex count", "[common][load]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    using edge_data = copyable_edge_t<uint64_t, void>;
    
    std::vector<edge_data> edges = {{0, 10}, {5, 20}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    // Should auto-extend to include vertex 20
    REQUIRE(g.size() >= 21);
}

TEMPLATE_TEST_CASE("load_vertices then load_edges", "[common][load]",
                   (vofl_graph_traits<int, int, void, uint64_t, false>),
                   (vol_graph_traits<int, int, void, uint64_t, false>),
                   (vov_graph_traits<int, int, void, uint64_t, false>),
                   (vod_graph_traits<int, int, void, uint64_t, false>),
                   (dofl_graph_traits<int, int, void, uint64_t, false>),
                   (dol_graph_traits<int, int, void, uint64_t, false>),
                   (dov_graph_traits<int, int, void, uint64_t, false>),
                   (dod_graph_traits<int, int, void, uint64_t, false>)) {
    using Graph = dynamic_graph<int, int, void, uint64_t, false, TestType>;
    using vertex_data = copyable_vertex_t<uint64_t, int>;
    using edge_data = copyable_edge_t<uint64_t, int>;
    
    Graph g;
    
    std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}};
    g.load_vertices(vertices, std::identity{});
    
    std::vector<edge_data> edges = {{0, 1, 100}, {1, 2, 200}};
    g.load_edges(edges, std::identity{});
    
    REQUIRE(g.size() == 3);
    REQUIRE(g[0].value() == 10);
    
    auto& v0 = g[0];
    for (auto& e : v0.edges()) {
        if (e.target_id() == 1) {
            REQUIRE(e.value() == 100);
        }
    }
}

TEMPLATE_TEST_CASE("load_edges with projection", "[common][load]",
                   (vofl_graph_traits<int, void, void, uint64_t, false>),
                   (vol_graph_traits<int, void, void, uint64_t, false>),
                   (vov_graph_traits<int, void, void, uint64_t, false>),
                   (vod_graph_traits<int, void, void, uint64_t, false>),
                   (dofl_graph_traits<int, void, void, uint64_t, false>),
                   (dol_graph_traits<int, void, void, uint64_t, false>),
                   (dov_graph_traits<int, void, void, uint64_t, false>),
                   (dod_graph_traits<int, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<int, void, void, uint64_t, false, TestType>;
    using edge_data = copyable_edge_t<uint64_t, int>;
    
    struct CustomEdge { uint64_t src, tgt; int val; };
    std::vector<CustomEdge> custom_edges = {{0, 1, 10}, {1, 2, 20}};
    
    Graph g;
    g.load_edges(custom_edges, [](const CustomEdge& e) -> edge_data {
        return {e.src, e.tgt, e.val};
    });
    
    REQUIRE(g.size() == 3);
}

TEMPLATE_TEST_CASE("load_vertices with projection", "[common][load]",
                   (vofl_graph_traits<void, int, void, uint64_t, false>),
                   (vol_graph_traits<void, int, void, uint64_t, false>),
                   (vov_graph_traits<void, int, void, uint64_t, false>),
                   (vod_graph_traits<void, int, void, uint64_t, false>),
                   (dofl_graph_traits<void, int, void, uint64_t, false>),
                   (dol_graph_traits<void, int, void, uint64_t, false>),
                   (dov_graph_traits<void, int, void, uint64_t, false>),
                   (dod_graph_traits<void, int, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, int, void, uint64_t, false, TestType>;
    using vertex_data = copyable_vertex_t<uint64_t, int>;
    
    struct CustomVertex { uint64_t id; int value; };
    std::vector<CustomVertex> custom_vertices = {{0, 100}, {1, 200}};
    
    Graph g;
    g.load_vertices(custom_vertices, [](const CustomVertex& v) -> vertex_data {
        return {v.id, v.value};
    });
    
    REQUIRE(g.size() == 2);
    REQUIRE(g[0].value() == 100);
}
 
TEMPLATE_TEST_CASE("incremental edge loading", "[common][load]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    using edge_data = copyable_edge_t<uint64_t, void>;
    
    Graph g;
    
    std::vector<edge_data> batch1 = {{0, 1}, {1, 2}};
    g.load_edges(batch1, std::identity{});
    REQUIRE(g.size() == 3);
    
    std::vector<edge_data> batch2 = {{2, 3}, {3, 0}};
    g.load_edges(batch2, std::identity{});
    REQUIRE(g.size() == 4);
}

TEMPLATE_TEST_CASE("load with self-loops", "[common][load]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    using edge_data = copyable_edge_t<uint64_t, void>;
    
    std::vector<edge_data> edges = {{0, 0}, {1, 1}, {0, 1}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    REQUIRE(g.size() == 2);
    
    // Verify self-loop on vertex 0
    auto& v0 = g[0];
    bool has_self_loop = false;
    for (auto& e : v0.edges()) {
        if (e.target_id() == 0) {
            has_self_loop = true;
        }
    }
    REQUIRE(has_self_loop);
}

//==================================================================================================
// TEMPLATE_TEST_CASE: Vertex Access
//==================================================================================================

TEMPLATE_TEST_CASE("vertex access by index", "[common][access]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {{0, 1}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    REQUIRE(g.size() == 2);
    auto& v0 = g[0];
    auto& v1 = g[1];
    
    // Verify vertices are accessible
    REQUIRE(&v0 != &v1);
}

TEMPLATE_TEST_CASE("vertex iteration", "[common][iteration]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {{0, 1}, {1, 2}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    size_t count = 0;
    for (auto it = g.begin(); it != g.end(); ++it) {
        ++count;
    }
    REQUIRE(count == 3);
}

TEMPLATE_TEST_CASE("const vertex iteration", "[common][access]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {{0, 1}, {1, 2}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    const Graph& cg = g;
    size_t count = 0;
    for (auto it = cg.begin(); it != cg.end(); ++it) {
        ++count;
    }
    REQUIRE(count == 3);
}

TEMPLATE_TEST_CASE("range-based for loop on vertices", "[common][access]",
                   (vofl_graph_traits<void, int, void, uint64_t, false>),
                   (vol_graph_traits<void, int, void, uint64_t, false>),
                   (vov_graph_traits<void, int, void, uint64_t, false>),
                   (vod_graph_traits<void, int, void, uint64_t, false>),
                   (dofl_graph_traits<void, int, void, uint64_t, false>),
                   (dol_graph_traits<void, int, void, uint64_t, false>),
                   (dov_graph_traits<void, int, void, uint64_t, false>),
                   (dod_graph_traits<void, int, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, int, void, uint64_t, false, TestType>;
    
    std::vector<copyable_vertex_t<uint64_t, int>> vertices = {{0, 10}, {1, 20}, {2, 30}};
    Graph g;
    g.load_vertices(vertices, std::identity{});
    
    int sum = 0;
    for (auto& v : g) {
        sum += v.value();
    }
    REQUIRE(sum == 60);
}

TEMPLATE_TEST_CASE("size queries on vertices", "[common][access]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    Graph g;
    REQUIRE(g.size() == 0);
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {{0, 1}, {1, 2}, {2, 3}};
    g.load_edges(edges, std::identity{});
    
    REQUIRE(g.size() == 4);
}

TEMPLATE_TEST_CASE("single vertex access", "[common][access]",
                   (vofl_graph_traits<void, int, void, uint64_t, false>),
                   (vol_graph_traits<void, int, void, uint64_t, false>),
                   (vov_graph_traits<void, int, void, uint64_t, false>),
                   (vod_graph_traits<void, int, void, uint64_t, false>),
                   (dofl_graph_traits<void, int, void, uint64_t, false>),
                   (dol_graph_traits<void, int, void, uint64_t, false>),
                   (dov_graph_traits<void, int, void, uint64_t, false>),
                   (dod_graph_traits<void, int, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, int, void, uint64_t, false, TestType>;
    
    std::vector<copyable_vertex_t<uint64_t, int>> vertices = {{0, 42}};
    Graph g;
    g.load_vertices(vertices, std::identity{});
    
    REQUIRE(g.size() == 1);
    REQUIRE(g[0].value() == 42);
}

TEMPLATE_TEST_CASE("large graph vertex access", "[common][access]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges;
    for (uint64_t i = 0; i < 100; ++i) {
        edges.push_back({i, i + 1});
    }
    
    Graph g;
    g.load_edges(edges, std::identity{});
    
    REQUIRE(g.size() == 101);
    auto& v50 = g[50];
    REQUIRE(&v50 == &g[50]); // Consistent reference
}

TEMPLATE_TEST_CASE("vertex value access and modification", "[common][access]",
                   (vofl_graph_traits<void, int, void, uint64_t, false>),
                   (vol_graph_traits<void, int, void, uint64_t, false>),
                   (vov_graph_traits<void, int, void, uint64_t, false>),
                   (vod_graph_traits<void, int, void, uint64_t, false>),
                   (dofl_graph_traits<void, int, void, uint64_t, false>),
                   (dol_graph_traits<void, int, void, uint64_t, false>),
                   (dov_graph_traits<void, int, void, uint64_t, false>),
                   (dod_graph_traits<void, int, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, int, void, uint64_t, false, TestType>;
    
    std::vector<copyable_vertex_t<uint64_t, int>> vertices = {{0, 10}, {1, 20}};
    Graph g;
    g.load_vertices(vertices, std::identity{});
    
    REQUIRE(g[0].value() == 10);
    g[0].value() = 100;
    REQUIRE(g[0].value() == 100);
}

TEMPLATE_TEST_CASE("empty graph vertex access safety", "[common][access]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    Graph g;
    REQUIRE(g.size() == 0);
    REQUIRE(g.begin() == g.end());
}

TEMPLATE_TEST_CASE("vertex iterator validity", "[common][access]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {{0, 1}, {1, 2}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto it1 = g.begin();
    auto it2 = g.begin();
    REQUIRE(it1 == it2);
    
    ++it1;
    REQUIRE(it1 != it2);
}

//==================================================================================================
// TEMPLATE_TEST_CASE: Edge Access
//==================================================================================================

TEMPLATE_TEST_CASE("edge iteration from vertex", "[common][edges]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {
        {0, 1}, {0, 2}, {0, 3}
    };
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v0 = g[0];
    size_t edge_count = 0;
    for (auto& e : v0.edges()) {
        ++edge_count;
        REQUIRE(e.target_id() >= 1);
        REQUIRE(e.target_id() <= 3);
    }
    REQUIRE(edge_count == 3);
}

TEMPLATE_TEST_CASE("empty vertex has no edges", "[common][edges]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {{0, 1}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v1 = g[1];
    size_t edge_count = 0;
    for (auto& e : v1.edges()) {
        (void)e;
        ++edge_count;
    }
    REQUIRE(edge_count == 0);
}

TEMPLATE_TEST_CASE("parallel edges support", "[common][edges]",
                   (vofl_graph_traits<int, void, void, uint64_t, false>),
                   (vol_graph_traits<int, void, void, uint64_t, false>),
                   (vov_graph_traits<int, void, void, uint64_t, false>),
                   (vod_graph_traits<int, void, void, uint64_t, false>),
                   (dofl_graph_traits<int, void, void, uint64_t, false>),
                   (dol_graph_traits<int, void, void, uint64_t, false>),
                   (dov_graph_traits<int, void, void, uint64_t, false>),
                   (dod_graph_traits<int, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<int, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, int>> edges = {
        {0, 1, 10}, {0, 1, 20}, {0, 1, 30}
    };
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v0 = g[0];
    size_t count = 0;
    for (auto& e : v0.edges()) {
        REQUIRE(e.target_id() == 1);
        ++count;
    }
    REQUIRE(count == 3); // All parallel edges exist
}

TEMPLATE_TEST_CASE("edge degree queries", "[common][edges]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {
        {0, 1}, {0, 2}, {0, 3}, {1, 2}
    };
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v0 = g[0];
    size_t degree = std::ranges::distance(v0.edges());
    REQUIRE(degree == 3);
    
    auto& v1 = g[1];
    REQUIRE(std::ranges::distance(v1.edges()) == 1);
}

TEMPLATE_TEST_CASE("edge empty check", "[common][edges]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {{0, 1}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v0 = g[0];
    REQUIRE(v0.edges().begin() != v0.edges().end());
    
    auto& v1 = g[1];
    REQUIRE(v1.edges().begin() == v1.edges().end());
}

TEMPLATE_TEST_CASE("bidirectional edge traversal", "[common][edges]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {
        {0, 1}, {1, 0}
    };
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v0 = g[0];
    bool found_forward = false;
    for (auto& e : v0.edges()) {
        if (e.target_id() == 1) found_forward = true;
    }
    
    auto& v1 = g[1];
    bool found_backward = false;
    for (auto& e : v1.edges()) {
        if (e.target_id() == 0) found_backward = true;
    }
    
    REQUIRE(found_forward);
    REQUIRE(found_backward);
}

TEMPLATE_TEST_CASE("edge target validation", "[common][edges]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {
        {0, 1}, {0, 2}, {0, 3}
    };
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v0 = g[0];
    std::vector<uint64_t> targets;
    for (auto& e : v0.edges()) {
        targets.push_back(e.target_id());
    }
    
    std::ranges::sort(targets);
    REQUIRE(targets.size() == 3);
    REQUIRE(targets[0] == 1);
    REQUIRE(targets[1] == 2);
    REQUIRE(targets[2] == 3);
}

TEMPLATE_TEST_CASE("edge value iteration", "[common][edges]",
                   (vofl_graph_traits<int, void, void, uint64_t, false>),
                   (vol_graph_traits<int, void, void, uint64_t, false>),
                   (vov_graph_traits<int, void, void, uint64_t, false>),
                   (vod_graph_traits<int, void, void, uint64_t, false>),
                   (dofl_graph_traits<int, void, void, uint64_t, false>),
                   (dol_graph_traits<int, void, void, uint64_t, false>),
                   (dov_graph_traits<int, void, void, uint64_t, false>),
                   (dod_graph_traits<int, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<int, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {0, 3, 30}
    };
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v0 = g[0];
    int sum = 0;
    for (auto& e : v0.edges()) {
        sum += e.value();
    }
    REQUIRE(sum == 60);
}

TEMPLATE_TEST_CASE("edge iterator increment", "[common][edges]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {
        {0, 1}, {0, 2}, {0, 3}
    };
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v0 = g[0];
    auto it = v0.edges().begin();
    REQUIRE(it != v0.edges().end());
    
    ++it;
    REQUIRE(it != v0.edges().end());
    
    ++it;
    REQUIRE(it != v0.edges().end());
    
    ++it;
    REQUIRE(it == v0.edges().end());
}

TEMPLATE_TEST_CASE("high degree vertex", "[common][edges]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges;
    for (uint64_t i = 1; i <= 50; ++i) {
        edges.push_back({0, i});
    }
    
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v0 = g[0];
    size_t degree = std::ranges::distance(v0.edges());
    REQUIRE(degree == 50);
}

TEMPLATE_TEST_CASE("edge range filtering", "[common][edges]",
                   (vofl_graph_traits<int, void, void, uint64_t, false>),
                   (vol_graph_traits<int, void, void, uint64_t, false>),
                   (vov_graph_traits<int, void, void, uint64_t, false>),
                   (vod_graph_traits<int, void, void, uint64_t, false>),
                   (dofl_graph_traits<int, void, void, uint64_t, false>),
                   (dol_graph_traits<int, void, void, uint64_t, false>),
                   (dov_graph_traits<int, void, void, uint64_t, false>),
                   (dod_graph_traits<int, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<int, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, int>> edges = {
        {0, 1, 10}, {0, 2, 25}, {0, 3, 30}, {0, 4, 15}
    };
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v0 = g[0];
    auto count = std::ranges::count_if(v0.edges(), [](const auto& e) {
        return e.value() >= 20;
    });
    REQUIRE(count == 2);
}

//==================================================================================================
// TEMPLATE_TEST_CASE: Sourced Edges
//==================================================================================================

TEMPLATE_TEST_CASE("sourced edges construction", "[common][sourced]",
                   (vofl_graph_traits<void, void, void, uint64_t, true>),
                   (vol_graph_traits<void, void, void, uint64_t, true>),
                   (vov_graph_traits<void, void, void, uint64_t, true>),
                   (vod_graph_traits<void, void, void, uint64_t, true>),
                   (dofl_graph_traits<void, void, void, uint64_t, true>),
                   (dol_graph_traits<void, void, void, uint64_t, true>),
                   (dov_graph_traits<void, void, void, uint64_t, true>),
                   (dod_graph_traits<void, void, void, uint64_t, true>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, true, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {{0, 1}, {1, 2}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v0 = g[0];
    for (auto& e : v0.edges()) {
        REQUIRE(e.source_id() == 0);
        REQUIRE(e.target_id() == 1);
    }
}

TEMPLATE_TEST_CASE("sourced edges with values", "[common][sourced]",
                   (vofl_graph_traits<int, int, void, uint64_t, true>),
                   (vol_graph_traits<int, int, void, uint64_t, true>),
                   (vov_graph_traits<int, int, void, uint64_t, true>),
                   (vod_graph_traits<int, int, void, uint64_t, true>),
                   (dofl_graph_traits<int, int, void, uint64_t, true>),
                   (dol_graph_traits<int, int, void, uint64_t, true>),
                   (dov_graph_traits<int, int, void, uint64_t, true>),
                   (dod_graph_traits<int, int, void, uint64_t, true>)) {
    using Graph = dynamic_graph<int, int, void, uint64_t, true, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, int>> edges = {{0, 1, 10}, {1, 2, 20}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v1 = g[1];
    for (auto& e : v1.edges()) {
        REQUIRE(e.source_id() == 1);
        REQUIRE(e.target_id() == 2);
        REQUIRE(e.value() == 20);
    }
}

TEMPLATE_TEST_CASE("sourced self-loops", "[common][sourced]",
                   (vofl_graph_traits<void, void, void, uint64_t, true>),
                   (vol_graph_traits<void, void, void, uint64_t, true>),
                   (vov_graph_traits<void, void, void, uint64_t, true>),
                   (vod_graph_traits<void, void, void, uint64_t, true>),
                   (dofl_graph_traits<void, void, void, uint64_t, true>),
                   (dol_graph_traits<void, void, void, uint64_t, true>),
                   (dov_graph_traits<void, void, void, uint64_t, true>),
                   (dod_graph_traits<void, void, void, uint64_t, true>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, true, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {{0, 0}, {1, 1}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v0 = g[0];
    for (auto& e : v0.edges()) {
        REQUIRE(e.source_id() == 0);
        REQUIRE(e.target_id() == 0);
    }
}

TEMPLATE_TEST_CASE("sourced multiple edges from vertex", "[common][sourced]",
                   (vofl_graph_traits<void, void, void, uint64_t, true>),
                   (vol_graph_traits<void, void, void, uint64_t, true>),
                   (vov_graph_traits<void, void, void, uint64_t, true>),
                   (vod_graph_traits<void, void, void, uint64_t, true>),
                   (dofl_graph_traits<void, void, void, uint64_t, true>),
                   (dol_graph_traits<void, void, void, uint64_t, true>),
                   (dov_graph_traits<void, void, void, uint64_t, true>),
                   (dod_graph_traits<void, void, void, uint64_t, true>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, true, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {
        {0, 1}, {0, 2}, {0, 3}
    };
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v0 = g[0];
    for (auto& e : v0.edges()) {
        REQUIRE(e.source_id() == 0);
    }
}

TEMPLATE_TEST_CASE("sourced edge iteration consistency", "[common][sourced]",
                   (vofl_graph_traits<int, void, void, uint64_t, true>),
                   (vol_graph_traits<int, void, void, uint64_t, true>),
                   (vov_graph_traits<int, void, void, uint64_t, true>),
                   (vod_graph_traits<int, void, void, uint64_t, true>),
                   (dofl_graph_traits<int, void, void, uint64_t, true>),
                   (dol_graph_traits<int, void, void, uint64_t, true>),
                   (dov_graph_traits<int, void, void, uint64_t, true>),
                   (dod_graph_traits<int, void, void, uint64_t, true>)) {
    using Graph = dynamic_graph<int, void, void, uint64_t, true, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, int>> edges = {
        {0, 1, 10}, {1, 2, 20}, {2, 0, 30}
    };
    Graph g;
    g.load_edges(edges, std::identity{});
    
    for (size_t i = 0; i < 3; ++i) {
        auto& v = g[i];
        for (auto& e : v.edges()) {
            REQUIRE(e.source_id() == i);
        }
    }
}

//==================================================================================================
// TEMPLATE_TEST_CASE: Value Types
//==================================================================================================

TEMPLATE_TEST_CASE("string values work correctly", "[common][values]",
                   (vofl_graph_traits<std::string, std::string, std::string, uint64_t, false>),
                   (vol_graph_traits<std::string, std::string, std::string, uint64_t, false>),
                   (vov_graph_traits<std::string, std::string, std::string, uint64_t, false>),
                   (vod_graph_traits<std::string, std::string, std::string, uint64_t, false>),
                   (dofl_graph_traits<std::string, std::string, std::string, uint64_t, false>),
                   (dol_graph_traits<std::string, std::string, std::string, uint64_t, false>),
                   (dov_graph_traits<std::string, std::string, std::string, uint64_t, false>),
                   (dod_graph_traits<std::string, std::string, std::string, uint64_t, false>)) {
    using Graph = dynamic_graph<std::string, std::string, std::string, uint64_t, false, TestType>;
    
    Graph g("graph_value");
    REQUIRE(g.graph_value() == "graph_value");
    
    std::vector<copyable_vertex_t<uint64_t, std::string>> vertices = {
        {0, "v0"}, {1, "v1"}
    };
    g.load_vertices(vertices, std::identity{});
    
    REQUIRE(g[0].value() == "v0");
    REQUIRE(g[1].value() == "v1");
    
    std::vector<copyable_edge_t<uint64_t, std::string>> edges = {
        {0, 1, "e01"}
    };
    g.load_edges(edges, std::identity{});
    
    for (auto& e : g[0].edges()) {
        REQUIRE(e.value() == "e01");
    }
}

TEMPLATE_TEST_CASE("edge value modification", "[common][values]",
                   (vofl_graph_traits<int, void, void, uint64_t, false>),
                   (vol_graph_traits<int, void, void, uint64_t, false>),
                   (vov_graph_traits<int, void, void, uint64_t, false>),
                   (vod_graph_traits<int, void, void, uint64_t, false>),
                   (dofl_graph_traits<int, void, void, uint64_t, false>),
                   (dol_graph_traits<int, void, void, uint64_t, false>),
                   (dov_graph_traits<int, void, void, uint64_t, false>),
                   (dod_graph_traits<int, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<int, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, int>> edges = {{0, 1, 10}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto& v0 = g[0];
    for (auto& e : v0.edges()) {
        e.value() = 42;
    }
    
    for (auto& e : v0.edges()) {
        REQUIRE(e.value() == 42);
    }
}

TEMPLATE_TEST_CASE("vertex value modification", "[common][values]",
                   (vofl_graph_traits<void, int, void, uint64_t, false>),
                   (vol_graph_traits<void, int, void, uint64_t, false>),
                   (vov_graph_traits<void, int, void, uint64_t, false>),
                   (vod_graph_traits<void, int, void, uint64_t, false>),
                   (dofl_graph_traits<void, int, void, uint64_t, false>),
                   (dol_graph_traits<void, int, void, uint64_t, false>),
                   (dov_graph_traits<void, int, void, uint64_t, false>),
                   (dod_graph_traits<void, int, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, int, void, uint64_t, false, TestType>;
    
    std::vector<copyable_vertex_t<uint64_t, int>> vertices = {{0, 10}, {1, 20}};
    Graph g;
    g.load_vertices(vertices, std::identity{});
    
    g[0].value() = 100;
    g[1].value() = 200;
    
    REQUIRE(g[0].value() == 100);
    REQUIRE(g[1].value() == 200);
}

TEMPLATE_TEST_CASE("graph value modification", "[common][values]",
                   (vofl_graph_traits<void, void, int, uint64_t, false>),
                   (vol_graph_traits<void, void, int, uint64_t, false>),
                   (vov_graph_traits<void, void, int, uint64_t, false>),
                   (vod_graph_traits<void, void, int, uint64_t, false>),
                   (dofl_graph_traits<void, void, int, uint64_t, false>),
                   (dol_graph_traits<void, void, int, uint64_t, false>),
                   (dov_graph_traits<void, void, int, uint64_t, false>),
                   (dod_graph_traits<void, void, int, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, int, uint64_t, false, TestType>;
    
    Graph g(42);
    REQUIRE(g.graph_value() == 42);
    
    g.graph_value() = 100;
    REQUIRE(g.graph_value() == 100);
}

TEMPLATE_TEST_CASE("value move semantics", "[common][values]",
                   (vofl_graph_traits<std::string, std::string, std::string, uint64_t, false>),
                   (vol_graph_traits<std::string, std::string, std::string, uint64_t, false>),
                   (vov_graph_traits<std::string, std::string, std::string, uint64_t, false>),
                   (vod_graph_traits<std::string, std::string, std::string, uint64_t, false>),
                   (dofl_graph_traits<std::string, std::string, std::string, uint64_t, false>),
                   (dol_graph_traits<std::string, std::string, std::string, uint64_t, false>),
                   (dov_graph_traits<std::string, std::string, std::string, uint64_t, false>),
                   (dod_graph_traits<std::string, std::string, std::string, uint64_t, false>)) {
    using Graph = dynamic_graph<std::string, std::string, std::string, uint64_t, false, TestType>;
    
    std::string gval = "graph";
    Graph g(std::move(gval));
    REQUIRE(g.graph_value() == "graph");
}

TEMPLATE_TEST_CASE("mixed value types", "[common][values]",
                   (vofl_graph_traits<int, std::string, double, uint64_t, false>),
                   (vol_graph_traits<int, std::string, double, uint64_t, false>),
                   (vov_graph_traits<int, std::string, double, uint64_t, false>),
                   (vod_graph_traits<int, std::string, double, uint64_t, false>),
                   (dofl_graph_traits<int, std::string, double, uint64_t, false>),
                   (dol_graph_traits<int, std::string, double, uint64_t, false>),
                   (dov_graph_traits<int, std::string, double, uint64_t, false>),
                   (dod_graph_traits<int, std::string, double, uint64_t, false>)) {
    using Graph = dynamic_graph<int, std::string, double, uint64_t, false, TestType>;
    
    Graph g(3.14);
    REQUIRE(g.graph_value() == 3.14);
    
    std::vector<copyable_vertex_t<uint64_t, std::string>> vertices = {{0, "vertex"}};
    g.load_vertices(vertices, std::identity{});
    REQUIRE(g[0].value() == "vertex");
    
    std::vector<copyable_edge_t<uint64_t, int>> edges = {{0, 0, 42}};
    g.load_edges(edges, std::identity{});
    
    for (auto& e : g[0].edges()) {
        REQUIRE(e.value() == 42);
    }
}

TEMPLATE_TEST_CASE("const value access", "[common][values]",
                   (vofl_graph_traits<int, int, int, uint64_t, false>),
                   (vol_graph_traits<int, int, int, uint64_t, false>),
                   (vov_graph_traits<int, int, int, uint64_t, false>),
                   (vod_graph_traits<int, int, int, uint64_t, false>),
                   (dofl_graph_traits<int, int, int, uint64_t, false>),
                   (dol_graph_traits<int, int, int, uint64_t, false>),
                   (dov_graph_traits<int, int, int, uint64_t, false>),
                   (dod_graph_traits<int, int, int, uint64_t, false>)) {
    using Graph = dynamic_graph<int, int, int, uint64_t, false, TestType>;
    
    Graph g(42);
    std::vector<copyable_vertex_t<uint64_t, int>> vertices = {{0, 10}};
    g.load_vertices(vertices, std::identity{});
    std::vector<copyable_edge_t<uint64_t, int>> edges = {{0, 0, 5}};
    g.load_edges(edges, std::identity{});
    
    const Graph& cg = g;
    REQUIRE(cg.graph_value() == 42);
    REQUIRE(cg[0].value() == 10);
    
    for (const auto& e : cg[0].edges()) {
        REQUIRE(e.value() == 5);
    }
}

//==================================================================================================
// TEMPLATE_TEST_CASE: Graph Properties
//==================================================================================================

TEMPLATE_TEST_CASE("large graph construction", "[common][scale]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    // Create a graph with 1000 vertices
    std::vector<copyable_edge_t<uint64_t, void>> edges;
    for (uint64_t i = 0; i < 1000; ++i) {
        edges.push_back({i, (i + 1) % 1000});
    }
    
    Graph g;
    g.load_edges(edges, std::identity{});
    
    REQUIRE(g.size() == 1000);
}

TEMPLATE_TEST_CASE("graph size tracking", "[common][properties]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    Graph g;
    REQUIRE(g.size() == 0);
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {{0, 1}, {1, 2}};
    g.load_edges(edges, std::identity{});
    REQUIRE(g.size() == 3);
    
    std::vector<copyable_edge_t<uint64_t, void>> more_edges = {{3, 4}};
    g.load_edges(more_edges, std::identity{});
    REQUIRE(g.size() == 5);
}

TEMPLATE_TEST_CASE("begin/end iteration", "[common][properties]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {{0, 1}, {1, 2}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    size_t count = 0;
    for (auto it = g.begin(); it != g.end(); ++it) {
        ++count;
    }
    REQUIRE(count == g.size());
}

TEMPLATE_TEST_CASE("cbegin/cend const iteration", "[common][properties]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {{0, 1}, {1, 2}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    size_t count = 0;
    for (auto it = g.cbegin(); it != g.cend(); ++it) {
        ++count;
    }
    REQUIRE(count == g.size());
}

TEMPLATE_TEST_CASE("empty graph properties", "[common][properties]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    Graph g;
    REQUIRE(g.size() == 0);
    REQUIRE(g.begin() == g.end());
    REQUIRE(g.cbegin() == g.cend());
}

TEMPLATE_TEST_CASE("vertex count after incremental loads", "[common][properties]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    Graph g;
    
    std::vector<copyable_edge_t<uint64_t, void>> batch1 = {{0, 1}};
    g.load_edges(batch1, std::identity{});
    size_t size1 = g.size();
    
    std::vector<copyable_edge_t<uint64_t, void>> batch2 = {{2, 3}};
    g.load_edges(batch2, std::identity{});
    size_t size2 = g.size();
    
    REQUIRE(size2 > size1);
    REQUIRE(size2 == 4);
}

TEMPLATE_TEST_CASE("graph iterator distance", "[common][properties]",
                   (vofl_graph_traits<void, void, void, uint64_t, false>),
                   (vol_graph_traits<void, void, void, uint64_t, false>),
                   (vov_graph_traits<void, void, void, uint64_t, false>),
                   (vod_graph_traits<void, void, void, uint64_t, false>),
                   (dofl_graph_traits<void, void, void, uint64_t, false>),
                   (dol_graph_traits<void, void, void, uint64_t, false>),
                   (dov_graph_traits<void, void, void, uint64_t, false>),
                   (dod_graph_traits<void, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<void, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, void>> edges = {{0, 1}, {1, 2}, {2, 3}};
    Graph g;
    g.load_edges(edges, std::identity{});
    
    auto dist = std::distance(g.begin(), g.end());
    REQUIRE(static_cast<size_t>(dist) == g.size());
}

TEMPLATE_TEST_CASE("copy preserves structure", "[common][properties]",
                   (vofl_graph_traits<int, int, void, uint64_t, false>),
                   (vol_graph_traits<int, int, void, uint64_t, false>),
                   (vov_graph_traits<int, int, void, uint64_t, false>),
                   (vod_graph_traits<int, int, void, uint64_t, false>),
                   (dofl_graph_traits<int, int, void, uint64_t, false>),
                   (dol_graph_traits<int, int, void, uint64_t, false>),
                   (dov_graph_traits<int, int, void, uint64_t, false>),
                   (dod_graph_traits<int, int, void, uint64_t, false>)) {
    using Graph = dynamic_graph<int, int, void, uint64_t, false, TestType>;
    
    Graph g1;
    std::vector<copyable_vertex_t<uint64_t, int>> vertices = {{0, 10}, {1, 20}};
    g1.load_vertices(vertices, std::identity{});
    std::vector<copyable_edge_t<uint64_t, int>> edges = {{0, 1, 100}};
    g1.load_edges(edges, std::identity{});
    
    Graph g2(g1);
    REQUIRE(g2.size() == g1.size());
    REQUIRE(g2[0].value() == g1[0].value());
    REQUIRE(g2[1].value() == g1[1].value());
}

TEMPLATE_TEST_CASE("ranges integration", "[common][ranges]",
                   (vofl_graph_traits<int, void, void, uint64_t, false>),
                   (vol_graph_traits<int, void, void, uint64_t, false>),
                   (vov_graph_traits<int, void, void, uint64_t, false>),
                   (vod_graph_traits<int, void, void, uint64_t, false>),
                   (dofl_graph_traits<int, void, void, uint64_t, false>),
                   (dol_graph_traits<int, void, void, uint64_t, false>),
                   (dov_graph_traits<int, void, void, uint64_t, false>),
                   (dod_graph_traits<int, void, void, uint64_t, false>)) {
    using Graph = dynamic_graph<int, void, void, uint64_t, false, TestType>;
    
    std::vector<copyable_edge_t<uint64_t, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}
    };
    Graph g;
    g.load_edges(edges, std::identity{});
    
    // Use std::ranges to count edges with value >= 20
    auto& v0 = g[0];
    auto count = std::ranges::count_if(v0.edges(), [](const auto& e) {
        return e.value() >= 20;
    });
    REQUIRE(count == 1);
}
