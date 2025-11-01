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

namespace _cpo_impls {
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

} // namespace _cpo_impls

namespace _cpo_impls {

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

} // namespace _cpo_impls

// =============================================================================
// vertices(g) - Public CPO instance and type aliases
// =============================================================================

inline namespace _cpo_instances {
    /**
     * @brief CPO for getting vertex range from a graph
     * 
     * Usage: auto verts = graph::vertices(my_graph);
     * 
     * Returns: vertex_descriptor_view
     */
    inline constexpr _cpo_impls::_vertices::_fn vertices{};
} // namespace _cpo_instances

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

namespace _cpo_impls {

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

} // namespace _cpo_impls

// =============================================================================
// vertex_id(g, u) - Public CPO instance and type alias
// =============================================================================

inline namespace _cpo_instances {
    /**
     * @brief CPO for getting vertex ID from a vertex descriptor
     * 
     * Usage: auto id = graph::vertex_id(my_graph, vertex_descriptor);
     * 
     * Returns: Vertex ID (index for vector, key for map, etc.)
     */
    inline constexpr _cpo_impls::_vertex_id::_fn vertex_id{};
} // namespace _cpo_instances

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

namespace _cpo_impls {

    // =========================================================================
    // find_vertex(g, uid) CPO
    // =========================================================================
    
    namespace _find_vertex {
        enum class _St { _none, _member, _adl, _associative, _random_access };
        
        // Use the public vertices CPO (already declared above)
        using graph::vertices;
        
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
        
        // Check if graph is an associative container with find() member
        // (map, unordered_map, etc. where the key is the vertex ID)
        template<typename G, typename VId>
        concept _has_associative = requires(G& g, const VId& uid) {
            { g.find(uid) } -> std::same_as<typename G::iterator>;
            { g.end() } -> std::same_as<typename G::iterator>;
        };
        
        // Check if vertices(g) is sized (which implies we can use std::ranges::next with offset)
        template<typename G, typename VId>
        concept _has_random_access = 
            std::ranges::sized_range<decltype(vertices(std::declval<G&>()))> &&
            requires(VId uid) {
                std::ranges::next(std::ranges::begin(vertices(std::declval<G&>())), uid);
            };
        
        template<typename G, typename VId>
        [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
            if constexpr (_has_member<G, VId>) {
                return {_St::_member, 
                        noexcept(std::declval<G&>().find_vertex(std::declval<const VId&>()))};
            } else if constexpr (_has_adl<G, VId>) {
                return {_St::_adl, 
                        noexcept(find_vertex(std::declval<G&>(), std::declval<const VId&>()))};
            } else if constexpr (_has_associative<G, VId>) {
                constexpr bool no_throw_find = noexcept(std::declval<G&>().find(std::declval<const VId&>()));
                // Note: We directly construct iterator from map iterator
                return {_St::_associative, no_throw_find};
            } else if constexpr (_has_random_access<G, VId>) {
                constexpr bool no_throw_vertices = noexcept(vertices(std::declval<G&>()));
                constexpr bool no_throw_begin = noexcept(std::ranges::begin(vertices(std::declval<G&>())));
                constexpr bool no_throw_next = noexcept(std::ranges::next(std::ranges::begin(vertices(std::declval<G&>())), std::declval<const VId&>()));
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
             * 3. g.find(uid) + convert to vertex iterator - associative container default
             * 4. std::ranges::next(begin(vertices(g)), uid) - random access default (lowest priority)
             * 
             * Complexity:
             * - Random-access containers (vector, deque): O(1) via indexed access
             * - Ordered associative (map): O(log n) via find()
             * - Unordered associative (unordered_map): O(1) average via find()
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
                } else if constexpr (_Choice<_G, _VId>._Strategy == _St::_associative) {
                    // For associative containers, use find() directly
                    // Construct vertex_descriptor_view iterator directly from map iterator
                    auto map_iter = g.find(uid);
                    using container_iterator = decltype(map_iter);
                    using view_type = vertex_descriptor_view<container_iterator>;
                    using view_iterator = typename view_type::iterator;
                    return view_iterator{map_iter};
                } else if constexpr (_Choice<_G, _VId>._Strategy == _St::_random_access) {
                    return std::ranges::next(std::ranges::begin(vertices(std::forward<G>(g))), uid);
                }
            }
        };
    } // namespace _find_vertex

} // namespace _cpo_impls

