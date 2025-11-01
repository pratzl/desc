/**
 * @file test_find_vertex_cpo.cpp
 * @brief Tests for find_vertex(g, uid) CPO
 */

#include <catch2/catch_test_macros.hpp>
#include <graph/detail/graph_cpo.hpp>
#include <vector>
#include <deque>
#include <map>
#include <list>

using namespace graph;

// ============================================================================
// Default Implementation Tests - Random Access
// ============================================================================

TEST_CASE("find_vertex - vector graph (random access default)", "[graph_cpo][find_vertex][random_access]") {
    using Graph = std::vector<std::vector<int>>;
    Graph g = {{1, 2}, {0, 2}, {0, 1}, {0}};  // 4 vertices
    
    SECTION("Find vertex 0") {
        auto v_iter = find_vertex(g, 0);
        REQUIRE(vertex_id(g, *v_iter) == 0);
    }
    
    SECTION("Find vertex 2") {
        auto v_iter = find_vertex(g, 2);
        REQUIRE(vertex_id(g, *v_iter) == 2);
    }
    
    SECTION("Find last vertex") {
        auto v_iter = find_vertex(g, 3);
        REQUIRE(vertex_id(g, *v_iter) == 3);
    }
    
    SECTION("Iterator navigation") {
        auto v_iter = find_vertex(g, 1);
        auto verts = vertices(g);
        auto first = std::ranges::begin(verts);
        REQUIRE(v_iter == std::ranges::next(first, 1));
    }
}

TEST_CASE("find_vertex - deque graph (random access default)", "[graph_cpo][find_vertex][random_access]") {
    using Graph = std::deque<std::deque<int>>;
    Graph g = {{1}, {0, 2}, {1}};  // 3 vertices
    
    SECTION("Find vertex 0") {
        auto v_iter = find_vertex(g, 0);
        REQUIRE(vertex_id(g, *v_iter) == 0);
    }
    
    SECTION("Find vertex 1") {
        auto v_iter = find_vertex(g, 1);
        REQUIRE(vertex_id(g, *v_iter) == 1);
    }
}

// ============================================================================
// Member Function Customization Tests
// ============================================================================

// Custom graph with find_vertex member  
struct CustomGraphWithMember {
    std::vector<std::vector<int>> adj_list;
    
    // Custom find_vertex that validates ID range and returns iterator to vertex_descriptor_view
    auto find_vertex(std::size_t uid) {
        auto verts = graph::vertices(adj_list);
        if (uid >= adj_list.size()) {
            return std::ranges::end(verts);
        }
        return std::ranges::next(std::ranges::begin(verts), uid);
    }
};

TEST_CASE("find_vertex - custom member function", "[graph_cpo][find_vertex][member]") {
    CustomGraphWithMember g{{{1, 2}, {0, 2}, {0, 1}}};
    
    SECTION("Valid vertex ID") {
        auto v_iter = find_vertex(g, 1);
        auto verts = graph::vertices(g.adj_list);
        REQUIRE(v_iter != std::ranges::end(verts));
        REQUIRE(vertex_id(g.adj_list, *v_iter) == 1);
    }
    
    SECTION("Out of range returns end") {
        auto v_iter = find_vertex(g, 10);
        auto verts = graph::vertices(g.adj_list);
        REQUIRE(v_iter == std::ranges::end(verts));
    }
}

// ============================================================================
// ADL Customization Tests
// ============================================================================

namespace custom_ns {
    struct GraphWithADL {
        std::vector<std::list<int>> adj_list;
    };
    
    // ADL find_vertex for sparse ID lookup
    auto find_vertex(GraphWithADL& g, std::size_t uid) {
        auto verts = graph::vertices(g.adj_list);
        if (uid >= g.adj_list.size()) {
            return std::ranges::end(verts);
        }
        return std::ranges::next(std::ranges::begin(verts), uid);
    }
}

