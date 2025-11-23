/**
 * @file test_dynamic_graph_vol.cpp
 * @brief Comprehensive tests for dynamic_graph with vector vertices + list edges
 * 
 * Phase 1.2: Core Container Combinations
 * Tests vol_graph_traits (vector vertices + list edges)
 * Bidirectional edge iteration with std::list
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <graph/container/dynamic_graph.hpp>
#include <graph/graph_info.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ranges>

using namespace graph::container;

// Type aliases for common test configurations
using vol_void_void_void = dynamic_graph<void, void, void, uint32_t, false, vol_graph_traits<void, void, void, uint32_t, false>>;
using vol_int_void_void = dynamic_graph<int, void, void, uint32_t, false, vol_graph_traits<int, void, void, uint32_t, false>>;
using vol_void_int_void = dynamic_graph<void, int, void, uint32_t, false, vol_graph_traits<void, int, void, uint32_t, false>>;
using vol_int_int_void = dynamic_graph<int, int, void, uint32_t, false, vol_graph_traits<int, int, void, uint32_t, false>>;
using vol_void_void_int = dynamic_graph<void, void, int, uint32_t, false, vol_graph_traits<void, void, int, uint32_t, false>>;
using vol_int_int_int = dynamic_graph<int, int, int, uint32_t, false, vol_graph_traits<int, int, int, uint32_t, false>>;

using vol_string_string_string = dynamic_graph<std::string, std::string, std::string, uint32_t, false, 
                                                  vol_graph_traits<std::string, std::string, std::string, uint32_t, false>>;

using vol_sourced = dynamic_graph<void, void, void, uint32_t, true, vol_graph_traits<void, void, void, uint32_t, true>>;
using vol_int_sourced = dynamic_graph<int, void, void, uint32_t, true, vol_graph_traits<int, void, void, uint32_t, true>>;

//==================================================================================================
// 1. Construction Tests
//==================================================================================================

TEST_CASE("vol default constructor creates empty graph", "[vol][construction]") {
    vol_void_void_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vol default constructor with void types", "[vol][construction]") {
    vol_void_void_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vol default constructor with int edge values", "[vol][construction]") {
    vol_int_void_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vol default constructor with int vertex values", "[vol][construction]") {
    vol_void_int_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vol default constructor with all int values", "[vol][construction]") {
    vol_int_int_int g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vol default constructor with string values", "[vol][construction]") {
    vol_string_string_string g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vol constructor with graph value (int)", "[vol][construction]") {
    vol_void_void_int g(42);
    REQUIRE(g.size() == 0);
    REQUIRE(g.graph_value() == 42);
}

TEST_CASE("vol constructor with graph value (string copy)", "[vol][construction]") {
    std::string value = "test";
    vol_string_string_string g(value);
    REQUIRE(g.size() == 0);
    REQUIRE(g.graph_value() == "test");
    REQUIRE(value == "test"); // Original unchanged
}

TEST_CASE("vol constructor with graph value (string move)", "[vol][construction]") {
    std::string value = "test";
    vol_string_string_string g(std::move(value));
    REQUIRE(g.size() == 0);
    REQUIRE(g.graph_value() == "test");
}

TEST_CASE("vol sourced graph construction", "[vol][construction][sourced]") {
    vol_sourced g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vol sourced graph with edge values", "[vol][construction][sourced]") {
    vol_int_sourced g;
    REQUIRE(g.size() == 0);
}

//==================================================================================================
// Initializer List Constructor Tests
//==================================================================================================

TEST_CASE("vol initializer_list constructor with void edge values", "[vol][construction][initializer_list]") {
    using G = vol_void_void_void;

    SECTION("empty initializer list") {
        G g({});
        REQUIRE(g.size() <= 1);
    }

    SECTION("single edge") {
        G g({{0, 1}});
        REQUIRE(g.size() == 2);
        auto& u = g[0];
        auto edges = u.edges();
        REQUIRE(std::ranges::distance(edges) == 1);
        auto it = edges.begin();
        REQUIRE(it->target_id() == 1);
    }

    SECTION("multiple edges from same vertex") {
        G g({{0, 1}, {0, 2}, {0, 3}});
        REQUIRE(g.size() == 4);
        auto& u = g[0];
        auto edges = u.edges();
        REQUIRE(std::ranges::distance(edges) == 3);
    }

    SECTION("bidirectional iteration of edges") {
        G g({{0, 1}, {0, 2}, {0, 3}});
        auto& u = g[0];
        auto edges = u.edges();
        
        // Forward iteration
        int count_forward = 0;
        for (auto it = edges.begin(); it != edges.end(); ++it) {
            ++count_forward;
        }
        REQUIRE(count_forward == 3);
        
        // Backward iteration (list-specific)
        int count_backward = 0;
        auto it = edges.end();
        while (it != edges.begin()) {
            --it;
            ++count_backward;
        }
        REQUIRE(count_backward == 3);
    }

    SECTION("triangle graph") {
        G g({{0, 1}, {1, 2}, {2, 0}});
        REQUIRE(g.size() == 3);
        
        auto& v0 = g[0];
        REQUIRE(std::ranges::distance(v0.edges()) == 1);
        REQUIRE(v0.edges().begin()->target_id() == 1);
        
        auto& v1 = g[1];
        REQUIRE(std::ranges::distance(v1.edges()) == 1);
        REQUIRE(v1.edges().begin()->target_id() == 2);
        
        auto& v2 = g[2];
        REQUIRE(std::ranges::distance(v2.edges()) == 1);
        REQUIRE(v2.edges().begin()->target_id() == 0);
    }
}

TEST_CASE("vol initializer_list constructor with int edge values", "[vol][construction][initializer_list]") {
    using G = vol_int_void_void;

    SECTION("edges with values") {
        G g({{0, 1, 10}, {1, 2, 20}, {2, 0, 30}});
        REQUIRE(g.size() == 3);
        
        auto& v0 = g[0];
        auto edges0 = v0.edges();
        REQUIRE(std::ranges::distance(edges0) == 1);
        REQUIRE(edges0.begin()->target_id() == 1);
        REQUIRE(edges0.begin()->value() == 10);
    }

    SECTION("reverse iteration with values") {
        G g({{0, 1, 10}, {0, 2, 20}, {0, 3, 30}});
        auto& v0 = g[0];
        auto edges = v0.edges();
        
        // Collect values in reverse
        std::vector<int> values;
        auto it = edges.end();
        while (it != edges.begin()) {
            --it;
            values.push_back(it->value());
        }
        
        // Values should be in reverse order of insertion
        REQUIRE(values.size() == 3);
        REQUIRE(values[0] == 30);
        REQUIRE(values[1] == 20);
        REQUIRE(values[2] == 10);
    }
}

TEST_CASE("vol initializer_list with graph value", "[vol][construction][initializer_list]") {
    using G = vol_void_void_int;

    SECTION("construct with graph value and edges") {
        int graph_val = 42;
        G g(graph_val, {{0, 1}, {1, 2}});
        REQUIRE(g.size() == 3);
        REQUIRE(g.graph_value() == 42);
    }

    SECTION("move graph value") {
        G g(100, {{0, 1}});
        REQUIRE(g.graph_value() == 100);
    }
}

//==================================================================================================
// Load Operations Tests
//==================================================================================================

TEST_CASE("vol load_vertices with identity projection", "[vol][load_vertices]") {
    using G = vol_int_int_void;
    using vertex_data = copyable_vertex_t<uint32_t, int>;

    SECTION("load empty vertex range") {
        G g;
        std::vector<vertex_data> vertices;
        g.load_vertices(vertices, std::identity{});
        REQUIRE(g.size() == 0);
    }

    SECTION("load single vertex") {
        G g;
        std::vector<vertex_data> vertices = {{0, 100}};
        g.load_vertices(vertices, std::identity{});
        REQUIRE(g.size() == 1);
        REQUIRE(g[0].value() == 100);
    }

    SECTION("load multiple vertices") {
        G g;
        std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}};
        g.load_vertices(vertices, std::identity{});
        REQUIRE(g.size() == 3);
        REQUIRE(g[0].value() == 10);
        REQUIRE(g[1].value() == 20);
        REQUIRE(g[2].value() == 30);
    }
}

TEST_CASE("vol load_edges with identity projection", "[vol][load_edges]") {
    using G = vol_int_int_void;
    using vertex_data = copyable_vertex_t<uint32_t, int>;
    using edge_data = copyable_edge_t<uint32_t, int>;

    SECTION("load edges into populated graph") {
        G g;
        std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}};
        g.load_vertices(vertices, std::identity{});

        std::vector<edge_data> edges = {{0, 1, 100}, {1, 2, 200}};
        g.load_edges(edges, std::identity{});

        REQUIRE(g.size() == 3);
        
        // Verify edge from vertex 0
        auto& v0 = g[0];
        auto edges0 = v0.edges();
        REQUIRE(std::ranges::distance(edges0) == 1);
        REQUIRE(edges0.begin()->target_id() == 1);
        REQUIRE(edges0.begin()->value() == 100);
    }

    SECTION("bidirectional edge iteration after load") {
        G g;
        std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}};
        g.load_vertices(vertices, std::identity{});

        std::vector<edge_data> edges = {{0, 1, 10}, {0, 2, 20}, {0, 3, 30}};
        g.load_edges(edges, std::identity{});

        auto& v0 = g[0];
        auto edge_list = v0.edges();
        
        // Forward
        std::vector<int> forward_values;
        for (const auto& e : edge_list) {
            forward_values.push_back(e.value());
        }
        
        // Backward
        std::vector<int> backward_values;
        auto it = edge_list.end();
        while (it != edge_list.begin()) {
            --it;
            backward_values.push_back(it->value());
        }
        
        REQUIRE(forward_values.size() == 3);
        REQUIRE(backward_values.size() == 3);
        
        // Backward should be reverse of forward
        std::reverse(backward_values.begin(), backward_values.end());
        REQUIRE(forward_values == backward_values);
    }
}

TEST_CASE("vol load_edges with large dataset", "[vol][load_edges][performance]") {
    using G = vol_int_void_void;
    using edge_data = copyable_edge_t<uint32_t, int>;

    SECTION("load 1000 edges") {
        G g;
        std::vector<edge_data> edges;
        edges.reserve(1000);
        
        for (int i = 0; i < 1000; ++i) {
            edges.push_back({static_cast<uint32_t>(i % 100), 
                           static_cast<uint32_t>((i + 1) % 100), 
                           i});
        }
        
        g.load_edges(edges, std::identity{});
        REQUIRE(g.size() == 100);
        
        // Verify some vertex has edges
        bool found_edges = false;
        for (auto& v : g) {
            if (std::ranges::distance(v.edges()) > 0) {
                found_edges = true;
                break;
            }
        }
        REQUIRE(found_edges);
    }
}

//==================================================================================================
// Vertex and Edge Access Tests
//==================================================================================================

TEST_CASE("vol vertex access in populated graph", "[vol][vertex_access]") {
    using G = vol_int_int_void;
    using vertex_data = copyable_vertex_t<uint32_t, int>;

    SECTION("access vertices with values") {
        G g;
        std::vector<vertex_data> vertices = {{0, 100}, {1, 200}, {2, 300}};
        g.load_vertices(vertices, std::identity{});

        REQUIRE(g[0].value() == 100);
        REQUIRE(g[1].value() == 200);
        REQUIRE(g[2].value() == 300);
    }

    SECTION("modify vertex values") {
        G g;
        std::vector<vertex_data> vertices = {{0, 10}, {1, 20}};
        g.load_vertices(vertices, std::identity{});

        g[0].value() = 999;
        g[1].value() = 888;

        REQUIRE(g[0].value() == 999);
        REQUIRE(g[1].value() == 888);
    }
}

TEST_CASE("vol edge iteration patterns", "[vol][edge_access]") {
    using G = vol_int_int_void;
    using vertex_data = copyable_vertex_t<uint32_t, int>;
    using edge_data = copyable_edge_t<uint32_t, int>;

    SECTION("forward and backward iteration") {
        G g;
        std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}, {3, 40}};
        g.load_vertices(vertices, std::identity{});

        std::vector<edge_data> edges = {{0, 1, 100}, {0, 2, 200}, {0, 3, 300}};
        g.load_edges(edges, std::identity{});

        auto& v0 = g[0];
        auto edge_list = v0.edges();
        
        // Forward iteration
        std::vector<uint32_t> targets_forward;
        for (const auto& e : edge_list) {
            targets_forward.push_back(e.target_id());
        }
        
        // Backward iteration
        std::vector<uint32_t> targets_backward;
        auto it = edge_list.end();
        while (it != edge_list.begin()) {
            --it;
            targets_backward.push_back(it->target_id());
        }
        
        REQUIRE(targets_forward.size() == 3);
        REQUIRE(targets_backward.size() == 3);
        
        // Verify backward is reverse of forward
        std::reverse(targets_backward.begin(), targets_backward.end());
        REQUIRE(targets_forward == targets_backward);
    }

    SECTION("modify edge values during iteration") {
        G g;
        std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}};
        g.load_vertices(vertices, std::identity{});

        std::vector<edge_data> edges = {{0, 1, 100}, {0, 2, 200}};
        g.load_edges(edges, std::identity{});

        auto& v0 = g[0];
        for (auto& edge : v0.edges()) {
            edge.value() *= 2;
        }

        // Verify modifications
        int sum = 0;
        for (const auto& edge : v0.edges()) {
            sum += edge.value();
        }
        REQUIRE(sum == 600); // (100*2) + (200*2)
    }
}

TEST_CASE("vol complex graph structures", "[vol][patterns]") {
    using G = vol_int_void_void;

    SECTION("complete graph K4") {
        G g({
            {0, 1, 1}, {0, 2, 2}, {0, 3, 3},
            {1, 0, 4}, {1, 2, 5}, {1, 3, 6},
            {2, 0, 7}, {2, 1, 8}, {2, 3, 9},
            {3, 0, 10}, {3, 1, 11}, {3, 2, 12}
        });
        REQUIRE(g.size() == 4);
        
        // Each vertex should have 3 outgoing edges
        for (uint32_t i = 0; i < 4; ++i) {
            REQUIRE(std::ranges::distance(g[i].edges()) == 3);
        }
    }

    SECTION("star graph") {
        G g({{0, 1, 1}, {0, 2, 2}, {0, 3, 3}, {0, 4, 4}, {0, 5, 5}});
        REQUIRE(g.size() == 6);
        
        auto& center = g[0];
        REQUIRE(std::ranges::distance(center.edges()) == 5);
        
        // Verify bidirectional iteration works
        auto edges = center.edges();
        auto it = edges.end();
        int count = 0;
        while (it != edges.begin()) {
            --it;
            ++count;
        }
        REQUIRE(count == 5);
    }
}

//==================================================================================================
// Edge Cases and Error Handling
//==================================================================================================

TEST_CASE("vol edge cases with empty containers", "[vol][edge_cases]") {
    using G = vol_int_int_void;
    using vertex_data = copyable_vertex_t<uint32_t, int>;
    using edge_data = copyable_edge_t<uint32_t, int>;

    SECTION("empty vertex and edge ranges") {
        G g;
        std::vector<vertex_data> empty_vertices;
        std::vector<edge_data> empty_edges;
        
        g.load_vertices(empty_vertices, std::identity{});
        REQUIRE(g.size() == 0);
        
        g.load_edges(empty_edges, std::identity{});
        REQUIRE(g.size() <= 1);
    }
}

TEST_CASE("vol duplicate edges behavior", "[vol][duplicates]") {
    using G = vol_void_void_void;

    SECTION("list preserves duplicate edges") {
        G g({{0, 1}, {0, 1}, {0, 1}});
        REQUIRE(g.size() == 2);
        
        auto& u = g[0];
        auto edges = u.edges();
        // std::list preserves all duplicates
        REQUIRE(std::ranges::distance(edges) == 3);
    }
}

TEST_CASE("vol self-loops", "[vol][self_loops]") {
    using G = vol_int_void_void;

    SECTION("single self-loop") {
        G g({{0, 0, 10}});
        REQUIRE(g.size() == 1);
        
        auto& v0 = g[0];
        auto edges = v0.edges();
        REQUIRE(std::ranges::distance(edges) == 1);
        REQUIRE(edges.begin()->target_id() == 0);
        REQUIRE(edges.begin()->value() == 10);
    }

    SECTION("multiple self-loops") {
        G g({{0, 0, 10}, {0, 0, 20}, {0, 0, 30}});
        REQUIRE(g.size() == 1);
        
        auto& v0 = g[0];
        auto edges = v0.edges();
        REQUIRE(std::ranges::distance(edges) == 3);
    }
}

//==================================================================================================
// Property Tests
//==================================================================================================

TEST_CASE("vol graph properties", "[vol][properties]") {
    using G = vol_int_int_void;
    using vertex_data = copyable_vertex_t<uint32_t, int>;
    using edge_data = copyable_edge_t<uint32_t, int>;

    SECTION("degree calculation") {
        G g;
        std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}};
        g.load_vertices(vertices, std::identity{});

        std::vector<edge_data> edges = {{0, 1, 100}, {0, 2, 200}, {1, 2, 300}};
        g.load_edges(edges, std::identity{});

        // Vertex 0 has degree 2
        REQUIRE(std::ranges::distance(g[0].edges()) == 2);
        // Vertex 1 has degree 1
        REQUIRE(std::ranges::distance(g[1].edges()) == 1);
        // Vertex 2 has degree 0 (sink)
        REQUIRE(std::ranges::distance(g[2].edges()) == 0);
    }
}

//==================================================================================================
// Iterator and Ranges Tests
//==================================================================================================

TEST_CASE("vol std::ranges integration", "[vol][ranges]") {
    using G = vol_int_int_void;
    using vertex_data = copyable_vertex_t<uint32_t, int>;

    SECTION("count vertices with predicate") {
        G g;
        std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}, {3, 40}, {4, 50}};
        g.load_vertices(vertices, std::identity{});

        auto count = std::ranges::count_if(g, [](const auto& v) {
            return v.value() > 25;
        });
        REQUIRE(count == 3); // 30, 40, 50
    }

    SECTION("find vertex with value") {
        G g;
        std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}};
        g.load_vertices(vertices, std::identity{});

        auto it = std::ranges::find_if(g, [](const auto& v) {
            return v.value() == 20;
        });
        REQUIRE(it != g.end());
        REQUIRE(it->value() == 20);
    }
}

TEST_CASE("vol algorithm compatibility", "[vol][algorithms]") {
    using G = vol_int_int_void;
    using vertex_data = copyable_vertex_t<uint32_t, int>;

    SECTION("accumulate vertex values") {
        G g;
        std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}};
        g.load_vertices(vertices, std::identity{});

        int sum = 0;
        for (const auto& v : g) {
            sum += v.value();
        }
        REQUIRE(sum == 60);
    }
}

//==================================================================================================
// Performance Tests
//==================================================================================================

TEST_CASE("vol performance characteristics", "[vol][performance]") {
    using G = vol_int_void_void;
    using edge_data = copyable_edge_t<uint32_t, int>;

    SECTION("dense graph - many edges per vertex") {
        G g;
        std::vector<edge_data> edges;
        
        const uint32_t n = 50;
        const uint32_t edges_per_vertex = 10;
        
        for (uint32_t i = 0; i < n; ++i) {
            for (uint32_t j = 0; j < edges_per_vertex; ++j) {
                edges.push_back({i, (i + j + 1) % n, static_cast<int>(i * 100 + j)});
            }
        }
        
        g.load_edges(edges, std::identity{});
        REQUIRE(g.size() == n);
        
        // Verify each vertex has expected edges
        for (uint32_t i = 0; i < n; ++i) {
            REQUIRE(std::ranges::distance(g[i].edges()) == edges_per_vertex);
        }
    }

    SECTION("large graph with 10k vertices") {
        G g;
        std::vector<edge_data> edges;
        
        const uint32_t n = 10000;
        for (uint32_t i = 0; i < n - 1; ++i) {
            edges.push_back({i, i + 1, static_cast<int>(i)});
        }
        
        g.load_edges(edges, std::identity{});
        REQUIRE(g.size() == n);
    }
}

//==================================================================================================
// Workflow Scenarios
//==================================================================================================

TEST_CASE("vol complete workflow scenarios", "[vol][workflow]") {
    using G = vol_int_int_int;
    using vertex_data = copyable_vertex_t<uint32_t, int>;
    using edge_data = copyable_edge_t<uint32_t, int>;

    SECTION("build, query, and modify workflow") {
        // Build graph
        G g(1000); // graph value = 1000
        std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}};
        g.load_vertices(vertices, std::identity{});
        
        std::vector<edge_data> edges = {{0, 1, 100}, {1, 2, 200}};
        g.load_edges(edges, std::identity{});
        
        // Query
        REQUIRE(g.graph_value() == 1000);
        REQUIRE(g.size() == 3);
        REQUIRE(g[1].value() == 20);
        
        // Modify
        g.graph_value() = 2000;
        g[1].value() = 25;
        for (auto& edge : g[0].edges()) {
            edge.value() += 50;
        }
        
        // Verify modifications
        REQUIRE(g.graph_value() == 2000);
        REQUIRE(g[1].value() == 25);
        REQUIRE(g[0].edges().begin()->value() == 150);
    }
}

// Additional static checks
static_assert(std::ranges::range<vol_void_void_void>);
static_assert(std::ranges::range<vol_int_int_int>);
static_assert(std::ranges::range<vol_string_string_string>);
static_assert(std::ranges::bidirectional_range<vol_void_void_void::vertex_type::edges_type>);
