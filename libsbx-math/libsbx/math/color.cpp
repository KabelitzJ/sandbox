#include <libsbx/math/color.hpp>

namespace sbx::math {

template<std::size_t Shift>
constexpr auto extract(std::uint32_t rgba) noexcept -> std::uint8_t {
  return static_cast<std::uint8_t>((rgba >> Shift) & 0xFFu);
}

constexpr auto scale(std::uint8_t component) noexcept -> std::float_t {
  return static_cast<std::float_t>(component) / 255.0f;
}

enum class component : std::uint8_t {
  red = 24u,
  green = 16u,
  blue = 8u,
  alpha = 0u
}; // enum class component

const color color::black{0.0f, 0.0f, 0.0f, 1.0f};
const color color::white{1.0f, 1.0f, 1.0f, 1.0f};
const color color::red{1.0f, 0.0f, 0.0f, 1.0f};
const color color::green{0.0f, 1.0f, 0.0f, 1.0f};
const color color::blue{0.0f, 0.0f, 1.0f, 1.0f};

template<component Component>
auto extract_component(std::uint32_t rgba) noexcept -> std::float_t {
  return scale(extract<static_cast<std::size_t>(Component)>(rgba));
}

color::color(std::uint32_t rgba) noexcept
: _red(extract_component<component::red>(rgba)),
  _green(extract_component<component::green>(rgba)),
  _blue(extract_component<component::blue>(rgba)),
  _alpha(extract_component<component::alpha>(rgba)) { }

color::color(std::float_t red, std::float_t green, std::float_t blue, std::float_t alpha) noexcept
: _red(red),
  _green(green),
  _blue(blue),
  _alpha(alpha) { }

auto color::r() const noexcept -> const std::float_t& {
  return _red;
}

auto color::r() noexcept -> std::float_t& {
  return _red;
}

auto color::g() const noexcept -> const std::float_t& {
  return _green;
}

auto color::g() noexcept -> std::float_t& {
  return _green;
}

auto color::b() const noexcept -> const std::float_t& {
  return _blue;
}

auto color::b() noexcept -> std::float_t& {
  return _blue;
}

auto color::a() const noexcept -> const std::float_t& {
  return _alpha;
}

auto color::a() noexcept -> std::float_t& {
  return _alpha;
}

auto operator==(const color& lhs, const color& rhs) noexcept -> bool {
  return lhs.r() == rhs.r() && lhs.g() == rhs.g() && lhs.b() == rhs.b() && lhs.a() == rhs.a();
}

} // namespace sbx::math
