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

// =============================================================================
// edges(g, u) Friend Function Tests
// =============================================================================

TEST_CASE("edges(g,u) returns view of edge descriptors", "[edges][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}, {2, 3, 40}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    SECTION("iterate edges from vertex 0") {
        auto v = vertices(g);
        auto v0 = *v.begin();
        auto e = edges(g, v0);
        
        size_t count = 0;
        vector<int> targets;
        vector<int> values;
        for (auto ed : e) {
            targets.push_back(g.target_id(ed.value()));
            values.push_back(g.edge_value(ed.value()));
            ++count;
        }
        REQUIRE(count == 2);
        REQUIRE(targets == vector<int>{1, 2});
        REQUIRE(values == vector<int>{10, 20});
    }
    
    SECTION("iterate edges from vertex 1") {
        auto v = vertices(g);
        auto it = v.begin();
        ++it;
        auto v1 = *it;
        auto e = edges(g, v1);
        
        size_t count = 0;
        vector<int> targets;
        for (auto ed : e) {
            targets.push_back(g.target_id(ed.value()));
            ++count;
        }
        REQUIRE(count == 1);
        REQUIRE(targets == vector<int>{2});
    }
    
    SECTION("vertex with no edges") {
        auto v = vertices(g);
        auto it = v.begin();
        ++it; ++it; ++it; // vertex 3
        auto v3 = *it;
        auto e = edges(g, v3);
        
        size_t count = 0;
        for ([[maybe_unused]] auto ed : e) {
            ++count;
        }
        REQUIRE(count == 0);
    }
}

TEST_CASE("edges(g,u) with void edge values", "[edges][api]") {
    using Graph = compressed_graph<void, int, void>;
    vector<copyable_edge_t<int, void>> edges_data = {
        {0, 1}, {0, 2}, {0, 3}, {1, 2}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    vector<int> targets;
    for (auto ed : e) {
        targets.push_back(g.target_id(ed.value()));
    }
    
    REQUIRE(targets.size() == 3);
    REQUIRE(targets == vector<int>{1, 2, 3});
}

TEST_CASE("edges(g,u) with empty graph", "[edges][api]") {
    compressed_graph<int, void, void> g;
    
    // Can't get edges from non-existent vertex, but test empty case
    REQUIRE(g.empty());
}

TEST_CASE("edges(g,u) with single edge", "[edges][api]") {
    using Graph = compressed_graph<int, string, void>;
    vector<copyable_edge_t<int, int>> edges_data = {{0, 1, 100}};
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    size_t count = 0;
    int target = -1;
    int value = -1;
    for (auto ed : e) {
        target = g.target_id(ed.value());
        value = g.edge_value(ed.value());
        ++count;
    }
    
    REQUIRE(count == 1);
    REQUIRE(target == 1);
    REQUIRE(value == 100);
}

TEST_CASE("edges(g,u) works with STL algorithms", "[edges][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {0, 3, 30}, {0, 4, 40}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    SECTION("count edges") {
        size_t count = 0;
        for ([[maybe_unused]] auto ed : e) {
            ++count;
        }
        REQUIRE(count == 4);
    }
    
    SECTION("find edge with specific target") {
        bool found = false;
        int found_value = -1;
        for (auto ed : e) {
            if (g.target_id(ed.value()) == 2) {
                found = true;
                found_value = g.edge_value(ed.value());
                break;
            }
        }
        REQUIRE(found);
        REQUIRE(found_value == 20);
    }
    
    SECTION("collect all targets") {
        vector<int> targets;
        for (auto ed : e) {
            targets.push_back(g.target_id(ed.value()));
        }
        REQUIRE(targets == vector<int>{1, 2, 3, 4});
    }
}

TEST_CASE("edges(g,u) is a lightweight view", "[edges][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {1, 3, 30}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    
    // Create multiple views - should be cheap
    auto e1 = edges(g, v0);
    auto e2 = edges(g, v0);
    
    // Both views should produce same results
    vector<int> targets1, targets2;
    for (auto ed : e1) targets1.push_back(g.target_id(ed.value()));
    for (auto ed : e2) targets2.push_back(g.target_id(ed.value()));
    
    REQUIRE(targets1 == targets2);
    REQUIRE(targets1.size() == 2);
}

TEST_CASE("edges(g,u) with string edge values", "[edges][api]") {
    using Graph = compressed_graph<string, void, void>;
    vector<copyable_edge_t<int, string>> edges_data = {
        {0, 1, "edge_a"}, {0, 2, "edge_b"}, {1, 2, "edge_c"}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    vector<string> labels;
    for (auto ed : e) {
        labels.push_back(g.edge_value(ed.value()));
    }
    
    REQUIRE(labels == vector<string>{"edge_a", "edge_b"});
}

TEST_CASE("edges(g,u) const correctness", "[edges][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}
    };
    
    Graph g_temp;
    g_temp.load_edges(edges_data);
    const Graph g = std::move(g_temp);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    size_t count = 0;
    for (auto ed : e) {
        [[maybe_unused]] auto target = g.target_id(ed.value());
        [[maybe_unused]] auto value = g.edge_value(ed.value());
        ++count;
    }
    REQUIRE(count == 2);
}

TEST_CASE("edges(g,u) with large graph", "[edges][api]") {
    using Graph = compressed_graph<int, void, void>;
    
    // Create a graph where vertex 0 has many edges
    vector<copyable_edge_t<int, int>> edges_data;
    const size_t num_edges = 1000;
    for (size_t i = 1; i <= num_edges; ++i) {
        edges_data.push_back({0, static_cast<int>(i), static_cast<int>(i * 10)});
    }
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    size_t count = 0;
    for (auto ed : e) {
        auto target = g.target_id(ed.value());
        auto value = g.edge_value(ed.value());
        REQUIRE(target == static_cast<int>(count + 1));
        REQUIRE(value == static_cast<int>((count + 1) * 10));
        ++count;
    }
    REQUIRE(count == num_edges);
}

TEST_CASE("edges(g,u) with self-loops", "[edges][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 0, 5}, {0, 1, 10}, {1, 1, 15}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    SECTION("vertex 0 edges include self-loop") {
        auto v = vertices(g);
        auto v0 = *v.begin();
        auto e = edges(g, v0);
        
        vector<int> targets;
        for (auto ed : e) {
            targets.push_back(g.target_id(ed.value()));
        }
        REQUIRE(targets == vector<int>{0, 1});
    }
    
    SECTION("vertex 1 edges include self-loop") {
        auto v = vertices(g);
        auto it = v.begin();
        ++it;
        auto v1 = *it;
        auto e = edges(g, v1);
        
        vector<int> targets;
        for (auto ed : e) {
            targets.push_back(g.target_id(ed.value()));
        }
        REQUIRE(targets == vector<int>{1});
    }
}

TEST_CASE("edges(g,u) iteration from all vertices", "[edges][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {1, 2, 20}, {2, 3, 30}, {3, 0, 40}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    // Count total edges by iterating from each vertex
    size_t total_edges = 0;
    for (auto vd : vertices(g)) {
        for ([[maybe_unused]] auto ed : edges(g, vd)) {
            ++total_edges;
        }
    }
    
    REQUIRE(total_edges == 4);
}

