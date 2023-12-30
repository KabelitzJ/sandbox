#ifndef LIBSBX_MATH_VECTOR_HPP_
#define LIBSBX_MATH_VECTOR_HPP_

#include <cinttypes>
#include <array>
#include <concepts>
#include <ranges>
#include <cmath>
#include <algorithm>
#include <span>

#include <libsbx/math/concepts.hpp>

namespace sbx::math {

template<std::size_t Size, arithmetic Type>
class basic_vector {

public:

  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using size_type = std::size_t;
  using length_type = std::float_t;

  template<std::convertible_to<value_type> Other = value_type>
  constexpr basic_vector(const Other value = static_cast<value_type>(0)) noexcept {
    std::ranges::fill(_components, static_cast<value_type>(value));
  }

  template<std::convertible_to<value_type>... Args>
  requires (sizeof...(Args) == Size)
  constexpr basic_vector(Args&&... args) noexcept
  : _components{static_cast<value_type>(std::forward<Args>(args))...} { }
  
  template<std::convertible_to<value_type> Other = value_type>
  constexpr basic_vector(const basic_vector<Size, Other>& other) noexcept {
    for (auto i : std::views::iota(0u, Size)) {
      _components[i] = static_cast<value_type>(other[i]);
    }
  }

  template<std::derived_from<basic_vector<Size, value_type>> Derived>
  constexpr basic_vector(const Derived& other) noexcept {
    for (auto i : std::views::iota(0u, Size)) {
      _components[i] = static_cast<value_type>(other[i]);
    }
  }

  virtual ~basic_vector() noexcept = default;

  template<std::convertible_to<value_type> Other = value_type>
  constexpr auto operator=(const basic_vector<Size, Other>& other) noexcept -> basic_vector& {
    if (this != &other) {
      for (auto i : std::views::iota(0u, Size)) {
        _components[i] = static_cast<value_type>(other[i]);
      }
    }

    return *this;
  }

  template<std::derived_from<basic_vector<Size, value_type>> Derived>
  constexpr auto operator=(const Derived& other) noexcept -> basic_vector& {
    if (this != &other) {
      for (auto i : std::views::iota(0u, Size)) {
        _components[i] = static_cast<value_type>(other[i]);
      }
    }

    return *this;
  }

  [[nodiscard]] static constexpr auto axis(const size_type index) noexcept -> basic_vector {
    auto result = basic_vector{};

    result[index] = static_cast<value_type>(1);

    return result;
  }

  [[nodiscard]] static constexpr auto zero() noexcept -> basic_vector {
    return basic_vector{};
  }

  [[nodiscard]] static constexpr auto one() noexcept -> basic_vector {
    return basic_vector{static_cast<value_type>(1)};
  }

  [[nodiscard]] static constexpr auto normalized(const basic_vector& vector) -> basic_vector {
    const auto length = vector.length();
  
    if (length != static_cast<value_type>(0)) {
      return vector / length;
    }

    return vector;
  }

  [[nodiscard]] static constexpr auto from_array(std::span<const value_type, Size> array) noexcept -> basic_vector {
    auto result = basic_vector{};

    std::ranges::copy(array, result._components.begin());

    return result;
  }

  [[nodiscard]] constexpr auto operator[](const size_type index) noexcept -> reference {
    return component(index);
  }

  [[nodiscard]] constexpr auto operator[](const size_type index) const noexcept -> const_reference {
    return component(index);
  }

  template<std::convertible_to<value_type> Other = value_type>
  constexpr auto operator+=(const basic_vector<Size, Other>& other) noexcept -> basic_vector& {
    for (auto i : std::views::iota(0u, Size)) {
      _components[i] += static_cast<value_type>(other[i]);
    }

    return *this;
  }

  template<std::convertible_to<value_type> Other = value_type>
  constexpr auto operator-=(const basic_vector<Size, Other>& other) noexcept -> basic_vector& {
    for (auto i : std::views::iota(0u, Size)) {
      _components[i] -= static_cast<value_type>(other[i]);
    }

    return *this;
  }

