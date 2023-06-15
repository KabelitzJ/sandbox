#ifndef LIBSBX_GRAPHICS_BUFFER_UNIFORM_BUFFER_HPP_
#define LIBSBX_GRAPHICS_BUFFER_UNIFORM_BUFFER_HPP_

#include <vulkan/vulkan.hpp>

#include <libsbx/graphics/buffer/buffer.hpp>

#include <libsbx/graphics/descriptor/descriptor.hpp>

namespace sbx::graphics {

class uniform_buffer : public buffer, public descriptor {

public:

  uniform_buffer(VkDeviceSize size, const void *data = nullptr);

  auto update(const void *data) const -> void;

  auto write_descriptor_set(std::uint32_t binding, VkDescriptorType descriptor_type) const noexcept -> graphics::write_descriptor_set override;

}; // class uniform_buffer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_BUFFER_UNIFORM_BUFFER_HPP_
