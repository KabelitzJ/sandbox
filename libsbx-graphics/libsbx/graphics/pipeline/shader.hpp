#ifndef LIBSBX_GRAPHICS_PIPELINE_SHADER_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_SHADER_HPP_

#include <filesystem>
#include <unordered_map>

#include <spirv_cross/spirv_cross.hpp>

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>

namespace sbx::graphics {

class shader : public utility::noncopyable {

public:

  shader(const std::filesystem::path& path, VkShaderStageFlagBits stage);

  ~shader();

  auto handle() const noexcept -> const VkShaderModule&;

  operator const VkShaderModule&() const noexcept;

  auto stage() const noexcept -> VkShaderStageFlagBits;

private:

  VkShaderStageFlagBits _stage{};
  VkShaderModule _handle{};

}; // class shader

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_SHADER_HPP_
