#ifndef LIBSBX_UTILITY_ITERATOR_HPP_
#define LIBSBX_UTILITY_ITERATOR_HPP_

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <ranges>

#include <range/v3/all.hpp>

namespace sbx::utility {

template<typename Category, typename Type, typename Distance = std::ptrdiff_t, typename Pointer = Type*, typename Reference = Type&>
struct iterator {
  using iterator_category = Category;
  using value_type = Type;
  using difference_type = Distance;
  using pointer = Pointer;
  using reference = Reference;
}; // struct iterator

template<typename Type>
concept iterable = requires(Type t) {
  { std::begin(t) } -> std::same_as<typename Type::iterator>;
  { std::end(t) } -> std::same_as<typename Type::iterator>;
} || requires(Type t) {
  { std::begin(t) } -> std::same_as<typename Type::const_iterator>;
  { std::end(t) } -> std::same_as<typename Type::const_iterator>;
} || std::is_array_v<Type>;

template<template<typename> typename To, ranges::input_range Range, std::invocable<const ranges::range_value_t<Range>&> Fn>
requires (ranges::output_range<To<std::invoke_result_t<Fn, const ranges::range_value_t<Range>&>>, std::invoke_result_t<Fn, const ranges::range_value_t<Range>&>>)
auto map_to(Range&& range, Fn&& fn) -> To<std::invoke_result_t<Fn, const ranges::range_value_t<Range>&>> {
  return std::forward<Range>(range) | ranges::views::transform(std::forward<Fn>(fn)) | ranges::to<To>();
}

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_ITERATOR_HPP_