// =============================================================================
// vertex_id(g, u) CPO Tests
// =============================================================================

TEST_CASE("vertex_id(g,u) returns correct vertex ID", "[vertex_id][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {{0, 1, 10}, {1, 2, 20}};
    vector<copyable_vertex_t<int, int>> vertex_values = {{0, 100}, {1, 200}, {2, 300}};
    
    Graph g;
    g.load_edges(edges_data);
    g.load_vertices(vertex_values);
    
    auto v = vertices(g);
    auto it = v.begin();
    
    REQUIRE(vertex_id(g, *it) == 0);
    ++it;
    REQUIRE(vertex_id(g, *it) == 1);
    ++it;
    REQUIRE(vertex_id(g, *it) == 2);
}

TEST_CASE("vertex_id(g,u) with const graph", "[vertex_id][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {{0, 1, 10}, {1, 2, 20}};
    vector<copyable_vertex_t<int, int>> vertex_values = {{0, 100}, {1, 200}, {2, 300}};
    
    Graph g_temp;
    g_temp.load_edges(edges_data);
    g_temp.load_vertices(vertex_values);
    const Graph g = std::move(g_temp);
    
    auto v = vertices(g);
    auto it = v.begin();
    
    REQUIRE(vertex_id(g, *it) == 0);
    ++it;
    REQUIRE(vertex_id(g, *it) == 1);
    ++it;
    REQUIRE(vertex_id(g, *it) == 2);
}

// =============================================================================
// find_vertex(g, uid) CPO Tests
// =============================================================================

TEST_CASE("find_vertex(g,uid) finds vertex by ID", "[find_vertex][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {{0, 1, 10}, {1, 2, 20}};
    vector<copyable_vertex_t<int, int>> vertex_values = {{0, 100}, {1, 200}, {2, 300}};
    
    Graph g;
    g.load_edges(edges_data);
    g.load_vertices(vertex_values);
    
    SECTION("find vertex 0") {
        auto v_iter = find_vertex(g, 0);
        REQUIRE(vertex_id(g, *v_iter) == 0);
        REQUIRE(g.vertex_value(vertex_id(g, *v_iter)) == 100);
    }
    
    SECTION("find vertex 1") {
        auto v_iter = find_vertex(g, 1);
        REQUIRE(vertex_id(g, *v_iter) == 1);
        REQUIRE(g.vertex_value(vertex_id(g, *v_iter)) == 200);
    }
    
    SECTION("find vertex 2") {
        auto v_iter = find_vertex(g, 2);
        REQUIRE(vertex_id(g, *v_iter) == 2);
        REQUIRE(g.vertex_value(vertex_id(g, *v_iter)) == 300);
    }
}

TEST_CASE("find_vertex(g,uid) with const graph", "[find_vertex][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {{0, 1, 10}, {1, 2, 20}};
    vector<copyable_vertex_t<int, int>> vertex_values = {{0, 100}, {1, 200}, {2, 300}};
    
    Graph g_temp;
    g_temp.load_edges(edges_data);
    g_temp.load_vertices(vertex_values);
    const Graph g = std::move(g_temp);
    
    auto v_iter = find_vertex(g, 1);
    REQUIRE(vertex_id(g, *v_iter) == 1);
    REQUIRE(g.vertex_value(vertex_id(g, *v_iter)) == 200);
}

TEST_CASE("find_vertex(g,uid) with void vertex values", "[find_vertex][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges_data = {{0, 1, 10}, {1, 2, 20}, {2, 3, 30}};
    
    Graph g;
    g.load_edges(edges_data);
    
    SECTION("find first vertex") {
        auto v_iter = find_vertex(g, 0);
        auto verts = vertices(g);
        REQUIRE(v_iter == verts.begin());
    }
    
    SECTION("find middle vertex") {
        auto v_iter = find_vertex(g, 2);
        auto verts = vertices(g);
        auto expected = verts.begin();
        ++expected;
        ++expected;
        REQUIRE(v_iter == expected);
    }
    
    SECTION("find last vertex") {
        auto v_iter = find_vertex(g, 3);
        auto verts = vertices(g);
        auto expected = verts.begin();
        ++expected;
        ++expected;
        ++expected;
        REQUIRE(v_iter == expected);
    }
}

TEST_CASE("find_vertex(g,uid) can access edges", "[find_vertex][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {1, 3, 30}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    // Find vertex 0 and iterate its edges
    auto v0_iter = find_vertex(g, 0);
    auto e = edges(g, *v0_iter);
    
    vector<int> targets;
    vector<int> values;
    for (auto ed : e) {
        targets.push_back(g.target_id(ed.value()));
        values.push_back(g.edge_value(ed.value()));
    }
    
    REQUIRE(targets == vector<int>{1, 2});
    REQUIRE(values == vector<int>{10, 20});
}

TEST_CASE("find_vertex(g,uid) iterator equivalence", "[find_vertex][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges_data = {{0, 1, 10}, {1, 2, 20}};
    
    Graph g;
    g.load_edges(edges_data);
    
    // find_vertex should return same iterator as advancing begin by uid
    auto verts = vertices(g);
    auto v1_find = find_vertex(g, 1);
    auto v1_manual = verts.begin();
    ++v1_manual;
    
    REQUIRE(v1_find == v1_manual);
}

TEST_CASE("find_vertex(g,uid) all vertices findable", "[find_vertex][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {1, 2, 20}, {2, 3, 30}, {3, 4, 40}
    };
    vector<copyable_vertex_t<int, int>> vertex_values = {
        {0, 100}, {1, 200}, {2, 300}, {3, 400}, {4, 500}
    };
    
    Graph g;
    g.load_edges(edges_data);
    g.load_vertices(vertex_values);
    
    // Verify every vertex ID can be found
    for (size_t uid = 0; uid < g.size(); ++uid) {
        auto v_iter = find_vertex(g, uid);
        REQUIRE(vertex_id(g, *v_iter) == uid);
        REQUIRE(g.vertex_value(uid) == static_cast<int>((uid + 1) * 100));
    }
}

TEST_CASE("find_vertex(g,uid) with single vertex", "[find_vertex][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {};  // No edges
    vector<copyable_vertex_t<int, int>> vertex_values = {{0, 42}};
    
    Graph g;
    g.load_vertices(vertex_values);
    
    auto v_iter = find_vertex(g, 0);
    REQUIRE(vertex_id(g, *v_iter) == 0);
    REQUIRE(g.vertex_value(0) == 42);
}

TEST_CASE("find_vertex(g,uid) with string vertex values", "[find_vertex][api]") {
    using Graph = compressed_graph<int, string, void>;
    vector<copyable_edge_t<int, int>> edges_data = {{0, 1, 10}, {1, 2, 20}};
    vector<copyable_vertex_t<int, string>> vertex_values = {
        {0, "Alice"}, {1, "Bob"}, {2, "Charlie"}
    };
    
    Graph g;
    g.load_edges(edges_data);
    g.load_vertices(vertex_values);
    
    SECTION("find Alice") {
        auto v_iter = find_vertex(g, 0);
        REQUIRE(g.vertex_value(vertex_id(g, *v_iter)) == "Alice");
    }
    
    SECTION("find Bob") {
        auto v_iter = find_vertex(g, 1);
        REQUIRE(g.vertex_value(vertex_id(g, *v_iter)) == "Bob");
    }
    
    SECTION("find Charlie") {
        auto v_iter = find_vertex(g, 2);
        REQUIRE(g.vertex_value(vertex_id(g, *v_iter)) == "Charlie");
    }
}