// =============================================================================
// find_vertex(g, uid) - Public CPO instance
// =============================================================================

inline namespace _cpo_instances {
    /**
     * @brief CPO for finding a vertex by its ID
     * 
     * Usage: auto v_iter = graph::find_vertex(my_graph, vertex_id);
     * 
     * Returns: Iterator to the vertex (vertex_iterator_t<G>)
     */
    inline constexpr _cpo_impls::_find_vertex::_fn find_vertex{};
} // namespace _cpo_instances

namespace _cpo_impls {

    // =========================================================================
    // edges(g, u) and edges(g, uid) CPO
    // =========================================================================
    
    namespace _edges {
        // Use the public CPO instances (already declared above)
        using graph::find_vertex;
        
        // Strategy enum for edges(g, u) - vertex descriptor version
        enum class _St_u { _none, _member, _adl, _edge_value_pattern };
        
        // Check for g.edges(u) member function - vertex descriptor
        template<typename G, typename U>
        concept _has_member_u = requires(G& g, const U& u) {
            { g.edges(u) } -> std::ranges::forward_range;
        };
        
        // Check for ADL edges(g, u) - vertex descriptor
        template<typename G, typename U>
        concept _has_adl_u = requires(G& g, const U& u) {
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
        [[nodiscard]] consteval _Choice_t<_St_u> _Choose_u() noexcept {
            if constexpr (_has_member_u<G, U>) {
                return {_St_u::_member, noexcept(std::declval<G&>().edges(std::declval<const U&>()))};
            } else if constexpr (_has_adl_u<G, U>) {
                return {_St_u::_adl, noexcept(edges(std::declval<G&>(), std::declval<const U&>()))};
            } else if constexpr (_has_edge_value_pattern<G, U>) {
                return {_St_u::_edge_value_pattern, noexcept(std::declval<const U&>().inner_value(std::declval<G&>()))};
            } else {
                return {_St_u::_none, false};
            }
        }
        
        // Strategy enum for edges(g, uid) - vertex ID version
        enum class _St_uid { _none, _member, _adl, _default };
        
        // Check for g.edges(uid) member function - vertex ID
        template<typename G, typename VId>
        concept _has_member_uid = requires(G& g, const VId& uid) {
            { g.edges(uid) } -> std::ranges::forward_range;
        };
        
        // Check for ADL edges(g, uid) - vertex ID
        template<typename G, typename VId>
        concept _has_adl_uid = requires(G& g, const VId& uid) {
            { edges(g, uid) } -> std::ranges::forward_range;
        };
        
        // Check if we can use default implementation: edges(g, *find_vertex(g, uid))
        // We check that find_vertex works and returns something dereferenceable to a vertex descriptor
        template<typename G, typename VId>
        concept _has_default_uid = requires(G& g, const VId& uid) {
            { find_vertex(g, uid) } -> std::input_iterator;
            requires vertex_descriptor_type<decltype(*find_vertex(g, uid))>;
            requires _has_edge_value_pattern<G, decltype(*find_vertex(g, uid))>;
        };
        
        template<typename G, typename VId>
        [[nodiscard]] consteval _Choice_t<_St_uid> _Choose_uid() noexcept {
            if constexpr (_has_member_uid<G, VId>) {
                return {_St_uid::_member, noexcept(std::declval<G&>().edges(std::declval<const VId&>()))};
            } else if constexpr (_has_adl_uid<G, VId>) {
                return {_St_uid::_adl, noexcept(edges(std::declval<G&>(), std::declval<const VId&>()))};
            } else if constexpr (_has_default_uid<G, VId>) {
                // Default is not noexcept as it depends on find_vertex and edges(g,u)
                return {_St_uid::_default, false};
            } else {
                return {_St_uid::_none, false};
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
            static constexpr _Choice_t<_St_u> _Choice_u = _Choose_u<std::remove_cvref_t<G>, std::remove_cvref_t<U>>();
            
            template<typename G, typename VId>
            static constexpr _Choice_t<_St_uid> _Choice_uid = _Choose_uid<std::remove_cvref_t<G>, std::remove_cvref_t<VId>>();
            
        public:
            /**
             * @brief Get range of outgoing edges from a vertex (by descriptor)
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
                noexcept(_Choice_u<std::remove_cvref_t<G>, std::remove_cvref_t<U>>._No_throw)
                requires (_Choice_u<std::remove_cvref_t<G>, std::remove_cvref_t<U>>._Strategy != _St_u::_none)
            {
                using _G = std::remove_cvref_t<G>;
                using _U = std::remove_cvref_t<U>;
                if constexpr (_Choice_u<_G, _U>._Strategy == _St_u::_member) {
                    return _wrap_if_needed(g.edges(u), u);
                } else if constexpr (_Choice_u<_G, _U>._Strategy == _St_u::_adl) {
                    return _wrap_if_needed(edges(g, u), u);
                } else if constexpr (_Choice_u<_G, _U>._Strategy == _St_u::_edge_value_pattern) {
                    return edge_descriptor_view(u.inner_value(g), u);
                }
            }
            
            /**
             * @brief Get range of outgoing edges from a vertex (by ID)
             * 
             * IMPORTANT: This CPO MUST always return an edge_descriptor_view.
             * 
             * This is a convenience function that combines find_vertex + edges(g,u).
             * 
             * Resolution order:
             * 1. g.edges(uid) - Member function (highest priority)
             * 2. edges(g, uid) - ADL (medium priority)
             * 3. edges(g, *find_vertex(g, uid)) - Default (lowest priority)
             * 
             * The default implementation:
             * - Uses find_vertex(g, uid) to get the vertex iterator
             * - Dereferences to get the vertex descriptor
             * - Calls edges(g, u) with the vertex descriptor
             * 
             * Complexity matches find_vertex:
             * - Random-access containers (vector, deque): O(1)
             * - Ordered associative (map): O(log n)
             * - Unordered associative (unordered_map): O(1) average
             * 
             * @tparam G Graph type
             * @tparam VId Vertex ID type (typically vertex_id_t<G>)
             * @param g Graph container
             * @param uid Vertex ID
             * @return edge_descriptor_view wrapping the edges
             */
            template<typename G, typename VId>
                requires (!vertex_descriptor_type<VId>)
            [[nodiscard]] constexpr auto operator()(G&& g, const VId& uid) const
                noexcept(_Choice_uid<std::remove_cvref_t<G>, std::remove_cvref_t<VId>>._No_throw)
                requires (_Choice_uid<std::remove_cvref_t<G>, std::remove_cvref_t<VId>>._Strategy != _St_uid::_none)
            {
                using _G = std::remove_cvref_t<G>;
                using _VId = std::remove_cvref_t<VId>;
                
                if constexpr (_Choice_uid<_G, _VId>._Strategy == _St_uid::_member) {
                    // Member function: g.edges(uid)
                    // Wrap result but we don't have the vertex descriptor yet
                    // The member function must handle this appropriately
                    auto result = g.edges(uid);
                    // For member function, we need to get the vertex descriptor to wrap properly
                    auto v = *find_vertex(g, uid);
                    return _wrap_if_needed(std::move(result), v);
                } else if constexpr (_Choice_uid<_G, _VId>._Strategy == _St_uid::_adl) {
                    // ADL: edges(g, uid)
                    auto result = edges(g, uid);
                    auto v = *find_vertex(g, uid);
                    return _wrap_if_needed(std::move(result), v);
                } else if constexpr (_Choice_uid<_G, _VId>._Strategy == _St_uid::_default) {
                    // Default: find vertex then call edges(g, u)
                    auto v = *find_vertex(std::forward<G>(g), uid);
                    return (*this)(std::forward<G>(g), v);
                }
            }
        };
    } // namespace _edges

} // namespace _cpo_impls

// =============================================================================
// edges(g, u) - Public CPO instance and type aliases
// =============================================================================

inline namespace _cpo_instances {
    /**
     * @brief CPO for getting outgoing edges from a vertex
     * 
     * Usage: auto vertex_edges = graph::edges(my_graph, vertex_descriptor);
     * 
     * Returns: edge_descriptor_view
     */
    inline constexpr _cpo_impls::_edges::_fn edges{};
} // namespace _cpo_instances

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

namespace _cpo_impls {

