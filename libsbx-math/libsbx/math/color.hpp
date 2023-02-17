#ifndef LIBSBX_MATH_COLOR_HPP_
#define LIBSBX_MATH_COLOR_HPP_

#include <cinttypes>

namespace sbx::math {

class color {

public:

  static const color black;
  static const color white;
  static const color red;
  static const color green;
  static const color blue;

  std::uint8_t r{};
  std::uint8_t g{};
  std::uint8_t b{};
  std::uint8_t a{};

  constexpr color() noexcept = default;

  constexpr color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 0xFF) noexcept;

  constexpr color(std::uint32_t rgba) noexcept;

  constexpr color(const color& other) noexcept = default;

  constexpr color(color&& other) noexcept = default;

  ~color() = default;

  constexpr auto operator=(const color& other) noexcept -> color& = default;

  constexpr auto operator=(color&& other) noexcept -> color& = default;

}; // class color

} // namespace sbx::math

#endif // LIBSBX_MATH_COLOR_HPP_