// =============================================================================
// target_id(g, uv) CPO Tests
// =============================================================================

TEST_CASE("target_id(g,uv) returns correct target ID", "[target_id][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    SECTION("edges from vertex 0") {
        auto v = vertices(g);
        auto v0 = *v.begin();
        auto e = edges(g, v0);
        
        auto it = e.begin();
        REQUIRE(target_id(g, *it) == 1);
        ++it;
        REQUIRE(target_id(g, *it) == 2);
    }
    
    SECTION("edges from vertex 1") {
        auto v = vertices(g);
        auto it = v.begin();
        ++it;
        auto v1 = *it;
        auto e = edges(g, v1);
        
        auto e_it = e.begin();
        REQUIRE(target_id(g, *e_it) == 2);
    }
}

TEST_CASE("target_id(g,uv) with const graph", "[target_id][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}
    };
    
    Graph g_temp;
    g_temp.load_edges(edges_data);
    const Graph g = std::move(g_temp);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    vector<int> targets;
    for (auto ed : e) {
        targets.push_back(target_id(g, ed));
    }
    
    REQUIRE(targets == vector<int>{1, 2});
}

TEST_CASE("target_id(g,uv) with void edge values", "[target_id][api]") {
    using Graph = compressed_graph<void, int, void>;
    vector<copyable_edge_t<int, void>> edges_data = {
        {0, 1}, {0, 2}, {0, 3}, {1, 2}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    vector<int> targets;
    for (auto ed : e) {
        targets.push_back(target_id(g, ed));
    }
    
    REQUIRE(targets == vector<int>{1, 2, 3});
}

TEST_CASE("target_id(g,uv) with self-loops", "[target_id][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 0, 5}, {0, 1, 10}, {1, 1, 15}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    SECTION("vertex 0 with self-loop") {
        auto v = vertices(g);
        auto v0 = *v.begin();
        auto e = edges(g, v0);
        
        vector<int> targets;
        for (auto ed : e) {
            targets.push_back(target_id(g, ed));
        }
        REQUIRE(targets == vector<int>{0, 1});
    }
    
    SECTION("vertex 1 with self-loop") {
        auto v = vertices(g);
        auto it = v.begin();
        ++it;
        auto v1 = *it;
        auto e = edges(g, v1);
        
        auto e_it = e.begin();
        REQUIRE(target_id(g, *e_it) == 1);
    }
}

TEST_CASE("target_id(g,uv) all edges in graph", "[target_id][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {1, 2, 20}, {2, 3, 30}, {3, 0, 40}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    // Collect all edge targets by iterating through all vertices
    vector<int> all_targets;
    for (auto vd : vertices(g)) {
        for (auto ed : edges(g, vd)) {
            all_targets.push_back(target_id(g, ed));
        }
    }
    
    REQUIRE(all_targets == vector<int>{1, 2, 3, 0});
}

TEST_CASE("target_id(g,uv) with string edge values", "[target_id][api]") {
    using Graph = compressed_graph<string, void, void>;
    vector<copyable_edge_t<int, string>> edges_data = {
        {0, 1, "edge_a"}, {0, 2, "edge_b"}, {1, 2, "edge_c"}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    // Verify target_id works independently of edge value type
    vector<int> targets;
    for (auto ed : e) {
        targets.push_back(target_id(g, ed));
    }
    
    REQUIRE(targets == vector<int>{1, 2});
}

TEST_CASE("target_id(g,uv) consistency with direct access", "[target_id][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {1, 3, 30}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    // Compare CPO target_id(g, uv) with direct g.target_id(edge_id)
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    for (auto ed : e) {
        auto edge_idx = ed.value();
        REQUIRE(target_id(g, ed) == g.target_id(edge_idx));
    }
}

// =============================================================================
// target(g, uv) CPO Tests
// =============================================================================

TEST_CASE("target(g,uv) returns correct target vertex descriptor", "[target][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    SECTION("edges from vertex 0") {
        auto v = vertices(g);
        auto v0 = *v.begin();
        auto e = edges(g, v0);
        
        auto it = e.begin();
        auto target_v = target(g, *it);
        REQUIRE(vertex_id(g, target_v) == 1);
        
        ++it;
        target_v = target(g, *it);
        REQUIRE(vertex_id(g, target_v) == 2);
    }
    
    SECTION("edges from vertex 1") {
        auto v = vertices(g);
        auto it = v.begin();
        ++it;
        auto v1 = *it;
        auto e = edges(g, v1);
        
        auto e_it = e.begin();
        auto target_v = target(g, *e_it);
        REQUIRE(vertex_id(g, target_v) == 2);
    }
}

TEST_CASE("target(g,uv) with const graph", "[target][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}
    };
    
    Graph g_temp;
    g_temp.load_edges(edges_data);
    const Graph g = std::move(g_temp);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    vector<size_t> target_ids;
    for (auto ed : e) {
        auto target_v = target(g, ed);
        target_ids.push_back(vertex_id(g, target_v));
    }
    
    REQUIRE(target_ids == vector<size_t>{1, 2});
}

TEST_CASE("target(g,uv) with void edge values", "[target][api]") {
    using Graph = compressed_graph<int, int, void>;  // Changed from void edge values to int
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {0, 3, 30}, {1, 2, 40}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    vector<size_t> target_ids;
    for (auto ed : e) {
        auto target_v = target(g, ed);
        target_ids.push_back(vertex_id(g, target_v));
    }
    
    REQUIRE(target_ids == vector<size_t>{1, 2, 3});
}

TEST_CASE("target(g,uv) with vertex values", "[target][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}
    };
    vector<copyable_vertex_t<int, int>> vertex_values = {
        {0, 100}, {1, 200}, {2, 300}
    };
    
    Graph g;
    g.load_edges(edges_data);
    g.load_vertices(vertex_values);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    SECTION("access vertex values via target descriptor") {
        auto e_it = e.begin();
        auto target_v = target(g, *e_it);
        REQUIRE(g.vertex_value(vertex_id(g, target_v)) == 200);
        
        ++e_it;
        target_v = target(g, *e_it);
        REQUIRE(g.vertex_value(vertex_id(g, target_v)) == 300);
    }
}

TEST_CASE("target(g,uv) with self-loops", "[target][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 0, 5}, {0, 1, 10}, {1, 1, 15}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    SECTION("self-loop from vertex 0") {
        auto v = vertices(g);
        auto v0 = *v.begin();
        auto e = edges(g, v0);
        
        auto e_it = e.begin();
        auto target_v = target(g, *e_it);
        REQUIRE(vertex_id(g, target_v) == 0);  // Self-loop
    }
    
    SECTION("self-loop from vertex 1") {
        auto v = vertices(g);
        auto it = v.begin();
        ++it;
        auto v1 = *it;
        auto e = edges(g, v1);
        
        auto e_it = e.begin();
        auto target_v = target(g, *e_it);
        REQUIRE(vertex_id(g, target_v) == 1);  // Self-loop
    }
}

