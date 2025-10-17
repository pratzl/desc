/**
 * @file vertex_descriptor.hpp
 * @brief Vertex descriptor template for graph vertices
 */

#pragma once

#include "descriptor.hpp"
#include <compare>
#include <functional>
#include <ranges>

namespace desc {

/**
 * @brief Descriptor for vertices in a graph
 * 
 * Provides a lightweight, type-safe handle to vertices stored in various container types.
 * Supports both index-based (vector) and key-value based (map) storage strategies.
 * 
 * @tparam VertexIter Iterator type of the underlying vertex container
 */
template<vertex_iterator VertexIter>
class vertex_descriptor {
public:
    using iterator_type = VertexIter;
    using value_type = typename std::iterator_traits<VertexIter>::value_type;
    
    // Conditional storage type: size_t for random access, iterator for bidirectional
    using storage_type = std::conditional_t<
        std::random_access_iterator<VertexIter>,
        std::size_t,
        VertexIter
    >;
    
    // Default constructor
    constexpr vertex_descriptor() noexcept 
        requires std::default_initializable<storage_type>
        : storage_{} {}
    
    // Constructor from storage value
    constexpr explicit vertex_descriptor(storage_type val) noexcept
        : storage_(val) {}
    
    /**
     * @brief Get the underlying storage value (index or iterator)
     * @return The stored index (for random access) or iterator (for bidirectional)
     */
    [[nodiscard]] constexpr storage_type value() const noexcept {
        return storage_;
    }
    
    /**
     * @brief Get the vertex ID
     * @return For random access: the index. For bidirectional: the key from the pair
     */
    [[nodiscard]] constexpr auto vertex_id() const noexcept {
        if constexpr (std::random_access_iterator<VertexIter>) {
            return storage_;
        } else {
            // Bidirectional iterator - extract key from pair-like value_type
            return std::get<0>(*storage_);
        }
    }
    
    /**
     * @brief Get the underlying container value (the actual vertex data)
     * @param container The underlying vertex container
     * @return Reference to the vertex data from the container
     * 
     * For random access iterators, accesses container[index].
     * For bidirectional iterators, dereferences the stored iterator.
     */
    template<typename Container>
    [[nodiscard]] constexpr decltype(auto) underlying_value(Container& container) const noexcept {
        if constexpr (std::random_access_iterator<VertexIter>) {
            return (container[storage_]);
        } else {
            return (*storage_);
        }
    }
    
    /**
     * @brief Get the underlying container value (const version)
     * @param container The underlying vertex container
     * @return Const reference to the vertex data from the container
     */
    template<typename Container>
    [[nodiscard]] constexpr decltype(auto) underlying_value(const Container& container) const noexcept {
        if constexpr (std::random_access_iterator<VertexIter>) {
            return (container[storage_]);
        } else {
            return (*storage_);
        }
    }
    
    /**
     * @brief Get the inner/data value (excluding the vertex ID/key)
     * @param container The underlying vertex container
     * @return Reference to the vertex data (for maps: the .second value; for vectors: the whole value)
     * 
     * For random access containers (vector), returns the entire value.
     * For bidirectional containers (map), returns the .second part (the actual data, not the key).
     */
    template<typename Container>
    [[nodiscard]] constexpr decltype(auto) inner_value(Container& container) const noexcept {
        using vt = typename std::iterator_traits<VertexIter>::value_type;
        
        if constexpr (std::random_access_iterator<VertexIter>) {
            // For random access (vector), the value is the data itself
            return (container[storage_]);
        } else {
            // For bidirectional, check if it's a pair-like type
            if constexpr (pair_like_value<vt>) {
                // For map, bind the pair first, then return .second
                auto& element = *storage_;
                return (element.second);
            } else {
                // For non-pair-like, return the whole value
                return (*storage_);
            }
        }
    }
    
    /**
     * @brief Get the inner/data value (const version)
     * @param container The underlying vertex container
     * @return Const reference to the vertex data
     */
    template<typename Container>
    [[nodiscard]] constexpr decltype(auto) inner_value(const Container& container) const noexcept {
        using vt = typename std::iterator_traits<VertexIter>::value_type;
        
        if constexpr (std::random_access_iterator<VertexIter>) {
            return (container[storage_]);
        } else {
            if constexpr (pair_like_value<vt>) {
                const auto& element = *storage_;
                return (element.second);
            } else {
                return (*storage_);
            }
        }
    }
    
    // Pre-increment
    constexpr vertex_descriptor& operator++() noexcept {
        ++storage_;
        return *this;
    }
    
    // Post-increment
    constexpr vertex_descriptor operator++(int) noexcept {
        vertex_descriptor tmp = *this;
        ++storage_;
        return tmp;
    }
    
    // Comparison operators (C++20 spaceship operator)
    [[nodiscard]] auto operator<=>(const vertex_descriptor&) const noexcept = default;
    [[nodiscard]] bool operator==(const vertex_descriptor&) const noexcept = default;
    
private:
    storage_type storage_;
};

} // namespace desc

// Hash specialization for std::unordered containers
namespace std {
    template<desc::vertex_iterator VertexIter>
    struct hash<desc::vertex_descriptor<VertexIter>> {
        [[nodiscard]] size_t operator()(const desc::vertex_descriptor<VertexIter>& vd) const noexcept {
            if constexpr (std::random_access_iterator<VertexIter>) {
                // Hash the size_t index
                return std::hash<std::size_t>{}(vd.value());
            } else {
                // Hash the vertex_id from the iterator's value
                auto id = vd.vertex_id();
                using id_type = decltype(id);
                return std::hash<std::remove_cvref_t<id_type>>{}(id);
            }
        }
    };
} // namespace std
