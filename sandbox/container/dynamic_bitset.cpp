#include "dynamic_bitset.hpp"

#include <iostream>
#include <bitset>

namespace sbx {

dynamic_bitset::dynamic_bitset() noexcept
: _allocator{},
  _buffer{nullptr},
  _size{0} { }

dynamic_bitset::dynamic_bitset(const size_type size)
: _allocator{},
  _buffer{nullptr},
  _size{0} {
  _resize(size);
}

dynamic_bitset::dynamic_bitset(const dynamic_bitset& other)
: _allocator{other._allocator},
  _buffer{nullptr},
  _size{other._size} {
  if (_size != 0) {
    _buffer = allocator_traits::allocate(_allocator, _size);
    std::copy_n(other._buffer, _size, _buffer);
  }
}

dynamic_bitset::dynamic_bitset(const std::initializer_list<size_type> values) 
: _allocator{},
  _buffer{nullptr},
  _size{0} {
  for (auto value : values) {
    set(value);
  }
}

dynamic_bitset::dynamic_bitset(dynamic_bitset&& other) noexcept
: _allocator{std::move(other._allocator)},
  _buffer{other._buffer},
  _size{other._size} {
  other._buffer = nullptr;
  other._size = 0;
}

dynamic_bitset::~dynamic_bitset() {
  if (_buffer != nullptr) {
    allocator_traits::deallocate(_allocator, _buffer, _size);
    _buffer = nullptr;
  }
}

dynamic_bitset& dynamic_bitset::operator=(const dynamic_bitset& other) {
  if (this != &other) {
    if (_buffer != nullptr) {
      allocator_traits::deallocate(_allocator, _buffer, _size);
      _buffer = nullptr;
    }

    _size = other._size;

    if (_size != 0) {
      _buffer = allocator_traits::allocate(_allocator, _size);
      std::copy_n(other._buffer, _size, _buffer);
    }
  }

  return *this;
}

dynamic_bitset& dynamic_bitset::operator=(dynamic_bitset&& other) noexcept {
  if (this != &other) {
    if (_buffer != nullptr) {
      allocator_traits::deallocate(_allocator, _buffer, _size);
      _buffer = nullptr;
    }

    _allocator = std::move(other._allocator);
    _buffer = other._buffer;
    _size = other._size;

    other._buffer = nullptr;
    other._size = 0;
  }

  return *this;
}

void dynamic_bitset::set(const size_type index) {
  const auto internal_index = index / underlying_type_bit_count;

  if (internal_index >= _size) {
    _resize(internal_index + 1);
  }

  const auto byte_index = _size - 1 - internal_index;
  const auto bit_index = index % underlying_type_bit_count;

  _buffer[byte_index] |= static_cast<underlying_type>(std::size_t{1} << bit_index); 
}

void dynamic_bitset::clear(const size_type index) {
  const auto internal_index = index / underlying_type_bit_count;

  if (internal_index >= _size) {
    return;
  }

  const auto byte_index = _size - 1 - internal_index;
  const auto bit_index = index % underlying_type_bit_count;

  _buffer[byte_index] &= static_cast<underlying_type>(~(std::size_t{1} << bit_index));

  auto bytes_to_remove = size_type{0};

  for (auto i = size_type{0}; i < _size; ++i) {
    if (_buffer[i] == 0) {
      ++bytes_to_remove;
    } else {
      break;
    }
  }

  const auto size = _size - bytes_to_remove;

  if (size != _size) {
    _resize(size);
  }
}

void dynamic_bitset::reset() {
  _resize(size_type{0});
}

bool dynamic_bitset::test(const size_type index) const noexcept {
  const auto internal_index = index / underlying_type_bit_count;

  if (internal_index >= _size) {
    return false;
  }

  const auto byte_index = _size - 1 - internal_index;
  const auto bit_index = index % underlying_type_bit_count;

  return _buffer[byte_index] & static_cast<underlying_type>(std::size_t{1} << bit_index);
}

bool dynamic_bitset::test(const dynamic_bitset& other) const noexcept {
  if (other._size > _size) {
    return false;
  }

  for (auto i = size_type{0}; i < other._size; ++i) {
    if ((_buffer[i] & other._buffer[i]) != other._buffer[i]) {
      return false;
    }
  }

  return true;
}

void dynamic_bitset::_resize(const size_type size) {
  if (size == 0 && _buffer) {
    allocator_traits::deallocate(_allocator, _buffer, _size);
    _buffer = nullptr;
    _size = 0;
  } else if (size > _size) {
    const auto buffer = allocator_traits::allocate(_allocator, size);

    if (_buffer) {
      std::copy(_buffer, _buffer + _size, buffer + size - _size);
      allocator_traits::deallocate(_allocator, _buffer, _size);
    }

    std::fill(buffer, buffer + size - _size, underlying_type{0});

    _buffer = buffer;
    _size = size;
  } else if (size < _size) {
    const auto buffer = allocator_traits::allocate(_allocator, size);

    std::copy(_buffer + _size - size, _buffer + _size, buffer);

    allocator_traits::deallocate(_allocator, _buffer, _size);

    _buffer = buffer;
    _size = size;
  }
}

bool operator==(const dynamic_bitset& lhs, const dynamic_bitset& rhs) noexcept {
  if (lhs._size != rhs._size) {
    return false;
  }

  for (auto i = dynamic_bitset::size_type{0}; i < lhs._size; ++i) {
    if (lhs._buffer[i] != rhs._buffer[i]) {
      return false;
    }
  }

  return true;
}

} // namespace sbx