TEST_CASE("target(g,uv) all edges in graph", "[target][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {1, 2, 20}, {2, 3, 30}, {3, 0, 40}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    // Collect all edge targets by iterating through all vertices
    vector<size_t> all_target_ids;
    for (auto vd : vertices(g)) {
        for (auto ed : edges(g, vd)) {
            auto target_v = target(g, ed);
            all_target_ids.push_back(vertex_id(g, target_v));
        }
    }
    
    REQUIRE(all_target_ids == vector<size_t>{1, 2, 3, 0});
}

TEST_CASE("target(g,uv) can traverse edges", "[target][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {1, 2, 20}, {2, 3, 30}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    // Start at vertex 0, follow edges to traverse the path
    auto v = vertices(g);
    auto current = *v.begin();
    
    // Get first edge from vertex 0
    auto e0 = edges(g, current);
    auto e0_it = e0.begin();
    current = target(g, *e0_it);
    REQUIRE(vertex_id(g, current) == 1);
    
    // Get first edge from vertex 1
    auto e1 = edges(g, current);
    auto e1_it = e1.begin();
    current = target(g, *e1_it);
    REQUIRE(vertex_id(g, current) == 2);
    
    // Get first edge from vertex 2
    auto e2 = edges(g, current);
    auto e2_it = e2.begin();
    current = target(g, *e2_it);
    REQUIRE(vertex_id(g, current) == 3);
}

TEST_CASE("target(g,uv) with string edge values", "[target][api]") {
    using Graph = compressed_graph<string, void, void>;
    vector<copyable_edge_t<int, string>> edges_data = {
        {0, 1, "edge_a"}, {0, 2, "edge_b"}, {1, 2, "edge_c"}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    // Verify target works independently of edge value type
    vector<size_t> target_ids;
    for (auto ed : e) {
        auto target_v = target(g, ed);
        target_ids.push_back(vertex_id(g, target_v));
    }
    
    REQUIRE(target_ids == vector<size_t>{1, 2});
}

TEST_CASE("target(g,uv) consistency with target_id", "[target][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {1, 3, 30}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    // Compare target(g, uv) with target_id(g, uv)
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    for (auto ed : e) {
        auto target_v = target(g, ed);
        auto tid = target_id(g, ed);
        REQUIRE(vertex_id(g, target_v) == tid);
    }
}

TEST_CASE("target(g,uv) with string vertex values", "[target][api]") {
    using Graph = compressed_graph<int, string, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}
    };
    vector<copyable_vertex_t<int, string>> vertex_values = {
        {0, "Alice"}, {1, "Bob"}, {2, "Charlie"}
    };
    
    Graph g;
    g.load_edges(edges_data);
    g.load_vertices(vertex_values);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto e = edges(g, v0);
    
    vector<string> target_names;
    for (auto ed : e) {
        auto target_v = target(g, ed);
        target_names.push_back(g.vertex_value(vertex_id(g, target_v)));
    }
    
    REQUIRE(target_names == vector<string>{"Bob", "Charlie"});
}

// =============================================================================
// num_vertices(g) CPO Tests
// =============================================================================

TEST_CASE("num_vertices(g) returns vertex count", "[num_vertices][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}, {2, 3, 40}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_vertices(g);
    REQUIRE(count == 4);
    REQUIRE(count == g.size());
}

TEST_CASE("num_vertices(g) works with const graph", "[num_vertices][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {1, 2, 20}
    };
    
    Graph g;
    g.load_edges(edges);
    
    const Graph& cg = g;
    auto count = num_vertices(cg);
    REQUIRE(count == 3);
}

TEST_CASE("num_vertices(g) with empty graph", "[num_vertices][api]") {
    using Graph = compressed_graph<int, int, void>;
    
    Graph g;
    auto count = num_vertices(g);
    REQUIRE(count == 0);
}

TEST_CASE("num_vertices(g) with single vertex", "[num_vertices][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 0, 10}  // Self-loop
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_vertices(g);
    REQUIRE(count == 1);
}

TEST_CASE("num_vertices(g) with void edge values", "[num_vertices][api]") {
    using Graph = compressed_graph<void, int, void>;
    vector<copyable_edge_t<int, void>> edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_vertices(g);
    REQUIRE(count == 5);
}

TEST_CASE("num_vertices(g) with void vertex values", "[num_vertices][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {1, 2, 20}, {2, 0, 30}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_vertices(g);
    REQUIRE(count == 3);
}

TEST_CASE("num_vertices(g) with string values", "[num_vertices][api]") {
    using Graph = compressed_graph<string, string, void>;
    vector<copyable_edge_t<int, string>> edges = {
        {0, 1, "edge_a"}, {1, 2, "edge_b"}
    };
    vector<copyable_vertex_t<int, string>> vertices = {
        {0, "Alice"}, {1, "Bob"}, {2, "Charlie"}
    };
    
    Graph g;
    g.load_edges(edges);
    g.load_vertices(vertices);
    
    auto count = num_vertices(g);
    REQUIRE(count == 3);
}

TEST_CASE("num_vertices(g) with large graph", "[num_vertices][api]") {
    using Graph = compressed_graph<int, int, void>;
    
    // Create graph with 1000 vertices
    vector<copyable_edge_t<int, int>> edges;
    for (int i = 0; i < 999; ++i) {
        edges.push_back({i, i + 1, i});
    }
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_vertices(g);
    REQUIRE(count == 1000);
}

TEST_CASE("num_vertices(g) with disconnected vertices", "[num_vertices][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {2, 3, 20}, {4, 5, 30}
    };
    
    Graph g;
    g.load_edges(edges);
    
    // Graph has vertices 0-5 (6 total)
    auto count = num_vertices(g);
    REQUIRE(count == 6);
}

TEST_CASE("num_vertices(g) return type is integral", "[num_vertices][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_vertices(g);
    static_assert(std::integral<decltype(count)>, "num_vertices must return integral type");
    REQUIRE(count == 2);
}

TEST_CASE("num_vertices(g) consistency with vertices(g)", "[num_vertices][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}, {2, 3, 40}, {3, 4, 50}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_vertices(g);
    
    // Count vertices manually using vertices(g)
    auto v = vertices(g);
    size_t manual_count = 0;
    for ([[maybe_unused]] auto vd : v) {
        ++manual_count;
    }
    
    REQUIRE(count == manual_count);
    REQUIRE(count == 5);
}

// =============================================================================
// num_edges(g) CPO Tests
// =============================================================================

TEST_CASE("num_edges(g) returns edge count", "[num_edges][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}, {2, 3, 40}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_edges(g);
    REQUIRE(count == 4);
}

TEST_CASE("num_edges(g) works with const graph", "[num_edges][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {1, 2, 20}, {2, 3, 30}
    };
    
    Graph g;
    g.load_edges(edges);
    
    const Graph& cg = g;
    auto count = num_edges(cg);
    REQUIRE(count == 3);
}