    // =========================================================================
    // target_id(g, uv) CPO
    // =========================================================================
    
    namespace _target_id {
        enum class _St { _none, _adl_descriptor, _descriptor };
        
        // Check for ADL target_id(g, descriptor) - highest priority custom override
        // This allows customization by providing a free function that takes the descriptor
        // Accepts either edge_descriptor or underlying edge value type
        template<typename G, typename E>
        concept _has_adl_descriptor = 
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

} // namespace _cpo_impls

// =============================================================================
// target_id(g, uv) - Public CPO instance
// =============================================================================

inline namespace _cpo_instances {
    /**
     * @brief CPO for getting target vertex ID from an edge
     * 
     * Usage: auto target = graph::target_id(my_graph, edge_descriptor);
     * 
     * Returns: Vertex ID of the target vertex
     */
    inline constexpr _cpo_impls::_target_id::_fn target_id{};
} // namespace _cpo_instances

namespace _cpo_impls {

    // =========================================================================
    // target(g, uv) CPO
    // =========================================================================
    
    namespace _target {
        enum class _St { _none, _member, _adl, _default };
        
        // Use the public CPO instances (already declared above)
        using graph::find_vertex;
        using graph::target_id;
        
        // Check for g.target(uv) member function
        // Accepts either edge_descriptor or underlying edge value type
        template<typename G, typename E>
        concept _has_member = 
            requires(G& g, const E& uv) {
                { g.target(uv) };
            };
        
