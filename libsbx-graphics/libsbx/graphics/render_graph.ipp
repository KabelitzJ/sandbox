#include <libsbx/graphics/render_graph.hpp>

namespace sbx::graphics {

namespace detail {

template<typename... Args>
auto graph_base::_emplace_back(Args&&... args) -> graph_node* {
  base::push_back(std::make_unique<graph_node>(std::forward<Args>(args)...));
  return back().get();
}

template<typename Callable>
graph_node::graphics_pass_node::graphics_pass_node(Callable&& callable)
: work{std::forward<Callable>(callable)} { }

template<typename Callable>
graph_node::compute_pass_node::compute_pass_node(Callable&& callable)
: work{std::forward<Callable>(callable)} { }

template<typename... Args>
graph_node::graph_node(const graphics_pass_parameters& parameters, graph_node* parent, Args&&... args)
: _name{parameters.name},
  _data{parameters.data},
  _parent{parent},
  _num_successors{0u},
  _handle{std::forward<Args>(args)...} { }

template<typename... Args>
graph_node::graph_node(const default_graphics_pass_parameters& parameters, graph_node* parent, Args&&...args)
: _data{nullptr},
  _parent{parent},
  _num_successors{0u},
  _handle{std::forward<Args>(args)...} { }

template<typename... Passes>
auto pass::precede(Passes&&... passes) -> pass& {
  (_node->_precede(passes._node), ...);
  return *this;
}

template<typename... Passes>
auto pass::succeed(Passes&&... passes) -> pass& {
  (passes._node->_precede(_node), ...);
  return *this;
}

template <typename Callable>
requires (is_graphics_pass_v<Callable>)
auto graph_builder::emplace(Callable&& callable) -> pass {
  return pass{_graph._emplace_back(default_graphics_pass_parameters{}, nullptr, std::in_place_type_t<graph_node::graphics_pass_node>{}, std::forward<Callable>(callable) )};
}

template <typename Callable>
requires (is_compute_pass_v<Callable>)
auto graph_builder::emplace(Callable&& callable) -> pass {
  return pass{_graph._emplace_back(default_compute_pass_parameters{}, nullptr, std::in_place_type_t<graph_node::compute_pass_node>{}, std::forward<Callable>(callable) )};
}

template<typename... Callables>
requires (sizeof...(Callables) > 1u)
auto graph_builder::emplace(Callables&&... callables) -> decltype(auto) {
  _graph.reserve(sizeof...(Callables));
  return std::make_tuple(emplace(std::forward<Callables>(callables))...);
}

} // namespace detail

} // namespace sbx::graphics
