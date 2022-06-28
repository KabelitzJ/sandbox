#ifndef SBX_MEMORY_ALIGNED_STORAGE_HPP_
#define SBX_MEMORY_ALIGNED_STORAGE_HPP_

#include <memory>
#include <type_traits>

#include <meta/concepts.hpp>

namespace sbx {

template<standard_layout Type>
class aligned_storage {

public:

  using value_type = Type;
  using size_type = std::size_t;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using void_pointer = void*;
  using const_void_pointer = const void*;

  aligned_storage() = default;

  ~aligned_storage() = default;

  pointer data() noexcept {
    return static_cast<pointer>(static_cast<void_pointer>(_data));
  }

  const_pointer data() const noexcept {
    return static_cast<const_pointer>(static_cast<const_void_pointer>(_data));
  }

private:

  alignas(alignof(value_type)) std::byte _data[sizeof(value_type)];

};

} // namespace sbx

#endif // SBX_MEMORY_ALIGNED_STORAGE_HPP_
