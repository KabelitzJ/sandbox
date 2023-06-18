#ifndef LIBSBX_GRAPHICS_IMAGES_DEPTH_IMAGE_HPP_
#define LIBSBX_GRAPHICS_IMAGES_DEPTH_IMAGE_HPP_

#include <libsbx/graphics/images/image.hpp>

namespace sbx::graphics {

class depth_image : public image {

public:

  depth_image(const math::vector2u& extent, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

}; // class depth_image

} // sbx::graphics

#endif // LIBSBX_GRAPHICS_IMAGES_DEPTH_IMAGE_HPP_
