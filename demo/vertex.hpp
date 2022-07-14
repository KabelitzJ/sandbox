#ifndef DEMO_VERTEX_HPP_
#define DEMO_VERTEX_HPP_

#include <array>

#include <vulkan/vulkan.hpp>

#include <math/vector3.hpp>

namespace demo {

class vertex {

public:

  vertex(const sbx::vector3& position, const sbx::vector3& color)
  : _position{position},
    _color{color} { }

  ~vertex() = default;

  static std::array<VkVertexInputBindingDescription, 1> binding_descriptions() {
    const auto binding_description = VkVertexInputBindingDescription{
      .binding = 0,
      .stride = sizeof(vertex),
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    return std::array{binding_description};
  }

  static std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions() {
    const auto position_attribute_description = VkVertexInputAttributeDescription{
      .location = 0,
      .binding = 0,
      .format = VK_FORMAT_R32G32_SFLOAT,
      .offset = offsetof(vertex, _position)
    };

    const auto color_attribute_description = VkVertexInputAttributeDescription{
      .location = 1,
      .binding = 0,
      .format = VK_FORMAT_R32G32_SFLOAT,
      .offset = offsetof(vertex, _color)
    };

    return std::array{position_attribute_description, color_attribute_description};
  }

private:

  sbx::vector3 _position{};
  sbx::vector3 _color{};

}; // class vertex

} // namespace demo

#endif // DEMO_VERTEX_HPP_