        // Check for ADL target(g, uv)
        // Accepts either edge_descriptor or underlying edge value type
        template<typename G, typename E>
        concept _has_adl = 
            requires(G& g, const E& uv) {
                { target(g, uv) };
            };
        
        // Check if we can use default implementation: find_vertex(g, target_id(g, uv))
        template<typename G, typename E>
        concept _has_default = 
            is_edge_descriptor_v<std::remove_cvref_t<E>> &&
            requires(G& g, const E& uv) {
                { target_id(g, uv) };
                { find_vertex(g, target_id(g, uv)) };
            };
        
        template<typename G, typename E>
        [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
            if constexpr (_has_member<G, E>) {
                return {_St::_member, 
                        noexcept(std::declval<G&>().target(std::declval<const E&>()))};
            } else if constexpr (_has_adl<G, E>) {
                return {_St::_adl, 
                        noexcept(target(std::declval<G&>(), std::declval<const E&>()))};
            } else if constexpr (_has_default<G, E>) {
                // Default is not noexcept as it depends on target_id and find_vertex
                return {_St::_default, false};
            } else {
                return {_St::_none, false};
            }
        }
        
        // Helper to convert result to vertex descriptor if needed
        // Supports two cases:
        // 1. Result is already a vertex_descriptor -> return as-is
        // 2. Result is an iterator (to vertex_descriptor_view) -> dereference to get vertex_descriptor
        //
        // Note: Custom implementations should return either:
        // - A vertex_descriptor directly (vertex_t<G>)
        // - An iterator from vertices(g) (vertex_iterator_t<G>)
        //
        // This checks if dereferencing the result yields a vertex_descriptor to handle both cases.
        template<typename G, typename Result>
        [[nodiscard]] constexpr auto _to_vertex_descriptor(G&&, Result&& result) noexcept {
            using ResultType = std::remove_cvref_t<Result>;
            if constexpr (is_vertex_descriptor_v<ResultType>) {
                // Already a vertex_descriptor, return as-is
                return std::forward<Result>(result);
            } else {
                // Assume it's an iterator - dereference to get vertex_descriptor
                // This works for vertex_descriptor_view::iterator which yields vertex_descriptor when dereferenced
                return *std::forward<Result>(result);
            }
        }
        
