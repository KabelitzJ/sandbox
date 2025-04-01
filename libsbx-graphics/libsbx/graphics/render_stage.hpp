#ifndef LIBSBX_GRAPHICS_RENDER_STAGE_HPP_
#define LIBSBX_GRAPHICS_RENDER_STAGE_HPP_

#include <string>
#include <cinttypes>
#include <optional>
#include <vector>

#include <vulkan/vulkan.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/math/color.hpp>
#include <libsbx/math/vector2.hpp>

#include <libsbx/graphics/images/depth_image.hpp>
#include <libsbx/graphics/images/image2d.hpp>

#include <libsbx/graphics/render_pass/swapchain.hpp>

namespace sbx::graphics {

enum class format : std::uint32_t {
  undefined = VK_FORMAT_UNDEFINED,
  r32_sfloat = VK_FORMAT_R32_SFLOAT,
  r32g32_sfloat = VK_FORMAT_R32G32_SFLOAT,
  r8g8b8a8_unorm = VK_FORMAT_R8G8B8A8_UNORM,
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
    swapchain
  }; // enum class type

  attachment(const std::uint32_t binding, const std::string& name, type type, const math::color& clear_color = math::color::black(), const format format = format::r8g8b8a8_unorm, const address_mode address_mode = address_mode::repeat) noexcept
  : _binding{binding}, 
    _name{std::move(name)}, 
    _type{type},
    _clear_color{clear_color},
    _format{format}, 
    _address_mode{address_mode} { }

  auto binding() const noexcept -> std::uint32_t {
    return _binding;
  }

  auto name() const noexcept -> const std::string& {
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

  std::uint32_t _binding;
  std::string _name;
  type _type;
  bool _is_multi_sampled;
  math::color _clear_color;
  graphics::format _format;
  graphics::address_mode _address_mode;

}; // class attachment

class subpass_binding {

public:

  subpass_binding(std::uint32_t binding, std::vector<std::uint32_t> color_attachments, std::vector<std::uint32_t> input_attachments = {}) noexcept
  : _binding{binding}, 
    _color_attachments{std::move(color_attachments)},
    _input_attachments{std::move(input_attachments)} { }

  auto binding() const noexcept -> std::uint32_t {
    return _binding;
  }

  auto color_attachments() const noexcept -> const std::vector<std::uint32_t>& {
    return _color_attachments;
  }

  auto input_attachments() const noexcept -> const std::vector<std::uint32_t>& {
    return _input_attachments;
  }

private:

  std::uint32_t _binding;
  std::vector<std::uint32_t> _color_attachments;
  std::vector<std::uint32_t> _input_attachments;

}; // class subpass_binding

class viewport {

public:

  viewport() noexcept
  : _scale{1.0f, 1.0f}, 
    _offset{0, 0}, 
    _size{std::nullopt} { }

  viewport(const math::vector2u& size) noexcept
  : _scale{1.0f, 1.0f}, 
    _offset{0, 0}, 
    _size{size} { }

  auto scale() const noexcept -> const math::vector2f& {
    return _scale;
  }

  auto set_scale(const math::vector2f& scale) noexcept -> void {
    _scale = scale;
  }

  auto offset() const noexcept -> const math::vector2i& {
    return _offset;
  }

  auto set_offset(const math::vector2i& offset) noexcept -> void {
    _offset = offset;
  }

  auto size() const noexcept -> const std::optional<math::vector2u>& {
    return _size;
  }

  auto set_size(const math::vector2u& size) noexcept -> void {
    _size = size;
  }

private:

  math::vector2f _scale;
  math::vector2i _offset;
  std::optional<math::vector2u> _size;

}; // class viewport

class render_area {

public:

  render_area(const math::vector2u& extent = math::vector2u{}, const math::vector2i& offset = math::vector2i{}) noexcept
  : _extent{extent}, 
    _offset{offset}, 
    _aspect_ratio{static_cast<std::float_t>(extent.x()) / static_cast<std::float_t>(extent.y())} { }

  auto operator==(const render_area& other) const noexcept -> bool {
    return _extent == other._extent && _offset == other._offset;
  }

  auto extent() const noexcept -> const math::vector2u& {
    return _extent;
  }

  auto set_extent(const math::vector2u& extent) noexcept -> void {
    _extent = extent;
  }

  auto offset() const noexcept -> const math::vector2i& {
    return _offset;
  }

  auto set_offset(const math::vector2i& offset) noexcept -> void {
    _offset = offset;
  }

  auto aspect_ratio() const noexcept -> std::float_t {
    return _aspect_ratio;
  }

  auto set_aspect_ratio(std::float_t aspect_ratio) noexcept -> void {
    _aspect_ratio = aspect_ratio;
  }

private:

  math::vector2u _extent;
  math::vector2i _offset;
  std::float_t _aspect_ratio;

}; // class render_area

class render_stage {

public:

  render_stage(std::vector<attachment>&& attachments, std::vector<subpass_binding>&& subpass_bindings, const graphics::viewport& viewport = graphics::viewport{});

  ~render_stage();

  auto attachments() const noexcept -> const std::vector<graphics::attachment>&;

  auto find_attachment(const std::string& name) const noexcept -> std::optional<graphics::attachment>;

  auto find_attachment(std::uint32_t binding) const noexcept -> std::optional<graphics::attachment>;

  auto subpasses() const noexcept -> const std::vector<subpass_binding>&;

  auto attachment_count(std::uint32_t subpass) const -> std::uint32_t;

  auto clear_values() const noexcept -> const std::vector<VkClearValue>&;

  auto has_depth_attachment() const noexcept -> bool;

  auto has_swapchain_attachment() const noexcept -> bool;

  auto viewport() const noexcept -> const viewport&;

  auto render_area() const noexcept -> const render_area&;

  auto render_pass() const noexcept -> const VkRenderPass&;

  auto rebuild(const swapchain& swapchain) -> void;

  auto framebuffer(std::uint32_t index) noexcept -> const VkFramebuffer&;

  auto descriptor(const std::string& name) const noexcept -> memory::observer_ptr<const graphics::descriptor>;

  auto descriptors() const noexcept -> const std::map<std::string, memory::observer_ptr<const graphics::descriptor>>&;

private:

  auto _create_render_pass(VkFormat depth_format, VkFormat surface_format) -> void;

  auto _rebuild_framebuffers(const swapchain& swapchain) -> void;

  auto _update_subpass_attachment_counts(const graphics::attachment& attachment) -> void;

  auto _create_attachment_descriptions(VkFormat depth_format, VkFormat surface_format) -> std::vector<VkAttachmentDescription>;

  auto _create_subpass_dependencies() -> std::vector<VkSubpassDependency>;

  std::vector<graphics::attachment> _attachments;
  std::vector<subpass_binding> _subpass_bindings;

  graphics::viewport _viewport;

  VkRenderPass _render_pass;

  std::map<std::string, memory::observer_ptr<const graphics::descriptor>> _descriptors;

  std::unique_ptr<graphics::depth_image> _depth_image;
  std::unordered_map<std::uint32_t, std::unique_ptr<graphics::image2d>> _color_images;

  std::vector<VkFramebuffer> _framebuffers;

  std::vector<VkClearValue> _clear_values;
  std::vector<std::uint32_t> _subpass_attachment_counts;
  std::optional<graphics::attachment> _depth_attachment;
  std::optional<graphics::attachment> _swapchain_attachment;

  graphics::render_area _render_area;

}; // class render_stage

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_RENDER_STAGE_HPP_
