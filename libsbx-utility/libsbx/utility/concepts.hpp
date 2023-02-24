#ifndef LIBSBX_UTILITY_CONCEPTS_HPP_
#define LIBSBX_UTILITY_CONCEPTS_HPP_

#include <type_traits>

namespace sbx::utility {

template<typename Type, typename... TypeList>
struct is_one_of : std::false_type { };

template<typename Type, typename... TypeList>
struct is_one_of<Type, Type, TypeList...> : std::true_type { };

template<typename Type, typename Head, typename... Rest>
struct is_one_of<Type, Head, Rest...> : is_one_of<Type, Rest...> { };

template<typename Type, typename... TypeList>
inline constexpr auto is_one_of_v = is_one_of<Type, TypeList...>::value;

template<typename Type, typename... TypeList>
concept one_of = is_one_of_v<Type, TypeList...>;

template<typename Type, typename... TypeList>
concept none_of = !is_one_of_v<Type, TypeList...>;



template<typename T, typename... Rest>
struct are_all_unique : std::bool_constant<!(std::is_same_v<T, Rest> || ...) && are_all_unique<Rest...>::value>{};

template<typename T>
struct are_all_unique<T> : std::true_type {};

template<typename... TypeList>
inline constexpr auto are_all_unique_v = are_all_unique<TypeList...>::value;

template<typename... TypeList>
concept all_unique = are_all_unique_v<TypeList...>;



template<typename Type, typename... TypeList>
struct is_convertible_to_one_of : std::false_type{ };

template<typename Type, typename... TypeList>
struct is_convertible_to_one_of<Type, Type, TypeList...> : std::true_type{ };

template<typename Type, typename Head, typename... Rest>
struct is_convertible_to_one_of<Type, Head, Rest...> : std::conditional_t<std::is_convertible_v<Type, Head>, std::true_type, is_convertible_to_one_of<Type, Rest...>> { };

template<typename Type, typename... TypeList>
inline constexpr auto is_convertible_to_one_of_v = is_convertible_to_one_of<Type, TypeList...>::value;

template<typename Type, typename... TypeList>
concept convertible_to_one_of = is_convertible_to_one_of_v<Type, TypeList...>;


template<typename Type, typename Base>
concept implements = !std::is_abstract_v<Type> && std::is_abstract_v<Base> && std::is_base_of_v<Base, Type>; 

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_CONCEPTS_HPP_
