#include <libsbx/graphics/render_graph.hpp>

namespace sbx::graphics {

namespace detail {

template<typename Type, typename... Args>
auto graph_base::emplace_back(Args&&... args) -> Type& {
  // _nodes.emplace_back(std::in_place_type_t<Type>{}, std::forward<Args>(args)...);
  // return std::get<Type>(_nodes.back());

  if constexpr (std::is_same_v<Type, graphics_node>) {
    _graphics_nodes.emplace_back(std::forward<Args>(args)...);
    return _graphics_nodes.back();
  } else {
    _compute_nodes.emplace_back(std::forward<Args>(args)...);
    return _compute_nodes.back();
  }
}

template<typename... Names>
requires (... && (std::is_same_v<std::remove_cvref_t<Names>, utility::hashed_string> || std::is_constructible_v<utility::hashed_string, Names>))
auto graphics_pass::uses(Names&&... names) -> void {
  (_node._inputs.emplace_back(std::forward<Names>(names)), ...);
}

// template<typename... Args>
// requires (std::is_constructible_v<attachment, Args...>)
// auto patch_attachment(Args&&... args) -> attachment {
//   auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

//   auto value = attachment{std::forward<Args>(args)...};

//   if (value.image_type() == attachment::type::swapchain) {
//     value.format = graphics_module.surface().format();
//   }

//   return value;
// }

template<typename... Args>
requires (std::is_constructible_v<attachment, Args...>)
auto graphics_pass::produces(Args&&... args) -> void {
  _node._outputs.emplace_back(std::forward<Args>(args)...);
}

// template<typename Type, typename... Args>
// requires (std::is_constructible_v<Type, Args...>)
// auto graphics_pass::add_draw_list(const utility::hashed_string& name, Args&&... args) -> Type& {
//   auto result = _graph._draw_lists.emplace(name, std::make_unique<Type>(std::forward<Args>(args)...));
//   return *static_cast<Type*>(result.first->second.get());
// }

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

template<typename Type, typename... Types>
struct count_r {
  inline static constexpr auto value = ((std::is_same_v<Type, Types> ? 1 : 0) + ... + 0);
}; // struct count_r

template<typename Type, typename... Types>
inline constexpr auto count_r_v = count_r<Type, Types...>::value;

template<typename... Callables>
requires (sizeof...(Callables) > 1u)
auto graph_builder::emplace(Callables&&... callables) -> decltype(auto) {
  constexpr auto graphics_pass_count = count_r_v<graphics_pass, std::remove_cvref_t<std::invoke_result_t<Callables, context&>>...>;
  constexpr auto compute_pass_count = count_r_v<compute_pass, std::remove_cvref_t<std::invoke_result_t<Callables, context&>>...>;

  utility::logger<"graphics">::warn("graphics_pass_count: {}", graphics_pass_count);
  utility::logger<"graphics">::warn("compute_pass_count: {}", compute_pass_count);

  _graph.reserve(graphics_pass_count, compute_pass_count);
  _color_images.reserve(graphics_pass_count * 6u); // This is just a guess
  _depth_images.reserve(graphics_pass_count);
  
  return std::tuple{emplace(std::forward<Callables>(callables))...};
}

} // namespace detail

} // namespace sbx::graphics
