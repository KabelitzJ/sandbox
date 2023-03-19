#ifndef LIBSBX_UTILITY_TYPE_LIST_HPP_
#define LIBSBX_UTILITY_TYPE_LIST_HPP_

namespace sbx::utility {

template<typename... Types>
struct type_list {
  static constexpr auto size = sizeof...(Types);
}; // struct type_list



template<typename... Types>
struct type_list_front;

template<typename Type, typename... Rest>
struct type_list_front<type_list<Type, Rest...>> {
  using type = Type;
}; // struct type_list_front

template<typename... Types>
using type_list_front_t = typename type_list_front<Types...>::type;



template<typename... Types>
struct type_list_back;

template<typename Type, typename... Rest>
struct type_list_back<type_list<Type, Rest...>> {
  using type = type_list<Rest...>;
}; // struct type_list_back

template<typename... Types>
using type_list_back_t = typename type_list_back<Types...>::type;



template<typename... Types>
struct type_list_push_front;

template<typename Type, typename... Rest>
struct type_list_push_front<type_list<Rest...>, Type> {
  using type = type_list<Type, Rest...>;
}; // struct type_list_push_front

template<typename... Types>
using type_list_push_front_t = typename type_list_push_front<Types...>::type;



template<typename... Types>
struct type_list_push_back;

template<typename Type, typename... Rest>
struct type_list_push_back<type_list<Rest...>, Type> {
  using type = type_list<Rest..., Type>;
}; // struct type_list_push_back

template<typename... Types>
using type_list_push_back_t = typename type_list_push_back<Types...>::type;



template<typename... Types>
struct type_list_pop_front;

template<typename Type, typename... Rest>
struct type_list_pop_front<type_list<Type, Rest...>> {
  using type = type_list<Rest...>;
}; // struct type_list_pop_front

template<typename... Types>
using type_list_pop_front_t = typename type_list_pop_front<Types...>::type;



template<typename... Types>
struct type_list_pop_back;

template<typename Type, typename... Rest>
struct type_list_pop_back<type_list<Type, Rest...>> {
  using type = type_list<Type>;
}; // struct type_list_pop_back

template<typename... Types>
using type_list_pop_back_t = typename type_list_pop_back<Types...>::type;



template<typename... Types>
struct type_list_concat;

template<typename... Lhs, typename... Rhs>
struct type_list_concat<type_list<Lhs...>, type_list<Rhs...>> {
  using type = type_list<Lhs..., Rhs...>;
}; // struct type_list_concat

template<typename... Types>
using type_list_concat_t = typename type_list_concat<Types...>::type;

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_TYPE_LIST_HPP_
