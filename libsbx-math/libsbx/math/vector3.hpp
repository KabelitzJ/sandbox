#ifndef LIBSBX_MATH_VECTOR3_HPP_
#define LIBSBX_MATH_VECTOR3_HPP_

#include <concepts>
#include <cstddef>
#include <cmath>
#include <fstream>
#include <ostream>
#include <type_traits>

#include <yaml-cpp/yaml.h>

#include <fmt/format.h>

#include <libsbx/math/concepts.hpp>
#include <libsbx/math/vector.hpp>
#include <libsbx/math/vector2.hpp>
#include <libsbx/math/angle.hpp>

namespace sbx::math {

template<arithmetic Type>
class basic_vector4;

/**
 * @brief A vector in three-dimensional space.
 *
 * @tparam Type The type of the vectors components.
 */
template<arithmetic Type>
class basic_vector3 : public basic_vector<3u, Type> {

  using base_type = basic_vector<3u, Type>;

public:

  // -- Type aliases --

  /** @brief The type of the vector components. */
  using value_type = base_type::value_type;

  /** @brief The value_type& type of the vector components. */
  using reference = base_type::reference;

  /** @brief The const value_type& type of the vector components. */
  using const_reference = base_type::const_reference;

  /** @brief The pointer type of the vector components. */
  using pointer = base_type::pointer;

  /** @brief The const pointer type of the vector components. */
  using const_pointer = base_type::const_pointer;

  /** @brief The type that can describe the length of the vector */
  using length_type = base_type::length_type;

  /** @brief The type that can describe the index of the vector components. */
  using size_type = base_type::size_type;

  // -- Constants --

  inline static constexpr auto x_axis = size_type{0};

  inline static constexpr auto y_axis = size_type{1};

  inline static constexpr auto z_axis = size_type{2};

  // -- Constructors --

  /** @brief Inherit the constructors from the base class. */
  using base_type::base_type;

  /**
   * @brief Constructs a vector from the base class.
   * 
   * @param base The base class to construct from. 
   */
  constexpr basic_vector3(const base_type& base) noexcept
  : base_type{base} { }

  /**
   * @brief Copy constructs a vector from another vector.
   */
  constexpr basic_vector3(const basic_vector3& other) noexcept
  : base_type{other} { }

  /**
   * @brief Constructs a three dimensional vector from a two dimensional vector.
   * 
   * @param vector A vector to copy the components from.
   * @param z The value for the z component. (Default: 1)
   */
  explicit constexpr basic_vector3(const basic_vector2<value_type>& vector, const value_type z = value_type{0}) noexcept
  : base_type{vector.x(), vector.y(), z} { }

  explicit constexpr basic_vector3(const basic_vector4<value_type>& vector) noexcept
  : base_type{vector.x(), vector.y(), vector.z()} { }

  /** @brief Destroys the vector */
  constexpr ~basic_vector3() noexcept override = default;

  // -- Assignment operators --

  constexpr auto operator=(const basic_vector3& other) noexcept -> basic_vector3& {
    base_type::operator=(other);
    return *this;
  }

  // -- Static member functions --

  [[nodiscard]] static constexpr auto right() -> basic_vector3 {
    return basic_vector3{base_type::axis(x_axis)};
  }

  [[nodiscard]] static constexpr auto left() -> basic_vector3 {
    return basic_vector3{-base_type::axis(x_axis)};
  }

  [[nodiscard]] static constexpr auto up() -> basic_vector3 {
    return basic_vector3{base_type::axis(y_axis)};
  }

  [[nodiscard]] static constexpr auto down() -> basic_vector3 {
    return basic_vector3{-base_type::axis(y_axis)};
  }

  [[nodiscard]] static constexpr auto forward() -> basic_vector3 {
    return basic_vector3{-base_type::axis(z_axis)};
  }

  [[nodiscard]] static constexpr auto backward() -> basic_vector3 {
    return basic_vector3{base_type::axis(z_axis)};
  }

  [[nodiscard]] static constexpr auto zero() -> basic_vector3 {
    return basic_vector3{base_type::zero()};
  }

  [[nodiscard]] static constexpr auto one() -> basic_vector3 {
    return basic_vector3{base_type::one()};
  }

  [[nodiscard]] static constexpr auto normalized(const basic_vector3& vector) noexcept -> basic_vector3 {
    return basic_vector3{base_type::normalized(vector)};
  }