TEST_CASE("find_vertex - ADL customization", "[graph_cpo][find_vertex][adl]") {
    custom_ns::GraphWithADL g{{{1, 2}, {0}, {1}}};
    
    SECTION("Find via ADL") {
        auto v_iter = find_vertex(g, 1);
        auto verts = graph::vertices(g.adj_list);
        REQUIRE(v_iter != std::ranges::end(verts));
    }
    
    SECTION("ADL validates range") {
        auto v_iter = find_vertex(g, 5);
        auto verts = graph::vertices(g.adj_list);
        REQUIRE(v_iter == std::ranges::end(verts));
    }
}

// ============================================================================
// Associative Container Tests (Map-based graphs)
// ============================================================================

// Custom wrapper for map-based graphs with find_vertex member
struct MapGraphWrapper {
    std::map<int, std::vector<int>> data;
    
    // Custom find_vertex that uses map's efficient lookup
    auto find_vertex(int uid) {
        auto verts = graph::vertices(data);
        auto map_iter = data.find(uid);
        if (map_iter == data.end()) {
            return std::ranges::end(verts);
        }
        // Convert map iterator to vertex_descriptor_view iterator
        // by counting distance from begin
        auto begin_iter = data.begin();
        auto distance = std::distance(begin_iter, map_iter);
        return std::ranges::next(std::ranges::begin(verts), distance);
    }
};

TEST_CASE("find_vertex - map with member function", "[graph_cpo][find_vertex][map]") {
    MapGraphWrapper g{{{10, {20, 30}}, {20, {10, 30}}, {30, {10, 20}}}};
    
    SECTION("Find existing vertex") {
        auto v_iter = find_vertex(g, 20);
        auto verts = vertices(g.data);
        REQUIRE(v_iter != std::ranges::end(verts));
        REQUIRE(vertex_id(g.data, *v_iter) == 20);
    }
    
    SECTION("Find first vertex") {
        auto v_iter = find_vertex(g, 10);
        auto verts = vertices(g.data);
        REQUIRE(v_iter != std::ranges::end(verts));
        REQUIRE(vertex_id(g.data, *v_iter) == 10);
    }
    
    SECTION("Find last vertex") {
        auto v_iter = find_vertex(g, 30);
        auto verts = vertices(g.data);
        REQUIRE(v_iter != std::ranges::end(verts));
        REQUIRE(vertex_id(g.data, *v_iter) == 30);
    }
    
    SECTION("Non-existent vertex returns end") {
        auto v_iter = find_vertex(g, 99);
        auto verts = vertices(g.data);
        REQUIRE(v_iter == std::ranges::end(verts));
    }
}

// ADL-based approach for map graphs
namespace map_adl_ns {
    struct MapGraph {
        std::map<int, std::vector<std::pair<int, double>>> adj_list;
    };
    
    // ADL find_vertex for map-based graph
    auto find_vertex(MapGraph& g, int uid) {
        auto verts = graph::vertices(g.adj_list);
        auto map_iter = g.adj_list.find(uid);
        if (map_iter == g.adj_list.end()) {
            return std::ranges::end(verts);
        }
        auto distance = std::distance(g.adj_list.begin(), map_iter);
        return std::ranges::next(std::ranges::begin(verts), distance);
    }
}

TEST_CASE("find_vertex - map with ADL", "[graph_cpo][find_vertex][map][adl]") {
    map_adl_ns::MapGraph g{{{5, {{10, 1.0}, {15, 2.0}}}, {10, {{15, 1.5}}}, {15, {}}}};
    
    SECTION("Find vertex via ADL") {
        auto v_iter = find_vertex(g, 10);
        auto verts = vertices(g.adj_list);
        REQUIRE(v_iter != std::ranges::end(verts));
        REQUIRE(vertex_id(g.adj_list, *v_iter) == 10);
    }
    
    SECTION("ADL handles non-existent keys") {
        auto v_iter = find_vertex(g, 100);
        auto verts = vertices(g.adj_list);
        REQUIRE(v_iter == std::ranges::end(verts));
    }
}