        class _fn {
        private:
            template<typename G, typename E>
            static constexpr _Choice_t<_St> _Choice = _Choose<std::remove_cvref_t<G>, std::remove_cvref_t<E>>();
            
        public:
            /**
             * @brief Get target vertex descriptor from an edge
             * 
             * Resolution order:
             * 1. g.target(uv) - Member function (highest priority)
             *    - May return either vertex_descriptor or vertex_iterator (auto-converted)
             * 2. target(g, uv) - ADL (medium priority)
             *    - May return either vertex_descriptor or vertex_iterator (auto-converted)
             * 3. find_vertex(g, target_id(g, uv)) - Default (lowest priority)
             *    - Returns vertex_iterator, dereferenced to get vertex_descriptor
             * 
             * Custom implementations (member/ADL) can return:
             * - vertex_descriptor directly (vertex_t<G>) - used as-is
             * - vertex_iterator (iterator to vertices) - dereferenced to get descriptor
             * 
             * The default implementation:
             * - Uses target_id(g, uv) to get the target vertex ID
             * - Uses find_vertex(g, target_id) to get the vertex iterator
             * - Dereferences the iterator to get the vertex descriptor
             * 
             * This is a convenience function that combines target_id and find_vertex.
             * For random-access graphs, this is O(1).
             * For associative graphs, this is O(log n) or O(1) average.
             * 
             * @tparam G Graph type
             * @tparam E Edge descriptor type (constrained to be an edge_descriptor_type)
             * @param g Graph container
             * @param uv Edge descriptor (must be edge_t<G> - the edge descriptor type for the graph)
             * @return Vertex descriptor for the target vertex (vertex_t<G>)
             */
            template<typename G, edge_descriptor_type E>
            [[nodiscard]] constexpr auto operator()(G&& g, const E& uv) const
                noexcept(_Choice<std::remove_cvref_t<G>, std::remove_cvref_t<E>>._No_throw)
                -> decltype(auto)
                requires (_Choice<std::remove_cvref_t<G>, std::remove_cvref_t<E>>._Strategy != _St::_none)
            {
                using _G = std::remove_cvref_t<G>;
                using _E = std::remove_cvref_t<E>;
                
                if constexpr (_Choice<_G, _E>._Strategy == _St::_member) {
                    // Member function may return vertex_descriptor or iterator
                    return _to_vertex_descriptor(g, g.target(uv));
                } else if constexpr (_Choice<_G, _E>._Strategy == _St::_adl) {
                    // ADL may return vertex_descriptor or iterator
                    return _to_vertex_descriptor(g, target(g, uv));
                } else if constexpr (_Choice<_G, _E>._Strategy == _St::_default) {
                    // Default: find_vertex returns an iterator, dereference to get vertex_descriptor
                    return *find_vertex(std::forward<G>(g), target_id(g, uv));
                }
            }
        };
    } // namespace _target

} // namespace _cpo_impls

// =============================================================================
// target(g, uv) - Public CPO instance
// =============================================================================

inline namespace _cpo_instances {
    /**
     * @brief CPO for getting target vertex descriptor from an edge
     * 
     * Usage: auto target_vertex = graph::target(my_graph, edge_descriptor);
     * 
     * Returns: Vertex descriptor (vertex_iterator_t<G>) for the target vertex
     */
    inline constexpr _cpo_impls::_target::_fn target{};
} // namespace _cpo_instances

namespace _cpo_impls {

    // =========================================================================
    // num_vertices(g) CPO
    // =========================================================================
    
    namespace _num_vertices {
        enum class _St { _none, _member, _adl, _ranges };
        
        // Check for g.num_vertices() member function
        template<typename G>
        concept _has_member = 
            requires(const G& g) {
                { g.num_vertices() } -> std::integral;
            };
        
        // Check for ADL num_vertices(g)
        template<typename G>
        concept _has_adl = 
            requires(const G& g) {
                { num_vertices(g) } -> std::integral;
            };
        
        // Check if graph is a sized_range (default)
        template<typename G>
        concept _has_ranges = std::ranges::sized_range<G>;
        
