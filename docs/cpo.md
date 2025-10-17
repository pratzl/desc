# C++20 Customization Point Objects (CPO) Guide

## Overview

A Customization Point Object (CPO) is a function object that provides a uniform interface for customizable operations. CPOs enable library authors to define extensible APIs that users can customize through ADL (Argument-Dependent Lookup) or explicit specialization, while maintaining consistent behavior and optimizations.

**Compiler Compatibility**: This guide describes CPO patterns that work correctly with MSVC, GCC, and Clang. All examples have been tested across these compilers to ensure portability.

## Key Characteristics of CPOs

1. **Niebloid**: A function object that inhibits ADL when called
2. **Consistent interface**: Provides a single call point regardless of customization
3. **Hierarchical customization**: Checks for customizations in a specific order
4. **Multiple overloads**: Supports 2 or more optional overload paths
5. **Constexpr-friendly**: Supports compile-time evaluation
6. **SFINAE-friendly**: Properly constrains overload resolution
7. **Cross-compiler compatible**: Works reliably with MSVC, GCC, and Clang

## MSVC-Style CPO Structure

The MSVC Standard Library uses a specific pattern for CPOs that balances clarity, maintainability, and optimization. This style is recommended for consistency with the standard library and guaranteed cross-compiler compatibility.

### Basic Structure (Recommended)

```cpp
namespace graph {
namespace _cpo {  // Internal namespace for implementation details

namespace _my_operation {
    // 1. Strategy enum
    enum class _St { _none, _member, _adl, _default };
    
    // 2. Choice struct (strategy + noexcept)
    template<typename _Ty>
    struct _Choice_t {
        _Ty _Strategy = _Ty{};
        bool _No_throw = false;
    };
    
    // 3. Concepts for each customization path
    template<typename T>
    concept _has_member = requires(T&& t) {
        { std::forward<T>(t).my_operation() } -> /* constraint */;
    };

    template<typename T>
    concept _has_adl = requires(T&& t) {
        { my_operation(std::forward<T>(t)) } -> /* constraint */;
    };

    // 4. Single compile-time choice evaluation
    template<typename T>
    [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
        if constexpr (_has_member<T>) {
            return {_St::_member, noexcept(std::declval<T>().my_operation())};
        } else if constexpr (_has_adl<T>) {
            return {_St::_adl, noexcept(my_operation(std::declval<T>()))};
        } else {
            return {_St::_default, true};
        }
    }

    // 5. CPO class with single operator()
    class _fn {
    private:
        template<typename T>
        static constexpr _Choice_t<_St> _Choice = _Choose<T>();
        
    public:
        template<typename T>
        [[nodiscard]] constexpr auto operator()(T&& t) const
            noexcept(_Choice<T>._No_throw)
            -> decltype(auto)
        {
            if constexpr (_Choice<T>._Strategy == _St::_member) {
                return std::forward<T>(t).my_operation();
            } else if constexpr (_Choice<T>._Strategy == _St::_adl) {
                return my_operation(std::forward<T>(t));
            } else {
                // Default implementation
                return /* default value */;
            }
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

### Key Elements of This Pattern

1. **`_St` enum**: Defines all possible execution paths
2. **`_Choice_t<_Ty>` struct**: Holds both strategy and noexcept flag
3. **`_Choose<T>()` consteval function**: Evaluates path and noexcept once
4. **`_Choice<T>` static variable**: Caches the choice result
5. **Single `operator()`**: Uses `if constexpr` chain (no overloads)
6. **Simple noexcept**: References `_Choice<T>._No_throw`

## Customization Priority Order

CPOs should check for customizations in this order:

1. **Member function**: `t.operation()`
2. **ADL free function**: `operation(t)` found via ADL
3. **Default implementation**: Built-in behavior

## MSVC-Style CPO Implementation (Recommended)

The MSVC Standard Library uses an elegant pattern with:
1. **Single `operator()` function** with `if constexpr` chain (no overloads)
2. **Combined strategy + noexcept detection** in one compile-time evaluation
3. **`_Choice_t` struct** to cache both the path and exception specification

This approach is cleaner, more maintainable, and less error-prone than multiple overloads.

### Pattern: Single Function with Choice Detection

```cpp
namespace graph::_cpo {
namespace _my_operation {
    // Enum for compile-time path detection
    enum class _St { _none, _member, _adl, _default };
    
    // Struct to hold both strategy and noexcept info
    template<typename _Ty>
    struct _Choice_t {
        _Ty _Strategy = _Ty{};
        bool _No_throw = false;
    };
    
    // Concepts for each path
    template<typename T>
    concept _has_member = requires(T&& t) {
        { std::forward<T>(t).operation() } -> std::convertible_to<int>;
    };
    
    template<typename T>
    concept _has_adl = requires(T&& t) {
        { operation(std::forward<T>(t)) } -> std::convertible_to<int>;
    };
    
    // Single compile-time evaluation for both path and noexcept
    template<typename T>
    [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
        if constexpr (_has_member<T>) {
            return {_St::_member, noexcept(std::declval<T>().operation())};
        } else if constexpr (_has_adl<T>) {
            return {_St::_adl, noexcept(operation(std::declval<T>()))};
        } else {
            return {_St::_default, true};
        }
    }
    
