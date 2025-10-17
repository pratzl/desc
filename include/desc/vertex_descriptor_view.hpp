/**
 * @file vertex_descriptor_view.hpp
 * @brief View over vertex storage that yields vertex descriptors
 */

#pragma once

#include "vertex_descriptor.hpp"
#include <ranges>
#include <iterator>

namespace graph {

/**
 * @brief Forward-only view over vertex storage yielding vertex descriptors
 * 
 * This view wraps an underlying vertex container and provides forward iteration
 * that yields vertex_descriptor objects. Descriptors are synthesized on-the-fly,
 * making random access incompatible with the design.
 * 
 * @tparam VertexIter Iterator type of the underlying vertex container
 */
template<vertex_iterator VertexIter>
class vertex_descriptor_view : public std::ranges::view_interface<vertex_descriptor_view<VertexIter>> {
public:
    using vertex_desc = vertex_descriptor<VertexIter>;
    using storage_type = typename vertex_desc::storage_type;
    
    /**
     * @brief Forward iterator that yields vertex_descriptor values
     */
    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = vertex_desc;
        using difference_type = std::ptrdiff_t;
        using pointer = const vertex_desc*;
        using reference = vertex_desc;
        
        constexpr iterator() noexcept = default;
        
        constexpr explicit iterator(storage_type pos) noexcept
            : current_(pos) {}
        
        // Dereference returns descriptor by value (synthesized on-the-fly)
        [[nodiscard]] constexpr vertex_desc operator*() const noexcept {
            return vertex_desc{current_};
        }
        
        // Pre-increment
        constexpr iterator& operator++() noexcept {
            ++current_;
            return *this;
        }
        
        // Post-increment
        constexpr iterator operator++(int) noexcept {
            iterator tmp = *this;
            ++current_;
            return tmp;
        }
        
        // Comparison
        [[nodiscard]] constexpr bool operator==(const iterator& other) const noexcept {
            return current_ == other.current_;
        }
        
    private:
        storage_type current_{};
    };
    
    using const_iterator = iterator;
    
    // Default constructor
    constexpr vertex_descriptor_view() noexcept = default;
    
    /**
     * @brief Construct view from iterator range
     * @param begin_iter Starting iterator/index
     * @param end_iter Ending iterator/index
     */
    constexpr vertex_descriptor_view(storage_type begin_val, storage_type end_val) noexcept
        : begin_(begin_val), end_(end_val) {}
    
    /**
     * @brief Construct view from container with begin/end methods
     * @param container The underlying container
     */
    template<typename Container>
        requires requires(Container& c) {
            { c.begin() } -> std::convertible_to<VertexIter>;
            { c.end() } -> std::convertible_to<VertexIter>;
        }
    constexpr explicit vertex_descriptor_view(Container& container) noexcept {
        if constexpr (std::random_access_iterator<VertexIter>) {
            begin_ = 0;
            end_ = static_cast<storage_type>(container.size());
        } else {
            begin_ = container.begin();
            end_ = container.end();
        }
    }
    
    [[nodiscard]] constexpr iterator begin() const noexcept {
        return iterator{begin_};
    }
    
    [[nodiscard]] constexpr iterator end() const noexcept {
        return iterator{end_};
    }
    
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
        return begin();
    }
    
    [[nodiscard]] constexpr const_iterator cend() const noexcept {
        return end();
    }
    
    // Size if available (for random access)
    [[nodiscard]] constexpr auto size() const noexcept
        requires std::random_access_iterator<VertexIter>
    {
        return end_ - begin_;
    }
    
private:
    storage_type begin_{};
    storage_type end_{};
};

// Deduction guide
template<typename Container>
vertex_descriptor_view(Container&) -> vertex_descriptor_view<typename Container::iterator>;

} // namespace graph
