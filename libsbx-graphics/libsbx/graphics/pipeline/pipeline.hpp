#ifndef LIBSBX_GRAPHICS_PIPELINE_PIPELINE_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_PIPELINE_HPP_

#include <filesystem>
#include <vector>
#include <unordered_map>
#include <memory>

#include <vulkan/vulkan.h>

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/graphics/pipeline/shader.hpp>

namespace sbx::graphics {

class pipeline : public utility::noncopyable {

public:

  pipeline(const std::filesystem::path& path);

  ~pipeline();

  auto handle() const noexcept -> const VkPipeline&;

  operator const VkPipeline&() const noexcept;

private:

  auto _get_stage_from_name(const std::string& name) const noexcept -> VkShaderStageFlagBits;

  VkPipeline _handle{};
  std::vector<std::unique_ptr<shader>> _shaders{};

}; // class pipeline

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_PIPELINE_HPP_
