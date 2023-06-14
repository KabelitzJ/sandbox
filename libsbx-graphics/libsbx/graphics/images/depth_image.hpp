#include <libsbx/graphics/images/image.hpp>

namespace sbx::graphics {

class depth_image : public image {

public:

  depth_image(const math::vector2u& extent, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

}; // class depth_image

} // sbx::graphics
