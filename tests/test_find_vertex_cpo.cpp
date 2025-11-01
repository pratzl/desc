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
// Associative Container Tests (requires member override)
// ============================================================================

// Note: Complex custom implementations with maps would require more sophisticated
// iterator conversion. For now, testing with vector-based graphs is sufficient.
//
// Map-based sparse graphs would typically provide their own find_vertex
// implementation that returns a suitable iterator type.

// Note: Map-based graphs don't satisfy the sized_range requirement for the default
// find_vertex implementation. They would need to provide custom find_vertex implementations.
// This is expected behavior - associative containers need custom lookups.

TEST_CASE("find_vertex - map notes", "[graph_cpo][find_vertex][associative]") {
    // Maps would require custom find_vertex implementations
    // The default implementation requires sized_range which maps don't provide
    // through vertex_descriptor_view (bidirectional iterators, not random access)
    
    SECTION("Maps work with manual iteration") {
        std::map<int, std::vector<int>> g = {{10, {20, 30}}, {20, {10, 30}}, {30, {10, 20}}};
        auto verts = vertices(g);
        
        // Can iterate and find manually
        bool found = false;
        for (auto v : verts) {
            if (vertex_id(g, v) == 20) {
                found = true;
                break;
            }
        }
        REQUIRE(found);
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
