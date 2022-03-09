#ifndef DEMO_BUFFER_HPP_
#define DEMO_BUFFER_HPP_

#include <cstddef>
#include <memory>
#include <algorithm>

#include <types/primitives.hpp>

namespace demo {

class type_less_container_base {

  using allocator_type = std::allocator<std::byte>;
  using allocator_traits = std::allocator_traits<allocator_type>;

  static inline constexpr auto growth_factor = std::size_t{2};

public:

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = std::byte*;
  using const_pointer = const std::byte*;

  type_less_container_base(const size_type element_size) noexcept
  : _element_size{element_size},
    _size{0},
    _capacity{0},
    _allocator{},
    _data{nullptr} { }

  type_less_container_base(const type_less_container_base& other) = delete;

  type_less_container_base(type_less_container_base&& other) noexcept
  : _element_size{std::exchange(other._element_size, 0)},
    _size{std::exchange(other._size, 0)},
    _capacity{std::exchange(other._capacity, 0)},
    _allocator{std::move(other._allocator)},
    _data{std::exchange(other._data, nullptr)} { }

  virtual ~type_less_container_base() {
    std::cout << "~type_less_container_base()" << std::endl;
    if (_data) {
      allocator_traits::deallocate(_allocator, _data, _capacity * _element_size);
    }
  }

  type_less_container_base& operator=(const type_less_container_base& other) = delete;

  type_less_container_base& operator=(type_less_container_base&& other) noexcept {
    _element_size = std::exchange(other._element_size, 0);
    _size = std::exchange(other._size, 0);
    _capacity = std::exchange(other._capacity, 0);
    _allocator = std::move(other._allocator);
    _data = std::exchange(other._data, nullptr);

    return *this;
  }

  size_type size() const noexcept {
    return _size;
  }

  size_type capacity() const noexcept {
    return _capacity;
  }

  bool empty() const noexcept {
    return _size == size_type{0};
  }

  void swap(const size_type lhs, const size_type rhs) {
    if (lhs >= _size || rhs >= _size) {
      throw std::out_of_range("index out of range");
    }

    const auto lhs_data = _at(lhs);
    const auto rhs_data = _at(rhs);

    std::swap_ranges(lhs_data, lhs_data + _element_size, rhs_data);
  }

  void clear() {
    _size = size_type{0};
  }

protected:

  std::byte* _at(const size_type index) const {
    if (index >= _size) {
      throw std::out_of_range("index out of range");
    }

    return _data + index * _element_size;
  }

  void _reserve(const size_type capacity) {
    if (capacity > _capacity) {
      const auto data = allocator_traits::allocate(_allocator, capacity * _element_size);
      std::fill_n(data + _size * _element_size, (capacity - _size) * _element_size, std::byte{0x00});

      if (_data) {
        std::copy_n(_data, _size * _element_size, data);
        allocator_traits::deallocate(_allocator, _data, _capacity * _element_size);
        _data = nullptr;
      }

      _data = data;
      _capacity = capacity;
    }
  }

  void _push_back() {
    if (_size == _capacity) {
      const auto target_capacity = _capacity * growth_factor;
      const auto capacity = target_capacity ? target_capacity : 2;
      _reserve(capacity);
    }

    ++_size;
  }

  void _pop_back() {
    if (_size == 0) {
      throw std::out_of_range("index out of range");
    }

    --_size;
  }
  
  void _remove(const size_type index) {
    if (index >= _size) {
      throw std::out_of_range("index out of range");
    }

    const auto data = _at(index);
    std::copy_n(data + _element_size, (_size - index - 1) * _element_size, data);
    _pop_back();
  }

private:

  size_type _element_size{};
  size_type _size{};
  size_type _capacity{};
  allocator_type _allocator{};
  std::byte* _data{};

};

} // namespace demo

#endif // DEMO_BUFFER_HPP_