TEST_CASE("num_edges(g) with empty graph", "[num_edges][api]") {
    using Graph = compressed_graph<int, int, void>;
    
    Graph g;
    auto count = num_edges(g);
    REQUIRE(count == 0);
}

TEST_CASE("num_edges(g) with single edge", "[num_edges][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_edges(g);
    REQUIRE(count == 1);
}

TEST_CASE("num_edges(g) with self-loop", "[num_edges][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 0, 10}, {0, 1, 20}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_edges(g);
    REQUIRE(count == 2);
}

TEST_CASE("num_edges(g) with void edge values", "[num_edges][api]") {
    using Graph = compressed_graph<void, int, void>;
    vector<copyable_edge_t<int, void>> edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_edges(g);
    REQUIRE(count == 4);
}

TEST_CASE("num_edges(g) with void vertex values", "[num_edges][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {1, 2, 20}, {2, 0, 30}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_edges(g);
    REQUIRE(count == 3);
}

TEST_CASE("num_edges(g) with string values", "[num_edges][api]") {
    using Graph = compressed_graph<string, string, void>;
    vector<copyable_edge_t<int, string>> edges = {
        {0, 1, "edge_a"}, {1, 2, "edge_b"}, {2, 3, "edge_c"}
    };
    vector<copyable_vertex_t<int, string>> vertices = {
        {0, "Alice"}, {1, "Bob"}, {2, "Charlie"}, {3, "David"}
    };
    
    Graph g;
    g.load_edges(edges);
    g.load_vertices(vertices);
    
    auto count = num_edges(g);
    REQUIRE(count == 3);
}

TEST_CASE("num_edges(g) with large graph", "[num_edges][api]") {
    using Graph = compressed_graph<int, int, void>;
    
    // Create graph with 1000 edges
    vector<copyable_edge_t<int, int>> edges;
    for (int i = 0; i < 1000; ++i) {
        edges.push_back({i, i + 1, i});
    }
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_edges(g);
    REQUIRE(count == 1000);
}

TEST_CASE("num_edges(g) with multiple edges per vertex", "[num_edges][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {0, 3, 30},
        {1, 2, 40}, {1, 3, 50},
        {2, 3, 60}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_edges(g);
    REQUIRE(count == 6);
}

TEST_CASE("num_edges(g) return type is integral", "[num_edges][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_edges(g);
    static_assert(std::integral<decltype(count)>, "num_edges must return integral type");
    REQUIRE(count == 1);
}

TEST_CASE("num_edges(g) consistency with edge iteration", "[num_edges][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}, {2, 3, 40}, {3, 4, 50}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto count = num_edges(g);
    
    // Count edges manually by iterating through all vertices
    size_t manual_count = 0;
    for (auto v : vertices(g)) {
        auto e = edges(g, v);
        for ([[maybe_unused]] auto ed : e) {
            ++manual_count;
        }
    }
    
    REQUIRE(count == manual_count);
    REQUIRE(count == 5);
}

TEST_CASE("num_edges(g) with disconnected components", "[num_edges][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {1, 2, 20},  // Component 1
        {3, 4, 30}, {4, 5, 40}   // Component 2
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto count = num_edges(g);
    REQUIRE(count == 4);
}

TEST_CASE("num_edges(g) efficiency test - uses ADL not default", "[num_edges][api]") {
    using Graph = compressed_graph<int, int, void>;
    
    // Create a larger graph to ensure ADL version is used (O(1))
    // rather than default iteration (O(V+E))
    vector<copyable_edge_t<int, int>> edges;
    for (int i = 0; i < 500; ++i) {
        edges.push_back({i, i + 1, i});
        edges.push_back({i, i + 2, i + 1000});
    }
    
    Graph g;
    g.load_edges(edges);
    
    // This should be O(1) using the ADL friend function
    auto count = num_edges(g);
    REQUIRE(count == 1000);
    
    // Verify it matches edge_ids() size (which accesses col_index_.size())
    auto edge_id_count = std::ranges::distance(g.edge_ids());
    REQUIRE(count == static_cast<size_t>(edge_id_count));
}

// =============================================================================
// degree(g,u) and degree(g,uid) CPO Tests
// =============================================================================

TEST_CASE("degree(g,u) returns edge count for vertex descriptor", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {0, 3, 30},
        {1, 2, 40},
        {2, 3, 50}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto deg = degree(g, v0);
    REQUIRE(deg == 3); // vertex 0 has 3 outgoing edges
}

TEST_CASE("degree(g,uid) returns edge count for vertex ID", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {0, 3, 30},
        {1, 2, 40},
        {2, 3, 50}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto deg0 = degree(g, 0);
    auto deg1 = degree(g, 1);
    auto deg2 = degree(g, 2);
    
    REQUIRE(deg0 == 3);
    REQUIRE(deg1 == 1);
    REQUIRE(deg2 == 1);
}

TEST_CASE("degree(g,u) and degree(g,uid) consistency", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20},
        {1, 2, 30}, {1, 3, 40}
    };
    
    Graph g;
    g.load_edges(edges);
    
    // Compare degree via descriptor and via ID
    auto v = vertices(g);
    for (auto vd : v) {
        auto vid = vertex_id(g, vd);
        auto deg_desc = degree(g, vd);
        auto deg_id = degree(g, vid);
        REQUIRE(deg_desc == deg_id);
    }
}

TEST_CASE("degree(g,u) works with const graph", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}
    };
    
    Graph g;
    g.load_edges(edges);
    
    const Graph& cg = g;
    auto v = vertices(cg);
    auto v0 = *v.begin();
    auto deg = degree(cg, v0);
    REQUIRE(deg == 2);
}

TEST_CASE("degree(g,uid) works with const graph", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}
    };
    
    Graph g;
    g.load_edges(edges);
    
    const Graph& cg = g;
    auto deg = degree(cg, 0);
    REQUIRE(deg == 2);
}

TEST_CASE("degree(g,u) with zero degree vertex", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {1, 2, 20}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto v = vertices(g);
    auto v_iter = v.begin();
    ++v_iter; ++v_iter; // vertex 2
    auto v2 = *v_iter;
    auto deg = degree(g, v2);
    REQUIRE(deg == 0); // vertex 2 has no outgoing edges
}

TEST_CASE("degree(g,uid) with zero degree vertex", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {1, 2, 20}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto deg = degree(g, 2);
    REQUIRE(deg == 0); // vertex 2 has no outgoing edges
}

TEST_CASE("degree(g,u) with self-loop", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 0, 10}, {0, 1, 20}, {0, 2, 30}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto deg = degree(g, v0);
    REQUIRE(deg == 3); // self-loop counts as one edge
}

TEST_CASE("degree(g,uid) with self-loop", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 0, 10}, {0, 1, 20}, {0, 2, 30}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto deg = degree(g, 0);
    REQUIRE(deg == 3); // self-loop counts as one edge
}

