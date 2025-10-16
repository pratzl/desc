/**
 * @file edge_descriptor.hpp
 * @brief Edge descriptor template for graph edges
 */

#pragma once

#include "vertex_descriptor.hpp"
#include <compare>
#include <functional>

namespace desc {

/**
 * @brief Descriptor for edges in a graph
 * 
 * Provides a lightweight, type-safe handle to edges stored in various container types.
 * Maintains both the edge location and the source vertex descriptor.
 * 
 * @tparam EdgeIter Iterator type of the underlying edge container
 * @tparam VertexIter Iterator type of the vertex container
 */
template<edge_iterator EdgeIter, vertex_iterator VertexIter>
class edge_descriptor {
public:
    using edge_iterator_type = EdgeIter;
    using vertex_iterator_type = VertexIter;
    using vertex_desc = vertex_descriptor<VertexIter>;
    
    // Conditional storage type for edge: size_t for random access, iterator for forward
    using edge_storage_type = std::conditional_t<
        std::random_access_iterator<EdgeIter>,
        std::size_t,
        EdgeIter
    >;
    
    // Default constructor
    constexpr edge_descriptor() noexcept 
        requires std::default_initializable<edge_storage_type> && 
                 std::default_initializable<vertex_desc>
        : edge_storage_{}, source_{} {}
    
    // Constructor from edge storage and source vertex descriptor
    constexpr edge_descriptor(edge_storage_type edge_val, vertex_desc source) noexcept
        : edge_storage_(edge_val), source_(source) {}
    
    /**
     * @brief Get the underlying edge storage value (index or iterator)
     * @return The stored edge index (for random access) or iterator (for forward)
     */
    [[nodiscard]] constexpr edge_storage_type value() const noexcept {
        return edge_storage_;
    }
    
    /**
     * @brief Get the source vertex descriptor
     * @return The vertex descriptor representing the source of this edge
     */
    [[nodiscard]] constexpr vertex_desc source() const noexcept {
        return source_;
    }
    
    /**
     * @brief Get the target vertex ID from the edge data
     * @param edges The edge container to access edge data
     * @return The target vertex identifier extracted from the edge
     * 
     * For random access iterators, uses the stored index to access the container.
     * For forward/bidirectional iterators, dereferences the stored iterator.
     * 
     * Edge data extraction:
     * - Simple integral types: returns the value directly as target ID
     * - Pair-like types: returns .first as target ID
     * - Tuple-like types: returns std::get<0> as target ID
     */
    template<typename EdgeContainer>
    [[nodiscard]] constexpr auto target_id(const EdgeContainer& edges) const noexcept {
        using edge_value_type = typename std::iterator_traits<EdgeIter>::value_type;
        
        // Get the edge value from container (for random access) or iterator (for forward)
        const auto& edge_value = [&]() -> decltype(auto) {
            if constexpr (std::random_access_iterator<EdgeIter>) {
                return edges[edge_storage_];
            } else {
                return *edge_storage_;
            }
        }();
        
        // Extract target ID from edge value based on its type
        if constexpr (std::integral<edge_value_type>) {
            // Simple type: the value itself is the target ID
            return edge_value;
        }
        else if constexpr (requires { edge_value.first; }) {
            // Pair-like: .first is the target ID
            return edge_value.first;
        }
        else if constexpr (requires { std::get<0>(edge_value); }) {
            // Tuple-like: first element is the target ID
            return std::get<0>(edge_value);
        }
        else {
            // Fallback: assume the value itself is the target
            return edge_value;
        }
    }
    
    /**
     * @brief Get the underlying container value (the actual edge data)
     * @param edges The edge container to access edge data
     * @return Reference to the edge data from the container
     * 
     * For random access iterators, accesses edges[index].
     * For forward/bidirectional iterators, dereferences the stored iterator.
     */
    template<typename EdgeContainer>
    [[nodiscard]] constexpr decltype(auto) underlying_value(EdgeContainer& edges) const noexcept {
        if constexpr (std::random_access_iterator<EdgeIter>) {
            return edges[edge_storage_];
        } else {
            return *edge_storage_;
        }
    }
    
    /**
     * @brief Get the underlying container value (const version)
     * @param edges The edge container to access edge data
     * @return Const reference to the edge data from the container
     */
    template<typename EdgeContainer>
    [[nodiscard]] constexpr decltype(auto) underlying_value(const EdgeContainer& edges) const noexcept {
        if constexpr (std::random_access_iterator<EdgeIter>) {
            return edges[edge_storage_];
        } else {
            return *edge_storage_;
        }
    }
    
    // Pre-increment: advances edge position, keeps source unchanged
    constexpr edge_descriptor& operator++() noexcept {
        ++edge_storage_;
        return *this;
    }
    
    // Post-increment
    constexpr edge_descriptor operator++(int) noexcept {
        edge_descriptor tmp = *this;
        ++edge_storage_;
        return tmp;
    }
    
    // Comparison operators
    [[nodiscard]] constexpr auto operator<=>(const edge_descriptor&) const noexcept = default;
    [[nodiscard]] constexpr bool operator==(const edge_descriptor&) const noexcept = default;
    
private:
    edge_storage_type edge_storage_;
    vertex_desc source_;
};

} // namespace desc

// Hash specialization for std::unordered containers
namespace std {
    template<desc::edge_iterator EdgeIter, desc::vertex_iterator VertexIter>
    struct hash<desc::edge_descriptor<EdgeIter, VertexIter>> {
        [[nodiscard]] size_t operator()(const desc::edge_descriptor<EdgeIter, VertexIter>& ed) const noexcept {
            // Combine hash of edge storage and source vertex
            size_t h1 = [&ed]() {
                if constexpr (std::random_access_iterator<EdgeIter>) {
                    return std::hash<std::size_t>{}(ed.value());
                } else {
                    // For forward iterators, hash the iterator somehow
                    // Note: This is a simplification - in practice you might need custom hashing
                    return std::hash<std::size_t>{}(
                        reinterpret_cast<std::size_t>(&(*ed.value()))
                    );
                }
            }();
            
            size_t h2 = std::hash<desc::vertex_descriptor<VertexIter>>{}(ed.source());
            
            // Combine hashes using a simple mixing function
            return h1 ^ (h2 << 1);
        }
    };
} // namespace std
