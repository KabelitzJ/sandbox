#ifndef SBX_TYPES_ITERATOR_TRAITS_HPP_
#define SBX_TYPES_ITERATOR_TRAITS_HPP_

#include <type_traits>
#include <iterator>

namespace sbx {

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

} // namespace sbx

#endif // SBX_TYPES_ITERATOR_TRAITS_HPP_