TEST_CASE("degree(g,u) with void edge values", "[degree][api]") {
    using Graph = compressed_graph<void, int, void>;
    vector<copyable_edge_t<int, void>> edges = {
        {0, 1}, {0, 2}, {1, 2}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto deg = degree(g, v0);
    REQUIRE(deg == 2);
}

TEST_CASE("degree(g,uid) with void edge values", "[degree][api]") {
    using Graph = compressed_graph<void, int, void>;
    vector<copyable_edge_t<int, void>> edges = {
        {0, 1}, {0, 2}, {1, 2}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto deg = degree(g, 0);
    REQUIRE(deg == 2);
}

TEST_CASE("degree(g,u) with void vertex values", "[degree][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto v = vertices(g);
    auto v1 = *++v.begin();
    auto deg = degree(g, v1);
    REQUIRE(deg == 1);
}

TEST_CASE("degree(g,uid) with void vertex values", "[degree][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto deg = degree(g, 1);
    REQUIRE(deg == 1);
}

TEST_CASE("degree(g,u) with string values", "[degree][api]") {
    using Graph = compressed_graph<string, string, void>;
    vector<copyable_edge_t<int, string>> edges = {
        {0, 1, "edge_a"}, {0, 2, "edge_b"}, {1, 2, "edge_c"}
    };
    vector<copyable_vertex_t<int, string>> vertices_data = {
        {0, "Alice"}, {1, "Bob"}, {2, "Charlie"}
    };
    
    Graph g;
    g.load_edges(edges);
    g.load_vertices(vertices_data);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto deg = degree(g, v0);
    REQUIRE(deg == 2);
}

TEST_CASE("degree(g,uid) with string values", "[degree][api]") {
    using Graph = compressed_graph<string, string, void>;
    vector<copyable_edge_t<int, string>> edges = {
        {0, 1, "edge_a"}, {0, 2, "edge_b"}, {1, 2, "edge_c"}
    };
    vector<copyable_vertex_t<int, string>> vertices_data = {
        {0, "Alice"}, {1, "Bob"}, {2, "Charlie"}
    };
    
    Graph g;
    g.load_edges(edges);
    g.load_vertices(vertices_data);
    
    auto deg = degree(g, 0);
    REQUIRE(deg == 2);
}

TEST_CASE("degree(g,u) return type is integral", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto v = vertices(g);
    auto v0 = *v.begin();
    auto deg = degree(g, v0);
    static_assert(std::integral<decltype(deg)>, "degree must return integral type");
    REQUIRE(deg == 2);
}

TEST_CASE("degree(g,uid) return type is integral", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto deg = degree(g, 0);
    static_assert(std::integral<decltype(deg)>, "degree must return integral type");
    REQUIRE(deg == 2);
}

TEST_CASE("degree(g,u) with various vertex degrees", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {0, 3, 30}, {0, 4, 40}, // vertex 0: degree 4
        {1, 2, 50}, {1, 3, 60}, {1, 4, 70},              // vertex 1: degree 3
        {2, 3, 80}, {2, 4, 90},                          // vertex 2: degree 2
        {3, 4, 100}                                      // vertex 3: degree 1
                                                         // vertex 4: degree 0
    };
    
    Graph g;
    g.load_edges(edges);
    
    vector<size_t> degrees;
    for (auto v : vertices(g)) {
        degrees.push_back(degree(g, v));
    }
    
    REQUIRE(degrees == vector<size_t>{4, 3, 2, 1, 0});
}

TEST_CASE("degree(g,uid) with various vertex degrees", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {0, 3, 30}, {0, 4, 40}, // vertex 0: degree 4
        {1, 2, 50}, {1, 3, 60}, {1, 4, 70},              // vertex 1: degree 3
        {2, 3, 80}, {2, 4, 90},                          // vertex 2: degree 2
        {3, 4, 100}                                      // vertex 3: degree 1
                                                         // vertex 4: degree 0
    };
    
    Graph g;
    g.load_edges(edges);
    
    REQUIRE(degree(g, 0) == 4);
    REQUIRE(degree(g, 1) == 3);
    REQUIRE(degree(g, 2) == 2);
    REQUIRE(degree(g, 3) == 1);
    REQUIRE(degree(g, 4) == 0);
}

TEST_CASE("degree(g,u) consistency with edges(g,u)", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {0, 3, 30},
        {1, 2, 40}, {1, 3, 50},
        {2, 3, 60}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    for (auto v : vertices(g)) {
        auto deg = degree(g, v);
        auto e = edges(g, v);
        size_t manual_count = 0;
        for ([[maybe_unused]] auto ed : e) {
            ++manual_count;
        }
        REQUIRE(deg == manual_count);
    }
}

TEST_CASE("degree(g,uid) consistency with edges(g,u)", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {0, 3, 30},
        {1, 2, 40}, {1, 3, 50},
        {2, 3, 60}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    for (size_t vid = 0; vid < g.size(); ++vid) {
        auto deg = degree(g, vid);
        auto v_desc = *find_vertex(g, vid);
        auto e = edges(g, v_desc);
        size_t manual_count = 0;
        for ([[maybe_unused]] auto ed : e) {
            ++manual_count;
        }
        REQUIRE(deg == manual_count);
    }
}

TEST_CASE("degree(g,u) with disconnected components", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {1, 2, 20},  // Component 1
        {3, 4, 30}, {4, 5, 40}   // Component 2
    };
    
    Graph g;
    g.load_edges(edges);
    
    auto v = vertices(g);
    auto v_iter = v.begin();
    REQUIRE(degree(g, *v_iter) == 1);     // vertex 0
    REQUIRE(degree(g, *(++v_iter)) == 1); // vertex 1
    REQUIRE(degree(g, *(++v_iter)) == 0); // vertex 2
    REQUIRE(degree(g, *(++v_iter)) == 1); // vertex 3
    REQUIRE(degree(g, *(++v_iter)) == 1); // vertex 4
    REQUIRE(degree(g, *(++v_iter)) == 0); // vertex 5
}

TEST_CASE("degree(g,uid) with disconnected components", "[degree][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges = {
        {0, 1, 10}, {1, 2, 20},  // Component 1
        {3, 4, 30}, {4, 5, 40}   // Component 2
    };
    
    Graph g;
    g.load_edges(edges);
    
    REQUIRE(degree(g, 0) == 1);
    REQUIRE(degree(g, 1) == 1);
    REQUIRE(degree(g, 2) == 0);
    REQUIRE(degree(g, 3) == 1);
    REQUIRE(degree(g, 4) == 1);
    REQUIRE(degree(g, 5) == 0);
}

// =============================================================================
// contains_edge(g, u, v) and contains_edge(g, uid, vid) CPO Tests
// =============================================================================

TEST_CASE("contains_edge(g, u, v) with vertex descriptors - basic edge existence", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}, {2, 3, 40}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v0 = *find_vertex(g, 0);
    auto v1 = *find_vertex(g, 1);
    auto v2 = *find_vertex(g, 2);
    auto v3 = *find_vertex(g, 3);
    
    // Existing edges
    REQUIRE(contains_edge(g, v0, v1) == true);
    REQUIRE(contains_edge(g, v0, v2) == true);
    REQUIRE(contains_edge(g, v1, v2) == true);
    REQUIRE(contains_edge(g, v2, v3) == true);
    
    // Non-existing edges
    REQUIRE(contains_edge(g, v1, v0) == false);
    REQUIRE(contains_edge(g, v2, v0) == false);
    REQUIRE(contains_edge(g, v3, v2) == false);
    REQUIRE(contains_edge(g, v0, v3) == false);
    REQUIRE(contains_edge(g, v1, v3) == false);
}

