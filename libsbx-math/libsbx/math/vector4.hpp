#ifndef LIBSBX_MATH_VECTOR4_HPP_
#define LIBSBX_MATH_VECTOR4_HPP_

#include <cstddef>
#include <cmath>
#include <cstdint>
#include <concepts>
#include <fstream>
#include <ostream>
#include <type_traits>

#include <yaml-cpp/yaml.h>

#include <libsbx/math/concepts.hpp>
#include <libsbx/math/vector.hpp>
#include <libsbx/math/vector3.hpp>

namespace sbx::math {

template<arithmetic Type>
class basic_vector4 : public basic_vector<4u, Type> {

  using base_type = basic_vector<4u, Type>;

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

  inline static constexpr auto z_axis = size_type{2};

  inline static constexpr auto w_axis = size_type{3};

  // -- Constructors --

  using base_type::base_type;

  /**
   * @brief Constructs a vector from the base class.
   * 
   * @param base The base class to construct from. 
   */
  constexpr basic_vector4(const base_type& base) noexcept
  : base_type{base} { }

  /**
   * @brief Copy constructs a vector from another vector.
   */
  constexpr basic_vector4(const basic_vector4& other) noexcept
  : base_type{other} { }

  /**
   * @brief Uses a three dimensional vector and a w-component to construct a four dimensional vector.
   * 
   * @param vector Three dimensional vector.
   * @param w The value for the w component. (Default: 0)
   */
  explicit constexpr basic_vector4(const basic_vector3<value_type>& vector, const value_type w = value_type{0}) noexcept
  : base_type{vector.x(), vector.y(), vector.z(), w} { }

  /** @brief Destroys the vector */
  constexpr ~basic_vector4() noexcept override = default;

  constexpr auto operator=(const basic_vector4& other) noexcept -> basic_vector4& {
    base_type::operator=(other);
    return *this;
  }

  // -- Static member functions --

  // -- Member functions --

  [[nodiscard]] constexpr auto x() const noexcept -> const_reference;

  [[nodiscard]] constexpr auto x() noexcept -> reference;

  [[nodiscard]] constexpr auto y() const noexcept -> const_reference;

  [[nodiscard]] constexpr auto y() noexcept -> reference;

  [[nodiscard]] constexpr auto z() const noexcept -> const_reference;

  [[nodiscard]] constexpr auto z() noexcept -> reference;

  [[nodiscard]] constexpr auto w() const noexcept -> const_reference;

  [[nodiscard]] constexpr auto w() noexcept -> reference;

}; // class basic_vector4

// -- Type aliases --

/** @brief Type alias for a four-dimensional vector with 32 bit floating-point components. */
using vector4f = basic_vector4<std::float_t>;

/** @brief Type alias for a four-dimensional vector with 32 bit integer components. */
using vector4i = basic_vector4<std::int32_t>;

/** @brief Type alias for vector2f. */
using vector4 = vector4f;

} // namespace sbx::math

template<sbx::math::arithmetic Type>
struct std::hash<sbx::math::basic_vector4<Type>> {
  auto operator()(const sbx::math::basic_vector4<Type>& vector) const noexcept -> std::size_t;
}; // struct std::hash

template<sbx::math::arithmetic Type>
struct YAML::convert<sbx::math::basic_vector4<Type>> {
  static auto encode(const sbx::math::basic_vector4<Type>& vector) -> Node;
  static auto decode(const Node& node, sbx::math::basic_vector4<Type>& vector) -> bool;
}; // struct YAML::convert

#include <libsbx/math/vector4.ipp>

#endif // LIBSBX_MATH_VECTOR4_HPP_