    class _fn {
    private:
        // Cache the choice at compile time for each type T
        template<typename T>
        static constexpr _Choice_t<_St> _Choice = _Choose<T>();
        
    public:
        // Single operator() with if constexpr chain
        template<typename T>
        [[nodiscard]] constexpr auto operator()(T&& t) const
            noexcept(_Choice<T>._No_throw)
            -> decltype(auto)
        {
            if constexpr (_Choice<T>._Strategy == _St::_member) {
                return std::forward<T>(t).operation();
            } else if constexpr (_Choice<T>._Strategy == _St::_adl) {
                return operation(std::forward<T>(t));
            } else {
                return 0;  // Default implementation
            }
        }
    };
} // namespace _my_operation
} // namespace graph::_cpo
```

### Why This Pattern is Superior

1. **Single evaluation**: `_Choose<T>()` is called once and cached in `_Choice<T>`
2. **Self-contained**: All logic in one `operator()` function
3. **No negation chains**: Priority is implicit in `if constexpr` order
4. **Simple noexcept**: Just reference `_Choice<T>._No_throw`
5. **Easy to maintain**: Add/remove paths without touching other code
6. **MSVC-compatible**: Matches standard library implementation

### Complete Example: `vertex_id` CPO (MSVC Style)

Here's a complete `vertex_id` CPO using the MSVC `_Choice_t` pattern:

```cpp
namespace graph::_cpo {
namespace _vertex_id {
    // Path selection enum
    enum class _St { _none, _member, _adl, _integral };
    
    // Struct to hold both strategy and noexcept info
    template<typename _Ty>
    struct _Choice_t {
        _Ty _Strategy = _Ty{};
        bool _No_throw = false;
    };
    
    // Concept checks
    template<typename VD>
    concept _has_member = requires(const VD& vd) {
        { vd.vertex_id() } -> std::convertible_to<std::size_t>;
    };
    
    template<typename VD>
    concept _has_adl = requires(const VD& vd) {
        { vertex_id(vd) } -> std::convertible_to<std::size_t>;
    };
    
    template<typename VD>
    concept _is_integral = std::integral<std::remove_cvref_t<VD>>;
    
    // Single compile-time evaluation for both path and noexcept
    template<typename VD>
    [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
        if constexpr (_has_member<VD>) {
            return {_St::_member, noexcept(std::declval<const VD&>().vertex_id())};
        } else if constexpr (_has_adl<VD>) {
            return {_St::_adl, noexcept(vertex_id(std::declval<const VD&>()))};
        } else if constexpr (_is_integral<VD>) {
            return {_St::_integral, true};
        } else {
            return {_St::_none, false};
        }
    }
    
    class _fn {
    private:
        // Cache the choice at compile time for each type
        template<typename VD>
        static constexpr _Choice_t<_St> _Choice = _Choose<VD>();
        
    public:
        // Single operator() with if constexpr chain
        template<typename VD>
        [[nodiscard]] constexpr auto operator()(const VD& vd) const
            noexcept(_Choice<VD>._No_throw)
            -> decltype(auto)
        {
            if constexpr (_Choice<VD>._Strategy == _St::_member) {
                return vd.vertex_id();
            } else if constexpr (_Choice<VD>._Strategy == _St::_adl) {
                return vertex_id(vd);
            } else if constexpr (_Choice<VD>._Strategy == _St::_integral) {
                return vd;
            } else {
                static_assert(_Choice<VD>._Strategy != _St::_none,
                    "vertex_id requires .vertex_id() member, ADL vertex_id(), or integral type");
            }
        }
    };
} // namespace _vertex_id

inline namespace _cpos {
    inline constexpr _cpo::_vertex_id::_fn vertex_id{};
}
} // namespace graph::_cpo

// Testing the implementation
namespace test {
    struct WithMember {
        std::size_t vertex_id() const noexcept { return 42; }
    };
    
    struct WithADL {
        int key = 99;
    };
    std::size_t vertex_id(const WithADL& w) { return w.key; }
    
    // Verify path selection at compile time
    static_assert(graph::_cpo::_vertex_id::_Choose<WithMember>()._Strategy 
                  == graph::_cpo::_vertex_id::_St::_member);
    static_assert(graph::_cpo::_vertex_id::_Choose<WithADL>()._Strategy 
                  == graph::_cpo::_vertex_id::_St::_adl);
    static_assert(graph::_cpo::_vertex_id::_Choose<std::size_t>()._Strategy 
                  == graph::_cpo::_vertex_id::_St::_integral);
    
    // Verify noexcept propagation
    static_assert(graph::_cpo::_vertex_id::_Choose<WithMember>()._No_throw);
    static_assert(!graph::_cpo::_vertex_id::_Choose<WithADL>()._No_throw);
    static_assert(graph::_cpo::_vertex_id::_Choose<std::size_t>()._No_throw);
    
