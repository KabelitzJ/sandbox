#ifndef LIBSBX_GRAPHICS_BUFFERS_STORAGE_BUFFER_HPP_
#define LIBSBX_GRAPHICS_BUFFERS_STORAGE_BUFFER_HPP_

#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/graphics/descriptor/descriptor.hpp>

#include <libsbx/graphics/buffers/buffer.hpp>

namespace sbx::graphics {

class storage_buffer : public descriptor, public buffer  {

public:

  inline static constexpr auto max_elements = 512;

  storage_buffer(VkDeviceSize size, memory::observer_ptr<const void> data = nullptr);

  ~storage_buffer() override;

  auto update(memory::observer_ptr<const void> data, VkDeviceSize size) -> void;

  auto write_descriptor_set(std::uint32_t binding, VkDescriptorType descriptor_type) const noexcept -> graphics::write_descriptor_set override;

  static auto create_descriptor_set_layout_binding(std::uint32_t binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags) noexcept -> VkDescriptorSetLayoutBinding;

private:

  memory::observer_ptr<void> _mapped_memory;

}; // class storage_buffer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_BUFFERS_STORAGE_BUFFER_HPP_

