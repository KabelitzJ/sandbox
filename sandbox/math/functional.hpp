#ifndef SBX_MATH_FUNCTIONAL_HPP_
#define SBX_MATH_FUNCTIONAL_HPP_

#include <cstddef>
#include <concepts>
#include <bit>

namespace sbx {

template<std::size_t Value, std::size_t Modulo>
requires (std::has_single_bit(Modulo))
[[nodiscard]] constexpr std::size_t fast_mod() noexcept;

} // namespace sbx

#include "functional.inl"

#endif // SBX_MATH_FUNCTIONAL_HPP_