  [[nodiscard]] static constexpr auto absolute(const basic_vector3& vector) noexcept -> basic_vector3; 

  /**
   * @brief Returns the dot product of two vectors.
   * 
   * @param lhs The left hand side vector.
   * @param rhs The right hand side vector.
   * 
   * @return value_type The dot product.
   */
  [[nodiscard]] static constexpr auto dot(const basic_vector3& lhs, const basic_vector3& rhs) noexcept -> value_type;

  /**
   * @brief Returns the cross product of two vectors.
   * 
   * @param lhs The left hand side vector.
   * @param rhs The right hand side vector.
   * 
   * @return basic_vector3 The cross product.
   */
  [[nodiscard]] static constexpr auto cross(const basic_vector3& lhs, const basic_vector3& rhs) noexcept -> basic_vector3;

  /**
   * @brief Returns a vector which is the result of linear interpolation between two vectors by a given scale. 
   * 
   * @tparam Scale The type of the scale.
   * 
   * @param lhs The left hand side vector.
   * @param rhs The right hand side vector.
   * @param scale The scale to interpolate by. Must be in the range [0.0, 1.0].
   * 
   * @return basic_vector3 The interpolated vector.
   */
  // template<std::convertible_to<value_type> Scale>
  // [[nodiscard]] static constexpr auto lerp(const basic_vector3& lhs, const basic_vector3& rhs, const Scale& scale) noexcept -> basic_vector3;


  [[nodiscard]] static constexpr auto distance_squared(const basic_vector3& lhs, const basic_vector3& rhs) noexcept -> length_type;

  [[nodiscard]] static constexpr auto distance(const basic_vector3& lhs, const basic_vector3& rhs) noexcept -> length_type;

  // -- Member functions --

  [[nodiscard]] constexpr auto x() const noexcept -> const_reference;

  [[nodiscard]] constexpr auto x() noexcept -> reference;

  [[nodiscard]] constexpr auto y() const noexcept -> const_reference;

  [[nodiscard]] constexpr auto y() noexcept -> reference;

  [[nodiscard]] constexpr auto z() const noexcept -> const_reference;

  [[nodiscard]] constexpr auto z() noexcept -> reference;

  [[nodiscard]] constexpr auto xy() const noexcept -> basic_vector2<value_type>;

}; // class vector3

template<arithmetic Type>
auto operator<<(std::ostream& output_stream, const basic_vector3<Type>& vector) -> std::ostream&;

// -- Type aliases --

/** @brief Type alias for a three dimensional vector with 32 bit floating-point components. */
using vector3f = basic_vector3<std::float_t>;

/** @brief Type alias for a three dimensional vector with 32 bit signed integer components. */
using vector3i = basic_vector3<std::int32_t>;

/** @brief Type alias for a three dimensional vector with 32 bit unsigned integer components. */
using vector3u = basic_vector3<std::uint32_t>;

/** @brief Type alias for vector3f. */
using vector3 = vector3f;

} // namespace sbx::math

/**
 * @brief Specialization of std::hash for sbx::math::basic_vector3.
 * @tparam Type The type of the vectors components.
 */
template<sbx::math::arithmetic Type>
struct std::hash<sbx::math::basic_vector3<Type>> {
  auto operator()(const sbx::math::basic_vector3<Type>& vector) const noexcept -> std::size_t;
}; // struct std::hash

template<sbx::math::arithmetic Type>
struct YAML::convert<sbx::math::basic_vector3<Type>> {
  static auto encode(const sbx::math::basic_vector3<Type>& vector) -> Node;
  static auto decode(const Node& node, sbx::math::basic_vector3<Type>& vector) -> bool;
}; // struct YAML::convert

template<sbx::math::arithmetic Type>
struct fmt::formatter<sbx::math::basic_vector3<Type>> : formatter<std::string_view> {

  using underlying_formatter_type = formatter<std::string_view>;

  template<typename ParseContext>
  constexpr auto parse(ParseContext& context) -> decltype(context.begin()) {
    return underlying_formatter_type::parse(context);
  }

  template<typename FormatContext>
  auto format(const sbx::math::basic_vector3<Type>& vector, FormatContext& context) -> decltype(context.out()) {
    return underlying_formatter_type::format(fmt::format("({:.2f}, {:.2f}, {:.2f})", vector.x(), vector.y(), vector.z()), context);
  }

}; // struct fmt::formatter

#include <libsbx/math/vector3.ipp>

#endif // LIBSBX_MATH_VECTOR3_HPP_

