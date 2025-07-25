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

#include <libsbx/graphics/viewport.hpp>

#include <libsbx/graphics/images/image2d.hpp>
#include <libsbx/graphics/images/depth_image.hpp>

#include <libsbx/graphics/render_pass/swapchain.hpp>

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

  attachment(const utility::hashed_string& name, type type, const math::color& clear_color = math::color::black(), const format format = format::r8g8b8a8_unorm, const address_mode address_mode = address_mode::repeat) noexcept;

  auto name() const noexcept -> const utility::hashed_string&;

  auto image_type() const noexcept -> type;

  auto format() const noexcept -> graphics::format;

  auto address_mode() const noexcept -> graphics::address_mode;

  auto clear_color() const noexcept -> const math::color&;

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
  friend class graph_builder;

public:

  graphics_node(const utility::hashed_string& name, const viewport& viewport = viewport::window());

private:

  utility::hashed_string _name;

  viewport _viewport;
  render_area _render_area;

  std::vector<utility::hashed_string> _inputs;
  std::vector<attachment> _outputs;

}; // class graphics_node

class compute_node {

  friend class compute_pass;
  friend class graph_builder;

public:

  compute_node(const utility::hashed_string& name);

private:

  utility::hashed_string _name;

}; // class compute_node

class graph_base  {

  friend class graph_builder;

public:

  template<typename Type, typename... Args>
  auto emplace_back(Args&&... args) -> Type&;

  auto reserve(const std::size_t graphics, const std::size_t compute) -> void;

private:

  std::vector<graphics_node> _graphics_nodes;
  std::vector<compute_node> _compute_nodes;

}; // class graph_base

class graphics_pass {

  friend class context;

public:

  template<typename... Names>
  requires (... && (std::is_same_v<std::remove_cvref_t<Names>, utility::hashed_string> || std::is_constructible_v<utility::hashed_string, Names>))
  auto uses(Names&&... names) -> void;

  template<typename... Args>
  requires (std::is_constructible_v<attachment, Args...>)
  auto produces(Args&&... args) -> void;

  auto name() const -> const utility::hashed_string&;

  auto attachments() const -> const std::vector<attachment>&;

private:

  graphics_pass(graphics_node& node);

  graphics_node& _node;

}; // class graphics_pass

class compute_pass {

  friend class context;

public:

private:

  compute_pass(compute_node& node);

  compute_node& _node;

}; // class compute_pass

class context {

  friend class graph_builder;

public:

  auto graphics_pass(const utility::hashed_string& name) -> detail::graphics_pass;

  auto compute_pass(const utility::hashed_string& name) -> detail::compute_pass;

private:

  context(graph_base& graph); 

  graph_base& _graph;

}; // class context

struct transition_instruction {
  utility::hashed_string attachment;
  VkImageLayout old_layout;
  VkImageLayout new_layout;
}; // struct transition_instruction

struct pass_instruction {
  utility::hashed_string pass_name;
  std::vector<utility::hashed_string> attachments;
}; // struct pass_instruction

using instruction = std::variant<transition_instruction, pass_instruction>;

template<typename... Callables>
struct overload : Callables... {
  using Callables::operator()...;
};

// deduction guide
template<typename... Callables>
overload(Callables...) -> overload<Callables...>;

class graph_builder {

public:

  graph_builder(graph_base& graph);

  virtual ~graph_builder() {
    _clear_attachments();
  }

  template <typename Callable>
  requires (std::is_invocable_r_v<graphics_pass, Callable, context&>)
  auto emplace(Callable&& callable) -> graphics_pass;

  template <typename Callable>
  requires (std::is_invocable_r_v<compute_pass, Callable, context&>)
  auto emplace(Callable&& callable) -> compute_pass;

  template<typename... Callables>
  requires (sizeof...(Callables) > 1u)
  auto emplace(Callables&&... callables) -> decltype(auto);

  auto build() -> void;

  auto resize() -> void;

  auto attachment(const std::string& name) const -> const descriptor&;

