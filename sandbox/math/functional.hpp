#ifndef SBX_MATH_FUNCTIONAL_HPP_
#define SBX_MATH_FUNCTIONAL_HPP_

#include <cstddef>
#include <concepts>
#include <bit>

namespace sbx {

template<std::unsigned_integral Type>
[[nodiscard]] constexpr Type fast_mod(const Type value, const Type modulo) noexcept;

} // namespace sbx

#include "functional.inl"

#endif // SBX_MATH_FUNCTIONAL_HPP_
