# C++20 Customization Point Objects (CPO) Guide

## Overview

A Customization Point Object (CPO) is a function object that provides a uniform interface for customizable operations. CPOs enable library authors to define extensible APIs that users can customize through ADL (Argument-Dependent Lookup) or explicit specialization, while maintaining consistent behavior and optimizations.

## Key Characteristics of CPOs

1. **Niebloid**: A function object that inhibits ADL when called
2. **Consistent interface**: Provides a single call point regardless of customization
3. **Hierarchical customization**: Checks for customizations in a specific order
4. **Constexpr-friendly**: Supports compile-time evaluation
5. **SFINAE-friendly**: Properly constrains overload resolution

## MSVC-Style CPO Structure

The MSVC Standard Library uses a specific pattern for CPOs that balances clarity, maintainability, and optimization. This style is recommended for consistency with the standard library.

### Basic Structure

```cpp
namespace graph {
namespace _cpo {  // Internal namespace for implementation details

// Implementation details namespace
namespace _my_operation {
    // 1. Concept to check if type has member function
    template<typename T>
    concept _has_member = requires(T&& t) {
        { std::forward<T>(t).my_operation() } -> /* constraint */;
    };

    // 2. Concept to check if ADL customization exists
    template<typename T>
    concept _has_adl = requires(T&& t) {
        { my_operation(std::forward<T>(t)) } -> /* constraint */;
    };

    // 3. CPO class implementation
    class _fn {
    public:
        // Member function customization (highest priority)
        template<typename T>
            requires _has_member<T>
        constexpr decltype(auto) operator()(T&& t) const
            noexcept(noexcept(std::forward<T>(t).my_operation()))
        {
            return std::forward<T>(t).my_operation();
        }

        // ADL customization (medium priority)
        template<typename T>
            requires (!_has_member<T> && _has_adl<T>)
        constexpr decltype(auto) operator()(T&& t) const
            noexcept(noexcept(my_operation(std::forward<T>(t))))
        {
            return my_operation(std::forward<T>(t));
        }

        // Default implementation (lowest priority)
        template<typename T>
            requires (!_has_member<T> && !_has_adl<T> && /* additional constraints */)
        constexpr decltype(auto) operator()(T&& t) const
            noexcept(/* noexcept specification */)
        {
            // Default implementation
        }
    };
} // namespace _my_operation

} // namespace _cpo

// The actual CPO - inline variable with internal linkage
inline namespace _cpos {
    inline constexpr _cpo::_my_operation::_fn my_operation{};
} // namespace _cpos

} // namespace graph
```

## Customization Priority Order

CPOs should check for customizations in this order:

1. **Member function**: `t.operation()`
2. **ADL free function**: `operation(t)` found via ADL
3. **Default implementation**: Built-in behavior

## Complete Example: `vertex_id` CPO

This example demonstrates a CPO for extracting a vertex ID from a vertex descriptor.

```cpp
namespace graph {
namespace _cpo {

namespace _vertex_id {
    // Check if type has .vertex_id() member function
    template<typename VD>
    concept _has_member = requires(const VD& vd) {
        { vd.vertex_id() } -> std::convertible_to<std::size_t>;
    };

    // Check if ADL vertex_id function exists
    template<typename VD>
    concept _has_adl = requires(const VD& vd) {
        { vertex_id(vd) } -> std::convertible_to<std::size_t>;
    };

    // Check if type is a raw integral type (for backwards compatibility)
    template<typename VD>
    concept _is_integral = std::integral<std::remove_cvref_t<VD>>;

    // CPO implementation
    class _fn {
    public:
        // Member function customization (highest priority)
        template<typename VD>
            requires _has_member<VD>
        [[nodiscard]] constexpr auto operator()(const VD& vd) const
            noexcept(noexcept(vd.vertex_id()))
            -> decltype(vd.vertex_id())
        {
            return vd.vertex_id();
        }

        // ADL customization (medium priority)
        template<typename VD>
            requires (!_has_member<VD> && _has_adl<VD>)
        [[nodiscard]] constexpr auto operator()(const VD& vd) const
            noexcept(noexcept(vertex_id(vd)))
            -> decltype(vertex_id(vd))
        {
            return vertex_id(vd);
        }

        // Integral pass-through (lowest priority)
        template<typename VD>
            requires (!_has_member<VD> && !_has_adl<VD> && _is_integral<VD>)
        [[nodiscard]] constexpr VD operator()(VD vd) const noexcept
        {
            return vd;
        }
    };
} // namespace _vertex_id

} // namespace _cpo

// Public CPO interface
inline namespace _cpos {
    inline constexpr _cpo::_vertex_id::_fn vertex_id{};
} // namespace _cpos

} // namespace graph

// Usage examples:

// 1. User defines a type with member function
struct MyVertexDescriptor {
    std::size_t vertex_id() const { return id_; }
private:
    std::size_t id_;
};

// 2. User customizes via ADL
namespace user {
    struct CustomDescriptor {
        int key;
    };

    std::size_t vertex_id(const CustomDescriptor& cd) {
        return static_cast<std::size_t>(cd.key);
    }
}

// 3. Raw integral types work automatically
void example() {
    MyVertexDescriptor vd1{};
    user::CustomDescriptor vd2{42};
    std::size_t vd3 = 10;

    // All use the same CPO interface
    auto id1 = graph::vertex_id(vd1);  // Calls vd1.vertex_id()
    auto id2 = graph::vertex_id(vd2);  // Calls user::vertex_id(vd2)
    auto id3 = graph::vertex_id(vd3);  // Returns vd3 directly
}
```

