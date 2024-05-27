#ifndef LIBSBX_MATH_UUID_HPP_
#define LIBSBX_MATH_UUID_HPP_

#include <cinttypes>

#include <fmt/format.h>

#include <range/v3/all.hpp>

#include <libsbx/utility/hash.hpp>

#include <libsbx/math/random.hpp>

namespace sbx::math {

class uuid {

  friend struct fmt::formatter<sbx::math::uuid>;
  friend struct std::hash<sbx::math::uuid>;

public:

  uuid() {
    // Taken from https://www.cryptosys.net/pki/uuid-rfc4122.html

    // 1. Generate 16 random bytes (=128 bits)
    for (auto& byte : _bytes) {
      byte = random::next<std::uint8_t>();
    }

    // 2. Adjust certain bits according to RFC 4122 section 4.4 as follows:
    //   (a) set the four most significant bits of the 7th byte to 0100'B, so the high nibble is "4"
    //   (b) set the two most significant bits of the 9th byte to 10'B, so the high nibble will be one of "8", "9", "A" or "B"
    _bytes[6] = 0x40 | (_bytes[6] & 0xf);
    _bytes[8] = 0x80 | (_bytes[8] & 0x3f);
  }

  ~uuid() = default;

  auto operator==(const uuid& other) const noexcept -> bool {
    return std::ranges::equal(_bytes, other._bytes);
  }

private:

  std::array<std::uint8_t, 16u> _bytes;

}; // class uuid

} // namespace sbx::math

template<>
struct fmt::formatter<sbx::math::uuid> {

  template<typename ParseContext>
  constexpr auto parse(ParseContext& context) -> decltype(context.begin()) {
    return context.begin();
  }

  template<typename FormatContext>
  auto format(const sbx::math::uuid& uuid, FormatContext& context) -> decltype(context.out()) {
    for (const auto [i, byte] : ranges::views::enumerate(uuid._bytes)) {
      fmt::format_to(context.out(), "{:02x}", byte);

      if (i == 3 || i == 5 || i == 7 || i == 9) {
        fmt::format_to(context.out(), "-");
      }
    }

    return context.out();
  }
}; // struct fmt::formatter<sbx::math::uuid>

template<>
struct std::hash<sbx::math::uuid> {
  auto operator()(const sbx::math::uuid& uuid) const noexcept -> std::size_t {
    auto seed = std::size_t{0};

    for (const auto& byte : uuid._bytes) {
      sbx::utility::hash_combine(seed, byte);
    }

    return seed;
  }
}; // struct std::hash<sbx::math::uuid>

#endif // LIBSBX_MATH_UUID_HPP_

