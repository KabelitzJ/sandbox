#ifndef SBX_ECS_COMPONENT_HPP_
#define SBX_ECS_COMPONENT_HPP_

#include <type_traits>

namespace sbx {
 
struct basic_component_traits {
  using in_place_delete = std::false_type;
  using ignore_if_empty = std::true_type;
};

template<typename Type, typename = void>
struct component_traits: basic_component_traits {
  static_assert(std::is_same_v<std::decay_t<Type>, Type>, "Unsupported type");
};

template<class Type>
inline constexpr bool in_place_delete_v = component_traits<Type>::in_place_delete::value;

template<class Type>
inline constexpr bool ignore_as_empty_v = component_traits<Type>::ignore_if_empty::value && std::is_empty_v<Type>;

} // namespace sbx

#endif // SBX_ECS_COMPONENT_HPP_