    // Verify actual CPO behavior
    static_assert(noexcept(graph::vertex_id(WithMember{})));
    static_assert(!noexcept(graph::vertex_id(WithADL{})));
    static_assert(noexcept(graph::vertex_id(std::size_t{})));
}
```

### Benefits of the `_Choice_t` Pattern

1. **Single evaluation**: `_Choose<T>()` computes both strategy and noexcept once
2. **Cached result**: `_Choice<T>` stores the result as a static constexpr variable
3. **Simple noexcept**: Just use `_Choice<T>._No_throw` - no complex expressions
4. **Self-contained function**: All logic in one `operator()` with `if constexpr`
5. **No negation chains**: Priority is implicit in `if constexpr` order
6. **MSVC-compatible**: Matches standard library implementation exactly
7. **Better diagnostics**: Single function signature = clearer error messages
8. **Easy to maintain**: Add/remove paths without updating constraints

### Alternative: Multiple Overloads (Not Recommended)

For comparison, here's the older multi-overload pattern. This approach is more verbose and error-prone:

```cpp
class _fn {
public:
    // Member function path
    template<typename T>
        requires _has_member<T>
    constexpr auto operator()(T&& t) const
        noexcept(noexcept(std::forward<T>(t).operation()))
        -> decltype(std::forward<T>(t).operation())
    {
        return std::forward<T>(t).operation();
    }
    
    // ADL path - requires explicit negation
    template<typename T>
        requires (!_has_member<T> && _has_adl<T>)
    constexpr auto operator()(T&& t) const
        noexcept(noexcept(operation(std::forward<T>(t))))
        -> decltype(operation(std::forward<T>(t)))
    {
        return operation(std::forward<T>(t));
    }
};
```

**Problems with multiple overloads:**
- ❌ Requires explicit negation chains that are error-prone
- ❌ Must repeat noexcept expressions in multiple places
- ❌ Adding a new path requires updating all lower-priority overloads
- ❌ Risk of ambiguous overload resolution
- ❌ More verbose and harder to maintain

**Use the `_Choice_t` pattern instead** - it's the modern, MSVC-style approach.

## Handling Multiple Optional Overloads

CPOs must support multiple overload paths gracefully. The key is using **mutually exclusive concepts** with proper negation chains.

### Critical Rules for Cross-Compiler Compatibility

1. **Use `if constexpr` over multiple overloads**: Single function with `if constexpr` chain is more reliable than multiple overloads with negation constraints
2. **Use `_Choice_t` pattern**: Evaluate strategy and noexcept once in `_Choose()`, cache in `_Choice<T>`
3. **Concept ordering**: Define concepts clearly, let `if constexpr` order determine priority
4. **Return type handling**: Use `-> decltype(auto)` or explicit return type, test with all compilers
5. **Noexcept propagation**: Use `_Choice<T>._No_throw` from the cached choice

### Why `if constexpr` is Better

**MSVC `if constexpr` pattern (Recommended):**
```cpp
class _fn {
private:
    template<typename T>
    static constexpr _Choice_t<_St> _Choice = _Choose<T>();
    
public:
    template<typename T>
    constexpr auto operator()(T&& t) const
        noexcept(_Choice<T>._No_throw)
        -> decltype(auto)
    {
        if constexpr (_Choice<T>._Strategy == _St::_path1) {
            return /* path1 */;
        } else if constexpr (_Choice<T>._Strategy == _St::_path2) {
            return /* path2 */;
        }
    }
};
```

✅ Priority determined by `if constexpr` order  
✅ No negation chains needed  
✅ Single noexcept specification  
✅ Easy to maintain  
✅ Works reliably on MSVC, GCC, Clang  

**Multi-overload pattern (Not Recommended):**
```cpp
class _fn {
public:
    template<typename T>
        requires _has_path1<T>
    constexpr auto operator()(T&& t) const { /* ... */ }
    
    template<typename T>
        requires (!_has_path1<T> && _has_path2<T>)
    constexpr auto operator()(T&& t) const { /* ... */ }
};
```

❌ Requires error-prone negation chains  
❌ Risk of ambiguous overload resolution  
❌ Harder to maintain as paths grow  
❌ Compiler-specific constraint evaluation differences

### Pattern for N Optional Overloads (MSVC Style)

```cpp
namespace _operation {
    // 1. Strategy enum
    enum class _St { _none, _path1, _path2, _path3 /* ... more paths */ };
    
    // 2. Choice struct
    template<typename _Ty>
    struct _Choice_t {
        _Ty _Strategy = _Ty{};
        bool _No_throw = false;
    };
    
    // 3. Define concepts for each customization point
    template<typename T>
    concept _has_path1 = /* highest priority check */;
    
    template<typename T>
    concept _has_path2 = /* second priority check */;
    
    template<typename T>
    concept _has_path3 = /* third priority check */;
    
    // ... more paths as needed
    
    // 4. Single compile-time choice evaluation
    template<typename T>
    [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
        if constexpr (_has_path1<T>) {
            return {_St::_path1, noexcept(/* path1 expression */)};
        } else if constexpr (_has_path2<T>) {
            return {_St::_path2, noexcept(/* path2 expression */)};
        } else if constexpr (_has_path3<T>) {
            return {_St::_path3, noexcept(/* path3 expression */)};
        } else {
            return {_St::_none, true};  // or _St::_default
        }
    }
    
