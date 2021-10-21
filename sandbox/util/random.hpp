#ifndef SBX_UTIL_RANDOM_HPP_
#define SBX_UTIL_RANDOM_HPP_

#include <limits>
#include <random>
#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {

struct random {

  template <typename Type>
  static Type next(const Type min = std::numeric_limits<Type>::min(), const Type max = std::numeric_limits<Type>::max()) {
    static_assert(std::is_arithmetic_v<Type>, "Type must be an arithmetic type");

    static std::random_device seed;
    static std::mt19937_64 engine;

    if constexpr (std::is_floating_point_v<Type>) {
      static std::uniform_real_distribution<Type> distribution{min, max};

      return distribution(engine);
    } else {
      static std::uniform_int_distribution<Type> distribution{min, max};

      return distribution(engine);
    }
  }

}; // struct random

} // namespace sbx

#endif // SBX_UTIL_RANDOM_HPP_
