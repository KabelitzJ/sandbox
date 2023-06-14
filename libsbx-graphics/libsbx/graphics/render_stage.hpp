#ifndef LIBSBX_GRAPHICS_RENDER_STAGE_HPP_
#define LIBSBX_GRAPHICS_RENDER_STAGE_HPP_

#include <string>
#include <cinttypes>
#include <optional>

#include <vulkan/vulkan.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/math/color.hpp>
#include <libsbx/math/vector2.hpp>

#include <libsbx/graphics/render_pass/swapchain.hpp>
#include <libsbx/graphics/render_pass/render_pass.hpp>

namespace sbx::graphics {

class attachment {

public:

  enum class type {
    color,
    depth,
    swapchain
  }; // enum class type

  attachment(std::uint32_t binding, std::string name, type type, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, const math::color& clear_color = math::color{0.0f, 0.0f, 0.0f, 1.0f}) noexcept
  : _binding{binding}, 
    _name{std::move(name)}, 
    _type{type}, 
    _format{format}, 
    _clear_color{clear_color} { }

  auto binding() const noexcept -> std::uint32_t {
    return _binding;
  }

  auto name() const noexcept -> const std::string& {
    return _name;
  }

  auto image_type() const noexcept -> type {
    return _type;
  }

  auto format() const noexcept -> VkFormat {
    return _format;
  }

  auto clear_color() const noexcept -> const math::color& {
    return _clear_color;
  }

private:

  std::uint32_t _binding;
  std::string _name;
  type _type;
  VkFormat _format;
  math::color _clear_color;

}; // class attachment

class subpass_binding {

public:

  subpass_binding(std::uint32_t binding, std::vector<std::uint32_t> attachment_bindings) noexcept
  : _binding{binding}, 
    _attachment_bindings{std::move(attachment_bindings)} { }

  auto binding() const noexcept -> std::uint32_t {
    return _binding;
  }

  auto attachment_bindings() const noexcept -> const std::vector<std::uint32_t>& {
    return _attachment_bindings;
  }

private:

  std::uint32_t _binding;
  std::vector<std::uint32_t> _attachment_bindings;

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
    _aspect_ratio{static_cast<std::float_t>(extent.x) / static_cast<std::float_t>(extent.y)} { }

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

  render_stage(std::vector<attachment> attachments, std::vector<subpass_binding> subpass_bindings, const viewport viewport = graphics::viewport{}) noexcept
  : _attachments{std::move(attachments)}, 
    _subpass_bindings{std::move(subpass_bindings)},
    _viewport{viewport},
    _subpass_attachment_count{static_cast<std::uint32_t>(_subpass_bindings.size()), 0} {
    for (const auto& attachment : _attachments) {
      auto clear_value = VkClearValue{};

      auto clear_color = attachment.clear_color();

      switch (attachment.image_type()) {
        case attachment::type::color: {
          clear_value.color = VkClearColorValue{clear_color.r, clear_color.g, clear_color.b, clear_color.a};

          for (const auto& subpass : _subpass_bindings) {
            if (auto bindings = subpass.attachment_bindings(); std::find(bindings.begin(), bindings.end(), attachment.binding()) != bindings.end()) {
              _subpass_attachment_count[subpass.binding()]++;
            }
          }

          break;
        }
        case attachment::type::depth: {
          clear_value.depthStencil = VkClearDepthStencilValue{1.0f, 0};
          _depth_attachment = attachment;
          break;
        }
        case attachment::type::swapchain: {
          clear_value.color = VkClearColorValue{clear_color.r, clear_color.g, clear_color.b, clear_color.a};
          _swapchain_attachment = attachment;
          break;
        }
      }

      _clear_values.push_back(clear_value);
    }
  }

  auto attachments() const noexcept -> const std::vector<attachment>& {
    return _attachments;
  }

  auto subpasses() const noexcept -> const std::vector<subpass_binding>& {
    return _subpass_bindings;
  }

  auto is_outdated() const noexcept -> bool {
    return _is_outdated;
  }

  auto clear_values() const noexcept -> const std::vector<VkClearValue>& {
    return _clear_values;
  }

  auto has_depth_attachment() const noexcept -> bool {
    return _depth_attachment.has_value();
  }

  auto has_swapchain_attachment() const noexcept -> bool {
    return _swapchain_attachment.has_value();
  }

  auto viewport() const noexcept -> const class viewport& {
    return _viewport;
  }

  auto render_area() const noexcept -> const class render_area& {
    return _render_area;
  }

  auto render_pass() const noexcept -> const graphics::render_pass& {
    return *_render_pass;
  }

  auto update() -> void;

  auto rebuild(const swapchain& swapchain) -> void;

  auto framebuffer(std::uint32_t index) const noexcept -> const graphics::framebuffer& {
    return *_framebuffers[index];
  }

private:

  std::vector<attachment> _attachments;
  std::vector<subpass_binding> _subpass_bindings;

  graphics::viewport _viewport;

  std::unique_ptr<graphics::render_pass> _render_pass;

  std::vector<std::uint32_t> _subpass_attachment_count;

  std::vector<std::unique_ptr<graphics::framebuffer>> _framebuffers{};

  std::vector<VkClearValue> _clear_values;

  std::optional<attachment> _depth_attachment;
  std::optional<attachment> _swapchain_attachment;

  graphics::render_area _render_area;

  bool _is_outdated;

}; // class render_stage

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_RENDER_STAGE_HPP_
