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
#include "graph/edge_descriptor_view.hpp"
#include "graph/descriptor.hpp"
#include "graph/descriptor_traits.hpp"

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
        
        // Helper to wrap result in vertex_descriptor_view if not already
        template<typename Result>
        [[nodiscard]] constexpr auto _wrap_if_needed(Result&& result) noexcept {
            using ResultType = std::remove_cvref_t<Result>;
            if constexpr (is_vertex_descriptor_view_v<ResultType>) {
                // Already a vertex_descriptor_view, return as-is
                return std::forward<Result>(result);
            } else {
                // Not a vertex_descriptor_view, wrap it
                return vertex_descriptor_view(std::forward<Result>(result));
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
             * 1. If g.vertices() exists -> use it (wrap in descriptor view if needed)
             * 2. If ADL vertices(g) exists -> use it (wrap in descriptor view if needed)
             * 3. If g follows inner value patterns -> return vertex_descriptor_view(g)
             * 
             * If custom g.vertices() or ADL vertices(g) already returns a 
             * vertex_descriptor_view, it's used as-is. Otherwise, the result is 
             * automatically wrapped in vertex_descriptor_view.
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
                    return _wrap_if_needed(g.vertices());
                } else if constexpr (_Choice<_G>._Strategy == _St::_adl) {
                    return _wrap_if_needed(vertices(g));
                } else if constexpr (_Choice<_G>._Strategy == _St::_inner_value_pattern) {
                    return vertex_descriptor_view(g);
                }
            }
        };
    } // namespace _vertices

    // =========================================================================
    // vertex_id(g, u) CPO
    // =========================================================================
    
    namespace _vertex_id {
        enum class _St { _none, _inner_value_member, _adl_inner_value, _adl_descriptor, _descriptor };
        
        // Check for inner_value.vertex_id(g) member function
        // Note: u.inner_value(g) returns the inner value from the descriptor
        template<typename G, typename U>
        concept _has_inner_value_member = 
            is_vertex_descriptor_v<std::remove_cvref_t<U>> &&
            requires(G& g, const U& u) {
                { u.inner_value(g).vertex_id(g) };
            };
        
        // Check for ADL vertex_id(g, inner_value)
        template<typename G, typename U>
        concept _has_adl_inner_value = 
            is_vertex_descriptor_v<std::remove_cvref_t<U>> &&
            requires(G& g, const U& u) {
                { vertex_id(g, u.inner_value(g)) };
            };
        
        // Check for ADL vertex_id(g, descriptor)
        template<typename G, typename U>
        concept _has_adl_descriptor = 
            is_vertex_descriptor_v<std::remove_cvref_t<U>> &&
            requires(const G& g, const U& u) {
                { vertex_id(g, u) };
            };
        
        // Check if descriptor has vertex_id() member (default)
        template<typename U>
        concept _has_descriptor = is_vertex_descriptor_v<std::remove_cvref_t<U>> &&
            requires(const U& u) {
                { u.vertex_id() };
            };
        
        template<typename G, typename U>
        [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
            if constexpr (_has_inner_value_member<G, U>) {
                return {_St::_inner_value_member, 
                        noexcept(std::declval<U&>().inner_value(std::declval<G&>()).vertex_id(std::declval<const G&>()))};
            } else if constexpr (_has_adl_inner_value<G, U>) {
                return {_St::_adl_inner_value, 
                        noexcept(vertex_id(std::declval<G&>(), std::declval<U&>().inner_value(std::declval<G&>())))};
            } else if constexpr (_has_adl_descriptor<G, U>) {
                return {_St::_adl_descriptor, 
                        noexcept(vertex_id(std::declval<const G&>(), std::declval<const U&>()))};
            } else if constexpr (_has_descriptor<U>) {
                return {_St::_descriptor, 
                        noexcept(std::declval<const U&>().vertex_id())};
            } else {
                return {_St::_none, false};
            }
        }
        
        class _fn {
        private:
            template<typename G, typename U>
            static constexpr _Choice_t<_St> _Choice = _Choose<std::remove_cvref_t<G>, std::remove_cvref_t<U>>();
            
        public:
            /**
             * @brief Get unique ID for a vertex
             * 
             * Resolution order (checks inner_value first, then descriptor):
             * 1. u.inner_value(g).vertex_id(g) - inner_value member function (highest priority)
             * 2. vertex_id(g, u.inner_value(g)) - ADL with inner_value
             * 3. vertex_id(g, u) - ADL with vertex_descriptor
             * 4. u.vertex_id() - descriptor's default method (lowest priority)
             * 
             * Where:
             * - u must be vertex_t<G> (the vertex descriptor type for graph G)
             * - u.inner_value(g) extracts the actual vertex data from the graph container
             * 
             * For random-access containers: returns the index
             * For associative containers: returns the key
             * For bidirectional containers: returns the iterator position
             * 
             * @tparam G Graph type
             * @tparam U Vertex descriptor type (constrained to be a vertex_descriptor_type)
             * @param g Graph container
             * @param u Vertex descriptor (must be vertex_t<G> - the vertex descriptor type for the graph)
             * @return Unique identifier for the vertex
             */
            template<typename G, vertex_descriptor_type U>
            [[nodiscard]] constexpr auto operator()(G& g, const U& u) const
                noexcept(_Choice<std::remove_cvref_t<G>, std::remove_cvref_t<U>>._No_throw)
                -> decltype(auto)
                requires (_Choice<std::remove_cvref_t<G>, std::remove_cvref_t<U>>._Strategy != _St::_none)
            {
                using _G = std::remove_cvref_t<G>;
                using _U = std::remove_cvref_t<U>;
                
                if constexpr (_Choice<_G, _U>._Strategy == _St::_inner_value_member) {
                    return u.inner_value(g).vertex_id(g);
                } else if constexpr (_Choice<_G, _U>._Strategy == _St::_adl_inner_value) {
                    return vertex_id(g, u.inner_value(g));
                } else if constexpr (_Choice<_G, _U>._Strategy == _St::_adl_descriptor) {
                    return vertex_id(g, u);
                } else if constexpr (_Choice<_G, _U>._Strategy == _St::_descriptor) {
                    return u.vertex_id();
                }
            }
        };
    } // namespace _vertex_id

    // =========================================================================
    // find_vertex(g, uid) CPO
    // =========================================================================
    
    namespace _find_vertex {
        enum class _St { _none, _member, _adl, _random_access };
        
        // Use the vertices CPO directly from _vertices namespace
        inline constexpr _vertices::_fn _vertices_cpo{};
        
        // Check for g.find_vertex(uid) member function
        template<typename G, typename VId>
        concept _has_member = requires(G& g, const VId& uid) {
            { g.find_vertex(uid) };
        };
        
        // Check for ADL find_vertex(g, uid)
        template<typename G, typename VId>
        concept _has_adl = requires(G& g, const VId& uid) {
            { find_vertex(g, uid) };
        };
        
        // Check if vertices(g) is sized (which implies we can use std::ranges::next with offset)
        template<typename G, typename VId>
        concept _has_random_access = 
            std::ranges::sized_range<decltype(_vertices_cpo(std::declval<G&>()))> &&
            requires(VId uid) {
                std::ranges::next(std::ranges::begin(_vertices_cpo(std::declval<G&>())), uid);
            };
        
        template<typename G, typename VId>
        [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
            if constexpr (_has_member<G, VId>) {
                return {_St::_member, 
                        noexcept(std::declval<G&>().find_vertex(std::declval<const VId&>()))};
            } else if constexpr (_has_adl<G, VId>) {
                return {_St::_adl, 
                        noexcept(find_vertex(std::declval<G&>(), std::declval<const VId&>()))};
            } else if constexpr (_has_random_access<G, VId>) {
                constexpr bool no_throw_vertices = noexcept(_vertices_cpo(std::declval<G&>()));
                constexpr bool no_throw_begin = noexcept(std::ranges::begin(_vertices_cpo(std::declval<G&>())));
                constexpr bool no_throw_next = noexcept(std::ranges::next(std::ranges::begin(_vertices_cpo(std::declval<G&>())), std::declval<const VId&>()));
                return {_St::_random_access, no_throw_vertices && no_throw_begin && no_throw_next};
            } else {
                return {_St::_none, false};
            }
        }
        
        class _fn {
        private:
            template<typename G, typename VId>
            static constexpr _Choice_t<_St> _Choice = _Choose<std::remove_cvref_t<G>, std::remove_cvref_t<VId>>();
            
        public:
            /**
             * @brief Find vertex by ID
             * 
             * Resolution order:
             * 1. g.find_vertex(uid) - member function (highest priority)
             * 2. find_vertex(g, uid) - ADL
             * 3. std::ranges::next(begin(vertices(g)), uid) - default for sized ranges (lowest priority)
             * 
             * For random-access containers (vector, deque): O(1) via indexed access
             * For ordered associative (map): O(log n) via member override
             * For unordered associative (unordered_map): O(1) average via member override
             * 
             * @tparam G Graph type
             * @tparam VId Vertex ID type (typically vertex_id_t<G>)
             * @param g Graph container
             * @param uid Vertex ID to find
             * @return Iterator to the vertex (vertex_iterator_t<G>)
             */
            template<typename G, typename VId>
            [[nodiscard]] constexpr auto operator()(G&& g, const VId& uid) const
                noexcept(_Choice<std::remove_cvref_t<G>, std::remove_cvref_t<VId>>._No_throw)
                -> decltype(auto)
                requires (_Choice<std::remove_cvref_t<G>, std::remove_cvref_t<VId>>._Strategy != _St::_none)
            {
                using _G = std::remove_cvref_t<G>;
                using _VId = std::remove_cvref_t<VId>;
                if constexpr (_Choice<_G, _VId>._Strategy == _St::_member) {
                    return std::forward<G>(g).find_vertex(uid);
                } else if constexpr (_Choice<_G, _VId>._Strategy == _St::_adl) {
                    return find_vertex(std::forward<G>(g), uid);
                } else if constexpr (_Choice<_G, _VId>._Strategy == _St::_random_access) {
                    return std::ranges::next(std::ranges::begin(_vertices_cpo(std::forward<G>(g))), uid);
                }
            }
        };
    } // namespace _find_vertex

    // =========================================================================
    // edges(g, u) CPO
    // =========================================================================
    
    namespace _edges {
        enum class _St { _none, _member, _adl, _edge_value_pattern };
        
        // Check for g.edges(u) member function
        template<typename G, typename U>
        concept _has_member = requires(G& g, const U& u) {
            { g.edges(u) } -> std::ranges::forward_range;
        };
        
        // Check for ADL edges(g, u)
        template<typename G, typename U>
        concept _has_adl = requires(G& g, const U& u) {
            { edges(g, u) } -> std::ranges::forward_range;
        };
        
        // Check if the vertex descriptor's inner value is a forward range
        // and its elements satisfy edge_value_type
        template<typename G, typename U>
        concept _has_edge_value_pattern = 
            is_vertex_descriptor_v<std::remove_cvref_t<U>> &&
            requires(G& g, const U& u) {
                { u.inner_value(g) } -> std::ranges::forward_range;
            } &&
            requires(G& g, const U& u) {
                typename std::ranges::range_value_t<decltype(u.inner_value(g))>;
                requires edge_value_type<std::ranges::range_value_t<decltype(u.inner_value(g))>>;
            };
        
        template<typename G, typename U>
        [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
            if constexpr (_has_member<G, U>) {
                return {_St::_member, noexcept(std::declval<G&>().edges(std::declval<const U&>()))};
            } else if constexpr (_has_adl<G, U>) {
                return {_St::_adl, noexcept(edges(std::declval<G&>(), std::declval<const U&>()))};
            } else if constexpr (_has_edge_value_pattern<G, U>) {
                return {_St::_edge_value_pattern, noexcept(std::declval<const U&>().inner_value(std::declval<G&>()))};
            } else {
                return {_St::_none, false};
            }
        }
        
        // Helper to wrap result in edge_descriptor_view if not already
        template<typename Result, typename U>
        [[nodiscard]] constexpr auto _wrap_if_needed(Result&& result, const U& source_vertex) noexcept {
            using ResultType = std::remove_cvref_t<Result>;
            if constexpr (is_edge_descriptor_view_v<ResultType>) {
                // Already an edge_descriptor_view, return as-is
                return std::forward<Result>(result);
            } else {
                // Not an edge_descriptor_view, wrap it
                return edge_descriptor_view(std::forward<Result>(result), source_vertex);
            }
        }
        
        class _fn {
        private:
            template<typename G, typename U>
            static constexpr _Choice_t<_St> _Choice = _Choose<std::remove_cvref_t<G>, std::remove_cvref_t<U>>();
            
        public:
            /**
             * @brief Get range of outgoing edges from a vertex
             * 
             * IMPORTANT: This CPO MUST always return an edge_descriptor_view.
             * 
             * Resolution order:
             * 1. If g.edges(u) exists -> use it (wrap in descriptor view if needed)
             * 2. If ADL edges(g, u) exists -> use it (wrap in descriptor view if needed)
             * 3. If u.inner_value(g) is a forward range of edge_value_type elements 
             *    -> return edge_descriptor_view(u.inner_value(g), u)
             * 
             * If custom g.edges(u) or ADL edges(g, u) already returns an 
             * edge_descriptor_view, it's used as-is. Otherwise, the result is 
             * automatically wrapped in edge_descriptor_view.
             * 
             * The default implementation works automatically for common patterns:
             * - Simple edges: vector<int> (target IDs)
             * - Pair edges: vector<pair<int, Weight>> (target + property)
             * - Tuple edges: vector<tuple<int, ...>> (target + properties)
             * - Custom edges: structs/classes with custom extraction
             * 
             * @tparam G Graph type
             * @tparam U Vertex descriptor type (constrained to be a vertex_descriptor_type)
             * @param g Graph container
             * @param u Vertex descriptor (must be vertex_t<G> - the vertex descriptor type for the graph)
             * @return edge_descriptor_view wrapping the edges
             */
            template<typename G, vertex_descriptor_type U>
            [[nodiscard]] constexpr auto operator()(G&& g, const U& u) const
                noexcept(_Choice<std::remove_cvref_t<G>, std::remove_cvref_t<U>>._No_throw)
                requires (_Choice<std::remove_cvref_t<G>, std::remove_cvref_t<U>>._Strategy != _St::_none)
            {
                using _G = std::remove_cvref_t<G>;
                using _U = std::remove_cvref_t<U>;
                if constexpr (_Choice<_G, _U>._Strategy == _St::_member) {
                    return _wrap_if_needed(g.edges(u), u);
                } else if constexpr (_Choice<_G, _U>._Strategy == _St::_adl) {
                    return _wrap_if_needed(edges(g, u), u);
                } else if constexpr (_Choice<_G, _U>._Strategy == _St::_edge_value_pattern) {
                    return edge_descriptor_view(u.inner_value(g), u);
                }
            }
        };
    } // namespace _edges

    // =========================================================================
    // target_id(g, uv) CPO
    // =========================================================================
    
    namespace _target_id {
        enum class _St { _none, _adl_descriptor, _descriptor };
        
        // Check for ADL target_id(g, descriptor) - highest priority custom override
        // This allows customization by providing a free function that takes the descriptor
        template<typename G, typename E>
        concept _has_adl_descriptor = 
            is_edge_descriptor_v<std::remove_cvref_t<E>> &&
            requires(const G& g, const E& uv) {
                { target_id(g, uv) };
            };
        
        // Check if descriptor has target_id() member (default lowest priority)
        // Note: edge_descriptor.target_id() requires the edge container, not the graph
        // So we check for target_id(edges) where edges is obtained from the vertex
        template<typename G, typename E>
        concept _has_descriptor = is_edge_descriptor_v<std::remove_cvref_t<E>> &&
            requires(G& g, const E& uv) {
                { uv.source().inner_value(g) };
                { uv.target_id(uv.source().inner_value(g)) };
            };
        
        template<typename G, typename E>
        [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
            if constexpr (_has_adl_descriptor<G, E>) {
                return {_St::_adl_descriptor, 
                        noexcept(target_id(std::declval<const G&>(), std::declval<const E&>()))};
            } else if constexpr (_has_descriptor<G, E>) {
                return {_St::_descriptor, 
                        noexcept(std::declval<const E&>().target_id(std::declval<const E&>().source().inner_value(std::declval<G&>())))};
            } else {
                return {_St::_none, false};
            }
        }
        
        class _fn {
        private:
            template<typename G, typename E>
            static constexpr _Choice_t<_St> _Choice = _Choose<std::remove_cvref_t<G>, std::remove_cvref_t<E>>();
            
        public:
            /**
             * @brief Get target vertex ID from an edge
             * 
             * Resolution order (two-tier approach):
             * 1. target_id(g, uv) - ADL with edge_descriptor (highest priority)
             * 2. uv.target_id(uv.source().inner_value(g)) - descriptor's default method (lowest priority)
             * 
             * Where:
             * - uv must be edge_t<G> (the edge descriptor type for graph G)
             * - The default implementation uses the edge container from the source vertex
             * 
             * Edge data extraction (default implementation):
             * - Simple integral type (int): Returns the value itself (the target ID)
             * - Pair<target, property>: Returns .first (the target ID)
             * - Tuple<target, prop1, ...>: Returns std::get<0> (the target ID)
             * - Custom struct/type: User provides custom extraction via ADL
             * 
             * @tparam G Graph type
             * @tparam E Edge descriptor type (constrained to be an edge_descriptor_type)
             * @param g Graph container
             * @param uv Edge descriptor (must be edge_t<G> - the edge descriptor type for the graph)
             * @return Target vertex identifier
             */
            template<typename G, edge_descriptor_type E>
            [[nodiscard]] constexpr auto operator()(G& g, const E& uv) const
                noexcept(_Choice<std::remove_cvref_t<G>, std::remove_cvref_t<E>>._No_throw)
                -> decltype(auto)
                requires (_Choice<std::remove_cvref_t<G>, std::remove_cvref_t<E>>._Strategy != _St::_none)
            {
                using _G = std::remove_cvref_t<G>;
                using _E = std::remove_cvref_t<E>;
                
                if constexpr (_Choice<_G, _E>._Strategy == _St::_adl_descriptor) {
                    return target_id(g, uv);
                } else if constexpr (_Choice<_G, _E>._Strategy == _St::_descriptor) {
                    // Default: use edge_descriptor.target_id() with the edge container from source vertex
                    return uv.target_id(uv.source().inner_value(g));
                }
            }
        };
    } // namespace _target_id

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
    
    /**
     * @brief CPO for getting vertex ID from a vertex descriptor
     * 
     * Usage: auto id = graph::vertex_id(my_graph, vertex_descriptor);
     * 
     * Returns: Vertex ID (index for vector, key for map, etc.)
     */
    inline constexpr _cpo::_vertex_id::_fn vertex_id{};
    
    /**
     * @brief CPO for finding a vertex by its ID
     * 
     * Usage: auto v_iter = graph::find_vertex(my_graph, vertex_id);
     * 
     * Returns: Iterator to the vertex (vertex_iterator_t<G>)
     */
    inline constexpr _cpo::_find_vertex::_fn find_vertex{};
    
    /**
     * @brief CPO for getting outgoing edges from a vertex
     * 
     * Usage: auto vertex_edges = graph::edges(my_graph, vertex_descriptor);
     * 
     * Returns: edge_descriptor_view
     */
    inline constexpr _cpo::_edges::_fn edges{};
    
    /**
     * @brief CPO for getting target vertex ID from an edge
     * 
     * Usage: auto target = graph::target_id(my_graph, edge_descriptor);
     * 
     * Returns: Vertex ID of the target vertex
     */
    inline constexpr _cpo::_target_id::_fn target_id{};
}