  template<typename Callable>
  auto execute(command_buffer& command_buffer, const swapchain& swapchain, Callable&& callable) -> void {
    for (const auto& instruction : _instructions) {
      std::visit(overload{
        [this, &command_buffer, &swapchain](const transition_instruction& instruction) {
          auto& state = _attachment_states.at(instruction.attachment);

          if (state.type == attachment::type::swapchain) {
            image::transition_image_layout(command_buffer, swapchain.image(swapchain.active_image_index()), swapchain.formt(), instruction.old_layout, instruction.new_layout, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1, 0);
          } else {
            image::transition_image_layout(command_buffer, state.image, state.format, instruction.old_layout, instruction.new_layout, (state.type == attachment::type::depth) ? (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT) : VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1, 0);
          }
        },
        [this, &command_buffer, &swapchain, &callable](const pass_instruction& instruction) {
          const auto& area = _pass_render_areas[instruction.pass_name];

          const auto& offset = area.offset();
          const auto& extent = area.extent();

          auto render_area = VkRect2D{};
          render_area.offset = VkOffset2D{offset.x(), offset.y()};
          render_area.extent = VkExtent2D{extent.x(), extent.y()};

          auto viewport = VkViewport{};
          viewport.x = 0.0f;
          viewport.y = 0.0f;
          viewport.width = static_cast<std::float_t>(render_area.extent.width);
          viewport.height = static_cast<std::float_t>(render_area.extent.height);
          viewport.minDepth = 0.0f;
          viewport.maxDepth = 1.0f;

          command_buffer.set_viewport(viewport);

          auto scissor = VkRect2D{};
          scissor.offset = render_area.offset;
          scissor.extent = render_area.extent;
          
          command_buffer.set_scissor(scissor);

          auto color_attachments = std::vector<VkRenderingAttachmentInfo>{};
          auto depth_attachment = std::optional<VkRenderingAttachmentInfo>{};

          for (const auto& attachment : instruction.attachments) {
            const auto& state = _attachment_states[attachment];
            const auto& clear_value = _clear_values[attachment];

            if (state.type == attachment::type::image) {
              auto rendering_attachment_info = VkRenderingAttachmentInfo{};
              rendering_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
              rendering_attachment_info.imageView = state.view;
              rendering_attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
              rendering_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
              rendering_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
              rendering_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
              rendering_attachment_info.clearValue = clear_value;

              color_attachments.push_back(rendering_attachment_info);
            } else if (state.type == attachment::type::depth) {
              auto rendering_attachment_info = VkRenderingAttachmentInfo{};
              rendering_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
              rendering_attachment_info.imageView = state.view;
              rendering_attachment_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
              rendering_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
              rendering_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
              rendering_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
              rendering_attachment_info.clearValue = clear_value;

              depth_attachment = rendering_attachment_info;
            } else if (state.type == attachment::type::swapchain) {
              auto rendering_attachment_info = VkRenderingAttachmentInfo{};
              rendering_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
              rendering_attachment_info.imageView = swapchain.image_view(swapchain.active_image_index());
              rendering_attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
              rendering_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
              rendering_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
              rendering_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
              rendering_attachment_info.clearValue = clear_value;

              color_attachments.push_back(rendering_attachment_info);
            }
          }

          auto rendering_info = VkRenderingInfo{};
          rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
          rendering_info.renderArea = render_area;
          rendering_info.layerCount = 1;
          rendering_info.colorAttachmentCount = static_cast<std::uint32_t>(color_attachments.size());
          rendering_info.pColorAttachments = color_attachments.data();
          rendering_info.pDepthAttachment = depth_attachment.has_value() ? &depth_attachment.value() : nullptr;
          rendering_info.pStencilAttachment = depth_attachment.has_value() ? &depth_attachment.value() : nullptr;

          command_buffer.begin_rendering(rendering_info);

          std::invoke(callable, instruction.pass_name);

          command_buffer.end_rendering();
        }
      }, instruction);
    }
  }

private:

  struct attachment_state {
    VkImage image;
    VkImageView view;
    VkImageLayout current_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkFormat format;
    VkExtent2D extent;
    attachment::type type;
  }; // struct attachment_state

  auto _update_viewports() -> void;

  auto _clear_attachments() -> void;

  auto _create_attachments(const graphics_node& node) -> void;

  graph_base& _graph;

  std::unordered_map<utility::hashed_string, image2d_handle> _color_images;
  std::unordered_map<utility::hashed_string, depth_image_handle> _depth_images;
  std::unordered_map<utility::hashed_string, VkClearValue> _clear_values;

  std::vector<instruction> _instructions;

  std::unordered_map<utility::hashed_string, attachment_state> _attachment_states;

  std::unordered_map<utility::hashed_string, render_area> _pass_render_areas;

}; // class graph_builder

} // namespace detail

class render_graph : public detail::graph_builder {

  using base = detail::graph_builder;

public:

  using graphics_pass = detail::graphics_pass;
  using compute_pass = detail::compute_pass;
  using context = detail::context;

  render_graph();

  ~render_graph() override = default;

private:

  detail::graph_base _graph;

}; // class render_graph

} // namespace sbx::graphics

#include <libsbx/graphics/render_graph.ipp>

#endif // LIBSBX_GRAPHICS_RENDER_GRAPH_HPP_
