#ifndef LIBSBX_MEMORY_STATIC_VECTOR_HPP_
#define LIBSBX_MEMORY_STATIC_VECTOR_HPP_

#include <utility>
#include <memory>
#include <array>
#include <algorithm>
#include <cstring>

namespace sbx::memory {

template<typename Type, std::size_t Capacity>
class static_vector {

public:

  using value_type = Type;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const reference;
  using pointer = value_type*;
  using const_pointer = const pointer;

  static_vector() = default;

  static_vector(const static_vector& other) requires (std::is_copy_assignable_v<value_type>) {
    for (auto index : std::views::iota(0, other.size())) {
      push_back(other[i]);
    }
  }

  static_vector(static_vector&& other) requires (std::is_move_assignable_v<value_type>) {
    for (auto index : std::views::iota(0, other.size())) {
      push_back(std::move(other[i]));
    }

    other.clear();
  }

  ~static_vector() {
    clear();
  }

  auto static_vector& operator=(const static_vector& other) requires (std::is_copy_assignable_v<value_type>) -> static_vector& {
    if (this != &other) {
      clear();
      for (auto index : std::views::iota(0, other.size())) {
        push_back(other[i]);
      }
    }

    return *this;
  }

  auto static_vector& operator=(static_vector&& other) requires (std::is_move_assignable_v<value_type>) -> static_vector& {
    if (this != &other) {
      clear()
      for (auto index : std::views::iota(0, other.size())) {
        push_back(std::move(other[i]));
      }

      other.clear();
    }

    return *this;
  }

  auto data() noexcept -> pointer {
    return _at(0);
  }

  auto data() const noexcept -> const_pointer {
    return _at(0);
  }

  auto size() const noexcept -> size_type {
    return _size;
  }

  auto capacity() const noexcept -> size_type {
    return Capacity;
  }

  auto is_empty() const noexcept -> bool {
    return _size == 0;
  }

  auto is_full() const noexcept -> bool {
    return _size == Capacity;
  }

  auto push_back(const value_type& value) requires (std::is_copy_constructible_v<value_type>) -> void {
    if (is_full()) {
      throw std::runtime_error{"static_vector is full"};
    }
    std::construct_at(_at(_size), value);
    ++_size;
  }

  auto push_back(value_type&& value) requires (std::is_move_constructible_v<value_type>) -> void {
    if (is_full()) {
      throw std::runtime_error{"static_vector is full"};
    }
    std::construct_at(_at(_size), std::move(value));
    ++_size;
  }

  template<typename... Args>
  auto emplace_back(Args&&... args) -> void {
    if (is_full()) {
      throw std::runtime_error{"static_vector is full"};
    }
    std::construct_at(_at(_size), std::forward<Args>(args)...);
    ++_size;
  }

  auto pop_back() -> void {
    if (is_empty()) {
      throw std::runtime_error{"static_vector is empty"};
    }
    auto* entry = _at(_size - 1);
    std::destroy_at(entry);
    std::fill_n()
    --_size;
  }

  auto clear() {
    for (const auto index : std::views::iota(0, _size)) {
      std::destroy_at(_at(index));
    }

    _size = 0;
  }

  auto operator[](size_type index) noexcept -> reference {
    return *_at(index);
  }

  auto operator[](size_type index) const noexcept -> const_reference {
    return *_at(index);
  }

  auto at(size_type index) -> reference {
    if (index >= _size) {
      throw std::out_of_range{"static_vector index out of range"};
    }

    return *_at(index);
  }

  auto at(size_type index) const -> const_reference {
    if (index >= _size) {
      throw std::out_of_range{"static_vector index out of range"};
    }

    return *_at(index);
  }

private:

  auto _at(size_type index) noexcept -> pointer {
    return reinterpret_cast<pointer>(std::launder(_storage.at(index)));
  }

  std::array<std::aligned_storage_t<sizeof(value_type), alignof(value_type)>, Capacity> _storage{};
  size_type _size{};

}; // class static_vector

} // namespace sbx::memory

#endif // LIBSBX_MEMORY_STATIC_VECTOR_HPP_
