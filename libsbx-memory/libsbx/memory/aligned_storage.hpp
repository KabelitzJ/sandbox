#ifndef LIBSBX_MEMORY_ALIGNED_STORAGE_HPP_
#define LIBSBX_MEMORY_ALIGNED_STORAGE_HPP_

#include <memory>
#include <type_traits>

namespace sbx::memory {

template<typename Type>
class aligned_storage {

public:

  constexpr aligned_storage() noexcept = default;

  constexpr aligned_storage(const aligned_storage&) noexcept = default;

  constexpr aligned_storage(aligned_storage&&) noexcept = default;

  constexpr ~aligned_storage() noexcept = default;

  constexpr auto operator=(const aligned_storage&) noexcept -> aligned_storage& = default;

  constexpr auto operator=(aligned_storage&&) noexcept -> aligned_storage& = default;

  template<typename... Args>
  auto construct(Args&&... args) noexcept(std::is_nothrow_constructible_v<Type, Args...>) -> void {
    std::construct_at(_pointer(), std::forward<Args>(args)...);
  }

  auto destroy() noexcept(std::is_nothrow_destructible_v<Type>) -> void {
    std::destroy_at(_pointer());
  }

  

private:

  auto _pointer() noexcept -> Type* {
    return std::launder(reinterpret_cast<Type*>(_storage));
  }

  auto _pointer() const noexcept -> const Type* {
    return std::launder(reinterpret_cast<const Type*>(_storage));
  }

  alignas(alignof(Type)) std::byte _storage[sizeof(Type)];

}; // class aligned_storage

} // namespace sbx::memory

#endif // LIBSBX_MEMORY_ALIGNED_STORAGE_HPP_
