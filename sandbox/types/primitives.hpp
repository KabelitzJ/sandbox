#ifndef SBX_TYPES_PRIMITIVES_HPP_
#define SBX_TYPES_PRIMITIVES_HPP_

#include <cinttypes>
#include <cstddef>

namespace sbx {

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
using intmax = std::intmax_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using uintmax = std::uintmax_t;

using byte = uint8;

using size_type = std::size_t;

using float32 = float;
using float64 = double;

using cstring = const char*;

using time = float32;

} // namespace sbx

#endif // SBX_TYPES_PRIMITIVES_HPP_