TEST_CASE("find_vertex - map sparse vertex IDs", "[graph_cpo][find_vertex][map]") {
    // Test with non-contiguous vertex IDs
    MapGraphWrapper g{{{100, {200}}, {200, {300}}, {300, {100}}, {500, {}}}};
    
    SECTION("Find sparse vertex IDs") {
        auto v_iter = find_vertex(g, 200);
        REQUIRE(vertex_id(g.data, *v_iter) == 200);
        
        v_iter = find_vertex(g, 500);
        REQUIRE(vertex_id(g.data, *v_iter) == 500);
    }
    
    SECTION("Gaps in ID space return end") {
        auto v_iter = find_vertex(g, 150);  // ID between 100 and 200
        auto verts = vertices(g.data);
        REQUIRE(v_iter == std::ranges::end(verts));
        
        v_iter = find_vertex(g, 400);  // ID between 300 and 500
        REQUIRE(v_iter == std::ranges::end(verts));
    }
}

TEST_CASE("find_vertex - map integration with vertices", "[graph_cpo][find_vertex][map][integration]") {
    MapGraphWrapper g{{{1, {2, 3}}, {2, {3}}, {3, {1}}}};
    
    SECTION("Round-trip through all vertices") {
        auto verts = vertices(g.data);
        for (auto v : verts) {
            auto vid = vertex_id(g.data, v);
            auto found = find_vertex(g, vid);
            REQUIRE(found != std::ranges::end(verts));
            REQUIRE(vertex_id(g.data, *found) == vid);
        }
    }
}

TEST_CASE("find_vertex - map with weighted edges", "[graph_cpo][find_vertex][map]") {
    struct WeightedMapGraph {
        std::map<int, std::vector<std::pair<int, double>>> data = {
            {0, {{1, 1.5}, {2, 2.5}}},
            {1, {{2, 3.5}}},
            {2, {}}
        };
        
        auto find_vertex(int uid) {
            auto verts = graph::vertices(data);
            auto map_iter = data.find(uid);
            if (map_iter == data.end()) {
                return std::ranges::end(verts);
            }
            auto distance = std::distance(data.begin(), map_iter);
            return std::ranges::next(std::ranges::begin(verts), distance);
        }
    };
    
    WeightedMapGraph g;
    
    SECTION("Find vertices in weighted map graph") {
        auto v_iter = find_vertex(g, 1);
        auto verts = vertices(g.data);
        REQUIRE(v_iter != std::ranges::end(verts));
        REQUIRE(vertex_id(g.data, *v_iter) == 1);
        
        // Verify edges are accessible
        auto edge_range = edges(g.data, *v_iter);
        size_t edge_count = 0;
        for (auto e : edge_range) {
            (void)e;
            ++edge_count;
        }
        REQUIRE(edge_count == 1);  // Vertex 1 has one edge to vertex 2
    }
}

TEST_CASE("find_vertex - empty map", "[graph_cpo][find_vertex][map][edge_cases]") {
    MapGraphWrapper g{{}};
    
    SECTION("Finding in empty map returns end") {
        auto v_iter = find_vertex(g, 0);
        auto verts = vertices(g.data);
        REQUIRE(v_iter == std::ranges::end(verts));
    }
}

TEST_CASE("find_vertex - map single vertex", "[graph_cpo][find_vertex][map][edge_cases]") {
    MapGraphWrapper g{{{42, {}}}};
    
    SECTION("Find single vertex") {
        auto v_iter = find_vertex(g, 42);
        auto verts = vertices(g.data);
        REQUIRE(v_iter != std::ranges::end(verts));
        REQUIRE(vertex_id(g.data, *v_iter) == 42);
    }
    
    SECTION("Wrong ID returns end") {
        auto v_iter = find_vertex(g, 43);
        auto verts = vertices(g.data);
        REQUIRE(v_iter == std::ranges::end(verts));
    }
}

// ============================================================================
// Const Correctness Tests
// ============================================================================