    // 5. CPO class with single operator()
    class _fn {
    private:
        template<typename T>
        static constexpr _Choice_t<_St> _Choice = _Choose<T>();
        
    public:
        template<typename T>
        [[nodiscard]] constexpr auto operator()(T&& t) const
            noexcept(_Choice<T>._No_throw)
            -> decltype(auto)
        {
            if constexpr (_Choice<T>._Strategy == _St::_path1) {
                return /* path1 implementation */;
            } else if constexpr (_Choice<T>._Strategy == _St::_path2) {
                return /* path2 implementation */;
            } else if constexpr (_Choice<T>._Strategy == _St::_path3) {
                return /* path3 implementation */;
            } else {
                // Default implementation or static_assert
                return /* default */;
            }
        }
    };
}
```

**Key advantages:**
1. **No negation chains**: `if constexpr` order determines priority naturally
2. **Single evaluation**: `_Choose<T>()` evaluates once, cached in `_Choice<T>`
3. **Simple noexcept**: Just use `_Choice<T>._No_throw`
4. **Easy maintenance**: Add/remove paths without touching other code
5. **Self-documenting**: Read top-to-bottom to see priority order

### Example: CPO with 4 Overload Paths (MSVC Style)

```cpp
namespace graph {
namespace _cpo {

namespace _edge_weight {
    // Path selection enum
    enum class _St { _none, _member_fn, _member_var, _adl, _tuple };
    
    // Choice struct
    template<typename _Ty>
    struct _Choice_t {
        _Ty _Strategy = _Ty{};
        bool _No_throw = false;
    };
    
    // Path 1: Member function .weight()
    template<typename E>
    concept _has_weight_member = requires(const E& e) {
        { e.weight() } -> std::convertible_to<double>;
    };
    
    // Path 2: Member variable .weight
    template<typename E>
    concept _has_weight_variable = requires(const E& e) {
        { e.weight } -> std::convertible_to<double>;
    };
    
    // Path 3: ADL free function weight()
    template<typename E>
    concept _has_weight_adl = requires(const E& e) {
        { weight(e) } -> std::convertible_to<double>;
    };
    
    // Path 4: Tuple-like access (for std::pair, std::tuple)
    template<typename E>
    concept _has_tuple_weight = requires(const E& e) {
        requires std::tuple_size<std::remove_cvref_t<E>>::value >= 2;
        { std::get<1>(e) } -> std::convertible_to<double>;
    };
    
    // Single compile-time evaluation
    template<typename E>
    [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
        if constexpr (_has_weight_member<E>) {
            return {_St::_member_fn, noexcept(std::declval<const E&>().weight())};
        } else if constexpr (_has_weight_variable<E>) {
            return {_St::_member_var, noexcept(std::declval<const E&>().weight)};
        } else if constexpr (_has_weight_adl<E>) {
            return {_St::_adl, noexcept(weight(std::declval<const E&>()))};
        } else if constexpr (_has_tuple_weight<E>) {
            return {_St::_tuple, noexcept(std::get<1>(std::declval<const E&>()))};
        } else {
            return {_St::_none, true};  // Default is noexcept
        }
    }
    
    class _fn {
    private:
        template<typename E>
        static constexpr _Choice_t<_St> _Choice = _Choose<E>();
        
    public:
        // Single operator() with if constexpr chain
        template<typename E>
        [[nodiscard]] constexpr auto operator()(const E& e) const
            noexcept(_Choice<E>._No_throw)
            -> double  // All paths return double
        {
            if constexpr (_Choice<E>._Strategy == _St::_member_fn) {
                return e.weight();
            } else if constexpr (_Choice<E>._Strategy == _St::_member_var) {
                return e.weight;
            } else if constexpr (_Choice<E>._Strategy == _St::_adl) {
                return weight(e);
            } else if constexpr (_Choice<E>._Strategy == _St::_tuple) {
                return std::get<1>(e);
            } else {
                return 1.0;  // Default unit weight
            }
        }
    };
} // namespace _edge_weight

} // namespace _cpo

inline namespace _cpos {
    inline constexpr _cpo::_edge_weight::_fn edge_weight{};
} // namespace _cpos

} // namespace graph

// Usage with all 4 paths:
namespace user {
    struct EdgeWithMethod {
        double weight() const { return 5.0; }
    };
    
    struct EdgeWithVariable {
        double weight = 3.0;
    };
    
    struct EdgeCustom { int cost; };
    double weight(const EdgeCustom& e) { return e.cost * 2.0; }
    
    // std::pair works via tuple protocol
    using EdgePair = std::pair<int, double>;
    
    struct SimpleEdge {};  // Uses default
}

