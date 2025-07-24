#ifndef LIBSBX_GRAPHICS_RENDER_GRAPH_HPP_
#define LIBSBX_GRAPHICS_RENDER_GRAPH_HPP_

#include <vector>
#include <variant>
#include <string>
#include <memory>
#include <unordered_map>
#include <optional>
#include <functional>

#include <vulkan/vulkan.h>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/hashed_string.hpp>

#include <libsbx/math/color.hpp>

#include <libsbx/memory/observer_ptr.hpp>

namespace sbx::graphics {

enum class format : std::uint32_t {
  undefined = VK_FORMAT_UNDEFINED,
  r32_sfloat = VK_FORMAT_R32_SFLOAT,
  r32_uint = VK_FORMAT_R32_UINT,
  r64_uint = VK_FORMAT_R64_UINT,
  r32g32_sfloat = VK_FORMAT_R32G32_SFLOAT,
  r32g32_uint = VK_FORMAT_R32G32_UINT,
  r8g8b8a8_unorm = VK_FORMAT_R8G8B8A8_UNORM,
  b8g8r8a8_srgb = VK_FORMAT_B8G8R8A8_SRGB,
  r32g32b32a32_sfloat = VK_FORMAT_R32G32B32A32_SFLOAT
}; // enum class format

enum class address_mode : std::uint32_t {
  repeat = VK_SAMPLER_ADDRESS_MODE_REPEAT,
  clamp_to_edge = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
}; // enum class address_mode

class attachment {

public:

  enum class type {
    image,
    depth,
    storage,
    swapchain
  }; // enum class type

  attachment(const utility::hashed_string& name, type type, const math::color& clear_color = math::color::black(), const format format = format::r8g8b8a8_unorm, const address_mode address_mode = address_mode::repeat) noexcept
  : _name{std::move(name)}, 
    _type{type},
    _clear_color{clear_color},
    _format{format}, 
    _address_mode{address_mode} { }

  auto name() const noexcept -> const utility::hashed_string& {
    return _name;
  }

  auto image_type() const noexcept -> type {
    return _type;
  }

  auto format() const noexcept -> graphics::format {
    return _format;
  }

  auto address_mode() const noexcept -> graphics::address_mode {
    return _address_mode;
  }

  auto clear_color() const noexcept -> const math::color& {
    return _clear_color;
  }

private:

  utility::hashed_string _name;
  type _type;
  bool _is_multi_sampled;
  math::color _clear_color;
  graphics::format _format;
  graphics::address_mode _address_mode;

}; // class attachment

namespace detail {
  
class graphics_node {

  friend class graphics_pass;

public:

  graphics_node(const utility::hashed_string& name)
  : _name(name) { }

  graphics_node(const graphics_node& other) = delete;

  template<typename... Names>
  requires (... && (std::is_same_v<std::remove_cvref_t<Names>, utility::hashed_string> || std::is_constructible_v<utility::hashed_string, Names>))
  void uses(Names&&... names) {
    (_inputs.emplace_back(std::forward<Names>(names)), ...);
  }

  template<typename... Args>
  requires std::is_constructible_v<attachment, Args...>
  void produces(Args&&... args) {
    _outputs.emplace_back(std::forward<Args>(args)...);
  }

private:

  utility::hashed_string _name;

  std::vector<utility::hashed_string> _inputs;
  std::vector<attachment> _outputs;

}; // class graphics_node

class compute_node {

  friend class graphics_pass;

public:

  compute_node(const utility::hashed_string& name)
  : _name(name) { }

  compute_node(const compute_node& other) = delete;

private:

  utility::hashed_string _name;

}; // class compute_node

class compute_pass {

public:

  compute_pass(const utility::hashed_string& name)
  : _name(name) { }

private:

  utility::hashed_string _name;

}; // class compute_pass

class graph_base  {

public:

  template<typename Type, typename... Args>
  auto emplace_back(Args&&... args) -> Type& {
    _nodes.emplace_back(std::in_place_type_t<Type>{}, std::forward<Args>(args)...);
    return std::get<Type>(_nodes.back());
  }

private:

  std::vector<std::variant<graphics_node, compute_pass>> _nodes;

}; // class graph_base

class graphics_pass {

  friend class context;

public:

  auto name() const -> const utility::hashed_string& {
    return _node._name;
  }

  auto attachments() const -> const std::vector<attachment>& {
    return _node._outputs;
  }

private:

  graphics_pass(graphics_node& node)
  : _node{node} { }

  graphics_node& _node;

}; // class graphics_pass

class compute_pass {

  friend class context;

public:

private:

  compute_pass(compute_node& node)
  : _node{node} { }

  compute_node& _node;

}; // class compute_pass

class context {

  friend class graph_builder;

public:

  auto graphics_pass(const utility::hashed_string& name) -> detail::graphics_pass {
    return detail::graphics_pass{_graph.emplace_back<detail::graphics_node>(name)};
  }

  auto compute_pass(const utility::hashed_string& name) -> detail::compute_pass {
    return detail::compute_pass{_graph.emplace_back<detail::compute_node>(name)};
  }

private:

  context(graph_base& graph)
  : _graph{graph} { }

  graph_base& _graph;

}; // class context

class graph_builder {

public:

  graph_builder(graph_base& graph)
  : _graph{graph} { }

  template <typename Callable>
  requires (std::is_invocable_r_v<graphics_pass&, Callable, context&>)
  auto emplace(Callable&& callable) -> graphics_pass& {
    auto ctx = context{_graph};
    return std::invoke(callable, ctx);
  }

  template <typename Callable>
  requires (std::is_invocable_r_v<compute_pass&, Callable, context&>)
  auto emplace(Callable&& callable) -> compute_pass& {
    auto ctx = context{_graph};
    return std::invoke(callable, ctx);
  }

  template<typename... Callables>
  requires (sizeof...(Callables) > 1u)
  auto emplace(Callables&&... callables) -> decltype(auto) {
    return std::tuple{emplace(std::forward<Callables>(callables))...};
  }

  auto build() -> void {
    utility::logger<"graphics">::info("build");
  }

private:

  graph_base& _graph;

}; // class graph_builder

} // namespace detail

class render_graph : public detail::graph_builder {

  using base = detail::graph_builder;

public:

  using graphics_pass = detail::graphics_pass;
  using compute_pass = detail::compute_pass;
  using context = detail::context;

  render_graph() 
  : base{_graph} { }

private:

  detail::graph_base _graph;

}; // class render_graph

} // namespace sbx::graphics

#include <libsbx/graphics/render_graph.ipp>

#endif // LIBSBX_GRAPHICS_RENDER_GRAPH_HPP_
