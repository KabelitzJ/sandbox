#ifndef LIBSBX_MATH_RANDOM_HPP_
#define LIBSBX_MATH_RANDOM_HPP_

#include <random>
#include <ranges>
#include <concepts>
#include <limits>

#include <libsbx/math/concepts.hpp>

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

} // namespace sbx::math

#endif // LIBSBX_MATH_RANDOM_HPP_
