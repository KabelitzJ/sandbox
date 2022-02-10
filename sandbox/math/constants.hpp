#ifndef SBX_MATH_CONSTANTS_HPP_
#define SBX_MATH_CONSTANTS_HPP_

#include <concepts>

#include <types/primitives.hpp>

namespace sbx {

/**
 * @brief Mathematical constant pi
 * 
 * @tparam Type The type of floating-point type
 */
template<std::floating_point Type>
inline constexpr auto pi_v = Type{3.14159265358979323851};

inline constexpr auto pi = pi_v<float32>;

} // namespace sbx

#endif // SBX_MATH_CONSTANTS_HPP_
