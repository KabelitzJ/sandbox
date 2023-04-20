#include <libsbx/graphics/images/image.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

image::image(VkImageType type, const VkExtent3D& extent, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkFormat format, std::uint32_t mip_levels, std::uint32_t array_layers, VkImageLayout layout)
: _type{type},
  _extent{extent},
  _samples{samples},
  _usage{usage},
  _format{format},
  _mip_levels{mip_levels},
  _array_layers{array_layers},
  _layout{layout} {
  _create_image();
  _allocate_memory();
  _create_image_view();
}

image::~image() {
  auto& logical_device = graphics_module::get().logical_device();

  vkDestroyImageView(logical_device.handle(), _view, nullptr);
  vkFreeMemory(logical_device.handle(), _memory, nullptr);
  vkDestroyImage(logical_device.handle(), _handle, nullptr);
}

auto image::handle() const noexcept -> const VkImage& {
  return _handle;
}

image::operator const VkImage&() const noexcept {
  return _handle;
}

auto image::view() const noexcept -> const VkImageView& {
  return _view;
}

auto image::extent() const noexcept -> const VkExtent3D& {
  return _extent;
}

auto image::format() const noexcept -> VkFormat {
  return _format;
}

auto image::_create_image() -> void {
  auto& logical_device = graphics_module::get().logical_device();
  
  const auto& sharing_mode = logical_device.queue_sharing_mode();

  auto image_create_info = VkImageCreateInfo{};
  image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_create_info.imageType = _type;
  image_create_info.extent = _extent;
  image_create_info.mipLevels = _mip_levels;
  image_create_info.arrayLayers = _array_layers;
  image_create_info.format = _format;
  image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  image_create_info.initialLayout = _layout;
  image_create_info.usage = _usage;
  image_create_info.samples = _samples;
  image_create_info.sharingMode = sharing_mode.mode;
  image_create_info.queueFamilyIndexCount = static_cast<std::uint32_t>(sharing_mode.queue_families.size());
  image_create_info.pQueueFamilyIndices = sharing_mode.queue_families.data();

  validate(vkCreateImage(logical_device, &image_create_info, nullptr, &_handle));
}

auto image::_allocate_memory() -> void {
  auto& logical_device = graphics_module::get().logical_device();
  auto& physical_device = graphics_module::get().physical_device();

  auto image_memory_requirements = VkMemoryRequirements{};
  vkGetImageMemoryRequirements(logical_device, _handle, &image_memory_requirements);

  auto memory_allocate_info = VkMemoryAllocateInfo{};
  memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memory_allocate_info.allocationSize = image_memory_requirements.size;
  memory_allocate_info.memoryTypeIndex = physical_device.find_memory_type(image_memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  validate(vkAllocateMemory(logical_device, &memory_allocate_info, nullptr, &_memory));

  validate(vkBindImageMemory(logical_device, _handle, _memory, 0));
}

auto image::_create_image_view() -> void {
  const auto& logical_device = graphics_module::get().logical_device();

  auto image_view_create_info = VkImageViewCreateInfo{};
  image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_create_info.image = _handle;
  image_view_create_info.viewType = _type == VK_IMAGE_TYPE_2D ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_3D;
  image_view_create_info.format = _format;
  image_view_create_info.subresourceRange.aspectMask = _usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT;
  image_view_create_info.subresourceRange.baseMipLevel = 0;
  image_view_create_info.subresourceRange.levelCount = 1;
  image_view_create_info.subresourceRange.baseArrayLayer = 0;
  image_view_create_info.subresourceRange.layerCount = 1;

  validate(vkCreateImageView(logical_device, &image_view_create_info, nullptr, &_view));
}

} // namespace sbx::graphics
