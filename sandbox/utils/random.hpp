#ifndef SBX_UTILS_RANDOM_HPP_
#define SBX_UTILS_RANDOM_HPP_

#include <type_traits>
#include <limits>
#include <random>

#include <meta/concepts.hpp>

namespace sbx {

class random {

public:

  template<arithmetic Type>
  static Type next(const Type min = std::numeric_limits<Type>::min(), const Type max = std::numeric_limits<Type>::max()) {
    using distribution_type = std::conditional_t<
      std::is_floating_point_v<Type>,
      std::uniform_real_distribution<Type>,
      std::uniform_int_distribution<Type>
    >;

    static auto device = std::random_device{};
    static auto generator = std::mt19937{device()};

    auto distribution = distribution_type{min, max};

    return static_cast<Type>(distribution(generator));
  }

private:

}; // class random

} // namespace sbx

#endif // SBX_UTILS_RANDOM_HPP_