TEST_CASE("find_vertex - const correctness", "[graph_cpo][find_vertex][const]") {
    std::vector<std::vector<int>> g = {{1}, {0, 2}, {1}};
    
    SECTION("Works with non-const graph") {
        auto v_iter = find_vertex(g, 1);
        REQUIRE(vertex_id(g, *v_iter) == 1);
    }
    
    // Note: Const graph support would require const-correct vertex_descriptor_view
    // which is a future enhancement
}

// ============================================================================
// Integration Tests with vertices() and vertex_id()
// ============================================================================

TEST_CASE("find_vertex - integration with vertices and vertex_id", "[graph_cpo][find_vertex][integration]") {
    std::vector<std::vector<int>> g = {{1, 2}, {0, 2}, {0, 1}, {2}};
    
    SECTION("Round-trip: vertices -> vertex_id -> find_vertex") {
        auto verts = vertices(g);
        for (auto v : verts) {
            auto vid = vertex_id(g, v);
            auto found = find_vertex(g, vid);
            REQUIRE(found == std::ranges::next(std::ranges::begin(verts), vid));
            REQUIRE(vertex_id(g, *found) == vid);
        }
    }
    
    SECTION("find_vertex matches direct vertex access") {
        for (std::size_t i = 0; i < g.size(); ++i) {
            auto v_iter = find_vertex(g, i);
            auto verts = vertices(g);
            auto direct = std::ranges::next(std::ranges::begin(verts), i);
            REQUIRE(v_iter == direct);
        }
    }
}

// ============================================================================
// Weighted Graph Tests
// ============================================================================

TEST_CASE("find_vertex - weighted graph with pairs", "[graph_cpo][find_vertex][weighted]") {
    using Edge = std::pair<int, double>;
    using Graph = std::vector<std::vector<Edge>>;
    Graph g = {{{1, 1.5}, {2, 2.5}}, {{0, 1.5}, {2, 3.5}}, {{0, 2.5}, {1, 3.5}}};
    
    SECTION("Find vertex in weighted graph") {
        auto v_iter = find_vertex(g, 1);
        REQUIRE(vertex_id(g, *v_iter) == 1);
    }
}

TEST_CASE("find_vertex - weighted graph with tuples", "[graph_cpo][find_vertex][weighted]") {
    using Edge = std::tuple<int, double, std::string>;
    using Graph = std::vector<std::vector<Edge>>;
    Graph g = {{{1, 1.5, "a"}, {2, 2.5, "b"}}, {{0, 1.5, "c"}}, {{1, 3.5, "d"}}};
    
    SECTION("Find vertex in weighted graph with tuples") {
        auto v_iter = find_vertex(g, 2);
        REQUIRE(vertex_id(g, *v_iter) == 2);
    }
}

// ============================================================================
// Type Deduction Tests
// ============================================================================

TEST_CASE("find_vertex - return type", "[graph_cpo][find_vertex][types]") {
    std::vector<std::vector<int>> g = {{1}, {0}};
    
    SECTION("Returns iterator to vertex_descriptor_view") {
        auto v_iter = find_vertex(g, 0);
        using IterType = decltype(v_iter);
        using ExpectedType = vertex_iterator_t<decltype(g)>;
        static_assert(std::is_same_v<IterType, ExpectedType>);
    }
    
    SECTION("Dereference gives vertex_descriptor") {
        auto v_iter = find_vertex(g, 1);
        auto v = *v_iter;
        static_assert(is_vertex_descriptor_v<decltype(v)>);
    }
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_CASE("find_vertex - edge cases", "[graph_cpo][find_vertex][edge_cases]") {
    SECTION("Single vertex graph") {
        std::vector<std::vector<int>> g = {{}}; // One vertex with no edges
        auto v_iter = find_vertex(g, 0);
        REQUIRE(vertex_id(g, *v_iter) == 0);
    }
    
    SECTION("Empty edge lists") {
        std::vector<std::vector<int>> g = {{}, {}, {}};
        auto v_iter = find_vertex(g, 1);
        REQUIRE(vertex_id(g, *v_iter) == 1);
    }
}
