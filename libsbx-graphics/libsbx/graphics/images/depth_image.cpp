#include <libsbx/graphics/images/depth_image.hpp>

namespace sbx::graphics {

static const auto depth_formats = std::vector<VkFormat>{
  VK_FORMAT_D32_SFLOAT_S8_UINT,
  VK_FORMAT_D32_SFLOAT,
  VK_FORMAT_D24_UNORM_S8_UINT,
  VK_FORMAT_D16_UNORM_S8_UINT,
	VK_FORMAT_D16_UNORM
};

depth_image::depth_image(const math::vector2u& extent, VkSampleCountFlagBits samples)
: image{extent, } {
  
}

} // namespace sbx::graphics
