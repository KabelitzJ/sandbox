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
    

template<typename Type>
[[nodiscard]] constexpr auto to_address(Type &&ptr) noexcept {
  if constexpr(std::is_pointer_v<std::remove_const_t<std::remove_reference_t<Type>>>) {
    return ptr;
  } else {
    return to_address(std::forward<Type>(ptr).operator->());
  }
}
    
} // namespace sbx

#endif // SBX_UTIL_TYPE_TRAITS_HPP_
