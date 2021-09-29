#ifndef SBX_TYPES_BASIC_TRAITS_HPP_
#define SBX_TYPES_BASIC_TRAITS_HPP_

#include <type_traits>

namespace sbx {

template<typename T>
struct is_index : std::integral_constant<bool, std::is_unsigned_v<T> && std::is_integral_v<T>> {};

template<typename T>
inline constexpr auto is_index_v = is_index<T>::value;

} // namespace sbx

#endif // SBX_TYPES_BASIC_TRAITS_HPP_
