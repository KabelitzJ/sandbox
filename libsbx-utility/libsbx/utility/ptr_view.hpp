#ifndef LIBSBX_UTILITY_PTR_VIEW_HPP_
#define LIBSBX_UTILITY_PTR_VIEW_HPP_

namespace sbx::utility {

template<typename Type>
class ptr_view {

public:

  using value_type = Type;
  using pointer = value_type*;
  using const_pointer = const pointer;
  using reference = value_type&;
  using const_reference = const reference;

  ptr_view() noexcept = default;

  ptr_view(std::nullptr_t) noexcept
  : _value{nullptr} {}

  ptr_view(pointer value) noexcept
  : _value{value} {}

  ptr_view(const ptr_view&) noexcept = default;

  ptr_view(ptr_view&&) noexcept = default;

  ~ptr_view() = default;

  auto operator=(const ptr_view&) noexcept -> ptr_view& = default;

  auto operator=(ptr_view&&) noexcept -> ptr_view& = default;

  auto operator=(std::nullptr_t) noexcept -> ptr_view& {
    _value = nullptr;
    return *this;
  }

  auto operator=(pointer value) noexcept -> ptr_view& {
    _value = value;
    return *this;
  }

  operator bool() const noexcept {
    return _value != nullptr;
  }

  auto operator->() const noexcept -> const_pointer {
    return _value;
  }

  auto operator->() noexcept -> pointer {
    return _value;
  }

  auto operator*() const noexcept -> const_reference {
    return *_value;
  }

  auto operator*() noexcept -> reference {
    return *_value;
  }

private:

  pointer _value{};

}; // class ptr_view

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_PTR_VIEW_HPP_
