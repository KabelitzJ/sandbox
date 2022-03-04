#ifndef SBX_ECS_COMPONENT_HPP_
#define SBX_ECS_COMPONENT_HPP_

#include <type_traits>

namespace sbx {

template<typename Type>
concept component = !std::is_abstract_v<Type> && std::is_same_v<std::decay_t<Type>, Type>;

} // namespace sbx

#endif // SBX_ECS_COMPONENT_HPP_