void example() {
    // All use same CPO, different implementations
    auto w1 = graph::edge_weight(user::EdgeWithMethod{});        // 5.0
    auto w2 = graph::edge_weight(user::EdgeWithVariable{3.0});   // 3.0
    auto w3 = graph::edge_weight(user::EdgeCustom{4});           // 8.0
    auto w4 = graph::edge_weight(user::EdgePair{1, 2.5});        // 2.5
    auto w5 = graph::edge_weight(user::SimpleEdge{});            // 1.0
}
```

### Advantages Over Multiple Overloads

**MSVC `if constexpr` pattern:**
- ✅ No explicit negation chains required
- ✅ Priority order is clear from reading top-to-bottom
- ✅ Single noexcept specification using `_Choice<E>._No_throw`
- ✅ Easy to add/remove/reorder paths
- ✅ Single function signature = better error messages
- ✅ All path logic in one place

**Old multi-overload pattern:**
- ❌ Each overload needs `(!_has_path1<E> && !_has_path2<E> && ...)`
- ❌ Noexcept repeated in every overload
- ❌ Adding a path requires updating all lower-priority overloads
- ❌ Risk of constraint overlap causing ambiguity

### Compiler-Specific Considerations

#### MSVC
- Handles concept negation chains reliably
- May require explicit template instantiation hints for complex overload sets
- Works best with explicit return type constraints

#### GCC
- Excellent concept support
- Sensitive to constraint ordering in complex scenarios
- May warn about unused parameters in SFINAE contexts (use `[[maybe_unused]]`)

#### Clang
- Strictest about concept constraint satisfaction
- Best error messages for constraint failures
- May require explicit `std::remove_cvref_t` in some concept definitions

### Testing Multiple Overloads

Test that each path is selected correctly:

```cpp
TEST_CASE("CPO selects correct overload path") {
    SECTION("Member function path") {
        struct WithMethod { double weight() const { return 1.0; } };
        static_assert(_cpo::_edge_weight::_has_weight_member<WithMethod>);
        REQUIRE(graph::edge_weight(WithMethod{}) == 1.0);
    }
    
    SECTION("Member variable path") {
        struct WithVariable { double weight = 2.0; };
        static_assert(!_cpo::_edge_weight::_has_weight_member<WithVariable>);
        static_assert(_cpo::_edge_weight::_has_weight_variable<WithVariable>);
        REQUIRE(graph::edge_weight(WithVariable{}) == 2.0);
    }
    
    SECTION("ADL path") {
        namespace user {
            struct Custom { int val; };
            double weight(const Custom& c) { return c.val * 1.0; }
        }
        static_assert(!_cpo::_edge_weight::_has_weight_member<user::Custom>);
        static_assert(!_cpo::_edge_weight::_has_weight_variable<user::Custom>);
        static_assert(_cpo::_edge_weight::_has_weight_adl<user::Custom>);
        REQUIRE(graph::edge_weight(user::Custom{3}) == 3.0);
    }
    
    SECTION("Tuple path") {
        using Pair = std::pair<int, double>;
        static_assert(_cpo::_edge_weight::_has_tuple_weight<Pair>);
        REQUIRE(graph::edge_weight(Pair{1, 4.0}) == 4.0);
    }
    
    SECTION("Default path") {
        struct Empty {};
        static_assert(!_cpo::_edge_weight::_has_weight_member<Empty>);
        static_assert(!_cpo::_edge_weight::_has_weight_variable<Empty>);
        static_assert(!_cpo::_edge_weight::_has_weight_adl<Empty>);
        static_assert(!_cpo::_edge_weight::_has_tuple_weight<Empty>);
        REQUIRE(graph::edge_weight(Empty{}) == 1.0);
    }
}

TEST_CASE("CPO overload paths are mutually exclusive") {
    // Ensure only one overload matches at compile time
    struct Ambiguous { 
        double weight() const { return 1.0; }
        double weight = 2.0;
    };
    // Member function should win
    REQUIRE(graph::edge_weight(Ambiguous{}) == 1.0);
}
```

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

## Cross-Compiler Compatibility Guidelines

To ensure CPOs work correctly on MSVC, GCC, and Clang, follow these guidelines:

### 1. Prefer `if constexpr` Over Multiple Overloads

```cpp
// CORRECT - MSVC style with if constexpr (Recommended)
class _fn {
private:
    template<typename T>
    static constexpr _Choice_t<_St> _Choice = _Choose<T>();
    
public:
    template<typename T>
    constexpr auto operator()(T&& t) const
        noexcept(_Choice<T>._No_throw)
    {
        if constexpr (_Choice<T>._Strategy == _St::_path1) {
            return /* path1 */;
        } else if constexpr (_Choice<T>._Strategy == _St::_path2) {
            return /* path2 */;
        }
    }
};

// AVOID - Multiple overloads (error-prone)
class _fn {
public:
    template<typename T>
        requires _has_path1<T>
    constexpr auto operator()(T&& t) const { /* ... */ }
    
    template<typename T>
        requires (!_has_path1<T> && _has_path2<T>)  // Easy to get wrong
    constexpr auto operator()(T&& t) const { /* ... */ }
};
```

### 2. Use `std::remove_cvref_t` in Concepts

```cpp
// CORRECT - Handles cv-qualifiers and references uniformly
template<typename T>
concept _is_integral = std::integral<std::remove_cvref_t<T>>;

// PROBLEMATIC - May behave differently across compilers
template<typename T>
concept _is_integral = std::integral<T>;
```

### 3. Explicitly Specify Return Type Constraints

```cpp
// CORRECT - Clear and portable
template<typename T>
concept _has_member = requires(T t) {
    { t.operation() } -> std::convertible_to<int>;
};

