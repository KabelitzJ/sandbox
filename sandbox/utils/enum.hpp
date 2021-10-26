#ifndef SBX_UTILS_ENUM_HPP_
#define SBX_UTILS_ENUM_HPP_

#include <type_traits>

namespace sbx {

template<typename Type>
constexpr std::enable_if_t<std::is_enum<Type>, std::underlying_type_t<Type>> to_underlying(Type type) {
  return static_cast<std::underlying_type_t<Type>>(type);
}

template<typename Type, typename = void>
struct is_implicit_enum_class : std::false_type { };

template<typename Type>
inline constexpr auto is_implicit_enum_class_v = is_implicit_enum_class<Type>::value;

template<typename Type, typename = void>
struct implicit_enum_class { };

template<typename Type>
struct implicit_enum_class<Type, std::enable_if_t<std::is_enum<Type> && is_implicit_enum_class_v<Type>>> {

  using value_type = Type;

  implicit_enum_class(value_type value)
  : _value{value} { }

  ~implicit_enum_class() = default;

  operator std::underlying_type_t<value_type>() const noexcept {
    return static_cast<std::underlying_type_t<value_type>>(_value);
  }

private:
  value_type _value{};

};

} // namespace sbx

#endif // SBX_UTILS_ENUM_HPP_
