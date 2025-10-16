/**
 * @file descriptor.hpp
 * @brief Core descriptor concepts and type traits for graph descriptors
 */

#pragma once

#include <concepts>
#include <iterator>
#include <tuple>
#include <type_traits>

namespace desc {

/**
 * @brief Concept to check if a type is pair-like (has at least 2 members accessible via tuple protocol)
 * 
 * This is used to constrain bidirectional iterator value_types for vertex storage.
 */
template<typename T>
concept pair_like = requires {
    typename std::tuple_size<T>::type;
    requires std::tuple_size<T>::value >= 2;
    { std::get<0>(std::declval<T>()) };
    { std::get<1>(std::declval<T>()) };
};

/**
 * @brief Alternative pair-like concept checking for .first and .second members
 */
template<typename T>
concept has_first_second = requires(T t) {
    { t.first };
    { t.second };
};

/**
 * @brief Combined pair-like concept accepting either tuple protocol or pair members
 */
template<typename T>
concept pair_like_value = pair_like<T> || has_first_second<T>;

/**
 * @brief Concept for valid vertex container iterators
 * 
 * Vertex iterators must be either:
 * - Random access (for index-based storage like vector)
 * - Bidirectional with pair-like value_type (for map-based storage)
 */
template<typename Iter>
concept vertex_iterator = 
    std::random_access_iterator<Iter> ||
    (std::bidirectional_iterator<Iter> && 
     pair_like_value<typename std::iterator_traits<Iter>::value_type>);

/**
 * @brief Concept for valid edge container iterators
 * 
 * Edge iterators must be at least forward iterators
 */
template<typename Iter>
concept edge_iterator = std::forward_iterator<Iter>;

} // namespace desc
