#ifndef LIBSBX_MATH_VECTOR2_HPP_
#define LIBSBX_MATH_VECTOR2_HPP_

#include <cstddef>
#include <cinttypes>
#include <cmath>
#include <concepts>
#include <fstream>
#include <ostream>
#include <type_traits>

#include <yaml-cpp/yaml.h>

#include <fmt/format.h>

#include <libsbx/math/concepts.hpp>
#include <libsbx/math/vector.hpp>

namespace sbx::math {

/**
 * @brief A vector in two-dimensional space.
 *
 * @tparam Type The type of the vectors components.
 */
template<arithmetic Type>
class basic_vector2 : public basic_vector<2u, Type> {

  using base_type = basic_vector<2u, Type>;

public:

  // -- Type aliases --

  /** @brief The type of the vector components. */
  using value_type = base_type::value_type;

  /** @brief The reference type of the vector components. */
  using reference = base_type::reference;

  /** @brief The const reference type of the vector components. */
  using const_reference = base_type::const_reference;

  /** @brief The pointer type of the vector components. */
  using pointer = base_type::pointer;

  /** @brief The const pointer type of the vector components. */
  using const_pointer = base_type::const_pointer;

  /** @brief The type that can describe the length of the vector */
  using length_type = base_type::length_type;

  /** @brief The type that can index components */
  using size_type = base_type::size_type;

  // -- Constants --

  inline static constexpr auto x_axis = size_type{0};

  inline static constexpr auto y_axis = size_type{1};

  // -- Constructors --

  using base_type::base_type;

  constexpr basic_vector2(const base_type& base) noexcept
  : base_type{base} { }

  constexpr basic_vector2(const basic_vector2& other) noexcept
  : base_type{other} { }

  /** @brief Destroys the vector */
  constexpr ~basic_vector2() noexcept override = default;

  constexpr auto operator=(const basic_vector2& other) noexcept -> basic_vector2& {
    base_type::operator=(other);
    return *this;
  }

  // -- Member functions --

  [[nodiscard]] constexpr auto x() const noexcept -> const_reference;

  [[nodiscard]] constexpr auto x() noexcept -> reference;

  [[nodiscard]] constexpr auto y() const noexcept -> const_reference;

  [[nodiscard]] constexpr auto y() noexcept -> reference;

}; // class basic_vector2

// -- Free comparison operators --

/**
 * @brief Compares two vectors for equality.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side vector.
 * 
 * @return true The vectors are equal.
 * @return false The vectors are not equal.
 */
template<arithmetic Type>
[[nodiscard]] constexpr auto operator==(const basic_vector2<Type>& lhs, const basic_vector2<Type>& rhs) noexcept -> bool;

// -- Free arithmetic operators --

/**
 * @brief Adds two vectors.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side vector.
 * 
 * @return basic_vector2<Type> The sum of the two vectors.
 */
template<arithmetic Type>
[[nodiscard]] constexpr auto operator+(basic_vector2<Type> lhs, const basic_vector2<Type>& rhs) noexcept -> basic_vector2<Type>;

template<arithmetic Type, arithmetic Other>
[[nodiscard]] constexpr auto operator+(basic_vector2<Type> lhs, const basic_vector2<Other>& rhs) noexcept -> basic_vector2<Type>;


/**
 * @brief Subtracts two vectors.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side vector.
 * 
 * @return basic_vector2<Type> The difference of the two vectors. 
 */
template<arithmetic Type>
[[nodiscard]] constexpr auto operator-(basic_vector2<Type> lhs, const basic_vector2<Type>& rhs) noexcept -> basic_vector2<Type>;

/**
 * @brief Multiplies a vector by a scalar. 
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side scalar.
 * 
 * @return basic_vector2<Type> The product of the vector and scalar. 
 */
template<arithmetic Type>
[[nodiscard]] constexpr auto operator*(basic_vector2<Type> lhs, const Type rhs) noexcept -> basic_vector2<Type>;

template<arithmetic Type, arithmetic Other>
[[nodiscard]] constexpr auto operator*(basic_vector2<Type> lhs, const basic_vector2<Other>& rhs) noexcept -> basic_vector2<Type>;

/**
 * @brief Divides a vector by a scalar.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side scalar.
 * 
 * @throws std::domain_error If the scalar is zero.
 * 
 * @return basic_vector2<Type> The quotient of the vector and scalar.
 */
template<arithmetic Type>
[[nodiscard]] constexpr auto operator/(basic_vector2<Type> lhs, const Type rhs) -> basic_vector2<Type>;

// -- Type aliases --

/** @brief Type alias for a two-dimensional vector with 32 bit floating-point components. */
using vector2f = basic_vector2<std::float_t>;

/** @brief Type alias for a two-dimensional vector with 32 bit integer components. */
using vector2i = basic_vector2<std::int32_t>;

using vector2u = basic_vector2<std::uint32_t>;

/** @brief Type alias for vector2f. */
using vector2 = vector2f;

} // namespace ::math

template<sbx::math::arithmetic Type>
struct std::hash<sbx::math::basic_vector2<Type>> {
  auto operator()(const sbx::math::basic_vector2<Type>& vector) const noexcept -> std::size_t;
}; // struct std::hash

template<sbx::math::arithmetic Type>
struct YAML::convert<sbx::math::basic_vector2<Type>> {
  static auto encode(const sbx::math::basic_vector2<Type>& vector) -> Node;

  static auto decode(const Node& node, sbx::math::basic_vector2<Type>& vector) -> bool;
}; // struct YAML::convert

template<sbx::math::arithmetic Type>
struct fmt::formatter<sbx::math::basic_vector2<Type>> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext& context) -> decltype(context.begin());

  template<typename FormatContext>
  auto format(const sbx::math::basic_vector2<Type>& vector, FormatContext& context) -> decltype(context.out());
}; // struct fmt::formatter

#include <libsbx/math/vector2.ipp>

#endif // LIBSBX_MATH_VECTOR2_HPP_