  template<std::convertible_to<value_type> Other = value_type>
  constexpr auto operator*=(const Other scalar) noexcept -> basic_vector& {
    for (auto i : std::views::iota(0u, Size)) {
      _components[i] *= static_cast<value_type>(scalar);
    }

    return *this;
  }

  template<std::convertible_to<value_type> Other = value_type>
  constexpr auto operator/=(const Other scalar) noexcept -> basic_vector& {
    for (auto i : std::views::iota(0u, Size)) {
      _components[i] /= static_cast<value_type>(scalar);
    }

    return *this;
  }

  [[nodiscard]] constexpr auto size() const noexcept -> size_type {
    return Size;
  }

  [[nodiscard]] constexpr auto length() const noexcept -> length_type {
    return std::sqrt(length_squared());
  }

  [[nodiscard]] constexpr auto length_squared() const noexcept -> length_type {
    auto result = length_type{0.0f};

    for (const auto& component : _components) {
      result += component * component;
    }

    return result;
  }

  constexpr auto normalize() noexcept -> basic_vector& {
    const auto length = this->length();

    if (length != static_cast<length_type>(0)) {
      *this /= length;
    }

    return *this;
  }

  constexpr auto clamp(const value_type min, const value_type max) noexcept -> basic_vector& {
    for (auto i : std::views::iota(0u, Size)) {
      _components[i] = std::clamp(_components[i], min, max);
    }

    return *this;
  }

  [[nodiscard]] constexpr auto data() noexcept -> pointer {
    return _components.data();
  }

  [[nodiscard]] constexpr auto data() const noexcept -> const_pointer {
    return _components.data();
  }

protected:

  [[nodiscard]] constexpr auto component(const size_type index) const noexcept -> const_reference {
    return _components[index];
  }

  [[nodiscard]] constexpr auto component(const size_type index) noexcept -> reference {
    return _components[index];
  }

private:

  std::array<value_type, Size> _components;

}; // class basic_vector

template<std::size_t Size, arithmetic Lhs, arithmetic Rhs>
constexpr auto operator==(const basic_vector<Size, Lhs>& lhs, const basic_vector<Size, Rhs>& rhs) noexcept -> bool {
  for (auto i : std::views::iota(0u, Size)) {
    if (lhs[i] != static_cast<Rhs>(rhs[i])) {
      return false;
    }
  }

  return true;
}

template<std::size_t Size, arithmetic Lhs, arithmetic Rhs>
constexpr auto operator+(basic_vector<Size, Lhs> lhs, const basic_vector<Size, Rhs>& rhs) noexcept -> basic_vector<Size, Lhs> {
  return lhs += rhs;
}

template<std::size_t Size, arithmetic Lhs, arithmetic Rhs>
constexpr auto operator-(basic_vector<Size, Lhs> lhs, const basic_vector<Size, Rhs>& rhs) noexcept -> basic_vector<Size, Lhs> {
  return lhs -= rhs;
}

template<std::size_t Size, arithmetic Type>
constexpr auto operator-(const basic_vector<Size, Type>& vector) noexcept -> basic_vector<Size, Type> {
  auto result = basic_vector<Size, Type>{};

  for (auto i : std::views::iota(0u, Size)) {
    result[i] = -vector[i];
  }

  return result;
}

template<std::size_t Size, arithmetic Type, std::convertible_to<Type> Other>
constexpr auto operator*(basic_vector<Size, Type> vector, const Other scalar) noexcept -> basic_vector<Size, Type> {
  return vector *= scalar;
}

template<std::size_t Size, arithmetic Type, std::convertible_to<Type> Other>
constexpr auto operator/(basic_vector<Size, Type> vector, const Other scalar) noexcept -> basic_vector<Size, Type> {
  return vector /= scalar;
}

} // namespace sbx::math

#endif // LIBSBX_MATH_VECTOR_HPP_