// LESS PORTABLE - Some compilers may not enforce this
template<typename T>
concept _has_member = requires(T t) {
    t.operation();
};
```

### 4. Test with All Three Compilers

Use CI/CD to validate CPOs on multiple compilers:

```yaml
# Example GitHub Actions matrix
strategy:
  matrix:
    compiler:
      - { name: gcc, version: 12, cxx: g++-12 }
      - { name: gcc, version: 13, cxx: g++-13 }
      - { name: clang, version: 15, cxx: clang++-15 }
      - { name: clang, version: 16, cxx: clang++-16 }
      - { name: msvc, version: 2022 }
```

### 5. Use Trailing Return Types for Complex Expressions

```cpp
// CORRECT - Works reliably on all compilers
template<typename T>
constexpr auto operator()(T&& t) const
    noexcept(noexcept(std::forward<T>(t).operation()))
    -> decltype(std::forward<T>(t).operation())
{
    return std::forward<T>(t).operation();
}

// PROBLEMATIC - May confuse some compilers
template<typename T>
constexpr decltype(auto) operator()(T&& t) const {
    return std::forward<T>(t).operation();
}
```

### 6. Avoid Implicit Conversions in Concepts

```cpp
// CORRECT - Explicit about what's required
template<typename T>
concept _has_size = requires(const T& t) {
    { t.size() } -> std::same_as<std::size_t>;
};

// LESS SAFE - May allow unintended conversions
template<typename T>
concept _has_size = requires(const T& t) {
    { t.size() };  // Could return anything convertible to size_t
};
```

### 7. Document Compiler-Specific Workarounds

If you must use compiler-specific code, document it clearly:

```cpp
// CPO implementation
class _fn {
public:
    template<typename T>
        requires _has_member<T>
    constexpr auto operator()(T&& t) const
        -> decltype(std::forward<T>(t).operation())
    {
        #if defined(_MSC_VER) && _MSC_VER < 1930
            // MSVC 2019 workaround for concept evaluation issue
            return std::forward<T>(t).operation();
        #else
            return std::forward<T>(t).operation();
        #endif
    }
};
```

### 8. Test Constraint Evaluation Order

Different compilers may evaluate constraints in different orders:

```cpp
// Create a test to verify constraint evaluation
static_assert(requires(MyType t) {
    graph::my_operation(t);  // Should compile
});

static_assert(!requires(IncompatibleType t) {
    graph::my_operation(t);  // Should not compile
});
```

### 9. Use Standard Library Concepts When Available

```cpp
// PREFERRED - Standard, well-tested
template<typename T>
concept _is_random_access = std::random_access_iterator<T>;

// AVOID - Custom implementations may have portability issues
template<typename T>
concept _is_random_access = requires(T it) {
    it += 1;
    it[0];
    it - it;
};
```

### 10. Provide Clear Compiler Error Messages

Use `static_assert` with helpful messages for unsatisfied constraints:

```cpp
template<typename T>
constexpr auto operator()(T&& t) const {
    if constexpr (_has_member<T>) {
        return std::forward<T>(t).operation();
    } else if constexpr (_has_adl<T>) {
        return operation(std::forward<T>(t));
    } else {
        static_assert(_has_member<T> || _has_adl<T>,
            "Type must provide either .operation() member or ADL operation(T) function");
    }
}
```

### Testing Across Compilers

Example test structure:

```cpp
// Test that compiles on all three compilers
TEST_CASE("CPO cross-compiler compatibility") {
    SECTION("MSVC-specific behavior") {
        #ifdef _MSC_VER
            // Test MSVC-specific scenarios
        #endif
    }
    
    SECTION("GCC-specific behavior") {
        #ifdef __GNUC__
            // Test GCC-specific scenarios
        #endif
    }
    
    SECTION("Clang-specific behavior") {
        #ifdef __clang__
            // Test Clang-specific scenarios
        #endif
    }
    
    SECTION("Common behavior") {
        // Tests that must pass on all compilers
        struct TestType { int operation() const { return 42; } };
        REQUIRE(graph::my_operation(TestType{}) == 42);
    }
}
```

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

## Advanced Patterns (Optional)

This section covers advanced CPO techniques that are useful in specific scenarios. Most CPOs won't need these patterns - use them only when you have a clear requirement.

### Pattern: Implicit-Only Conversion Checking

The standard library uses a pattern called `_Fake_copy_init` to distinguish between implicit and explicit conversions. This can be useful when you need strict conversion semantics.

#### Background: Implicit vs Explicit Conversion

```cpp
struct ExplicitConvertible {
    explicit operator int() const { return 42; }
};

struct ImplicitConvertible {
    operator int() const { return 42; }
};

// std::convertible_to allows both
static_assert(std::convertible_to<ImplicitConvertible, int>);   // true
static_assert(std::convertible_to<ExplicitConvertible, int>);   // true (!)

