#ifndef SBX_UTIL_INDEX_TRAITS_HPP_
#define SBX_UTIL_INDEX_TRAITS_HPP_

#include <type_traits>

namespace sbx {

template<typename T>
struct is_unsigned_integral : std::integral_constant<bool, std::is_unsigned_v<T> && std::is_integral_v<T>> {};

template<typename T>
inline constexpr auto is_unsigned_integral_v = is_unsigned_integral<T>::value;

template<typename, typename = void>
struct index_type {};

template<typename T>
struct index_type<T, std::enable_if_t<is_unsigned_integral_v<T>>> {
  using type = T;
};

template<typename T>
struct index_type<T, std::enable_if_t<std::is_enum_v<T>>> : index_type<std::underlying_type_t<T>> {};

template<typename T>
using index_type_t = typename index_type<T>::type;

template<typename, typename = void>
struct is_index_type : std::false_type {};

template<typename T>
struct is_index_type<T, std::void_t<index_type_t<T>>> : std::true_type {};

template<typename T>
inline constexpr auto is_index_type_v = is_index_type<T>::value;

template<typename T>
inline index_type_t<T> to_index(T value) {
  return static_cast<index_type_t<T>>(value);
}

} // namespace sbx

#endif // SBX_UTIL_INDEX_TRAITS_HPP_
