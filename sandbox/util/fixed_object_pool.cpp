#include "fixed_object_pool.hpp"

#include <algorithm>

namespace sbx {

fixed_object_pool::fixed_object_pool(std::size_t size, std::size_t element_size)
: _element_size(element_size),
  _buffer(new std::uint8_t[element_size * size]) {
  std::fill_n(_buffer, element_size * size, 0);
}

fixed_object_pool::~fixed_object_pool() {
  delete[] _buffer;
  _buffer = nullptr;
}

template<typename T, typename... Args>
T* fixed_object_pool::construct(std::size_t index, Args&&... args) {
  const auto element = get<T>(index);

  if (element != nullptr) {
    return element;
  }

  return new(element) T{std::forward<Args>(args)...};
}

template<typename T>
void fixed_object_pool::destroy(std::size_t index) {
  const auto element = get<T>(index);

  if (element == nullptr) {
    return;
  }

  element->~T();

  std::fill_n(_buffer + index * _element_size , _element_size, 0);
}

template<typename T>
T* fixed_object_pool::get(std::size_t index) {
  return static_cast<T*>(static_cast<void*>(_buffer + index * _element_size));
}

} // namespace sbx
