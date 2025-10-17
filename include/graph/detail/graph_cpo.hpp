/**
 * @file graph_cpo.hpp
 * @brief Graph customization point objects (CPOs)
 * 
 * This file contains the implementation of all graph CPOs following the MSVC
 * standard library style as documented in docs/cpo.md.
 * 
 * All CPOs defined here allow customization through:
 * 1. Member functions (highest priority)
 * 2. ADL-findable free functions (medium priority)
 * 3. Default implementations (lowest priority)
 */

#pragma once

#include <concepts>
#include <ranges>
#include <iterator>

namespace graph {

namespace _cpo {
    /**
     * @brief Shared choice struct for all graph CPOs
     * 
     * Used to cache both the strategy (which customization path) and
     * the noexcept specification at compile time.
     */
    template<typename _Ty>
    struct _Choice_t {
        _Ty _Strategy = _Ty{};
        bool _No_throw = false;
    };

    // Future CPO implementations will be added here following the pattern:
    //
    // namespace _vertex_id {
    //     enum class _St { _none, _member, _adl, _custom };
    //     // ... concepts, _Choose(), _fn class
    // }
    //
    // namespace _num_vertices {
    //     enum class _St { _none, _member, _adl, _size };
    //     // ... concepts, _Choose(), _fn class
    // }

} // namespace _cpo

// Public CPO instances will be defined in inline namespace
// inline namespace _cpos {
//     inline constexpr _cpo::_vertex_id::_fn vertex_id{};
//     inline constexpr _cpo::_num_vertices::_fn num_vertices{};
//     // ... etc
// }

} // namespace graph
