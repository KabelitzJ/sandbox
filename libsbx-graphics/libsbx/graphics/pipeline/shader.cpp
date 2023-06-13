#include <libsbx/graphics/pipeline/shader.hpp>

#include <fstream>

#include <libsbx/io/read_file.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

shader::shader(const std::filesystem::path& path, VkShaderStageFlagBits stage)
: _stage{stage} {
  auto code = io::read_file(path);

  auto compiler = spirv_cross::Compiler{reinterpret_cast<std::uint32_t*>(code.data()), code.size() / 4};

  auto resources = compiler.get_shader_resources();

  const auto& logical_device = graphics_module::get().logical_device();

  auto create_info = VkShaderModuleCreateInfo{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = reinterpret_cast<const std::uint32_t*>(code.data());

  validate(vkCreateShaderModule(logical_device, &create_info, nullptr, &_handle));
}

shader::~shader() {
  const auto& logical_device = graphics_module::get().logical_device();

  vkDestroyShaderModule(logical_device, _handle, nullptr);
}

auto shader::handle() const noexcept -> const VkShaderModule& {
  return _handle;
}

shader::operator const VkShaderModule&() const noexcept {
  return _handle;
}

auto shader::stage() const noexcept -> VkShaderStageFlagBits {
  return _stage;
}

} // namespace sbx::graphics
