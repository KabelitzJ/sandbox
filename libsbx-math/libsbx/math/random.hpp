#ifndef LIBSBX_MATH_RANDOM_HPP_
#define LIBSBX_MATH_RANDOM_HPP_

#include <random>
#include <concepts>
#include <limits>

#include <libsbx/math/concepts.hpp>

namespace sbx::math {

struct random {

  template<numeric Type>
  static auto next(Type min = std::numeric_limits<Type>::min(), Type max = std::numeric_limits<Type>::max()) -> Type {
    using distribution_type = std::conditional_t<std::floating_point<Type>, std::uniform_real_distribution<Type>, std::uniform_int_distribution<Type>>;

    static auto device = std::random_device{};
    static auto generator = std::mt19937{device()};

    auto distribution = distribution_type{min, max};

    return distribution(generator);
  }

}; // struct random

} // namespace sbx::math

#endif // LIBSBX_MATH_RANDOM_HPP_
