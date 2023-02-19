#include <libsbx/graphics/pipeline/pipeline.hpp>

#include <unordered_set>

#include <fmt/format.h>

namespace sbx::graphics {

pipeline::pipeline(const std::filesystem::path& path) {
  const auto binary_path = path / "bin";

  if (!std::filesystem::exists(binary_path) && !std::filesystem::is_directory(binary_path)) {
    throw std::runtime_error{"Path does not exist"};
  }

  auto found_shader_stages = std::unordered_set<VkShaderStageFlagBits>{};

  for (const auto& entry : std::filesystem::directory_iterator(binary_path)) {
    if (entry.is_regular_file()) {
      const auto& file = entry.path();
      
      const auto& stem = file.stem().string();

      const auto stage = _get_stage_from_name(stem);

      _shaders.push_back(std::make_unique<shader>(file, stage));
      found_shader_stages.insert(stage);
    }
  }

  if (!found_shader_stages.contains(VK_SHADER_STAGE_VERTEX_BIT)) {
    throw std::runtime_error{"Required vertex shader not found"};
  }

  if (!found_shader_stages.contains(VK_SHADER_STAGE_FRAGMENT_BIT)) {
    throw std::runtime_error{"Required fragment shader not found"};
  }

  auto shader_stages = std::vector<VkPipelineShaderStageCreateInfo>{};

  for (const auto& shader : _shaders) {
    auto shader_stage = VkPipelineShaderStageCreateInfo{};

    shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage.stage = shader->stage();
    shader_stage.module = *shader;
    shader_stage.pName = "main";

    shader_stages.push_back(shader_stage);
  }
}

pipeline::~pipeline() {

}

auto pipeline::handle() const noexcept -> const VkPipeline& {
  return _handle;
}

pipeline::operator const VkPipeline&() const noexcept {
  return _handle;
}

auto pipeline::_get_stage_from_name(const std::string& name) const noexcept -> VkShaderStageFlagBits {
  if (name == "vertex") {
    return VK_SHADER_STAGE_VERTEX_BIT;
  } else if (name == "fragment") {
    return VK_SHADER_STAGE_FRAGMENT_BIT;
  }

  throw std::runtime_error{fmt::format("Unknown shader stage: {}", name)};
}

} // namespace sbx::graphics
