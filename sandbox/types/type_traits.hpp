#ifndef SBX_UTIL_TYPE_TRAITS_HPP_
#define SBX_UTIL_TYPE_TRAITS_HPP_

namespace sbx {

template<typename To, typename From>
struct constness_as {
  using type = std::remove_const_t<To>;
};

template<typename To, typename From>
struct constness_as<To, const From> {
  using type = std::add_const_t<To>;
};
  
template<typename To, typename From>
using constness_as_t = typename constness_as<To, From>::type;


template<typename, typename>
struct is_applicable: std::false_type {};

template<typename Function, template<typename...> class Tuple, typename... Args>
struct is_applicable<Function, Tuple<Args...>> : std::is_invocable<Function, Args...> {};

template<typename Function, template<typename...> class Tuple, typename... Args>
struct is_applicable<Function, const Tuple<Args...>> : std::is_invocable<Function, Args...> {};

template<typename Function, typename Args>
inline constexpr bool is_applicable_v = is_applicable<Function, Args>::value;

template<typename, typename, typename>
struct is_applicable_r: std::false_type {};

template<typename Return, typename Function, typename... Args>
struct is_applicable_r<Return, Function, std::tuple<Args...>> : std::is_invocable_r<Return, Function, Args...> {};

template<typename Return, typename Function, typename Args>
inline constexpr bool is_applicable_r_v = is_applicable_r<Return, Function, Args>::value;  

} // namespace sbx

#endif // SBX_UTIL_TYPE_TRAITS_HPP_
