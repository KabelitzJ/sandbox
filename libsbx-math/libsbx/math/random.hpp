#ifndef LIBSBX_MATH_RANDOM_HPP_
#define LIBSBX_MATH_RANDOM_HPP_

#include <random>
#include <ranges>
#include <concepts>
#include <limits>

#include <libsbx/math/concepts.hpp>
#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>

namespace sbx::math {

struct random {

  random() = delete;

  template<numeric Type>
  static auto next(Type min = std::numeric_limits<Type>::min(), Type max = std::numeric_limits<Type>::max()) -> Type {
    using distribution_type = std::conditional_t<std::floating_point<Type>, std::uniform_real_distribution<Type>, std::uniform_int_distribution<Type>>;

    static auto device = std::random_device{};
    static auto generator = std::mt19937{device()};

    auto distribution = distribution_type{min, max};

    return distribution(generator);
  }

}; // struct random

template<std::ranges::sized_range Range>
auto random_element(const Range& range) -> std::ranges::range_value_t<Range> {
  const auto size = static_cast<std::ranges::range_difference_t<Range>>(std::ranges::size(range));
  const auto index = random::next<std::ranges::range_difference_t<Range>>(0, size - 1);

  return *std::next(std::begin(range), index);
}

template<scalar Type>
auto random_point_in_circle(const basic_vector2<Type>& center, const Type radius) -> basic_vector2<Type> {
  const auto r = radius * std::sqrt(random::next<Type>(Type{0}, Type{1}));
  const auto theta = random::next<Type>(Type{0}, Type{2} * std::numbers::pi_v<Type>);

  return center + basic_vector2<Type>{r * std::cos(theta), r * std::sin(theta)};
}

template<scalar Type>
auto random_point_in_sphere(const basic_vector3<Type>& center, const Type radius) -> basic_vector3<Type> {
  const auto r = radius * std::cbrt(random::next<Type>(Type{0}, Type{1}));
  const auto theta = random::next<Type>(Type{0}, Type{2} * std::numbers::pi_v<Type>);
  const auto phi = random::next<Type>(Type{0}, std::numbers::pi_v<Type>);

  const auto x = r * std::sin(phi) * std::cos(theta);
  const auto y = r * std::sin(phi) * std::sin(theta);
  const auto z = r * std::cos(phi);

  return center + basic_vector3<Type>{x, y, z};
}

} // namespace sbx::math

#endif // LIBSBX_MATH_RANDOM_HPP_
