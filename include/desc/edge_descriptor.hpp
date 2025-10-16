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
