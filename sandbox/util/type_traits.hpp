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
  
} // namespace sbx

#endif // SBX_UTIL_TYPE_TRAITS_HPP_