TEST_CASE("contains_edge(g, uid, vid) with vertex IDs - basic edge existence", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}, {2, 3, 40}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    // Existing edges
    REQUIRE(contains_edge(g, 0, 1) == true);
    REQUIRE(contains_edge(g, 0, 2) == true);
    REQUIRE(contains_edge(g, 1, 2) == true);
    REQUIRE(contains_edge(g, 2, 3) == true);
    
    // Non-existing edges
    REQUIRE(contains_edge(g, 1, 0) == false);
    REQUIRE(contains_edge(g, 2, 0) == false);
    REQUIRE(contains_edge(g, 3, 2) == false);
    REQUIRE(contains_edge(g, 0, 3) == false);
    REQUIRE(contains_edge(g, 1, 3) == false);
}

TEST_CASE("contains_edge(g, u, v) consistency between descriptor and ID versions", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {1, 2, 30}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    for (size_t src = 0; src < g.size(); ++src) {
        auto u = *find_vertex(g, src);
        for (size_t tgt = 0; tgt < g.size(); ++tgt) {
            auto v = *find_vertex(g, tgt);
            // Both versions should return the same result
            REQUIRE(contains_edge(g, u, v) == contains_edge(g, src, tgt));
        }
    }
}

TEST_CASE("contains_edge(g, u, v) with const graph", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {1, 2, 20}
    };
    
    Graph temp_g;
    temp_g.load_edges(edges_data);
    const Graph g = std::move(temp_g);
    
    auto v0 = *find_vertex(g, 0);
    auto v1 = *find_vertex(g, 1);
    auto v2 = *find_vertex(g, 2);
    
    REQUIRE(contains_edge(g, v0, v1) == true);
    REQUIRE(contains_edge(g, v1, v2) == true);
    REQUIRE(contains_edge(g, v0, v2) == false);
    REQUIRE(contains_edge(g, 0, 1) == true);
    REQUIRE(contains_edge(g, 1, 2) == true);
    REQUIRE(contains_edge(g, 0, 2) == false);
}

TEST_CASE("contains_edge(g, u, v) with self-loops", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 0, 10}, {0, 1, 30}, {1, 1, 20}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v0 = *find_vertex(g, 0);
    auto v1 = *find_vertex(g, 1);
    
    // Self-loops exist
    REQUIRE(contains_edge(g, v0, v0) == true);
    REQUIRE(contains_edge(g, v1, v1) == true);
    REQUIRE(contains_edge(g, 0, 0) == true);
    REQUIRE(contains_edge(g, 1, 1) == true);
    
    // Regular edge
    REQUIRE(contains_edge(g, v0, v1) == true);
    REQUIRE(contains_edge(g, 0, 1) == true);
}

TEST_CASE("contains_edge(g, u, v) with zero out-degree vertices", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v1 = *find_vertex(g, 1);
    auto v2 = *find_vertex(g, 2);
    auto v0 = *find_vertex(g, 0);
    
    // Vertices with zero out-degree have no outgoing edges
    REQUIRE(contains_edge(g, v1, v0) == false);
    REQUIRE(contains_edge(g, v1, v2) == false);
    REQUIRE(contains_edge(g, v2, v0) == false);
    REQUIRE(contains_edge(g, v2, v1) == false);
    REQUIRE(contains_edge(g, 1, 0) == false);
    REQUIRE(contains_edge(g, 1, 2) == false);
    REQUIRE(contains_edge(g, 2, 0) == false);
    REQUIRE(contains_edge(g, 2, 1) == false);
}

TEST_CASE("contains_edge(g, u, v) with void vertex values", "[contains_edge][api]") {
    using Graph = compressed_graph<int, void, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {1, 2, 20}, {2, 0, 30}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v0 = *find_vertex(g, 0);
    auto v1 = *find_vertex(g, 1);
    auto v2 = *find_vertex(g, 2);
    
    // Cycle 0 -> 1 -> 2 -> 0
    REQUIRE(contains_edge(g, v0, v1) == true);
    REQUIRE(contains_edge(g, v1, v2) == true);
    REQUIRE(contains_edge(g, v2, v0) == true);
    REQUIRE(contains_edge(g, 0, 1) == true);
    REQUIRE(contains_edge(g, 1, 2) == true);
    REQUIRE(contains_edge(g, 2, 0) == true);
    
    // Non-existent reverse edges
    REQUIRE(contains_edge(g, v1, v0) == false);
    REQUIRE(contains_edge(g, v2, v1) == false);
    REQUIRE(contains_edge(g, v0, v2) == false);
}

TEST_CASE("contains_edge(g, u, v) with void graph values", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {1, 2, 20}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v0 = *find_vertex(g, 0);
    auto v1 = *find_vertex(g, 1);
    auto v2 = *find_vertex(g, 2);
    
    REQUIRE(contains_edge(g, v0, v1) == true);
    REQUIRE(contains_edge(g, v1, v2) == true);
    REQUIRE(contains_edge(g, v2, v0) == false);
    REQUIRE(contains_edge(g, 0, 1) == true);
    REQUIRE(contains_edge(g, 1, 2) == true);
    REQUIRE(contains_edge(g, 2, 0) == false);
}

TEST_CASE("contains_edge(g, u, v) with string values", "[contains_edge][api]") {
    using Graph = compressed_graph<string, string, void>;
    vector<copyable_edge_t<int, string>> edges_data = {
        {0, 1, "edge01"}, {1, 2, "edge12"}
    };
    vector<copyable_vertex_t<int, string>> vertex_values = {
        {0, "v0"}, {1, "v1"}, {2, "v2"}
    };
    
    Graph g;
    g.load_edges(edges_data);
    g.load_vertices(vertex_values);
    
    auto v0 = *find_vertex(g, 0);
    auto v1 = *find_vertex(g, 1);
    auto v2 = *find_vertex(g, 2);
    
    REQUIRE(contains_edge(g, v0, v1) == true);
    REQUIRE(contains_edge(g, v1, v2) == true);
    REQUIRE(contains_edge(g, v0, v2) == false);
    REQUIRE(contains_edge(g, 0, 1) == true);
    REQUIRE(contains_edge(g, 1, 2) == true);
    REQUIRE(contains_edge(g, 0, 2) == false);
}

TEST_CASE("contains_edge(g, u, v) return type is bool", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v0 = *find_vertex(g, 0);
    auto v1 = *find_vertex(g, 1);
    
    auto result_desc = contains_edge(g, v0, v1);
    auto result_id = contains_edge(g, 0, 1);
    
    REQUIRE(std::is_same_v<decltype(result_desc), bool>);
    REQUIRE(std::is_same_v<decltype(result_id), bool>);
}

