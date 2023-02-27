#ifndef LIBSBX_UNITS_BYTES_HPP_
#define LIBSBX_UNITS_BYTES_HPP_

#include <libsbx/units/quantity.hpp>

namespace sbx::units {

namespace detail {

struct byte_tag { };

} // namespace detail

using kibi = std::ratio<1024>;
using mebi = std::ratio<1024 * 1024>;
using gibi = std::ratio<1024 * 1024 * 1024>;

using gibibyte = quantity<detail::byte_tag, std::uint64_t, gibi>;
using gigabyte = quantity<detail::byte_tag, std::uint64_t, std::giga>;
using mebibyte = quantity<detail::byte_tag, std::uint64_t, mebi>;
using megabyte = quantity<detail::byte_tag, std::uint64_t, std::mega>;
using kibibyte = quantity<detail::byte_tag, std::uint64_t, kibi>;
using kilobyte = quantity<detail::byte_tag, std::uint64_t, std::kilo>;
using byte = quantity<detail::byte_tag, std::uint64_t>;

namespace literals {

constexpr auto operator"" _gib(unsigned long long value) -> gibibyte {
  return gibibyte{static_cast<gibibyte::value_type>(value)};
}

constexpr auto operator"" _gb(unsigned long long value) -> gigabyte {
  return gigabyte{static_cast<gigabyte::value_type>(value)};
}

constexpr auto operator"" _mib(unsigned long long value) -> mebibyte {
  return mebibyte{static_cast<mebibyte::value_type>(value)};
}

constexpr auto operator"" _mb(unsigned long long value) -> megabyte {
  return megabyte{static_cast<megabyte::value_type>(value)};
}

constexpr auto operator"" _kib(unsigned long long value) -> kibibyte {
  return kibibyte{static_cast<kibibyte::value_type>(value)};
}

constexpr auto operator"" _kb(unsigned long long value) -> kilobyte {
  return kilobyte{static_cast<kilobyte::value_type>(value)};
}

constexpr auto operator"" _b(unsigned long long value) -> byte {
  return byte{static_cast<byte::value_type>(value)};
}

} // namespace literals

} // namespace sbx::units

#endif // LIBSBX_UNITS_BYTES_HPP_
