#ifndef LIBSBX_MEMORY_ALIGNED_STORAGE_HPP_
#define LIBSBX_MEMORY_ALIGNED_STORAGE_HPP_

#include <memory>
#include <type_traits>

namespace sbx::memory {

template<std::size_t Size, std::size_t Alignment>
struct aligned_storage {
  struct type {
    alignas(Alignment) std::byte data[Size];
  }; // union type
}; // struct aligned_storage

template<std::size_t Size, std::size_t Alignment>
using aligned_storage_t = typename aligned_storage<Size, Alignment>::type;

} // namespace sbx::memory

#endif // LIBSBX_MEMORY_ALIGNED_STORAGE_HPP_