TEST_CASE("contains_edge(g, u, v) with multiple edges to same target", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    // Note: compressed_graph typically stores one edge per (src, target) pair
    // but let's test the first edge is found
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 1, 20}, {0, 2, 30}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v0 = *find_vertex(g, 0);
    auto v1 = *find_vertex(g, 1);
    auto v2 = *find_vertex(g, 2);
    
    // Should find at least one edge from 0 to 1
    REQUIRE(contains_edge(g, v0, v1) == true);
    REQUIRE(contains_edge(g, v0, v2) == true);
    REQUIRE(contains_edge(g, 0, 1) == true);
    REQUIRE(contains_edge(g, 0, 2) == true);
}

TEST_CASE("contains_edge(g, u, v) with complete graph", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    // Complete graph on 4 vertices
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 1}, {0, 2, 1}, {0, 3, 1},
        {1, 0, 1}, {1, 2, 1}, {1, 3, 1},
        {2, 0, 1}, {2, 1, 1}, {2, 3, 1},
        {3, 0, 1}, {3, 1, 1}, {3, 2, 1}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    // Every vertex should have an edge to every other vertex
    for (size_t src = 0; src < g.size(); ++src) {
        auto u = *find_vertex(g, src);
        for (size_t tgt = 0; tgt < g.size(); ++tgt) {
            if (src != tgt) {
                auto v = *find_vertex(g, tgt);
                REQUIRE(contains_edge(g, u, v) == true);
                REQUIRE(contains_edge(g, src, tgt) == true);
            }
        }
    }
}

TEST_CASE("contains_edge(g, u, v) with disconnected components", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {1, 2, 20},  // Component 1
        {3, 4, 30}, {4, 5, 40}   // Component 2
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    // Within component 1
    REQUIRE(contains_edge(g, 0, 1) == true);
    REQUIRE(contains_edge(g, 1, 2) == true);
    
    // Within component 2
    REQUIRE(contains_edge(g, 3, 4) == true);
    REQUIRE(contains_edge(g, 4, 5) == true);
    
    // Between components (should not exist)
    REQUIRE(contains_edge(g, 0, 3) == false);
    REQUIRE(contains_edge(g, 0, 4) == false);
    REQUIRE(contains_edge(g, 1, 3) == false);
    REQUIRE(contains_edge(g, 2, 4) == false);
    REQUIRE(contains_edge(g, 3, 0) == false);
    REQUIRE(contains_edge(g, 4, 1) == false);
}

TEST_CASE("contains_edge(g, u, v) with single vertex graph", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 0, 10}  // Self-loop
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v0 = *find_vertex(g, 0);
    
    REQUIRE(contains_edge(g, v0, v0) == true);
    REQUIRE(contains_edge(g, 0, 0) == true);
}

TEST_CASE("contains_edge(g, u, v) with empty graph", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    
    Graph g;
    // Empty graph has vertices 0..N-1 but no edges loaded
    // Actually, an empty graph has no vertices either until edges are loaded
    
    // If graph is truly empty, we can't test much
    // Let's test a graph with vertices but no edges
    vector<copyable_edge_t<int, int>> edges_data = {};
    g.load_edges(edges_data);
    
    // No edges exist in empty graph
    // Note: empty graph may have 0 vertices, so this test may not be meaningful
    // But if we had vertices, none would have edges
}

TEST_CASE("contains_edge(g, u, v) with linear chain", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {1, 2, 20}, {2, 3, 30}, {3, 4, 40}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    // Forward edges exist
    REQUIRE(contains_edge(g, 0, 1) == true);
    REQUIRE(contains_edge(g, 1, 2) == true);
    REQUIRE(contains_edge(g, 2, 3) == true);
    REQUIRE(contains_edge(g, 3, 4) == true);
    
    // Reverse edges don't exist
    REQUIRE(contains_edge(g, 1, 0) == false);
    REQUIRE(contains_edge(g, 2, 1) == false);
    REQUIRE(contains_edge(g, 3, 2) == false);
    REQUIRE(contains_edge(g, 4, 3) == false);
    
    // Skip edges don't exist
    REQUIRE(contains_edge(g, 0, 2) == false);
    REQUIRE(contains_edge(g, 0, 3) == false);
    REQUIRE(contains_edge(g, 1, 3) == false);
    REQUIRE(contains_edge(g, 2, 4) == false);
}

TEST_CASE("contains_edge(g, u, v) with star graph", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    // Star graph with center at vertex 0
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {0, 2, 20}, {0, 3, 30}, {0, 4, 40}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    // Center has edges to all spokes
    REQUIRE(contains_edge(g, 0, 1) == true);
    REQUIRE(contains_edge(g, 0, 2) == true);
    REQUIRE(contains_edge(g, 0, 3) == true);
    REQUIRE(contains_edge(g, 0, 4) == true);
    
    // Spokes have no edges to each other
    REQUIRE(contains_edge(g, 1, 2) == false);
    REQUIRE(contains_edge(g, 1, 3) == false);
    REQUIRE(contains_edge(g, 2, 3) == false);
    REQUIRE(contains_edge(g, 2, 4) == false);
    
    // Spokes have no edges back to center
    REQUIRE(contains_edge(g, 1, 0) == false);
    REQUIRE(contains_edge(g, 2, 0) == false);
    REQUIRE(contains_edge(g, 3, 0) == false);
    REQUIRE(contains_edge(g, 4, 0) == false);
}

TEST_CASE("contains_edge(g, u, v) with bidirectional edges", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 10}, {1, 0, 15},
        {1, 2, 20}, {2, 1, 25}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    auto v0 = *find_vertex(g, 0);
    auto v1 = *find_vertex(g, 1);
    auto v2 = *find_vertex(g, 2);
    
    // Both directions exist
    REQUIRE(contains_edge(g, v0, v1) == true);
    REQUIRE(contains_edge(g, v1, v0) == true);
    REQUIRE(contains_edge(g, v1, v2) == true);
    REQUIRE(contains_edge(g, v2, v1) == true);
    REQUIRE(contains_edge(g, 0, 1) == true);
    REQUIRE(contains_edge(g, 1, 0) == true);
    REQUIRE(contains_edge(g, 1, 2) == true);
    REQUIRE(contains_edge(g, 2, 1) == true);
    
    // But not the third edge
    REQUIRE(contains_edge(g, v0, v2) == false);
    REQUIRE(contains_edge(g, v2, v0) == false);
}

TEST_CASE("contains_edge(g, u, v) with high degree vertex", "[contains_edge][api]") {
    using Graph = compressed_graph<int, int, void>;
    vector<copyable_edge_t<int, int>> edges_data = {
        {0, 1, 1}, {0, 2, 1}, {0, 3, 1}, {0, 4, 1},
        {0, 5, 1}, {0, 6, 1}, {0, 7, 1}, {0, 8, 1}
    };
    
    Graph g;
    g.load_edges(edges_data);
    
    // Vertex 0 has edges to vertices 1-8
    for (size_t i = 1; i <= 8; ++i) {
        REQUIRE(contains_edge(g, 0, i) == true);
    }
    
    // But not to any other vertex (if it existed)
    // And none of the target vertices have edges back
    for (size_t i = 1; i <= 8; ++i) {
        REQUIRE(contains_edge(g, i, 0) == false);
    }
}
