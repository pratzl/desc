/**
 * @file graph_utility.hpp
 * @brief Utility functions and CPOs for graph operations
 * 
 * This file will contain common utility CPOs such as:
 * - vertex_id(g, v) - Get vertex ID
 * - num_vertices(g) - Get number of vertices
 * - num_edges(g) - Get number of edges
 * - vertices(g) - Get vertex range
 * - edges(g) - Get edge range
 * - out_edges(g, v) - Get outgoing edges from vertex
 */

#pragma once

#include <graph/descriptor.hpp>
#include <concepts>
#include <ranges>

namespace graph {

// Forward declarations for CPOs that will be implemented
// Following the MSVC-style CPO pattern documented in docs/cpo.md

namespace _cpo {
    // Shared _Choice_t struct for all CPOs
    template<typename _Ty>
    struct _Choice_t {
        _Ty _Strategy = _Ty{};
        bool _No_throw = false;
    };
    
    // Future: Implement vertex_id CPO
    // namespace _vertex_id { ... }
    
    // Future: Implement num_vertices CPO
    // namespace _num_vertices { ... }
    
    // Future: Implement num_edges CPO
    // namespace _num_edges { ... }
    
    // Future: Implement vertices CPO (returns range)
    // namespace _vertices { ... }
    
    // Future: Implement edges CPO (returns range)
    // namespace _edges { ... }
    
    // Future: Implement out_edges CPO (returns range)
    // namespace _out_edges { ... }
}

// Public CPO instances will be defined here
// inline namespace _cpos {
//     inline constexpr _cpo::_vertex_id::_fn vertex_id{};
//     inline constexpr _cpo::_num_vertices::_fn num_vertices{};
//     // ... etc
// }

} // namespace graph
