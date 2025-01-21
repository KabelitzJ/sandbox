#ifndef LIBSBX_MEMORY_ALIGNED_STORAGE_HPP_
#define LIBSBX_MEMORY_ALIGNED_STORAGE_HPP_

#include <memory>
#include <type_traits>
#include <cinttypes>

namespace sbx::memory {

template<std::size_t Size, std::size_t Alignment>
struct aligned_storage {
  struct type {
    alignas(Alignment) std::byte data[Size];
  }; // union type
}; // struct aligned_storage

template<std::size_t Size, std::size_t Alignment>
using aligned_storage_t = typename aligned_storage<Size, Alignment>::type;

template<typename Type>
struct storage_for {
  using type = alignas(alignof(Type)) std::byte[sizeof(Type)];
}; // struct storage_for

template<typename Type>
using storage_for_t = typename storage_for<Type>::type;

} // namespace sbx::memory

#endif // LIBSBX_MEMORY_ALIGNED_STORAGE_HPP_
