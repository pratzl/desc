/**
 * @file adjacency_list_concepts.hpp
 * @brief Concepts for adjacency list graph structures
 * 
 * This file defines concepts for edges, vertices, and adjacency lists
 * that are used to constrain graph containers and algorithms.
 */

#pragma once

#include <concepts>
#include <ranges>
#include "detail/graph_cpo.hpp"

namespace graph {

// =============================================================================
// Edge Concepts
// =============================================================================

/**
 * @brief Concept for edges that have a target vertex
 * 
 * A targeted_edge is an edge that can provide the target vertex ID and descriptor.
 * This is the most basic edge concept.
 * 
 * Requirements:
 * - target_id(g, e) must be valid (returns vertex ID)
 * - target(g, e) must be valid (returns vertex descriptor)
 * - e must be an edge_descriptor
 * 
 * Note: Return types are not constrained to allow better compiler error messages.
 * 
 * Examples:
 * - Simple edge list: vector<int> where each int is a target vertex ID
 * - Weighted edges: vector<pair<int, double>> where first is target
 * 
 * @tparam G Graph type
 * @tparam E Edge type (must be edge_descriptor)
 */
template<typename G, typename E>
concept targeted_edge = 
    is_edge_descriptor_v<std::remove_cvref_t<E>> &&
    requires(G& g, const E& e) {
        target_id(g, e);
        target(g, e);
    };

/**
 * @brief Concept for edges that have a source vertex
 * 
 * A sourced_edge is an edge that can provide the source vertex ID and descriptor.
 * This is used for bidirectional graphs and edge lists where edges
 * need to track their source.
 * 
 * Requirements:
 * - source_id(g, e) must be valid (returns vertex ID)
 * - source(g, e) must be valid (returns vertex descriptor)
 * - e must be an edge_descriptor
 * 
 * Note: Return types are not constrained to allow better compiler error messages.
 * 
 * Examples:
 * - Edge list: vector<pair<int, int>> where first is source, second is target
 * - Bidirectional edge: struct { int from; int to; }
 * 
 * @tparam G Graph type
 * @tparam E Edge type (must be edge_descriptor)
 */
template<typename G, typename E>
concept sourced_edge = 
    is_edge_descriptor_v<std::remove_cvref_t<E>> &&
    requires(G& g, const E& e) {
        source_id(g, e);
        source(g, e);
    };

/**
 * @brief Concept for edges that have both source and target vertices
 * 
 * A sourced_targeted_edge combines both sourced_edge and targeted_edge,
 * providing access to both the source and target vertex IDs.
 * 
 * Requirements:
 * - Must satisfy both targeted_edge and sourced_edge
 * - Both source_id(g, e) and target_id(g, e) must be valid
 * 
 * Examples:
 * - Full edge list: vector<tuple<int, int, double>> where elements are source, target, weight
 * - Bidirectional weighted edge: struct { int from; int to; double weight; }
 * 
 * @tparam G Graph type
 * @tparam E Edge type
 */
template<typename G, typename E>
concept sourced_targeted_edge = targeted_edge<G, E> && sourced_edge<G, E>;

// =============================================================================
// Edge Range Concepts
// =============================================================================

/**
 * @brief Concept for a forward range of targeted edges
 * 
 * A targeted_edge_range is a range where each element satisfies the
 * targeted_edge concept. This is used to represent the outgoing edges
 * from a vertex in an adjacency list.
 * 
 * Requirements:
 * - Must be a std::ranges::forward_range
 * - Range value type must satisfy targeted_edge
 * 
 * Examples:
 * - Adjacency list: vector<vector<int>> where inner vector is target IDs
 * - Weighted adjacency: vector<vector<pair<int, double>>> where pair is {target, weight}
 * 
 * @tparam R Range type
 * @tparam G Graph type (optional, for compatibility)
 */
template<typename R, typename G = void>
concept targeted_edge_range = 
    std::ranges::forward_range<R> &&
    targeted_edge<G, std::ranges::range_value_t<R>>;

/**
 * @brief Concept for a forward range of sourced and targeted edges
 * 
 * A sourced_targeted_edge_range is a range where each element satisfies
 * both the sourced_edge and targeted_edge concepts. This is used for
 * bidirectional graphs and edge lists where edges need to know both endpoints.
 * 
 * Requirements:
 * - Must be a std::ranges::forward_range
 * - Range value type must satisfy sourced_targeted_edge
 * 
 * Examples:
 * - Edge list: vector<tuple<int, int, double>> where elements are {source, target, weight}
 * - Bidirectional adjacency: where edges store both source and target explicitly
 * 
 * @tparam R Range type
 * @tparam G Graph type (optional, for compatibility)
 */
template<typename R, typename G = void>
concept sourced_targeted_edge_range = 
    std::ranges::forward_range<R> &&
    sourced_targeted_edge<G, std::ranges::range_value_t<R>>;

// =============================================================================
// Vertex Range Concepts
// =============================================================================

/**
 * @brief Concept for a graph with a forward range of vertices
 * 
 * A vertex_range is a graph where vertices can be iterated as a forward range,
 * with each element being a vertex descriptor.
 * 
 * Requirements:
 * - vertices(g) must return a std::ranges::forward_range
 * - Range value type must be a vertex_descriptor
 * 
 * Note: sized_range requirement removed since some vertex ranges (e.g., over
 * map iterators) don't naturally support O(1) size() without counting.
 * 
 * Note: The vertex_id(g, v) operation is expected to be available but not
 * checked in the concept to avoid circular dependencies and provide better
 * error messages when used incorrectly.
 * 
 * Examples:
 * - vertex_descriptor_view over std::vector<T>
 * - vertex_descriptor_view over std::map<K, V>
 * - vertex_descriptor_view over std::deque<T>
 * 
 * @tparam G Graph type
 */
template<typename G>
concept vertex_range = 
    std::ranges::forward_range<vertex_range_t<G>> &&
    is_vertex_descriptor_v<std::remove_cvref_t<std::ranges::range_value_t<vertex_range_t<G>>>>;

/**
 * @brief Concept for a graph with random access range of vertices
 * 
 * An index_vertex_range is a vertex_range where vertices additionally support
 * random access, allowing O(1) access to any vertex by index.
 * 
 * Requirements:
 * - Must satisfy vertex_range
 * - vertices(g) must return a std::ranges::random_access_range
 * - Supports operator[] or equivalent for O(1) access
 * 
 * Examples:
 * - vertex_descriptor_view over std::vector<T> (index-based)
 * - vertex_descriptor_view over std::deque<T> (index-based)
 * 
 * Note: std::map-based graphs do NOT satisfy this concept as they
 * only provide bidirectional iteration, not random access.
 * 
 * @tparam G Graph type
 */
template<typename G>
concept index_vertex_range = 
    vertex_range<G> &&
    std::ranges::random_access_range<vertex_range_t<G>>;

// =============================================================================
// Adjacency List Concepts
// =============================================================================

/**
 * @brief Concept for graphs with adjacency list structure
 * 
 * An adjacency_list is a graph where:
 * - Vertices can be iterated as a vertex_range (forward)
 * - Each vertex has outgoing edges as a targeted_edge_range
 * 
 * Requirements:
 * - vertices(g) returns a vertex_range
 * - edges(g, u) returns a targeted_edge_range for vertex u
 * - Supports vertex_id(g, u) for each vertex
 * - Supports target_id(g, e) and target(g, e) for each edge
 * 
 * Examples:
 * - std::vector<std::vector<int>> - vector-based adjacency list
 * - std::map<int, std::vector<int>> - map-based adjacency list
 * - std::deque<std::vector<pair<int, double>>> - weighted adjacency list
 * 
 * @tparam G Graph type
 */
template<typename G>
concept adjacency_list = 
    vertex_range<G> &&
    requires(G& g, vertex_t<G> u) {
        { edges(g, u) } -> targeted_edge_range<G>;
    };

/**
 * @brief Concept for graphs with index-based adjacency list structure
 * 
 * An index_adjacency_list is an adjacency_list where vertices support
 * random access via an index_vertex_range.
 * 
 * Requirements:
 * - Must satisfy adjacency_list
 * - vertices(g) returns an index_vertex_range (random access)
 * 
 * Examples:
 * - std::vector<std::vector<int>> - contiguous vector storage
 * - std::deque<std::vector<int>> - deque-based storage
 * 
 * Note: std::map-based graphs do NOT satisfy this concept.
 * 
 * @tparam G Graph type
 */
template<typename G>
concept index_adjacency_list = 
    adjacency_list<G> &&
    index_vertex_range<G>;

/**
 * @brief Concept for graphs with sourced adjacency list structure
 * 
 * A sourced_adjacency_list is an adjacency_list where edges also
 * provide source vertex information (sourced_targeted_edge_range).
 * 
 * Requirements:
 * - Must satisfy adjacency_list
 * - edges(g, u) returns a sourced_targeted_edge_range
 * - Supports source_id(g, e) and source(g, e) for each edge
 * 
 * Examples:
 * - Adjacency lists where edges know their source vertex
 * - Useful for bidirectional graph traversal
 * 
 * @tparam G Graph type
 */
template<typename G>
concept sourced_adjacency_list = 
    adjacency_list<G> &&
    requires(G& g, vertex_t<G> u) {
        { edges(g, u) } -> sourced_targeted_edge_range<G>;
    };

/**
 * @brief Concept for graphs with index-based sourced adjacency list structure
 * 
 * An index_sourced_adjacency_list combines the requirements of both
 * index_adjacency_list and sourced_adjacency_list.
 * 
 * Requirements:
 * - Must satisfy index_adjacency_list
 * - Must satisfy sourced_adjacency_list
 * 
 * Examples:
 * - std::vector<std::vector<edge_with_source>> where edges track their source
 * 
 * @tparam G Graph type
 */
template<typename G>
concept index_sourced_adjacency_list = 
    index_adjacency_list<G> &&
    sourced_adjacency_list<G>;


} // namespace graph
