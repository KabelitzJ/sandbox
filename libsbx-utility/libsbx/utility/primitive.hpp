#ifndef LIBSBX_UTILITY_PRIMITIVE_HPP_
#define LIBSBX_UTILITY_PRIMITIVE_HPP_

#include <concepts>
#include <type_traits>

#include <fmt/format.h>

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

  constexpr explicit primitive(Type value = static_cast<value_type>(0)) noexcept
  : _value{value} { }

  constexpr ~primitive() = default;

  constexpr operator Type() const noexcept {
    return _value;
  }

private:

  Type _value;

}; // class primitive

} // namespace sbx::utility

template<typename Type>
requires (std::is_base_of_v<sbx::utility::primitive<typename Type::value_type>, Type>)
struct fmt::formatter<Type> : fmt::formatter<typename Type::value_type> {
  using super = fmt::formatter<typename Type::value_type>;

  template<typename ParseContext>
  constexpr auto parse(ParseContext& context) -> decltype(context.begin()) {
    return super::parse(context);
  }

  template<typename FormatContext>
  auto format(const Type& value, FormatContext& context) -> decltype(context.out()) {
    return super::format(value, context);
  }

}; // struct fmt::formatter<sbx::utility::primitive<Type>>

#endif // LIBSBX_UTILITY_PRIMITIVE_HPP_
