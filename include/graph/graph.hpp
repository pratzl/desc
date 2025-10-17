/**
 * @file graph.hpp
 * @brief Main header file for the graph library with graph concepts and traits
 * 
 * This header provides the complete graph library interface including:
 * - Core descriptor types and concepts
 * - Graph concepts and traits
 * - Graph information structures
 * - Basic containers (edgelist)
 * - Graph containers (when implemented)
 * - Graph algorithms (when implemented)
 * - Graph views (when implemented)
 * 
 * Include this file to access the full graph library functionality.
 */

#pragma once

#include <concepts>
#include <ranges>
#include <iterator>

// Core descriptor types and concepts
#include <graph/descriptor.hpp>
#include <graph/descriptor_traits.hpp>
#include <graph/vertex_descriptor.hpp>
#include <graph/vertex_descriptor_view.hpp>
#include <graph/edge_descriptor.hpp>
#include <graph/edge_descriptor_view.hpp>

// Graph information and utilities
#include <graph/graph_info.hpp>
#include <graph/edgelist.hpp>

// Detail headers
#include <graph/detail/graph_using.hpp>
#include <graph/detail/graph_cpo.hpp>

// Future: Container implementations will be included here
// #include <graph/container/adjacency_list.hpp>
// #include <graph/container/compressed_sparse_row.hpp>

// Future: Algorithm implementations will be included here
// #include <graph/algorithm/breadth_first_search.hpp>
// #include <graph/algorithm/depth_first_search.hpp>
// #include <graph/algorithm/dijkstra_shortest_paths.hpp>

// Future: View implementations will be included here
// #include <graph/views/vertices.hpp>
// #include <graph/views/edges.hpp>

/**
 * @namespace graph
 * @brief Root namespace for the graph library
 * 
 * All graph library types, functions, and concepts are defined within this namespace.
 * The library follows C++20 standards and provides customization point objects (CPOs)
 * for all graph operations, allowing adaptation to existing graph data structures.
 */
