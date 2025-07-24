#include <libsbx/graphics/render_graph.hpp>

#include <libsbx/utility/logger.hpp>

#include <libsbx/math/color.hpp>

namespace sbx::graphics {

attachment::attachment(const utility::hashed_string& name, type type, const math::color& clear_color, const graphics::format format, const graphics::address_mode address_mode) noexcept
: _name{std::move(name)}, 
  _type{type},
  _clear_color{clear_color},
  _format{format}, 
  _address_mode{address_mode} { }

auto attachment::name() const noexcept -> const utility::hashed_string& {
  return _name;
}

auto attachment::image_type() const noexcept -> type {
  return _type;
}

auto attachment::format() const noexcept -> graphics::format {
  return _format;
}

auto attachment::address_mode() const noexcept -> graphics::address_mode {
  return _address_mode;
}

auto attachment::clear_color() const noexcept -> const math::color& {
  return _clear_color;
}

namespace detail {

graphics_node::graphics_node(const utility::hashed_string& name)
: _name{name} { }

compute_node::compute_node(const utility::hashed_string& name)
: _name{name} { }

auto graph_base::reserve(const std::size_t size) -> void {
  _nodes.reserve(size);
}

auto graphics_pass::name() const -> const utility::hashed_string& {
  return _node._name;
}

auto graphics_pass::attachments() const -> const std::vector<attachment>& {
  return _node._outputs;
}

graphics_pass::graphics_pass(graphics_node& node)
: _node{node} { }

compute_pass::compute_pass(compute_node& node)
: _node{node} { }

auto context::graphics_pass(const utility::hashed_string& name) -> detail::graphics_pass {
  return detail::graphics_pass{_graph.emplace_back<detail::graphics_node>(name)};
}

auto context::compute_pass(const utility::hashed_string& name) -> detail::compute_pass {
  return detail::compute_pass{_graph.emplace_back<detail::compute_node>(name)};
}

context::context(graph_base& graph)
: _graph{graph} { }

graph_builder::graph_builder(graph_base& graph)
: _graph{graph} { }

auto graph_builder::build() -> void {
  for (const auto& node : _graph._nodes) {
    if (std::holds_alternative<graphics_node>(node)) {
      const auto& graphics_node = std::get<detail::graphics_node>(node);

      
    } else if (std::holds_alternative<compute_node>(node)) {
      const auto& compute_node = std::get<detail::compute_node>(node);

    }
  }
}

} // namespace detail

render_graph::render_graph() 
: base{_graph} { }

} // namespace sbx::graphics
