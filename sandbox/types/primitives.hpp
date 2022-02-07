#ifndef SBX_TYPES_PRIMITIVES_HPP_
#define SBX_TYPES_PRIMITIVES_HPP_

#include <cstdint>

namespace sbx {

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using float32 = float;
using float64 = double;

static_assert(sizeof(int8) == 1, "Target platform does not support 8 bit signed integer numbers")
static_assert(sizeof(int16) == 2, "Target platform does not support 16 bit signed integer numbers")
static_assert(sizeof(int32) == 4, "Target platform does not support 32 bit signed integer numbers")
static_assert(sizeof(int64) == 8, "Target platform does not support 64 bit signed integer numbers")

static_assert(sizeof(uint8) == 1, "Target platform does not support 8 bit unsigned integer numbers")
static_assert(sizeof(uint16) == 2, "Target platform does not support 16 bit unsigned integer numbers")
static_assert(sizeof(uint32) == 4, "Target platform does not support 32 bit unsigned integer numbers")
static_assert(sizeof(uint64) == 8, "Target platform does not support 64 bit unsigned integer numbers")

static_assert(sizeof(float32) == 4, "Target platform does not support 32 bit floating-point numbers");
static_assert(sizeof(float64) == 8, "Target platform does not support 64 bit floating-point numbers");

} // namespace sbx

#endif // SBX_TYPES_PRIMITIVES_HPP_
