#include <libsbx/math/color.hpp>

namespace sbx::math {

const color color::black{0x00, 0x00, 0x00, 0xFF};
const color color::white{0xFF, 0xFF, 0xFF, 0xFF};
const color color::red{0xFF, 0x00, 0x00, 0xFF};
const color color::green{0x00, 0xFF, 0x00, 0xFF};
const color color::blue{0x00, 0x00, 0xFF, 0xFF};

constexpr color::color(std::uint8_t _r, std::uint8_t _g, std::uint8_t _b, std::uint8_t _a) noexcept
: r{_r},
  g{_g},
  b{_b},
  a{_a} {}

constexpr color::color(std::uint32_t rgba) noexcept
: r{static_cast<std::uint8_t>((rgba >> 24) & 0xFF)},
  g{static_cast<std::uint8_t>((rgba >> 16) & 0xFF)},
  b{static_cast<std::uint8_t>((rgba >> 8) & 0xFF)},
  a{static_cast<std::uint8_t>(rgba & 0xFF)} {}

} // namespace sbx::math
