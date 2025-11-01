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
 * @tparam G Graph type
 * @tparam R Range type
 */
template<typename G, typename R>
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
 * @tparam G Graph type
 * @tparam R Range type
 */
template<typename G, typename R>
concept sourced_targeted_edge_range = 
    std::ranges::forward_range<R> &&
    sourced_targeted_edge<G, std::ranges::range_value_t<R>>;

} // namespace graph