// ============================================================================
// Type Aliases based on vertices(g)
// ============================================================================

/**
 * @brief Range type returned by vertices(g)
 * 
 * This is always vertex_descriptor_view<Iter> where Iter is the iterator
 * type of the underlying container.
 */
template<typename G>
using vertex_range_t = decltype(vertices(std::declval<G&>()));

/**
 * @brief Iterator type for traversing vertices
 * 
 * Iterator over the vertex_descriptor_view returned by vertices(g).
 */
template<typename G>
using vertex_iterator_t = std::ranges::iterator_t<vertex_range_t<G>>;

/**
 * @brief Vertex descriptor type for graph G
 * 
 * This is the value_type of the vertex range - a vertex_descriptor<Iter>
 * that wraps an iterator into the graph's vertex container.
 */
template<typename G>
using vertex_t = std::ranges::range_value_t<vertex_range_t<G>>;

// ============================================================================
// Type Alias based on vertex_id(g, u)
// ============================================================================

/**
 * @brief Vertex ID type for graph G
 * 
 * The type of the unique identifier returned by vertex_id(g, u).
 * - For random-access containers (vector, deque): size_t (index)
 * - For associative containers (map): key type
 * - For bidirectional containers: iterator-based ID
 */
template<typename G>
using vertex_id_t = decltype(vertex_id(std::declval<G&>(), std::declval<vertex_t<G>>()));

// ============================================================================
// Type Aliases based on edges(g, u)
// ============================================================================

/**
 * @brief Range type returned by edges(g, u)
 * 
 * This is always edge_descriptor_view<EdgeIter, VertexIter> where EdgeIter
 * is the iterator type of the underlying edge container and VertexIter is
 * the iterator type of the vertex container.
 */
template<typename G>
using vertex_edge_range_t = decltype(edges(std::declval<G&>(), std::declval<vertex_t<G>>()));

/**
 * @brief Iterator type for traversing edges from a vertex
 * 
 * Iterator over the edge_descriptor_view returned by edges(g, u).
 */
template<typename G>
using vertex_edge_iterator_t = std::ranges::iterator_t<vertex_edge_range_t<G>>;

/**
 * @brief Edge descriptor type for graph G
 * 
 * This is the value_type of the edge range - an edge_descriptor<EdgeIter, VertexIter>
 * that wraps an edge and maintains its source vertex.
 */
template<typename G>
using edge_t = std::ranges::range_value_t<vertex_edge_range_t<G>>;

} // namespace graph
