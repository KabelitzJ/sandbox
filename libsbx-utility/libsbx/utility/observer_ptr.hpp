#ifndef LIBSBX_UTILITY_observer_ptr_HPP_
#define LIBSBX_UTILITY_observer_ptr_HPP_

#include <memory>
#include <utility>

namespace sbx::utility {

template<typename Type>
class observer_ptr {

public:

  using value_type = Type;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using reference = value_type&;
  using const_reference = const value_type&;

  constexpr observer_ptr() noexcept = default;

  constexpr observer_ptr(std::nullptr_t) noexcept
  : _value{nullptr} { }

  constexpr observer_ptr(pointer value) noexcept
  : _value{value} { }

  template<typename Other>
  requires (std::is_convertible_v<Other*, pointer>)
  constexpr observer_ptr(observer_ptr<Other>& other) noexcept
  : _value{reinterpret_cast<pointer>(other.get())} { }

  constexpr observer_ptr(const observer_ptr&) noexcept = default;

  constexpr observer_ptr(observer_ptr&&) noexcept = default;

  constexpr ~observer_ptr() noexcept = default;

  constexpr auto operator=(const observer_ptr&) noexcept -> observer_ptr& = default;

  constexpr auto operator=(observer_ptr&&) noexcept -> observer_ptr& = default;

  constexpr auto operator=(std::nullptr_t) noexcept -> observer_ptr& {
    _value = nullptr;
    return *this;
  }

  constexpr auto operator=(pointer value) noexcept -> observer_ptr& {
    _value = value;
    return *this;
  }

  constexpr auto release() noexcept -> pointer {
    auto value = _value;
    _value = nullptr;
    return value;
  }

  constexpr auto reset(pointer value = nullptr) noexcept -> void {
    _value = value;
  }

  constexpr auto swap(observer_ptr& other) noexcept -> void {
    std::swap(_value, other._value);
  }

  constexpr operator bool() const noexcept {
    return _value != nullptr;
  }

  constexpr auto operator->() const noexcept -> const_pointer {
    return _value;
  }

  constexpr auto operator->() noexcept -> pointer {
    return _value;
  }

  constexpr auto operator*() const noexcept -> const_reference {
    return *_value;
  }

  constexpr auto operator*() noexcept -> reference {
    return *_value;
  }

  constexpr auto get() const noexcept -> const_pointer {
    return _value;
  }

  constexpr auto get() noexcept -> pointer {
    return _value;
  }

private:

  pointer _value{};

}; // class observer_ptr

template<typename Type>
constexpr auto operator==(const observer_ptr<Type>& lhs, const observer_ptr<Type>& rhs) noexcept -> bool {
  return lhs.get() == rhs.get();
}

template<typename Type>
constexpr auto make_observer(Type* value) noexcept -> observer_ptr<Type> {
  return observer_ptr<Type>{value};
}

template<typename Type>
constexpr auto make_observer(Type& value) noexcept -> observer_ptr<Type> {
  return observer_ptr<Type>{std::addressof(value)};
}

template<typename Type>
constexpr auto make_observer(std::unique_ptr<Type>& value) noexcept -> observer_ptr<Type> {
  return observer_ptr<Type>{value.get()};
}

template<typename Type>
constexpr auto make_observer(std::shared_ptr<Type>& value) noexcept -> observer_ptr<Type> {
  return observer_ptr<Type>{value.get()};
}

} // namespace sbx::utility

template<typename Type>
struct std::hash<sbx::utility::observer_ptr<Type>> {
  constexpr auto operator()(const sbx::utility::observer_ptr<Type>& value) const noexcept -> std::size_t {
    return std::hash<Type*>{}(value.get());
  }
};

#endif // LIBSBX_UTILITY_observer_ptr_HPP_
