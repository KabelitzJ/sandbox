#ifndef LIBSBX_UTILITY_TYPE_LIST_HPP_
#define LIBSBX_UTILITY_TYPE_LIST_HPP_

#include <cstddef>

namespace sbx::utility {

template<typename... Type>
struct type_list {
  using type = type_list;

  static constexpr auto size = sizeof...(Type);
}; // struct type_list

template<typename, typename>
struct type_list_index;

template<typename Type, typename First, typename... Other>
struct type_list_index<Type, type_list<First, Other...>> {
  using value_type = std::size_t;

  static constexpr auto value = type_list_index<Type, type_list<Other...>>::value + value_type{1};
}; // struct type_list_index

template<typename Type, typename... Other>
struct type_list_index<Type, type_list<Type, Other...>> {
  static_assert(type_list_index<Type, type_list<Other...>>::value == sizeof...(Other), "Duplicate type in type list");

  using value_type = std::size_t;

  static constexpr auto value = value_type{0};
};

template<typename Type>
struct type_list_index<Type, type_list<>> {
  using value_type = std::size_t;

  static constexpr auto value = value_type{0};
}; // struct type_list_index

template<typename Type, typename List>
inline constexpr std::size_t type_list_index_v = type_list_index<Type, List>::value;

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_TYPE_LIST_HPP_
