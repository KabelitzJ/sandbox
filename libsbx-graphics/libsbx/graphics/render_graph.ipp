#include <libsbx/graphics/render_graph.hpp>

namespace sbx::graphics {

namespace detail {

// template<typename... Names>
// requires (... && (std::is_same_v<std::remove_cvref_t<Names>, utility::hashed_string> || std::is_constructible_v<utility::hashed_string, Names>))
// void graphics_node::uses(Names&&... names) {
//   (_inputs.emplace_back(std::forward<Names>(names)), ...);
// }

// template<typename... Args>
// requires std::is_constructible_v<attachment, Args...>
// void graphics_node::produces(Args&&... args) {
//   _outputs.emplace_back(std::forward<Args>(args)...);
// }

template<typename Type, typename... Args>
auto graph_base::emplace_back(Args&&... args) -> Type& {
  _nodes.emplace_back(std::in_place_type_t<Type>{}, std::forward<Args>(args)...);
  return std::get<Type>(_nodes.back());
}

template<typename... Names>
requires (... && (std::is_same_v<std::remove_cvref_t<Names>, utility::hashed_string> || std::is_constructible_v<utility::hashed_string, Names>))
void graphics_pass::uses(Names&&... names) {
  (_node._inputs.emplace_back(std::forward<Names>(names)), ...);
}

template<typename... Args>
requires (std::is_constructible_v<attachment, Args...>)
void graphics_pass::produces(Args&&... args) {
  _node._outputs.emplace_back(std::forward<Args>(args)...);
}

template <typename Callable>
requires (std::is_invocable_r_v<graphics_pass, Callable, context&>)
auto graph_builder::emplace(Callable&& callable) -> graphics_pass {
  auto ctx = context{_graph};
  return std::invoke(callable, ctx);
}

template <typename Callable>
requires (std::is_invocable_r_v<compute_pass, Callable, context&>)
auto graph_builder::emplace(Callable&& callable) -> compute_pass {
  auto ctx = context{_graph};
  return std::invoke(callable, ctx);
}

template<typename... Callables>
requires (sizeof...(Callables) > 1u)
auto graph_builder::emplace(Callables&&... callables) -> decltype(auto) {
  _graph.reserve(sizeof...(callables));
  return std::tuple{emplace(std::forward<Callables>(callables))...};
}

} // namespace detail

} // namespace sbx::graphics
