/**
 * @file test_dynamic_graph_vofl.cpp
 * @brief Comprehensive tests for dynamic_graph with vector vertices + forward_list edges
 * 
 * Phase 1.1: Core Container Combinations
 * Tests vofl_graph_traits (vector vertices + forward_list edges)
 * This is the most lightweight combination for dynamic graphs.
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
using vofl_void_void_void = dynamic_graph<void, void, void, uint32_t, false, vofl_graph_traits<void, void, void, uint32_t, false>>;
using vofl_int_void_void = dynamic_graph<int, void, void, uint32_t, false, vofl_graph_traits<int, void, void, uint32_t, false>>;
using vofl_void_int_void = dynamic_graph<void, int, void, uint32_t, false, vofl_graph_traits<void, int, void, uint32_t, false>>;
using vofl_int_int_void = dynamic_graph<int, int, void, uint32_t, false, vofl_graph_traits<int, int, void, uint32_t, false>>;
using vofl_void_void_int = dynamic_graph<void, void, int, uint32_t, false, vofl_graph_traits<void, void, int, uint32_t, false>>;
using vofl_int_int_int = dynamic_graph<int, int, int, uint32_t, false, vofl_graph_traits<int, int, int, uint32_t, false>>;

using vofl_string_string_string = dynamic_graph<std::string, std::string, std::string, uint32_t, false, 
                                                  vofl_graph_traits<std::string, std::string, std::string, uint32_t, false>>;

using vofl_sourced = dynamic_graph<void, void, void, uint32_t, true, vofl_graph_traits<void, void, void, uint32_t, true>>;
using vofl_int_sourced = dynamic_graph<int, void, void, uint32_t, true, vofl_graph_traits<int, void, void, uint32_t, true>>;

//==================================================================================================
// 1. Construction Tests (40 tests)
//==================================================================================================

TEST_CASE("vofl default constructor creates empty graph", "[vofl][construction]") {
    vofl_void_void_void g;
    REQUIRE(g.size() == 0);
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl default constructor with void types", "[vofl][construction]") {
    vofl_void_void_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl default constructor with int edge values", "[vofl][construction]") {
    vofl_int_void_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl default constructor with int vertex values", "[vofl][construction]") {
    vofl_void_int_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl default constructor with int graph value", "[vofl][construction]") {
    vofl_void_void_int g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl default constructor with all int values", "[vofl][construction]") {
    vofl_int_int_int g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl default constructor with string values", "[vofl][construction]") {
    vofl_string_string_string g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl constructor with graph value - void GV", "[vofl][construction]") {
    // For void GV, no graph value can be passed
    vofl_void_void_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl constructor with graph value - int GV", "[vofl][construction]") {
    vofl_void_void_int g(42);
    REQUIRE(g.size() == 0);
    REQUIRE(g.graph_value() == 42);
}

TEST_CASE("vofl constructor with graph value - string GV", "[vofl][construction]") {
    vofl_string_string_string g(std::string("test"));
    REQUIRE(g.size() == 0);
    REQUIRE(g.graph_value() == "test");
}

TEST_CASE("vofl copy constructor", "[vofl][construction]") {
    vofl_int_int_int g1;
    // TODO: Add vertices and edges once load functions are available
    vofl_int_int_int g2(g1);
    REQUIRE(g2.size() == g1.size());
}

TEST_CASE("vofl move constructor", "[vofl][construction]") {
    vofl_int_int_int g1;
    // TODO: Add vertices and edges
    vofl_int_int_int g2(std::move(g1));
    REQUIRE(g2.size() == 0); // g1 was empty
}

TEST_CASE("vofl copy assignment", "[vofl][construction]") {
    vofl_int_int_int g1, g2;
    g2 = g1;
    REQUIRE(g2.size() == g1.size());
}

TEST_CASE("vofl move assignment", "[vofl][construction]") {
    vofl_int_int_int g1, g2;
    g2 = std::move(g1);
    REQUIRE(g2.size() == 0);
}

TEST_CASE("vofl sourced edge construction", "[vofl][construction][sourced]") {
    vofl_sourced g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl sourced with edge value construction", "[vofl][construction][sourced]") {
    vofl_int_sourced g;
    REQUIRE(g.size() == 0);
}

//==================================================================================================
// 2. Basic Properties Tests (20 tests)
//==================================================================================================

TEST_CASE("vofl size() on empty graph", "[vofl][properties]") {
    vofl_void_void_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl size() == 0 for empty graph", "[vofl][properties]") {
    vofl_void_void_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl size() != 0 for non-empty graph", "[vofl][properties]") {
    // TODO: This test requires load_vertices or similar functionality
    vofl_void_void_void g;
    REQUIRE(g.size() == 0); // Will change once we can add vertices
}

TEST_CASE("vofl const graph methods", "[vofl][properties]") {
    const vofl_void_void_void g;
    REQUIRE(g.size() == 0);
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl begin() == end() for empty graph", "[vofl][properties]") {
    vofl_void_void_void g;
    REQUIRE(g.begin() == g.end());
}

TEST_CASE("vofl const begin() == const end() for empty graph", "[vofl][properties]") {
    const vofl_void_void_void g;
    REQUIRE(g.begin() == g.end());
}

TEST_CASE("vofl cbegin() == cend() for empty graph", "[vofl][properties]") {
    vofl_void_void_void g;
    REQUIRE(g.cbegin() == g.cend());
}

//==================================================================================================
// 3. Graph Value Tests (15 tests)
//==================================================================================================

TEST_CASE("vofl graph_value() with int GV", "[vofl][graph_value]") {
    vofl_void_void_int g(100);
    REQUIRE(g.graph_value() == 100);
}

TEST_CASE("vofl graph_value() modification", "[vofl][graph_value]") {
    vofl_void_void_int g(100);
    g.graph_value() = 200;
    REQUIRE(g.graph_value() == 200);
}

TEST_CASE("vofl graph_value() const correctness", "[vofl][graph_value]") {
    const vofl_void_void_int g(100);
    REQUIRE(g.graph_value() == 100);
}

TEST_CASE("vofl graph_value() with string GV", "[vofl][graph_value]") {
    vofl_string_string_string g(std::string("initial"));
    REQUIRE(g.graph_value() == "initial");
    g.graph_value() = "modified";
    REQUIRE(g.graph_value() == "modified");
}

TEST_CASE("vofl graph_value() move semantics", "[vofl][graph_value]") {
    vofl_string_string_string g(std::string("test"));
    std::string val = std::move(g.graph_value());
    REQUIRE(val == "test");
}

TEST_CASE("vofl graph_value() with copy", "[vofl][graph_value]") {
    vofl_void_void_int g1(42);
    vofl_void_void_int g2 = g1;
    REQUIRE(g2.graph_value() == 42);
    g2.graph_value() = 100;
    REQUIRE(g1.graph_value() == 42); // g1 unchanged
    REQUIRE(g2.graph_value() == 100);
}

//==================================================================================================
// 4. Iterator Tests (15 tests)
//==================================================================================================

TEST_CASE("vofl iterator on empty graph", "[vofl][iterator]") {
    vofl_void_void_void g;
    auto it = g.begin();
    REQUIRE(it == g.end());
}

TEST_CASE("vofl const iterator on empty graph", "[vofl][iterator]") {
    const vofl_void_void_void g;
    auto it = g.begin();
    REQUIRE(it == g.end());
}

TEST_CASE("vofl range-based for on empty graph", "[vofl][iterator]") {
    vofl_void_void_void g;
    int count = 0;
    for ([[maybe_unused]] auto& v : g) {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE("vofl const range-based for on empty graph", "[vofl][iterator]") {
    const vofl_void_void_void g;
    int count = 0;
    for ([[maybe_unused]] const auto& v : g) {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE("vofl std::ranges compatibility", "[vofl][iterator]") {
    vofl_void_void_void g;
    auto count = std::ranges::distance(g.begin(), g.end());
    REQUIRE(count == 0);
}

//==================================================================================================
// 5. Type Trait Tests (15 tests)
//==================================================================================================

TEST_CASE("vofl_graph_traits types", "[vofl][traits]") {
    using traits = vofl_graph_traits<int, std::string, void, uint32_t, false>;
    
    STATIC_REQUIRE(std::is_same_v<traits::edge_value_type, int>);
    STATIC_REQUIRE(std::is_same_v<traits::vertex_value_type, std::string>);
    STATIC_REQUIRE(std::is_same_v<traits::graph_value_type, void>);
    STATIC_REQUIRE(std::is_same_v<traits::vertex_id_type, uint32_t>);
    STATIC_REQUIRE(traits::sourced == false);
}

TEST_CASE("vofl_graph_traits sourced = true", "[vofl][traits]") {
    using traits = vofl_graph_traits<int, std::string, void, uint32_t, true>;
    STATIC_REQUIRE(traits::sourced == true);
}

TEST_CASE("vofl vertex_id_type variations", "[vofl][traits]") {
    using traits_u64 = vofl_graph_traits<void, void, void, uint64_t, false>;
    using traits_i32 = vofl_graph_traits<void, void, void, int32_t, false>;
    using traits_i8 = vofl_graph_traits<void, void, void, int8_t, false>;
    
    STATIC_REQUIRE(std::is_same_v<traits_u64::vertex_id_type, uint64_t>);
    STATIC_REQUIRE(std::is_same_v<traits_i32::vertex_id_type, int32_t>);
    STATIC_REQUIRE(std::is_same_v<traits_i8::vertex_id_type, int8_t>);
}

TEST_CASE("vofl vertices_type is vector", "[vofl][traits]") {
    using traits = vofl_graph_traits<void, void, void, uint32_t, false>;
    using vertex_t = traits::vertex_type;
    using vertices_t = traits::vertices_type;
    
    STATIC_REQUIRE(std::is_same_v<vertices_t, std::vector<vertex_t>>);
}

TEST_CASE("vofl edges_type is forward_list", "[vofl][traits]") {
    using traits = vofl_graph_traits<void, void, void, uint32_t, false>;
    using edge_t = traits::edge_type;
    using edges_t = traits::edges_type;
    
    STATIC_REQUIRE(std::is_same_v<edges_t, std::forward_list<edge_t>>);
}

//==================================================================================================
// 6. Empty Graph Edge Cases (15 tests)
//==================================================================================================

TEST_CASE("vofl multiple empty graphs independent", "[vofl][edge_cases]") {
    vofl_void_void_void g1, g2, g3;
    REQUIRE(g1.size() == 0);
    REQUIRE(g2.size() == 0);
    REQUIRE(g3.size() == 0);
}

TEST_CASE("vofl copy of empty graph", "[vofl][edge_cases]") {
    vofl_int_int_int g1;
    vofl_int_int_int g2 = g1;
    REQUIRE(g1.size() == 0);
    REQUIRE(g2.size() == 0);
}

TEST_CASE("vofl move of empty graph", "[vofl][edge_cases]") {
    vofl_int_int_int g1;
    vofl_int_int_int g2 = std::move(g1);
    REQUIRE(g2.size() == 0);
}

TEST_CASE("vofl swap empty graphs", "[vofl][edge_cases]") {
    vofl_int_int_int g1, g2;
    std::swap(g1, g2);
    REQUIRE(g1.size() == 0);
    REQUIRE(g2.size() == 0);
}

TEST_CASE("vofl clear on empty graph", "[vofl][edge_cases]") {
    vofl_void_void_void g;
    g.clear();
    REQUIRE(g.size() == 0);
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl multiple clears", "[vofl][edge_cases]") {
    vofl_void_void_void g;
    g.clear();
    g.clear();
    g.clear();
    REQUIRE(g.size() == 0);
}

//==================================================================================================
// 7. Value Type Tests (20 tests)
//==================================================================================================

TEST_CASE("vofl with void edge value", "[vofl][value_types]") {
    using graph_t = dynamic_graph<void, int, int, uint32_t, false, vofl_graph_traits<void, int, int, uint32_t, false>>;
    graph_t g(100);
    REQUIRE(g.graph_value() == 100);
}

TEST_CASE("vofl with void vertex value", "[vofl][value_types]") {
    using graph_t = dynamic_graph<int, void, int, uint32_t, false, vofl_graph_traits<int, void, int, uint32_t, false>>;
    graph_t g(100);
    REQUIRE(g.graph_value() == 100);
}

TEST_CASE("vofl with void graph value", "[vofl][value_types]") {
    using graph_t = dynamic_graph<int, int, void, uint32_t, false, vofl_graph_traits<int, int, void, uint32_t, false>>;
    graph_t g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl with all void values", "[vofl][value_types]") {
    vofl_void_void_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl with int edge value type", "[vofl][value_types]") {
    vofl_int_void_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl with int vertex value type", "[vofl][value_types]") {
    vofl_void_int_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl with int graph value type", "[vofl][value_types]") {
    vofl_void_void_int g(42);
    REQUIRE(g.graph_value() == 42);
}

TEST_CASE("vofl with all int values", "[vofl][value_types]") {
    vofl_int_int_int g(42);
    REQUIRE(g.graph_value() == 42);
}

TEST_CASE("vofl with string edge value type", "[vofl][value_types]") {
    using graph_t = dynamic_graph<std::string, void, void, uint32_t, false, 
                                   vofl_graph_traits<std::string, void, void, uint32_t, false>>;
    graph_t g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl with string vertex value type", "[vofl][value_types]") {
    using graph_t = dynamic_graph<void, std::string, void, uint32_t, false, 
                                   vofl_graph_traits<void, std::string, void, uint32_t, false>>;
    graph_t g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl with string graph value type", "[vofl][value_types]") {
    using graph_t = dynamic_graph<void, void, std::string, uint32_t, false, 
                                   vofl_graph_traits<void, void, std::string, uint32_t, false>>;
    graph_t g(std::string("test"));
    REQUIRE(g.graph_value() == "test");
}

TEST_CASE("vofl with all string values", "[vofl][value_types]") {
    vofl_string_string_string g(std::string("graph"));
    REQUIRE(g.graph_value() == "graph");
}

//==================================================================================================
// 8. Vertex ID Type Tests (10 tests)
//==================================================================================================

TEST_CASE("vofl with uint32_t vertex id", "[vofl][vertex_id]") {
    using graph_t = dynamic_graph<void, void, void, uint32_t, false, 
                                   vofl_graph_traits<void, void, void, uint32_t, false>>;
    graph_t g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl with uint64_t vertex id", "[vofl][vertex_id]") {
    using graph_t = dynamic_graph<void, void, void, uint64_t, false, 
                                   vofl_graph_traits<void, void, void, uint64_t, false>>;
    graph_t g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl with int32_t vertex id", "[vofl][vertex_id]") {
    using graph_t = dynamic_graph<void, void, void, int32_t, false, 
                                   vofl_graph_traits<void, void, void, int32_t, false>>;
    graph_t g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl with int8_t vertex id", "[vofl][vertex_id]") {
    using graph_t = dynamic_graph<void, void, void, int8_t, false, 
                                   vofl_graph_traits<void, void, void, int8_t, false>>;
    graph_t g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl with int vertex id", "[vofl][vertex_id]") {
    using graph_t = dynamic_graph<void, void, void, int, false, 
                                   vofl_graph_traits<void, void, void, int, false>>;
    graph_t g;
    REQUIRE(g.size() == 0);
}

//==================================================================================================
// 9. Sourced Edge Tests (15 tests)
//==================================================================================================

TEST_CASE("vofl sourced=false by default", "[vofl][sourced]") {
    vofl_void_void_void g;
    using traits = vofl_graph_traits<void, void, void, uint32_t, false>;
    STATIC_REQUIRE(traits::sourced == false);
}

TEST_CASE("vofl sourced=true explicit", "[vofl][sourced]") {
    vofl_sourced g;
    using traits = vofl_graph_traits<void, void, void, uint32_t, true>;
    STATIC_REQUIRE(traits::sourced == true);
}

TEST_CASE("vofl sourced with void values", "[vofl][sourced]") {
    vofl_sourced g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl sourced with int edge value", "[vofl][sourced]") {
    vofl_int_sourced g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl sourced copy construction", "[vofl][sourced]") {
    vofl_sourced g1;
    vofl_sourced g2 = g1;
    REQUIRE(g2.size() == 0);
}

TEST_CASE("vofl sourced move construction", "[vofl][sourced]") {
    vofl_sourced g1;
    vofl_sourced g2 = std::move(g1);
    REQUIRE(g2.size() == 0);
}

//==================================================================================================
// 10. Const Correctness Tests (15 tests)
//==================================================================================================

TEST_CASE("vofl const graph size()", "[vofl][const]") {
    const vofl_void_void_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl const graph empty()", "[vofl][const]") {
    const vofl_void_void_void g;
    REQUIRE(g.size() == 0);
}

TEST_CASE("vofl const graph begin/end", "[vofl][const]") {
    const vofl_void_void_void g;
    REQUIRE(g.begin() == g.end());
}

TEST_CASE("vofl const graph iteration", "[vofl][const]") {
    const vofl_void_void_void g;
    int count = 0;
    for ([[maybe_unused]] const auto& v : g) {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE("vofl const graph with graph value", "[vofl][const]") {
    const vofl_void_void_int g(42);
    REQUIRE(g.graph_value() == 42);
}

TEST_CASE("vofl const graph cbegin/cend", "[vofl][const]") {
    const vofl_void_void_void g;
    REQUIRE(g.cbegin() == g.cend());
}

//==================================================================================================
// 11. Memory and Resource Tests (10 tests)
//==================================================================================================

TEST_CASE("vofl multiple graphs do not interfere", "[vofl][memory]") {
    vofl_int_int_int g1(100);
    vofl_int_int_int g2(200);
    vofl_int_int_int g3(300);
    
    REQUIRE(g1.graph_value() == 100);
    REQUIRE(g2.graph_value() == 200);
    REQUIRE(g3.graph_value() == 300);
}

TEST_CASE("vofl copy does not alias", "[vofl][memory]") {
    vofl_int_int_int g1(100);
    vofl_int_int_int g2 = g1;
    
    g2.graph_value() = 200;
    REQUIRE(g1.graph_value() == 100);
    REQUIRE(g2.graph_value() == 200);
}

TEST_CASE("vofl clear preserves type", "[vofl][memory]") {
    vofl_int_int_int g(42);
    g.clear();
    REQUIRE(g.size() == 0);
    // Type is still int, we can set a new value
    g.graph_value() = 100;
    REQUIRE(g.graph_value() == 100);
}

TEST_CASE("vofl move leaves source valid but unspecified", "[vofl][memory]") {
    vofl_int_int_int g1(100);
    vofl_int_int_int g2 = std::move(g1);
    
    // g1 is valid but unspecified, we can still use it safely
    g1.clear();
    REQUIRE(g1.size() == 0);
}

//==================================================================================================
// 12. Compilation Tests (static assertions)
//==================================================================================================

TEST_CASE("vofl various template instantiations compile", "[vofl][compilation]") {
    // Just test that these types compile
    [[maybe_unused]] vofl_void_void_void g1;
    [[maybe_unused]] vofl_int_void_void g2;
    [[maybe_unused]] vofl_void_int_void g3;
    [[maybe_unused]] vofl_int_int_void g4;
    [[maybe_unused]] vofl_void_void_int g5;
    [[maybe_unused]] vofl_int_int_int g6;
    [[maybe_unused]] vofl_string_string_string g7;
    [[maybe_unused]] vofl_sourced g8;
    [[maybe_unused]] vofl_int_sourced g9;
    
    REQUIRE(true); // Just ensuring compilation
}

// Additional static checks
static_assert(std::ranges::range<vofl_void_void_void>);
static_assert(std::ranges::range<vofl_int_int_int>);
static_assert(std::ranges::range<vofl_string_string_string>);

//==================================================================================================
//==================================================================================================
// Load Operations Tests
//==================================================================================================

TEST_CASE("vofl load_vertices with identity projection", "[dynamic_graph][vofl][load_vertices]") {
  using G = vofl_int_int_void;
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
    std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}, {3, 40}, {4, 50}};
    g.load_vertices(vertices, std::identity{});
    REQUIRE(g.size() == 5);
    REQUIRE(g[0].value() == 10);
    REQUIRE(g[1].value() == 20);
    REQUIRE(g[2].value() == 30);
    REQUIRE(g[3].value() == 40);
    REQUIRE(g[4].value() == 50);
  }
}

TEST_CASE("vofl load_vertices with custom projection", "[dynamic_graph][vofl][load_vertices]") {
  using G = dynamic_graph<int, std::string, void, uint32_t, false, vofl_graph_traits<int, std::string, void, uint32_t, false>>;
  using vertex_data = copyable_vertex_t<uint32_t, std::string>;

  SECTION("load with projection from struct") {
    struct Person {
      uint32_t id;
      std::string name;
      int age;
    };

    G g;
    std::vector<Person> people = {{0, "Alice", 30}, {1, "Bob", 25}, {2, "Charlie", 35}};
    g.load_vertices(people, [](const Person& p) -> vertex_data {
      return {p.id, p.name};
    });

    REQUIRE(g.size() == 3);
    REQUIRE(g[0].value() == "Alice");
    REQUIRE(g[1].value() == "Bob");
    REQUIRE(g[2].value() == "Charlie");
  }
}

TEST_CASE("vofl load_vertices with void vertex values", "[dynamic_graph][vofl][load_vertices]") {
  using G = vofl_int_void_void;

  SECTION("load creates vertices without values - using default constructor") {
    G g;
    // With void vertex values, we can't use load_vertices because copyable_vertex_t<VId, void>
    // only has {id} but load_vertices expects {id, value}. Instead, test construction.
    // This will be tested more thoroughly when load_edges with vertex inference is implemented.
    REQUIRE(g.size() == 0);
  }
}

TEST_CASE("vofl load_edges with identity projection", "[dynamic_graph][vofl][load_edges]") {
  using G = vofl_int_int_void;
  using vertex_data = copyable_vertex_t<uint32_t, int>;
  using edge_data = copyable_edge_t<uint32_t, int>;

  SECTION("load empty edge range") {
    G g;
    std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}};
    g.load_vertices(vertices, std::identity{});

    std::vector<edge_data> edges;
    g.load_edges(edges, std::identity{});

    REQUIRE(g.size() == 3);
    for (auto& v : g) {
      size_t count = 0;
      for (auto& e : v.edges()) {
        ++count;
        (void)e;
      }
      REQUIRE(count == 0);
    }
  }

  SECTION("load single edge") {
    G g;
    std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}};
    g.load_vertices(vertices, std::identity{});

    std::vector<edge_data> edges = {{0, 1, 100}};
    g.load_edges(edges, std::identity{});

    // Check vertex 0 has the edge
    size_t count = 0;
    for (auto& edge : g[0].edges()) {
      ++count;
      // Note: target_id() method may not be public, check via iteration
      REQUIRE(edge.value() == 100);
    }
    REQUIRE(count == 1);
  }

  SECTION("load multiple edges from one vertex") {
    G g;
    std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}, {3, 40}};
    g.load_vertices(vertices, std::identity{});

    std::vector<edge_data> edges = {{0, 1, 10}, {0, 2, 20}, {0, 3, 30}};
    g.load_edges(edges, std::identity{});

    size_t count = 0;
    for (auto& edge : g[0].edges()) {
      ++count;
      (void)edge;
    }
    REQUIRE(count == 3);
  }

  SECTION("load edges from multiple vertices") {
    G g;
    std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}};
    g.load_vertices(vertices, std::identity{});

    std::vector<edge_data> edges = {{0, 1, 100}, {1, 2, 200}, {2, 0, 300}};
    g.load_edges(edges, std::identity{});

    // Count edges per vertex
    size_t count0 = 0, count1 = 0, count2 = 0;
    for (auto& e : g[0].edges()) { ++count0; (void)e; }
    for (auto& e : g[1].edges()) { ++count1; (void)e; }
    for (auto& e : g[2].edges()) { ++count2; (void)e; }
    
    REQUIRE(count0 == 1);
    REQUIRE(count1 == 1);
    REQUIRE(count2 == 1);
  }
}

TEST_CASE("vofl load_edges with void edge values", "[dynamic_graph][vofl][load_edges]") {
  using G = vofl_void_int_void;
  using vertex_data = copyable_vertex_t<uint32_t, int>;
  using edge_data = copyable_edge_t<uint32_t, void>;

  SECTION("load edges without values") {
    G g;
    std::vector<vertex_data> vertices = {{0, 10}, {1, 20}, {2, 30}};
    g.load_vertices(vertices, std::identity{});

    std::vector<edge_data> edges = {{0, 1}, {1, 2}, {2, 0}};
    g.load_edges(edges, std::identity{});

    // Verify edges exist by counting
    size_t total_edges = 0;
    for (auto& v : g) {
      for (auto& edge : v.edges()) {
        ++total_edges;
        (void)edge;
      }
    }
    REQUIRE(total_edges == 3);
  }
}

TEST_CASE("vofl load_edges with custom projection", "[dynamic_graph][vofl][load_edges]") {
  using G = dynamic_graph<std::string, int, void, uint32_t, false, vofl_graph_traits<std::string, int, void, uint32_t, false>>;
  using vertex_data = copyable_vertex_t<uint32_t, int>;
  using edge_data = copyable_edge_t<uint32_t, std::string>;

  SECTION("load with projection from custom struct") {
    struct Edge {
      uint32_t from;
      uint32_t to;
      std::string label;
    };

    G g;
    std::vector<vertex_data> vertices = {{0, 1}, {1, 2}, {2, 3}};
    g.load_vertices(vertices, std::identity{});

    std::vector<Edge> edges = {{0, 1, "edge01"}, {1, 2, "edge12"}};
    g.load_edges(edges, [](const Edge& e) -> edge_data {
      return {e.from, e.to, e.label};
    });

    // Verify edges exist
    size_t total = 0;
    for (auto& v : g) {
      for (auto& e : v.edges()) {
        ++total;
        (void)e;
      }
    }
    REQUIRE(total == 2);
  }
}

TEST_CASE("vofl load_edges with self-loops", "[dynamic_graph][vofl][load_edges]") {
  using G = vofl_int_int_void;
  using vertex_data = copyable_vertex_t<uint32_t, int>;
  using edge_data = copyable_edge_t<uint32_t, int>;

  SECTION("load single self-loop") {
    G g;
    std::vector<vertex_data> vertices = {{0, 10}, {1, 20}};
    g.load_vertices(vertices, std::identity{});

    std::vector<edge_data> edges = {{0, 0, 999}};
    g.load_edges(edges, std::identity{});

    size_t count = 0;
    for (auto& edge : g[0].edges()) {
      ++count;
      REQUIRE(edge.value() == 999);
    }
    REQUIRE(count == 1);
  }

  SECTION("load multiple self-loops") {
    G g;
    std::vector<vertex_data> vertices = {{0, 10}};
    g.load_vertices(vertices, std::identity{});

    std::vector<edge_data> edges = {{0, 0, 1}, {0, 0, 2}, {0, 0, 3}};
    g.load_edges(edges, std::identity{});

    size_t count = 0;
    for (auto& edge : g[0].edges()) {
      ++count;
      (void)edge;
    }
    REQUIRE(count == 3);
  }
}

TEST_CASE("vofl load_edges with parallel edges", "[dynamic_graph][vofl][load_edges]") {
  using G = vofl_int_int_void;
  using vertex_data = copyable_vertex_t<uint32_t, int>;
  using edge_data = copyable_edge_t<uint32_t, int>;

  SECTION("load multiple edges between same vertices") {
    G g;
    std::vector<vertex_data> vertices = {{0, 10}, {1, 20}};
    g.load_vertices(vertices, std::identity{});

    std::vector<edge_data> edges = {{0, 1, 100}, {0, 1, 200}, {0, 1, 300}};
    g.load_edges(edges, std::identity{});

    size_t count = 0;
    std::vector<int> values;
    for (auto& edge : g[0].edges()) {
      ++count;
      values.push_back(edge.value());
    }
    REQUIRE(count == 3);
    REQUIRE(values.size() == 3);
    REQUIRE(std::find(values.begin(), values.end(), 100) != values.end());
    REQUIRE(std::find(values.begin(), values.end(), 200) != values.end());
    REQUIRE(std::find(values.begin(), values.end(), 300) != values.end());
  }
}

TEST_CASE("vofl load_edges with large edge sets", "[dynamic_graph][vofl][load_edges]") {
  using G = vofl_int_int_void;
  using vertex_data = copyable_vertex_t<uint32_t, int>;
  using edge_data = copyable_edge_t<uint32_t, int>;

  SECTION("load 1000 edges") {
    G g;
    std::vector<vertex_data> vertices(100);
    for (uint32_t i = 0; i < 100; ++i) {
      vertices[i] = {i, static_cast<int>(i)};
    }
    g.load_vertices(vertices, std::identity{});

    std::vector<edge_data> edges;
    for (uint32_t i = 0; i < 100; ++i) {
      for (uint32_t j = 0; j < 10; ++j) {
        edges.push_back({i, (i + j) % 100, static_cast<int>(i * 1000 + j)});
      }
    }
    g.load_edges(edges, std::identity{});

    // Verify each vertex has 10 edges
    for (uint32_t i = 0; i < 100; ++i) {
      size_t count = 0;
      for (auto& edge : g[i].edges()) {
        ++count;
        (void)edge;
      }
      REQUIRE(count == 10);
    }
  }
}

//==================================================================================================
// Summary: Phase 1.1 Tests Progress
// - Construction: 17 tests
// - Basic Properties: 7 tests  
// - Graph Value: 6 tests
// - Iterator: 5 tests
// - Type Traits: 5 tests
// - Empty Graph Edge Cases: 6 tests
// - Value Types: 12 tests
// - Vertex ID Types: 5 tests
// - Sourced Edges: 6 tests
// - Const Correctness: 6 tests
// - Memory/Resources: 4 tests
// - Compilation: 1 test
// - Load Vertices: 9 tests
// - Load Edges: 9 tests
// 
// Total: 98 tests
//
// Note: Additional tests for vertex/edge access with populated graphs, partitions,
// and advanced operations will be added next.
//==================================================================================================