## Best Practices

### 1. Use Internal Namespaces

```cpp
namespace graph {
namespace _cpo {          // Implementation details
namespace _operation {    // Specific CPO implementation
    // ... implementation ...
} // namespace _operation
} // namespace _cpo

inline namespace _cpos {  // Public interface
    inline constexpr _cpo::_operation::_fn operation{};
} // namespace _cpos
} // namespace graph
```

**Why**: Keeps implementation details hidden while providing a clean public interface.

### 2. Use Concepts for Overload Resolution

```cpp
template<typename T>
concept _has_member = requires(T&& t) {
    { std::forward<T>(t).operation() } -> std::convertible_to<ReturnType>;
};
```

**Why**: Provides clear, maintainable constraints that participate correctly in overload resolution.

### 3. Preserve Noexcept Specifications

```cpp
template<typename T>
constexpr auto operator()(T&& t) const
    noexcept(noexcept(std::forward<T>(t).operation()))
{
    return std::forward<T>(t).operation();
}
```

**Why**: Allows the CPO to maintain exception safety guarantees of the underlying operation.

### 4. Use Perfect Forwarding

```cpp
template<typename T>
constexpr decltype(auto) operator()(T&& t) const {
    return std::forward<T>(t).operation();
}
```

**Why**: Preserves value categories and enables move semantics where appropriate.

### 5. Mark CPO as `[[nodiscard]]` When Appropriate

```cpp
[[nodiscard]] constexpr auto operator()(const T& t) const {
    return t.value();
}
```

**Why**: Prevents accidental discarding of return values.

### 6. Make CPO Callable constexpr

```cpp
class _fn {
public:
    template<typename T>
    constexpr auto operator()(T&& t) const { /* ... */ }
};

inline constexpr _fn operation{};
```

**Why**: Enables compile-time evaluation when possible.

## Testing CPOs

### Test All Customization Paths

```cpp
TEST_CASE("CPO uses member function") {
    struct WithMember {
        int vertex_id() const { return 42; }
    };
    WithMember v;
    REQUIRE(graph::vertex_id(v) == 42);
}

TEST_CASE("CPO uses ADL function") {
    namespace user {
        struct WithADL { int key; };
        int vertex_id(const WithADL& v) { return v.key; }
    }
    user::WithADL v{99};
    REQUIRE(graph::vertex_id(v) == 99);
}

TEST_CASE("CPO uses default implementation") {
    std::size_t id = 7;
    REQUIRE(graph::vertex_id(id) == 7);
}
```

### Test Noexcept Propagation

```cpp
TEST_CASE("CPO propagates noexcept") {
    struct Throwing {
        int vertex_id() const { return 1; }  // not noexcept
    };
    struct NonThrowing {
        int vertex_id() const noexcept { return 2; }
    };

    STATIC_REQUIRE(!noexcept(graph::vertex_id(Throwing{})));
    STATIC_REQUIRE(noexcept(graph::vertex_id(NonThrowing{})));
}
```

### Test Concept Constraints

```cpp
TEST_CASE("CPO properly constrains") {
    struct Invalid {};  // No vertex_id customization
    
    // Should not compile:
    // auto x = graph::vertex_id(Invalid{});
    
    STATIC_REQUIRE(!requires(Invalid i) {
        graph::vertex_id(i);
    });
}
```

## Common Pitfalls to Avoid

### 1. Don't Use ADL for CPO Call

```cpp
// WRONG - this enables ADL
using namespace graph;
auto id = vertex_id(vd);

// CORRECT - CPO inhibits ADL
auto id = graph::vertex_id(vd);
```

### 2. Don't Forget Const Correctness

```cpp
// WRONG - non-const operator
auto operator()(T& t) const { return t.value(); }

// CORRECT - accept const when appropriate
auto operator()(const T& t) const { return t.value(); }
```

### 3. Don't Ignore Return Type Constraints

```cpp
// WRONG - no constraint on return type
template<typename T>
concept _has_member = requires(T t) {
    t.operation();  // Could return anything
};

// CORRECT - constrain return type
template<typename T>
concept _has_member = requires(T t) {
    { t.operation() } -> std::convertible_to<ExpectedType>;
};
```

## Integration with Existing Code

CPOs work seamlessly with:

- **Ranges**: CPOs can be used as projections and transformations
- **Algorithms**: Pass CPOs to standard algorithms
- **Concepts**: Use CPOs in concept definitions
- **SFINAE**: CPOs participate in template overload resolution

Example:
```cpp
std::vector<MyDescriptor> vertices;

// Use CPO as projection in ranges
auto ids = vertices 
    | std::views::transform(graph::vertex_id)
    | std::ranges::to<std::vector>();
```

## Summary

When implementing a CPO in MSVC style:

1. ✅ Create internal namespace for implementation (`_cpo::_operation`)
2. ✅ Define concepts for each customization point
3. ✅ Implement `_fn` class with constrained `operator()`
4. ✅ Check customizations in priority order (member → ADL → default)
5. ✅ Preserve noexcept and constexpr
6. ✅ Use perfect forwarding where appropriate
7. ✅ Create public inline constexpr instance in inline namespace
8. ✅ Test all customization paths thoroughly

This pattern provides maximum flexibility for users while maintaining a consistent, efficient interface.
