#include <catch2/catch_test_macros.hpp>
#include "graph/container/compressed_graph.hpp"
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
using namespace graph;
using namespace graph::container;

// =============================================================================
// vertices(g) Friend Function Tests
// =============================================================================

TEST_CASE("vertices() returns view of vertex descriptors", "[vertices][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}, {2, 3, 40}
    };
    vector<copyable_vertex_t<int, int>> vertex_values = {
        {0, 100}, {1, 200}, {2, 300}, {3, 400}
    };
    
    Graph g;
    g.load_edges(edges);
    g.load_vertices(vertex_values);
    
    SECTION("basic iteration") {
        auto v = vertices(g);
        size_t count = 0;
        for (auto vd : v) {
            REQUIRE(vd.vertex_id() < g.size());
            ++count;
        }
        REQUIRE(count == 4);
    }
    
    SECTION("vertex IDs are sequential") {
        auto v = vertices(g);
        vector<size_t> ids;
        for (auto vd : v) {
            ids.push_back(vd.vertex_id());
        }
        REQUIRE(ids == vector<size_t>{0, 1, 2, 3});
    }
    
    SECTION("can access vertex values through vertex_id") {
        auto v = vertices(g);
        vector<int> values;
        for (auto vd : v) {
            values.push_back(g.vertex_value(vd.vertex_id()));
        }
        REQUIRE(values == vector<int>{100, 200, 300, 400});
    }
}

TEST_CASE("vertices() with empty graph", "[vertices][api]") {
    compressed_graph<void, void, void> g;
    
    auto v = vertices(g);
    size_t count = 0;
    for ([[maybe_unused]] auto vd : v) {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE("vertices() with void vertex values", "[vertices][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {1, 3, 30}
    };
    
    Graph g;
    g.load_edges(edges);
    
    SECTION("iteration works") {
        auto v = vertices(g);
        size_t count = 0;
        for (auto vd : v) {
            REQUIRE(vd.vertex_id() < g.size());
            ++count;
        }
        REQUIRE(count == 4); // vertices 0, 1, 2, 3
    }
    
    SECTION("vertex IDs are correct") {
        auto v = vertices(g);
        vector<size_t> ids;
        for (auto vd : v) {
            ids.push_back(vd.vertex_id());
        }
        REQUIRE(ids.size() == 4);
        REQUIRE(ids[0] == 0);
        REQUIRE(ids[1] == 1);
        REQUIRE(ids[2] == 2);
        REQUIRE(ids[3] == 3);
    }
}

TEST_CASE("vertices() with single vertex", "[vertices][api]") {
    using Graph = compressed_graph<void, int, void>;
    vector<copyable_edge_t<int, void>> edges = {}; // No edges
    vector<copyable_vertex_t<int, int>> vertex_values = {{0, 42}};
    
    Graph g;
    g.load_vertices(vertex_values);
    
    auto v = vertices(g);
    size_t count = 0;
    for (auto vd : v) {
        REQUIRE(vd.vertex_id() == 0);
        REQUIRE(g.vertex_value(vd.vertex_id()) == 42);
        ++count;
    }
    REQUIRE(count == 1);
}

TEST_CASE("vertices() works with STL algorithms", "[vertices][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {1, 2, 20}, {2, 3, 30}
    };
    vector<copyable_vertex_t<int, int>> vertex_values = {
        {0, 5}, {1, 15}, {2, 25}, {3, 35}
    };
    
    Graph g;
    g.load_edges(edges);
    g.load_vertices(vertex_values);
    auto v = vertices(g);
    
    SECTION("std::count_if") {
        size_t count = 0;
        for (auto vd : v) {
            if (g.vertex_value(vd.vertex_id()) > 10)
                ++count;
        }
        REQUIRE(count == 3); // vertices 1, 2, 3 have values > 10
    }
    
    SECTION("find vertex with value") {
        bool found = false;
        size_t found_id = 0;
        for (auto vd : v) {
            if (g.vertex_value(vd.vertex_id()) == 25) {
                found = true;
                found_id = vd.vertex_id();
                break;
            }
        }
        REQUIRE(found);
        REQUIRE(found_id == 2);
    }
    
    SECTION("extract vertex IDs") {
        vector<size_t> ids;
        for (auto vd : v) {
            ids.push_back(vd.vertex_id());
        }
        REQUIRE(ids == vector<size_t>{0, 1, 2, 3});
    }
}

TEST_CASE("vertices() is a lightweight view", "[vertices][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {1, 2, 20}, {2, 3, 30}, {3, 4, 40}
    };
    
    Graph g;
    g.load_edges(edges);
    
    // Create multiple views - should be cheap
    auto v1 = vertices(g);
    auto v2 = vertices(g);
    
    // Both views should produce same results
    vector<size_t> ids1, ids2;
    for (auto vd : v1) ids1.push_back(vd.vertex_id());
    for (auto vd : v2) ids2.push_back(vd.vertex_id());
    
    REQUIRE(ids1 == ids2);
    REQUIRE(ids1.size() == 5); // vertices 0-4
}

TEST_CASE("vertices() with string vertex values", "[vertices][api]") {
    using Graph = compressed_graph<void, string, void>;
    vector<copyable_edge_t<int, void>> edges = {{0, 1}, {1, 2}};
    vector<copyable_vertex_t<int, string>> vertex_values = {
        {0, "Alice"}, {1, "Bob"}, {2, "Charlie"}
    };
    
    Graph g;
    g.load_edges(edges);
    g.load_vertices(vertex_values);
    
    auto v = vertices(g);
    vector<string> names;
    for (auto vd : v) {
        names.push_back(g.vertex_value(vd.vertex_id()));
    }
    
    REQUIRE(names == vector<string>{"Alice", "Bob", "Charlie"});
}

TEST_CASE("vertices() const correctness", "[vertices][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {{0, 1, 10}};
    vector<copyable_vertex_t<int, int>> vertex_values = {{0, 100}, {1, 200}};
    
    Graph g_temp;
    g_temp.load_edges(edges);
    g_temp.load_vertices(vertex_values);
    const Graph g = std::move(g_temp);
    
    // Should work with const graph
    auto v = vertices(g);
    size_t count = 0;
    for (auto vd : v) {
        // Can read vertex values from const graph
        [[maybe_unused]] auto val = g.vertex_value(vd.vertex_id());
        ++count;
    }
    REQUIRE(count == 2);
}

TEST_CASE("vertices() with large graph", "[vertices][api]") {
    using Graph = compressed_graph<int, int, void>;
    
    // Create a larger graph
    vector<copyable_edge_t<int, int>> edges;
    vector<copyable_vertex_t<int, int>> vertex_values;
    
    const size_t n = 1000;
    for (size_t i = 0; i < n - 1; ++i) {
        edges.push_back({static_cast<int>(i), static_cast<int>(i + 1), static_cast<int>(i * 10)});
        vertex_values.push_back({static_cast<int>(i), static_cast<int>(i * 100)});
    }
    vertex_values.push_back({static_cast<int>(n - 1), static_cast<int>((n - 1) * 100)});
    
    Graph g;
    g.load_edges(edges);
    g.load_vertices(vertex_values);
    
    auto v = vertices(g);
    size_t count = 0;
    for (auto vd : v) {
        REQUIRE(vd.vertex_id() == count);
        REQUIRE(g.vertex_value(vd.vertex_id()) == static_cast<int>(count * 100));
        ++count;
    }
    REQUIRE(count == n);
}
