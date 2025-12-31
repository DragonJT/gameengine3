#pragma once

inline constexpr int GAME_OBJECT_V2_HPP_VERSION = 12345;

#include <tuple>
#include <vector>
#include <span>
#include <type_traits>
#include <utility>
#include <cstddef>

namespace dod {
// ------------------------------
// Field type (unique by Tag)
// ------------------------------
template<class Tag, class T>
struct Field {
  // A unique identiy for this field
  using tag        = Tag;

  //The stored value type: T
  using value_type = T;
};

// using is aliases, so that 
// Field<HealthTag, int>::tag is HealthTag
// Field<HealthTag, int>::value_type is int


// ------------------------------
// index_of<Field, Fields...>
// ------------------------------
// find “where is this field in a list of fields?”
// This declaration means  “A struct template named index_of exists; definition(s) come below.”
template<class F, class... Fs>
struct index_of;

//class... Fs is a parameter pack: it means “zero or more types.”


//Fix: use partial specializations (classic metaprogramming pattern)
// match: F is first
template<class F, class... Rest>
struct index_of<F, F, Rest...> : std::integral_constant<std::size_t, 0> {};



//This means: “If you have a list of at least one type (First) plus the rest (Rest...), here is how to compute.”
// recurse: F is not first
template<class F, class First, class... Rest>
struct index_of<F, First, Rest...>
  : std::integral_constant<std::size_t, 1 + index_of<F, Rest...>::value> {};


//This specialization matches when there are no more types left to search.
//!std::is_same_v<F, F> is alwazs false 

// false is a non-dependent constant expression → the compiler can reject it immediately when parsing/instantiating the template definition.
// !std::is_same_v<F, F> is also always false, but it is dependent on F (it mentions a template parameter), so the compiler only evaluates it when index_of<F> is instantiated.
//A common alternative that makes this intent clearer is:

//template<class>
//inline constexpr bool dependent_false_v = false;

//and then use static_assert(dependent_false_v<F>, "message");


template<class F>
struct index_of<F> {
  static_assert(!std::is_same_v<F, F>, "Requested Field is not present in ObjectType.");
};


// Convenience alias: index_of_v
// Instead of writing: index_of<PosY, Health, PosX, PosY>::value
// you can write: index_of_v<PosY, Health, PosX, PosY>

template<class F, class... Fs>
inline constexpr std::size_t index_of_v = index_of<F, Fs...>::value;

// ------------------------------
// ObjectType<Fields...> : SoA storage
// ------------------------------
template<class... Fields>
class ObjectType {
  static_assert(sizeof...(Fields) > 0, "ObjectType must have at least one Field.");

  using storage_t = std::tuple<std::vector<typename Fields::value_type>...>;
  storage_t cols_;

  template<std::size_t... Is>
  void reserve_impl(std::index_sequence<Is...>, std::size_t n) {
    (std::get<Is>(cols_).reserve(n), ...);
  }

  template<class Tuple, std::size_t... Is>
  void emplace_impl(Tuple&& t, std::index_sequence<Is...>) {
    // Push one element into each column, in Fields... order.
    (std::get<Is>(cols_).emplace_back(static_cast<typename std::tuple_element_t<Is, std::tuple<typename Fields::value_type...>>>(
        std::get<Is>(std::forward<Tuple>(t))
      )), ...);
  }

  template<std::size_t... Is>
  void erase_swap_pop_impl(std::index_sequence<Is...>, std::size_t i, std::size_t last) {
    // swap element i with last and pop_back for every column
    ((std::get<Is>(cols_)[i] = std::get<Is>(cols_)[last], std::get<Is>(cols_).pop_back()), ...);
  }

public:
  using size_type = std::size_t;

  // Column access by Field type
  template<class F>
  std::vector<typename F::value_type>& column() {
    static_assert((std::is_same_v<F, Fields> || ...), "Field not in this ObjectType.");
    return std::get<index_of_v<F, Fields...>>(cols_);
  }

  template<class F>
  const std::vector<typename F::value_type>& column() const {
    static_assert((std::is_same_v<F, Fields> || ...), "Field not in this ObjectType.");
    return std::get<index_of_v<F, Fields...>>(cols_);
  }

  // Span view (handy for SIMD / tight loops)
  template<class F>
  std::span<typename F::value_type> span() {
    auto& v = column<F>();
    return { v.data(), v.size() };
  }

  template<class F>
  std::span<const typename F::value_type> span() const {
    auto const& v = column<F>();
    return { v.data(), v.size() };
  }

  size_type size() const {
    // All columns are kept in sync. Use the first column size.
    return std::get<0>(cols_).size();
  }

  void reserve(size_type n) {
    reserve_impl(std::index_sequence_for<Fields...>{}, n);
  }

  // Append one "object" (values must match Fields... order)
  template<class... Vals>
  void emplace(Vals&&... vals) {
    static_assert(sizeof...(Vals) == sizeof...(Fields),
                  "emplace() requires one value per Field, in Fields... order.");
    static_assert((std::is_constructible_v<typename Fields::value_type, Vals&&> && ...),
                  "A provided value cannot construct the corresponding Field::value_type.");

    auto t = std::forward_as_tuple(std::forward<Vals>(vals)...);
    emplace_impl(t, std::index_sequence_for<Fields...>{});
  }

  /*
  // Remove object at index i by swap-with-last (keeps arrays dense)
  void erase_swap_pop(size_type i) {
    const size_type n = size();
    if (i >= n) return;
    const size_type last = n - 1;
    if (i != last) {
      erase_swap_pop_impl(std::index_sequence_for<Fields...>{}, i, last);
    } else {
      // just pop
      (std::get<std::vector<typename Fields::value_type>>(cols_).pop_back(), ...); // NOTE: see remark below
    }
  }
};
*/
//The line:(std::get<std::vector<typename Fields::value_type>>(cols_).pop_back(), ...);
//only works if each column vector type is unique, which is not guaranteed (e.g., two float fields). 
//The rest of the class avoids this ambiguity by indexing with std::get<Is>.
//To make erase_swap_pop() correct in all cases, implement the else using indices too:

  void erase_swap_pop(size_type i) {
    const size_type n = size();
    if (i >= n) return;
    const size_type last = n - 1;

    if (i != last) {
      erase_swap_pop_impl(std::index_sequence_for<Fields...>{}, i, last);
    } else {
      std::apply([](auto&... vecs){ (vecs.pop_back(), ...); }, cols_);
    }
  }

  static constexpr int sanity_marker = 123;
};
}
