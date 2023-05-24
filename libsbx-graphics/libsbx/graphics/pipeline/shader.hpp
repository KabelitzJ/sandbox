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

  auto _read_stage_data(const spirv_cross::Compiler& compiler, bool inputs) -> void;

  struct stage_data {
    std::uint32_t location;
    std::string type;
  }; // struct stage_data

  VkShaderStageFlagBits _stage{};
  VkShaderModule _handle{};

  std::unordered_map<std::string, stage_data> _inputs;
  std::unordered_map<std::string, stage_data> _outputs;

}; // class shader

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_SHADER_HPP_
