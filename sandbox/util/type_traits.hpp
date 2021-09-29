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
