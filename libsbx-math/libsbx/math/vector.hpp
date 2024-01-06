#ifndef LIBSBX_MATH_VECTOR_HPP_
#define LIBSBX_MATH_VECTOR_HPP_

#include <array>
#include <cmath>
#include <ranges>

#include <libsbx/utility/make_array.hpp>
#include <libsbx/utility/zip.hpp>
#include <libsbx/utility/hash.hpp>

#include <libsbx/math/concepts.hpp>

namespace sbx::math {

template<std::size_t Size, scalar Type>
class basic_vector {

  template<std::size_t S, scalar Lhs, scalar Rhs>
  friend constexpr auto operator==(const basic_vector<S, Lhs>& lhs, const basic_vector<S, Rhs>& rhs) noexcept -> bool;

public:

  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using size_type = std::size_t;
  using length_type = std::float_t;

  template<scalar Other = value_type>
  constexpr basic_vector(Other value = Other{0}) noexcept
  : _components{utility::make_array<value_type, Size>(value)} { }

  template<scalar Other>
  constexpr auto operator+=(const basic_vector<Size, Other>& other) noexcept -> basic_vector& {
    for (auto i : std::views::iota(0u, Size)) {
      _components[i] += static_cast<value_type>(other[i]);
    }

    return *this;
  }

  template<scalar Other>
  constexpr auto operator-=(const basic_vector<Size, Other>& other) noexcept -> basic_vector& {
    for (auto i : std::views::iota(0u, Size)) {
      _components[i] -= static_cast<value_type>(other[i]);
    }

    return *this;
  }

  constexpr auto operator-() noexcept -> basic_vector& {
    for (auto i : std::views::iota(0u, Size)) {
      _components[i] = -_components[i];
    }

    return *this;
  }

  template<scalar Other>
  constexpr auto operator*=(Other scalar) noexcept -> basic_vector& {
    for (auto i : std::views::iota(0u, Size)) {
      _components[i] *= static_cast<value_type>(scalar);
    }

    return *this;
  }

  template<scalar Other>
  constexpr auto operator/=(Other scalar) noexcept -> basic_vector& {
    for (auto i : std::views::iota(0u, Size)) {
      _components[i] /= static_cast<value_type>(scalar);
    }

    return *this;
  }

  constexpr auto length_squared() const noexcept -> length_type {
    auto result = length_type{0};

    for (auto i : std::views::iota(0u, Size)) {
      result += static_cast<length_type>(_components[i] * _components[i]);
    }

    return result;
  }

  constexpr auto length() const noexcept -> length_type {
    return std::sqrt(length_squared());
  }

  constexpr auto normalize() noexcept -> basic_vector& {
    auto length_squared = this->length_squared();

    if (!comparision_traits<length_type>::equal(length_squared, length_type{0})) {
      *this /= std::sqrt(length_squared);
    }

    return *this;
  }

protected:

  template<std::convertible_to<value_type>... Args>
  requires (sizeof...(Args) == Size)
  constexpr basic_vector(Args&&... args) noexcept
  : _components{utility::make_array<value_type, Size>(std::forward<Args>(args)...)} { }

  [[nodiscard]] constexpr auto operator[](size_type index) noexcept -> reference {
    return _components[index];
  }

  [[nodiscard]] constexpr auto operator[](size_type index) const noexcept -> const_reference {
    return _components[index];
  }

private:

  std::array<Type, Size> _components;

}; // class basic_vector

template<std::size_t Size, scalar Lhs, scalar Rhs>
constexpr auto operator==(const basic_vector<Size, Lhs>& lhs, const basic_vector<Size, Rhs>& rhs) noexcept -> bool {
  for (auto i : std::views::iota(0u, Size)) {
    if (!comparision_traits<Lhs>::equal(lhs[i], rhs[i])) {
      return false;
    }
  }

  return true;
}

template<std::size_t Size, scalar Lhs, scalar Rhs>
constexpr auto operator+(basic_vector<Size, Lhs> lhs, const basic_vector<Size, Rhs>& rhs) noexcept -> basic_vector<Size, Lhs> {
  return lhs += rhs;
}

template<std::size_t Size, scalar Lhs, scalar Rhs>
constexpr auto operator-(basic_vector<Size, Lhs> lhs, const basic_vector<Size, Rhs>& rhs) noexcept -> basic_vector<Size, Lhs> {
  return lhs -= rhs;
}

template<std::size_t Size, scalar Lhs, scalar Rhs>
constexpr auto operator*(basic_vector<Size, Lhs> lhs, Rhs scalar) noexcept -> basic_vector<Size, Lhs> {
  return lhs *= scalar;
}

template<std::size_t Size, scalar Lhs, scalar Rhs>
constexpr auto operator/(basic_vector<Size, Lhs> lhs, Rhs scalar) noexcept -> basic_vector<Size, Lhs> {
  return lhs /= scalar;
}

} // namespace sbx::math

#endif // LIBSBX_MATH_VECTOR_HPP_
