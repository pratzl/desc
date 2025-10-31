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
#include "graph/vertex_descriptor_view.hpp"
#include "graph/descriptor.hpp"

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

} // namespace _cpo

namespace _cpo {

    // =========================================================================
    // vertices(g) CPO
    // =========================================================================
    
    namespace _vertices {
        enum class _St { _none, _member, _adl, _inner_value_pattern };
        
        // Check for g.vertices() member function
        template<typename G>
        concept _has_member = requires(G& g) {
            { g.vertices() } -> std::ranges::forward_range;
        };
        
        // Check for ADL vertices(g)
        template<typename G>
        concept _has_adl = requires(G& g) {
            { vertices(g) } -> std::ranges::forward_range;
        };
        
        // Check if g is a range with iterators that follow inner value pattern
        template<typename G>
        concept _has_inner_value_pattern = std::ranges::forward_range<G> && 
            requires { typename std::ranges::iterator_t<G>; } &&
            has_inner_value_pattern<std::ranges::iterator_t<G>>;
        
        template<typename G>
        [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
            if constexpr (_has_member<G>) {
                return {_St::_member, noexcept(std::declval<G&>().vertices())};
            } else if constexpr (_has_adl<G>) {
                return {_St::_adl, noexcept(vertices(std::declval<G&>()))};
            } else if constexpr (_has_inner_value_pattern<G>) {
                return {_St::_inner_value_pattern, true};
            } else {
                return {_St::_none, false};
            }
        }
        
        class _fn {
        private:
            template<typename G>
            static constexpr _Choice_t<_St> _Choice = _Choose<std::remove_cvref_t<G>>();
            
        public:
            /**
             * @brief Get range of vertices in graph
             * 
             * IMPORTANT: This CPO MUST always return a vertex_descriptor_view.
             * 
             * Resolution order:
             * 1. If g.vertices() exists -> use it (must return vertex_descriptor_view)
             * 2. If ADL vertices(g) exists -> use it (must return vertex_descriptor_view)
             * 3. If g follows inner value patterns -> return vertex_descriptor_view(g)
             * 
             * @param g Graph container
             * @return vertex_descriptor_view wrapping the vertices
             */
            template<typename G>
            [[nodiscard]] constexpr auto operator()(G&& g) const
                noexcept(_Choice<std::remove_cvref_t<G>>._No_throw)
                requires (_Choice<std::remove_cvref_t<G>>._Strategy != _St::_none)
            {
                using _G = std::remove_cvref_t<G>;
                if constexpr (_Choice<_G>._Strategy == _St::_member) {
                    return g.vertices();
                } else if constexpr (_Choice<_G>._Strategy == _St::_adl) {
                    return vertices(g);
                } else if constexpr (_Choice<_G>._Strategy == _St::_inner_value_pattern) {
                    return vertex_descriptor_view(g);
                }
            }
        };
    } // namespace _vertices

} // namespace _cpo

// Public CPO instances
inline namespace _cpos {
    /**
     * @brief CPO for getting vertex range from a graph
     * 
     * Usage: auto verts = graph::vertices(my_graph);
     * 
     * Returns: vertex_descriptor_view
     */
    inline constexpr _cpo::_vertices::_fn vertices{};
}

} // namespace graph
