#ifndef SBX_ECS_COMPONENT_HPP_
#define SBX_ECS_COMPONENT_HPP_

#include <type_traits>

namespace sbx {

struct basic_component_traits {

  using in_place_delete = std::false_type;

  using ignore_if_empty = std::true_type;

}; // struct basic_component_traits

template<typename Component, typename = void>
struct component_traits : basic_component_traits {
  static_assert(std::is_same_v<std::decay_t<Component>, Component>, "Unsupported component type");
}; // struct component_traits

template<typename Component>
inline constexpr auto in_place_delete_v = component_traits<Component>::in_place_delete::value;

template<typename Component>
inline constexpr auto ignore_as_empty_v = component_traits<Component>::ignore_if_empty::value && std::is_empty_v<Component>;

} // namespace sbx

#endif // SBX_ECS_COMPONENT_HPP_
