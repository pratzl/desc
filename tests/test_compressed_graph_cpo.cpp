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
