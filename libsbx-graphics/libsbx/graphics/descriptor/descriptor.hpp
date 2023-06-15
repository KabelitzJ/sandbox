#ifndef LIBSBX_GRAPHICS_DESCRIPTOR_DESCRIPTOR_HPP_
#define LIBSBX_GRAPHICS_DESCRIPTOR_DESCRIPTOR_HPP_

#include <variant>
#include <cinttypes>

#include <vulkan/vulkan.hpp>

namespace sbx::graphics {

class write_descriptor_set {

public:

  write_descriptor_set(const VkWriteDescriptorSet& write_descriptor_set, const VkDescriptorImageInfo& descriptor_info) noexcept;

  write_descriptor_set(const VkWriteDescriptorSet& write_descriptor_set, const VkDescriptorBufferInfo& descriptor_info) noexcept;

  auto handle() const noexcept -> const VkWriteDescriptorSet&;

  operator const VkWriteDescriptorSet&() const noexcept;

private:

  VkWriteDescriptorSet _write_descriptor_set;
  std::variant<VkDescriptorImageInfo, VkDescriptorBufferInfo> _descriptor_info;

}; // class write_descriptor_set

class descriptor {

public:

  descriptor() = default;

  virtual ~descriptor() = default;

  virtual auto write_descriptor_set(std::uint32_t binding, VkDescriptorType descriptor_type) const noexcept -> graphics::write_descriptor_set = 0;

}; // class descriptor

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DESCRIPTOR_DESCRIPTOR_HPP_