        template<typename G>
        [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
            if constexpr (_has_member<G>) {
                return {_St::_member, 
                        noexcept(std::declval<const G&>().num_vertices())};
            } else if constexpr (_has_adl<G>) {
                return {_St::_adl, 
                        noexcept(num_vertices(std::declval<const G&>()))};
            } else if constexpr (_has_ranges<G>) {
                return {_St::_ranges, 
                        noexcept(std::ranges::size(std::declval<const G&>()))};
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
             * @brief Get the number of vertices in the graph
             * 
             * Resolution order (three-tier approach):
             * 1. g.num_vertices() - Member function (highest priority)
             * 2. num_vertices(g) - ADL (medium priority)
             * 3. std::ranges::size(g) - Ranges default (lowest priority)
             * 
             * The default implementation works for any sized_range (vector, deque, map, etc.)
             * Custom graph types can override by providing a member function or ADL function.
             * 
             * @tparam G Graph type
             * @param g Graph container
             * @return Number of vertices in the graph
             */
            template<typename G>
            [[nodiscard]] constexpr auto operator()(const G& g) const
                noexcept(_Choice<std::remove_cvref_t<G>>._No_throw)
                requires (_Choice<std::remove_cvref_t<G>>._Strategy != _St::_none)
            {
                using _G = std::remove_cvref_t<G>;
                
                if constexpr (_Choice<_G>._Strategy == _St::_member) {
                    return g.num_vertices();
                } else if constexpr (_Choice<_G>._Strategy == _St::_adl) {
                    return num_vertices(g);
                } else if constexpr (_Choice<_G>._Strategy == _St::_ranges) {
                    return std::ranges::size(g);
                }
            }
        };
    } // namespace _num_vertices

} // namespace _cpo_impls

// =============================================================================
// num_vertices(g) - Public CPO instance
// =============================================================================

inline namespace _cpo_instances {
    /**
     * @brief CPO for getting the number of vertices in the graph
     * 
     * Usage: auto count = graph::num_vertices(my_graph);
     * 
     * Returns: Number of vertices (size_t or similar integral type)
     */
    inline constexpr _cpo_impls::_num_vertices::_fn num_vertices{};
} // namespace _cpo_instances

namespace _cpo_impls {

    // =========================================================================
    // num_edges(g) CPO
    // =========================================================================
    
    namespace _num_edges {
        enum class _St { _none, _member, _adl, _default };
        
        // Check for g.num_edges() member function
        template<typename G>
        concept _has_member = 
            requires(const G& g) {
                { g.num_edges() } -> std::integral;
            };
        
        // Check for ADL num_edges(g)
        template<typename G>
        concept _has_adl = 
            requires(const G& g) {
                { num_edges(g) } -> std::integral;
            };
        
        // Check if we can iterate vertices and their edges
        template<typename G>
        concept _has_default = requires(G& g) {
            { vertices(g) };
            requires std::ranges::forward_range<decltype(vertices(g))>;
        };
        
