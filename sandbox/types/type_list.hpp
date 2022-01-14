#ifndef SBX_TYPES_TYPE_LIST_HPP_
#define SBX_TYPES_TYPE_LIST_HPP_

#include <cstddef>

namespace sbx {

template<typename... Types>
struct type_list {
  using type = type_list;
  static constexpr auto size = sizeof...(Types);
};

template<std::size_t, typename>
struct type_list_element;

template<std::size_t Index, typename Type, typename... Others>
struct type_list_element<Index, type_list<Type, Others...>> : type_list_element<Index - 1u, type_list<Others...>> {};

template<typename Type, typename... Others>
struct type_list_element<0u, type_list<Type, Others...>> {
  using type = Type;
};

template<std::size_t Index, typename List>
using type_list_element_t = typename type_list_element<Index, List>::type;

template<typename List, typename Type>
struct type_list_contains;

template<typename... Type, typename Other>
struct type_list_contains<type_list<Type...>, Other> : std::disjunction<std::is_same<Type, Other>...> {};

template<typename List, typename Type>
inline constexpr bool type_list_contains_v = type_list_contains<List, Type>::value;

} // namespace sbx

#endif // SBX_TYPES_TYPE_LIST_HPP_
