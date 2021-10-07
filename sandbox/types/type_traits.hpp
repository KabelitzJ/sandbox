#ifndef SBX_UTIL_TYPE_TRAITS_HPP_
#define SBX_UTIL_TYPE_TRAITS_HPP_

namespace sbx {

template<typename From, typename To>
struct constness_as {
    using type = std::remove_const_t<To>;
};

template<typename From, typename To>
struct constness_as<const From, To> {
    using type = std::add_const_t<To>;
};
  
template<typename From, typename To>
using constness_as_t = typename constness_as<From, To>::type;


template<typename Type, typename = void>
struct is_iterator: std::false_type {};

template<typename Type>
struct is_iterator<Type, std::void_t<typename std::iterator_traits<Type>::iterator_category>> : std::true_type {};

template<typename Type>
inline constexpr bool is_iterator_v = is_iterator<Type>::value;


template<typename Type, typename Iterator, typename = void>
struct is_iterator_type: std::false_type {};

template<typename Type, typename Iterator>
struct is_iterator_type<Type, Iterator, std::enable_if_t<is_iterator_v<Type> && std::is_same_v<Type, Iterator>>> : std::true_type {};

template<typename Type, typename Iterator>
struct is_iterator_type<Type, Iterator, std::enable_if_t<!std::is_same_v<Type, Iterator>, std::void_t<typename Iterator::iterator_type>>> : is_iterator_type<Type, typename Iterator::iterator_type> {};

template<typename Type, typename Iterator>
inline constexpr bool is_iterator_type_v = is_iterator_type<Type, Iterator>::value;


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

template<typename, typename>
struct is_applicable: std::false_type {};

template<typename Function, template<typename...> class Tuple, typename... Args>
struct is_applicable<Function, Tuple<Args...>>: std::is_invocable<Function, Args...> {};

template<typename Function, template<typename...> class Tuple, typename... Args>
struct is_applicable<Function, const Tuple<Args...>>: std::is_invocable<Function, Args...> {};

template<typename Function, typename... Args>
inline constexpr bool is_applicable_v = is_applicable<Function, Args...>::value;

template<typename, typename, typename>
struct is_applicable_r: std::false_type {};

template<typename Return, typename Function, typename... Args>
struct is_applicable_r<Return, Function, std::tuple<Args...>>: std::is_invocable_r<Return, Function, Args...> {};

template<typename Return, typename Function, typename... Args>
inline constexpr bool is_applicable_r_v = is_applicable_r<Return, Function, Args...>::value;
    
} // namespace sbx

#endif // SBX_UTIL_TYPE_TRAITS_HPP_
