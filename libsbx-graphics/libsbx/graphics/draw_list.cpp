#include <libsbx/graphics/draw_list.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

draw_list::~draw_list() {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  for (const auto& [name, handle] : _buffers) {
    graphics_module.remove_resource(handle);
  }

  _buffers.clear();

  clear();
}

auto draw_list::buffers() const noexcept -> const storage_buffer_container& {
  return _buffers;
}

auto draw_list::buffer(const utility::hashed_string& name) const -> const storage_buffer& {
  return get_buffer(name);
}

auto draw_list::images() const noexcept -> const separate_image2d_array& {
  return _images;
}

auto draw_list::sampler() const noexcept -> const separate_sampler& {
  return _sampler;
}

auto draw_list::draw_ranges() const noexcept -> const draw_command_range_container& {
  return _draw_ranges;
}

auto draw_list::clear() -> void {
  _draw_ranges.clear();
  _images.clear();
}

auto draw_list::create_buffer(const utility::hashed_string& name, VkDeviceSize size, VkBufferUsageFlags additional_usage) -> void {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
  
  _buffers.emplace(name, graphics_module.add_resource<storage_buffer>(size, additional_usage));
}

auto draw_list::get_buffer(const utility::hashed_string& name) -> storage_buffer& {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  if (const auto entry = _buffers.find(name); entry != _buffers.end()) {
    return graphics_module.get_resource<storage_buffer>(entry->second);
  }

  throw std::runtime_error{fmt::format("draw_list::get_buffer: buffer '{}' not found", name.str())};
}

auto draw_list::get_buffer(const utility::hashed_string& name) const -> const storage_buffer& {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  if (const auto entry = _buffers.find(name); entry != _buffers.end()) {
    return graphics_module.get_resource<storage_buffer>(entry->second);
  }

  throw std::runtime_error{fmt::format("draw_list::get_buffer: buffer '{}' not found", name.str())};
}

auto draw_list::add_image(const image2d_handle& handle) -> std::uint32_t {
  return _images.push_back(handle);
}

auto draw_list::push_draw_command_range(const math::uuid& id, const draw_command_range& range) -> void {
  _draw_ranges.emplace(id, range);
}

} // namespace sbx::graphics
