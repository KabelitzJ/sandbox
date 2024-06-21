#include <libsbx/math/random.hpp>

namespace sbx::math {

auto random_color() -> color {
  return color{random::next<std::float_t>(0.0f, 1.0f), random::next<std::float_t>(0.0f, 1.0f), random::next<std::float_t>(0.0f, 1.0f), 1.0f};
}

} // namespace sbx::math
