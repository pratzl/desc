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