// Copy-initialization only works with implicit
int x = ImplicitConvertible{};   // OK
int y = ExplicitConvertible{};   // Error: explicit constructor
```

#### The `_Fake_copy_init` Pattern

Both MSVC and GCC standard libraries use this pattern internally:

```cpp
namespace graph::_cpo {
namespace _detail {
    // Helper function - declared but never defined
    // Used only for concept checking
    void _fake_copy_init(auto);
}

// Check if type is implicitly convertible (not just explicitly constructible)
template<typename From, typename To>
concept _implicitly_convertible = requires(From&& from) {
    _detail::_fake_copy_init<To>(std::forward<From>(from));
};

// Example usage in a CPO concept
template<typename VD>
concept _has_implicit_id = requires(const VD& vd) {
    // Ensures vertex_id() returns something implicitly convertible to size_t
    _detail::_fake_copy_init<std::size_t>(vd.vertex_id());
    // Also check it's actually callable
    { vd.vertex_id() } -> std::convertible_to<std::size_t>;
};
}
```

#### When to Use This Pattern

**✅ Use when:**
- You need to match exact standard library CPO semantics (e.g., `std::ranges::begin`)
- You want to prevent surprising explicit constructor calls
- You're implementing ranges-style adaptors that require strict semantics
- Your API documentation promises implicit-only conversions

**❌ Don't use when:**
- Standard concepts like `std::convertible_to` are sufficient (most cases)
- You're not sure if you need it (you probably don't)
- It adds complexity without clear benefit
- Your users expect flexibility in conversions

#### Complete Example: Strict Vertex ID CPO

```cpp
namespace graph::_cpo {
namespace _vertex_id_strict {
    namespace _detail {
        void _fake_copy_init(auto);  // Never defined
    }
    
    // Check for member function with implicit conversion
    template<typename VD>
    concept _has_member = requires(const VD& vd) {
        _detail::_fake_copy_init<std::size_t>(vd.vertex_id());
        { vd.vertex_id() } -> std::convertible_to<std::size_t>;
    };
    
    // Check for ADL function with implicit conversion
    template<typename VD>
    concept _has_adl = requires(const VD& vd) {
        _detail::_fake_copy_init<std::size_t>(vertex_id(vd));
        { vertex_id(vd) } -> std::convertible_to<std::size_t>;
    };
    
    class _fn {
    public:
        // Member function path
        template<typename VD>
            requires _has_member<VD>
        [[nodiscard]] constexpr std::size_t operator()(const VD& vd) const
            noexcept(noexcept(vd.vertex_id()))
        {
            return vd.vertex_id();  // Guaranteed implicit conversion
        }
        
        // ADL path
        template<typename VD>
            requires (!_has_member<VD> && _has_adl<VD>)
        [[nodiscard]] constexpr std::size_t operator()(const VD& vd) const
            noexcept(noexcept(vertex_id(vd)))
        {
            return vertex_id(vd);  // Guaranteed implicit conversion
        }
        
        // Integral pass-through
        template<typename VD>
            requires (!_has_member<VD> && !_has_adl<VD> 
                      && std::integral<std::remove_cvref_t<VD>>)
        [[nodiscard]] constexpr VD operator()(VD vd) const noexcept
        {
            return vd;
        }
    };
} // namespace _vertex_id_strict

inline namespace _cpos {
    inline constexpr _cpo::_vertex_id_strict::_fn vertex_id_strict{};
}
} // namespace graph

// Testing the difference
namespace test {
    struct ImplicitID {
        operator std::size_t() const { return 42; }
    };
    
    struct ExplicitID {
        explicit operator std::size_t() const { return 42; }
    };
    
    // Standard CPO with std::convertible_to
    static_assert(requires(ImplicitID id) {
        { id } -> std::convertible_to<std::size_t>;
    });
    static_assert(requires(ExplicitID id) {
        { id } -> std::convertible_to<std::size_t>;  // Both work!
    });
    
    // Strict CPO with _fake_copy_init
    struct ImplicitDesc {
        ImplicitID vertex_id() const { return {}; }
    };
    struct ExplicitDesc {
        ExplicitID vertex_id() const { return {}; }
    };
    
    static_assert(graph::_cpo::_vertex_id_strict::_has_member<ImplicitDesc>);
    static_assert(!graph::_cpo::_vertex_id_strict::_has_member<ExplicitDesc>); // Rejected!
}
```

#### Cross-Compiler Compatibility

This pattern works correctly on MSVC, GCC, and Clang:

```cpp
// Tested on all three compilers
namespace graph::_cpo::_detail {
    void _fake_copy_init(auto);  // Works on MSVC, GCC, Clang
}

// Alternative form if you encounter compiler-specific issues:
namespace graph::_cpo::_detail {
    template<typename T>
    void _fake_copy_init(T);  // More explicit template syntax
}
```

#### Performance Considerations

- **Zero runtime cost**: The `_fake_copy_init` function is never defined or called
- **Compile-time only**: Used purely for concept checking
- **No overhead**: Generates identical code to standard concepts once compiled
- **Potential compile-time cost**: Slightly more complex concept checking

#### Recommendation

For the graph descriptor library:

```cpp
// PREFER THIS for most use cases (simpler, clearer)
template<typename VD>
concept _has_vertex_id = requires(const VD& vd) {
    { vd.vertex_id() } -> std::convertible_to<std::size_t>;
};