        template<typename G>
        [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
            if constexpr (_has_member<G>) {
                return {_St::_member, 
                        noexcept(std::declval<const G&>().num_edges())};
            } else if constexpr (_has_adl<G>) {
                return {_St::_adl, 
                        noexcept(num_edges(std::declval<const G&>()))};
            } else if constexpr (_has_default<G>) {
                // Default implementation is not noexcept as it may allocate
                return {_St::_default, false};
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
             * @brief Get the total number of edges in the graph
             * 
             * Resolution order:
             * 1. g.num_edges() - Member function (highest priority)
             * 2. num_edges(g) - ADL (medium priority)
             * 3. Default implementation (lowest priority) - Iterates through all vertices
             *    and sums the number of edges for each vertex
             * 
             * The default implementation:
             * - Iterates through all vertices using vertices(g)
             * - For each vertex u, gets edges(g, u)
             * - If the edges range is sized, uses std::ranges::size()
             * - Otherwise, uses std::ranges::distance() on the underlying range
             * 
             * For directed graphs, this counts each edge once.
             * For undirected graphs, this counts each edge twice (once per endpoint).
             * 
             * @tparam G Graph type
             * @param g Graph container
             * @return Total number of edges in the graph
             */
            template<typename G>
            [[nodiscard]] constexpr auto operator()(G&& g) const
                noexcept(_Choice<std::remove_cvref_t<G>>._No_throw)
                requires (_Choice<std::remove_cvref_t<G>>._Strategy != _St::_none)
            {
                using _G = std::remove_cvref_t<G>;
                
                if constexpr (_Choice<_G>._Strategy == _St::_member) {
                    return g.num_edges();
                } else if constexpr (_Choice<_G>._Strategy == _St::_adl) {
                    return num_edges(g);
                } else if constexpr (_Choice<_G>._Strategy == _St::_default) {
                    // Default implementation: iterate through vertices and sum edge counts
                    std::size_t count = 0;
                    
                    for (auto u : vertices(g)) {
                        auto edge_range = edges(g, u);
                        
                        // Try to use sized_range if available, otherwise use distance
                        if constexpr (std::ranges::sized_range<decltype(edge_range)>) {
                            count += std::ranges::size(edge_range);
                        } else {
                            // Use the underlying range for distance calculation
                            // edge_descriptor_view exposes the underlying container via begin()/end()
                            count += std::ranges::distance(edge_range.begin(), edge_range.end());
                        }
                    }
                    
                    return count;
                }
            }
        };
    } // namespace _num_edges

    // =========================================================================
    // degree(g, u) and degree(g, uid) CPO
    // =========================================================================
    
    namespace _degree {
        using graph::find_vertex;
        using graph::edges;
        
        // Strategy enum for degree(g, u) - vertex descriptor version
        enum class _St_u { _none, _member, _adl, _default };
        
        // Check for g.degree(u) member function - vertex descriptor
        template<typename G, typename U>
        concept _has_member_u = requires(G& g, const U& u) {
            { g.degree(u) } -> std::integral;
        };
        
        // Check for ADL degree(g, u) - vertex descriptor
        template<typename G, typename U>
        concept _has_adl_u = requires(G& g, const U& u) {
            { degree(g, u) } -> std::integral;
        };
        
        // Check if we can use default: std::ranges::size(edges(g, u))
        template<typename G, typename U>
        concept _has_default_u = requires(G& g, const U& u) {
            { edges(g, u) } -> std::ranges::sized_range;
        };
        
        template<typename G, typename U>
        [[nodiscard]] consteval _Choice_t<_St_u> _Choose_u() noexcept {
            if constexpr (_has_member_u<G, U>) {
                return {_St_u::_member, noexcept(std::declval<G&>().degree(std::declval<const U&>()))};
            } else if constexpr (_has_adl_u<G, U>) {
                return {_St_u::_adl, noexcept(degree(std::declval<G&>(), std::declval<const U&>()))};
            } else if constexpr (_has_default_u<G, U>) {
                return {_St_u::_default, noexcept(std::ranges::size(edges(std::declval<G&>(), std::declval<const U&>())))};
            } else {
                return {_St_u::_none, false};
            }
        }
        
        // Strategy enum for degree(g, uid) - vertex ID version
        enum class _St_uid { _none, _member, _adl, _default };
        
        // Check for g.degree(uid) member function - vertex ID
        template<typename G, typename VId>
        concept _has_member_uid = requires(G& g, const VId& uid) {
            { g.degree(uid) } -> std::integral;
        };
        
        // Check for ADL degree(g, uid) - vertex ID
        template<typename G, typename VId>
        concept _has_adl_uid = requires(G& g, const VId& uid) {
            { degree(g, uid) } -> std::integral;
        };
        
        // Check if we can use default implementation: degree(g, *find_vertex(g, uid))
        template<typename G, typename VId>
        concept _has_default_uid = requires(G& g, const VId& uid) {
            { find_vertex(g, uid) } -> std::input_iterator;
            requires vertex_descriptor_type<decltype(*find_vertex(g, uid))>;
            requires _has_default_u<G, decltype(*find_vertex(g, uid))>;
        };
        
