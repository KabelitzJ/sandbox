// #include <libsbx/graphics/render_graph.hpp>

// namespace sbx::graphics {

// namespace detail {

// template<typename... Args>
// auto graph_base::_emplace_back(Args&&... args) -> graph_node* {
//   base::push_back(std::make_unique<graph_node>(std::forward<Args>(args)...));
//   return back().get();
// }

// template<typename Callable>
// graph_node::graphics_pass_node::graphics_pass_node(Callable&& callable)
// : work{std::forward<Callable>(callable)} { }

// template<typename Callable>
// graph_node::compute_pass_node::compute_pass_node(Callable&& callable)
// : work{std::forward<Callable>(callable)} { }

// template<typename... Args>
// graph_node::graph_node(graphics_pass_tag, Args&&...args)
// : _handle{std::forward<Args>(args)...} { }

// template<typename... Args>
// graph_node::graph_node(compute_pass_tag, Args&&...args)
// : _handle{std::forward<Args>(args)...} { }

// // template<typename... Passes>
// // auto pass::precede(Passes&&... passes) -> pass& {
// //   (_node->_precede(passes._node), ...);
// //   return *this;
// // }

// // template<typename... Passes>
// // auto pass::succeed(Passes&&... passes) -> pass& {
// //   (passes._node->_precede(_node), ...);
// //   return *this;
// // }

// template<typename... Names>
// requires (... && (std::is_same_v<std::decay_t<Names>, std::string> || std::is_constructible_v<std::string, Names>))
// auto graphics_pass::input(Names&&... names) -> void {
//   (_node->_inputs.push_back(std::forward<Names>(names)), ...);
// }

// template<typename... Args>
// requires (std::is_constructible_v<attachment, Args...>)
// auto graphics_pass::output(Args&&... args) -> void {
//   _node->_outputs.emplace_back(std::forward<Args>(args)...);
// }

// template <typename Callable>
// requires (is_graphics_pass_v<Callable>)
// auto graph_builder::emplace(Callable&& callable) -> graphics_pass {
//   return graphics_pass{_graph._emplace_back(graphics_pass_tag{}, std::in_place_type_t<graph_node::graphics_pass_node>{}, std::forward<Callable>(callable) )};
// }

// template <typename Callable>
// requires (is_compute_pass_v<Callable>)
// auto graph_builder::emplace(Callable&& callable) -> compute_pass {
//   return compute_pass{_graph._emplace_back(compute_pass_tag{}, std::in_place_type_t<graph_node::compute_pass_node>{}, std::forward<Callable>(callable) )};
// }

// template<typename... Callables>
// requires (sizeof...(Callables) > 1u)
// auto graph_builder::emplace(Callables&&... callables) -> decltype(auto) {
//   _graph.reserve(sizeof...(Callables));
//   return std::make_tuple(emplace(std::forward<Callables>(callables))...);
// }

// } // namespace detail

// } // namespace sbx::graphics