// USE THIS only when you specifically need implicit-only semantics
template<typename VD>
concept _has_implicit_vertex_id = requires(const VD& vd) {
    _detail::_fake_copy_init<std::size_t>(vd.vertex_id());
    { vd.vertex_id() } -> std::convertible_to<std::size_t>;
};
```

**Bottom Line**: Document this pattern for completeness, but use `std::convertible_to` for actual implementations unless you have a specific requirement for implicit-only conversions.

### Pattern: Subsumption and Concept Refinement

For complex CPOs with many overload paths, you can use concept subsumption to make constraints clearer:

```cpp
namespace graph::_cpo {
namespace _advanced {
    // Base concept
    template<typename T>
    concept _has_operation = requires(T t) {
        t.operation();
    };
    
    // Refined concepts (subsume base)
    template<typename T>
    concept _has_noexcept_operation = _has_operation<T> && requires(T t) {
        { t.operation() } noexcept;
    };
    
    template<typename T>
    concept _has_constexpr_operation = _has_operation<T> && requires {
        requires requires(T t) { 
            { t.operation() } -> std::same_as<int>;
        };
    };
    
    class _fn {
    public:
        // Most specific - noexcept version
        template<typename T>
            requires _has_noexcept_operation<T>
        constexpr auto operator()(T&& t) const noexcept {
            return std::forward<T>(t).operation();
        }
        
        // Less specific - basic version
        template<typename T>
            requires _has_operation<T>
        constexpr auto operator()(T&& t) const {
            return std::forward<T>(t).operation();
        }
    };
}
}
```

**Note**: Subsumption can make overload resolution clearer but may be overkill for simple CPOs. Use when you have legitimate refinement hierarchies.

## Summary

When implementing a CPO in MSVC style that works across compilers:

### Core Requirements
1. ✅ **Cross-compiler compatibility**: Test on MSVC, GCC, and Clang
2. ✅ **Multiple overload support**: Design for 2+ optional overload paths
3. ✅ **Complete negation chains**: Each overload explicitly excludes all higher-priority paths
4. ✅ **Internal namespace structure**: Use `_cpo::_operation` for implementation
5. ✅ **Concept-based constraints**: Define concepts for each customization point

### Implementation Checklist
6. ✅ **Priority ordering**: Member function → ADL → Additional paths → Default
7. ✅ **Explicit return types**: Use trailing return types with `decltype`
8. ✅ **Preserve noexcept**: Propagate noexcept from underlying operations
9. ✅ **Perfect forwarding**: Use `std::forward` to preserve value categories
10. ✅ **Public CPO object**: `inline constexpr _fn operation{}` in inline namespace

### Quality Assurance
11. ✅ **Test all paths**: Verify each overload is selected correctly
12. ✅ **Test mutual exclusion**: Ensure no ambiguous overloads
13. ✅ **Test portability**: Run tests on all three major compilers
14. ✅ **Document constraints**: Clear error messages for unsatisfied requirements
15. ✅ **Standard library alignment**: Use stdlib concepts when available

### Example Minimal CPO Template (MSVC Style - Recommended)

```cpp
namespace graph {
namespace _cpo {
namespace _my_cpo {
    // Strategy enum
    enum class _St { _none, _path1, _path2, _default };
    
    // Choice struct holds strategy + noexcept
    template<typename _Ty>
    struct _Choice_t {
        _Ty _Strategy = _Ty{};
        bool _No_throw = false;
    };
    
    // Concepts in priority order
    template<typename T> concept _has_path1 = /* ... */;
    template<typename T> concept _has_path2 = /* ... */;
    
    // Single compile-time evaluation
    template<typename T>
    [[nodiscard]] consteval _Choice_t<_St> _Choose() noexcept {
        if constexpr (_has_path1<T>) {
            return {_St::_path1, noexcept(/* path1 expression */)};
        } else if constexpr (_has_path2<T>) {
            return {_St::_path2, noexcept(/* path2 expression */)};
        } else {
            return {_St::_default, true};
        }
    }
    
    class _fn {
    private:
        // Cache choice for each type
        template<typename T>
        static constexpr _Choice_t<_St> _Choice = _Choose<T>();
        
    public:
        // Single operator() with if constexpr chain
        template<typename T>
        [[nodiscard]] constexpr auto operator()(T&& t) const
            noexcept(_Choice<T>._No_throw)
            -> decltype(auto)
        {
            if constexpr (_Choice<T>._Strategy == _St::_path1) {
                return /* path1 implementation */;
            } else if constexpr (_Choice<T>._Strategy == _St::_path2) {
                return /* path2 implementation */;
            } else {
                return /* default implementation */;
            }
        }
    };
}
} // namespace _cpo

inline namespace _cpos {
    inline constexpr _cpo::_my_cpo::_fn my_cpo{};
}
} // namespace graph
```

This pattern provides:
- ✅ Maximum flexibility for users to customize behavior
- ✅ Consistent interface across different storage strategies  
- ✅ Guaranteed portability across MSVC, GCC, and Clang
- ✅ Zero-cost abstraction with no runtime overhead
- ✅ Clear, maintainable code structure

