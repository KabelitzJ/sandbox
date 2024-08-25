#ifndef LIBSBX_MATH_NOISE_HPP_
#define LIBSBX_MATH_NOISE_HPP_

#include <array>
#include <cinttypes>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/random.hpp>
#include <libsbx/math/concepts.hpp>

namespace sbx::math {

class noise {
 
  inline static auto permutation = std::array<std::uint8_t, 256>{
    151, 160, 137, 91, 90, 15,
    131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
    190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
    77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
    102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
    135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
    5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
    223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
    129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
    251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
    49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
  };

  inline static constexpr auto F2 = 0.366025403f;
  inline static constexpr auto G2 = 0.211324865f;

public:

  static constexpr auto fractal(const std::float_t x, const std::float_t y, std::uint32_t octaves) -> std::float_t {
    auto output = 0.f;
    auto denom  = 0.f;
    auto frequency = 1.0f;
    auto amplitude = 1.0f;

    for (auto i = 0; i < octaves; i++) {
      output += (amplitude * simplex(x * frequency, y * frequency));
      denom += amplitude;

      frequency *= 2.0;
      amplitude *= 0.5;
    }

    return (output / denom);
  }

  static constexpr auto simplex(const std::float_t x, const std::float_t y) -> std::float_t {
    auto n0 = 0.0f;
    auto n1 = 0.0f; 
    auto n2 = 0.0f;

    const auto s = (x + y) * F2;
    const auto xs = x + s;
    const auto ys = y + s;
    const auto i = fast_floor(xs);
    const auto j = fast_floor(ys);

    const auto t = static_cast<std::float_t>(i + j) * G2;
    const auto X0 = i - t;
    const auto Y0 = j - t;
    const auto x0 = x - X0;
    const auto y0 = y - Y0;

    auto i1 = 0;
    auto j1 = 0;

    if (x0 > y0) {
      i1 = 1;
      j1 = 0;
    } else {
      i1 = 0;
      j1 = 1;
    }

    const auto x1 = x0 - i1 + G2;
    const auto y1 = y0 - j1 + G2;
    const auto x2 = x0 - 1.0f + 2.0f * G2;
    const auto y2 = y0 - 1.0f + 2.0f * G2;

    const auto gi0 = hash(i + hash(j));
    const auto gi1 = hash(i + i1 + hash(j + j1));
    const auto gi2 = hash(i + 1 + hash(j + 1));

    auto t0 = 0.5f - x0 * x0 - y0 * y0;
    
    if (t0 < 0.0f) {
      n0 = 0.0f;
    } else {
      t0 *= t0;
      n0 = t0 * t0 * grad(gi0, x0, y0);
    }

    auto t1 = 0.5f - x1 * x1 - y1 * y1;

    if (t1 < 0.0f) {
      n1 = 0.0f;
    } else {
      t1 *= t1;
      n1 = t1 * t1 * grad(gi1, x1, y1);
    }

    auto t2 = 0.5f - x2 * x2 - y2 * y2;

    if (t2 < 0.0f) {
      n2 = 0.0f;
    } else {
      t2 *= t2;
      n2 = t2 * t2 * grad(gi2, x2, y2);
    }

    return 45.23065f * (n0 + n1 + n2);
  }

private:

  static constexpr auto fast_floor(std::float_t fp) -> std::int32_t {
    auto i = static_cast<std::int32_t>(fp);
    return (fp < i) ? (i - 1) : (i);
  }

  static constexpr auto hash(std::int32_t i) -> std::uint8_t {
    return permutation[static_cast<std::uint8_t>(i)];
  }

  static constexpr auto grad(std::int32_t hash, std::float_t x, std::float_t y) -> std::float_t {
    const auto h = hash & 0x3F;
    const auto u = h < 4 ? x : y;
    const auto v = h < 4 ? y : x;

    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
  }

}; // class noise

} // namespace sbx::math

#endif // LIBSBX_MATH_NOISE_HPP_