        template<typename G, typename VId>
        [[nodiscard]] consteval _Choice_t<_St_uid> _Choose_uid() noexcept {
            if constexpr (_has_member_uid<G, VId>) {
                return {_St_uid::_member, noexcept(std::declval<G&>().degree(std::declval<const VId&>()))};
            } else if constexpr (_has_adl_uid<G, VId>) {
                return {_St_uid::_adl, noexcept(degree(std::declval<G&>(), std::declval<const VId&>()))};
            } else if constexpr (_has_default_uid<G, VId>) {
                return {_St_uid::_default, false};
            } else {
                return {_St_uid::_none, false};
            }
        }
        
        class _fn {
        private:
            template<typename G, typename U>
            static constexpr _Choice_t<_St_u> _Choice_u = _Choose_u<std::remove_cvref_t<G>, std::remove_cvref_t<U>>();
            
            template<typename G, typename VId>
            static constexpr _Choice_t<_St_uid> _Choice_uid = _Choose_uid<std::remove_cvref_t<G>, std::remove_cvref_t<VId>>();
            
        public:
            // degree(g, u) - vertex descriptor version
            template<typename G, vertex_descriptor_type U>
            [[nodiscard]] constexpr auto operator()(G&& g, const U& u) const
                noexcept(_Choice_u<G, U>._No_throw)
                requires (_Choice_u<std::remove_cvref_t<G>, std::remove_cvref_t<U>>._Strategy != _St_u::_none)
            {
                using _G = std::remove_cvref_t<G>;
                using _U = std::remove_cvref_t<U>;
                
                if constexpr (_Choice_u<_G, _U>._Strategy == _St_u::_member) {
                    return g.degree(u);
                } else if constexpr (_Choice_u<_G, _U>._Strategy == _St_u::_adl) {
                    return degree(g, u);
                } else if constexpr (_Choice_u<_G, _U>._Strategy == _St_u::_default) {
                    return std::ranges::size(edges(std::forward<G>(g), u));
                }
            }
            
            // degree(g, uid) - vertex ID version
            template<typename G, typename VId>
                requires (!vertex_descriptor_type<VId>)
            [[nodiscard]] constexpr auto operator()(G&& g, const VId& uid) const
                noexcept(_Choice_uid<G, VId>._No_throw)
                requires (_Choice_uid<std::remove_cvref_t<G>, std::remove_cvref_t<VId>>._Strategy != _St_uid::_none)
            {
                using _G = std::remove_cvref_t<G>;
                using _VId = std::remove_cvref_t<VId>;
                
                if constexpr (_Choice_uid<_G, _VId>._Strategy == _St_uid::_member) {
                    return g.degree(uid);
                } else if constexpr (_Choice_uid<_G, _VId>._Strategy == _St_uid::_adl) {
                    return degree(g, uid);
                } else if constexpr (_Choice_uid<_G, _VId>._Strategy == _St_uid::_default) {
                    // Default: find vertex then call degree(g, u)
                    auto v = *find_vertex(std::forward<G>(g), uid);
                    return (*this)(std::forward<G>(g), v);
                }
            }
        };
    } // namespace _degree

} // namespace _cpo_impls

// =============================================================================
// num_edges(g) - Public CPO instance
// =============================================================================

inline namespace _cpo_instances {
    /**
     * @brief CPO for getting the total number of edges in the graph
     * 
     * Usage: auto count = graph::num_edges(my_graph);
     * 
     * Returns: Total number of edges (size_t)
     */
    inline constexpr _cpo_impls::_num_edges::_fn num_edges{};
} // namespace _cpo_instances

// =============================================================================
// degree(g, u) and degree(g, uid) - Public CPO instances
// =============================================================================

inline namespace _cpo_instances {
    /**
     * @brief CPO for getting the degree (number of outgoing edges) of a vertex
     * 
     * Usage: 
     *   auto deg = graph::degree(my_graph, vertex_descriptor);
     *   auto deg = graph::degree(my_graph, vertex_id);
     * 
     * Returns: Number of outgoing edges from the vertex (integral type)
     */
    inline constexpr _cpo_impls::_degree::_fn degree{};
} // namespace _cpo_instances

} // namespace graph
