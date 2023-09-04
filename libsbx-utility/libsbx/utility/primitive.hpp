#ifndef LIBSBX_UTILITY_PRIMITIVE_HPP_
#define LIBSBX_UTILITY_PRIMITIVE_HPP_

#include <concepts>
#include <type_traits>

namespace sbx::utility {

/**
 * This should be used to give primitives a meaningful name
 * 
 * @code{
 * 
 * class pixels : sbx::utility::primitive<std::uint32_t> {
 * 
 * public:
 * 
 *   using super = sbx::utility::primitive<std::uint32_t>;
 * 
 *   using super::super;
 * 
 * }; // class pixels
 * 
 * } 
 */
template<typename Type>
requires (std::is_arithmetic_v<Type>)
class primitive {

public:

  using value_type = Type;

  constexpr primitive(Type value) noexcept
  : _value{value} { }

  constexpr ~primitive() = default;

  constexpr operator Type() const noexcept {
    return _value;
  }

private:

  Type _value;

}; // class primitive

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_PRIMITIVE_HPP_
