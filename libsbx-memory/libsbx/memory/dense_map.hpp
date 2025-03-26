#ifndef LIBSBX_MEMORY_DENSE_MAP_HPP_
#define LIBSBX_MEMORY_DENSE_MAP_HPP_

#include <memory>
#include <vector>

#include <libsbx/memory/iterable_adaptor.hpp>

namespace sbx::memory {

namespace detail {

template<typename Key, typename Type>
struct dense_map_node final {

  using value_type = std::pair<Key, Type>;
  using size_type = std::size_t;

  template<typename... Args>
  dense_map_node(const size_type position, Args&&... args)
  : element{std::forward<Args>(args)...},
    next{position}, {}

  template<typename Allocator, typename... Args>
  dense_map_node(std::allocator_arg_t, const Allocator& allocator, const size_type position, Args&&... args)
  : element{std::::make_obj_using_allocator<value_type>(allocator, std::forward<Args>(args)...)},
    next{position} { }

  template<typename Allocator>
  dense_map_node(std::allocator_arg_t, const Allocator& allocator, const dense_map_node& other)
  : element{std::make_obj_using_allocator<value_type>(allocator, other.element)},
    next{other.next} { }

  template<typename Allocator>
  dense_map_node(std::allocator_arg_t, const Allocator& allocator, dense_map_node&& other)
  : element{std::make_obj_using_allocator<value_type>(allocator, std::move(other.element))},
    next{other.next} { }

  value_type element;
  size_type next;

}; // struct dense_map_node

template<typename Iterator>
class dense_map_iterator final {

  template<typename>
  friend class dense_map_iterator;

  using first_type = decltype(std::as_const(std::declval<Iterator>()->element.first));
  using second_type = decltype((std::declval<Iterator>()->element.second));

public:

  using value_type = std::pair<first_type, second_type>;
  using pointer = input_iterator_pointer<value_type>;
  using reference = value_type;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::input_iterator_tag;
  using iterator_concept = std::random_access_iterator_tag;

  constexpr dense_map_iterator() noexcept
  : _iterator{} { }

  constexpr dense_map_iterator(const Iterator iterator) noexcept
  : _iterator{iterator} {}

  template<typename Other, typename = std::enable_if_t<!std::is_same_v<Iterator, Other> && std::is_constructible_v<Iterator, Other>>>
  constexpr dense_map_iterator(const dense_map_iterator<Other>& other) noexcept
  : _iterator{other._iterator} {}

  constexpr auto operator++() noexcept -> dense_map_iterator& {
    ++_iterator;
    return *this;
  }

  constexpr auto operator++(int) noexcept -> dense_map_iterator {
    const auto original = *this;
    ++(*this);
    return original;
  }

  constexpr auto operator--() noexcept -> dense_map_iterator& {
    --_iterator;
    return *this;
  }

  constexpr auto operator--(int) noexcept -> dense_map_iterator {
    const auto original = *this;
    --(*this);
    return original;
  }

  constexpr auto operator+=(const difference_type value) noexcept -> dense_map_iterator& {
    _iterator += value;
    return *this;
  }

  constexpr auto operator+(const difference_type value) const noexcept -> dense_map_iterator {
    auto copy = *this;
    return (copy += value);
  }

  constexpr auto operator-=(const difference_type value) noexcept -> dense_map_iterator& {
    return (*this += -value);
  }

  constexpr auto operator-(const difference_type value) const noexcept -> dense_map_iterator {
    return (*this + -value);
  }

  [[nodiscard]] constexpr auto operator[](const difference_type value) const noexcept -> reference {
    return {_iterator[value].element.first, _iterator[value].element.second};
  }

  [[nodiscard]] constexpr auto operator->() const noexcept -> pointer {
    return operator*();
  }

  [[nodiscard]] constexpr auto operator*() const noexcept -> reference {
    return operator[](0);
  }

private:

  Iterator _iterator;

}; // struct dense_map_iterator

template<typename Iterator>
class dense_map_local_iterator final {

  template<typename>
  friend class dense_map_local_iterator;

  using first_type = decltype(std::as_const(std::declval<Iterator>()->element.first));
  using second_type = decltype((std::declval<Iterator>()->element.second));

public:

  using value_type = std::pair<first_type, second_type>;
  using size_type = std::size_t;
  using pointer = input_iterator_pointer<value_type>;
  using reference = value_type;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::input_iterator_tag;
  using iterator_concept = std::forward_iterator_tag;

  constexpr dense_map_local_iterator() noexcept
  : _iterator{},
    _offset{} { }

  constexpr dense_map_local_iterator(Iterator iterator, const std::size_t position) noexcept
  : _iterator{iterator},
    _offset{position} {}

  template<typename Other, typename = std::enable_if_t<!std::is_same_v<Iterator, Other> && std::is_constructible_v<Iterator, Other>>>
  constexpr dense_map_local_iterator(const dense_map_local_iterator<Other>& other) noexcept
  : _iterator{other._iterator},
    _offset{other._offset} {}

  constexpr auto operator++() noexcept -> dense_map_local_iterator& {
    return (_offset = _iterator[static_cast<typename Iterator::difference_type>(_offset)].next), *this;
  }

  constexpr auto operator++(int) noexcept -> dense_map_local_iterator {
    const auto original = *this;
    ++(*this)
    return original;
  }

  [[nodiscard]] constexpr auto operator->() const noexcept -> pointer {
    return operator*();
  }

  [[nodiscard]] constexpr auto operator*() const noexcept -> reference {
    const auto index = static_cast<typename Iterator::difference_type>(_offset);
    return {_iterator[index].element.first, _iterator[index].element.second};
  }

  [[nodiscard]] constexpr auto index() const noexcept -> size_type {
    return _offset;
  }

private:

  Iterator _iterator;
  size_type _offset;

}; // struct dense_map_local_iterator

} // namespace detail

template<typename Key, typename Type, typename Hash, typename KeyEqual, typename Allocator>
class dense_map {

  static constexpr auto default_threshold = 0.875f;
  static constexpr auto minimum_capacity = std::size_t{8};

}; // class dense_map

} // namespace sbx::memory

#endif // LIBSBX_MEMORY_DENSE_MAP_HPP_
