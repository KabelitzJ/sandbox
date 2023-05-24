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

  _read_stage_data(compiler, true);
  _read_stage_data(compiler, false);

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

auto shader::_read_stage_data(const spirv_cross::Compiler& compiler, bool inputs) -> void {
  const auto& resources = compiler.get_shader_resources();

  const auto& data = inputs ? resources.stage_inputs : resources.stage_outputs;

  for (auto& element : data) {
    const auto& type = compiler.get_type(element.type_id);
    
    auto type_name = std::string{};

    auto size = type.vecsize;
    auto is_array = type.array.size() > 0;

    switch (type.basetype) {
      case spirv_cross::SPIRType::Float: {
        type_name = "float";
        break;
      }
      case spirv_cross::SPIRType::Int: {
        type_name = "int";
        break;
      }
      case spirv_cross::SPIRType::UInt: {
        type_name = "uint";
        break;
      }
    }

    if (size > 1) {
      type_name += std::to_string(size);
    }

    if (is_array) {
      type_name += "[]";
    }

    auto location = compiler.get_decoration(element.id, spv::DecorationLocation);

    auto& map = inputs ? _inputs : _outputs;

    map[element.name] = stage_data{location, type_name};
  }
}

} // namespace sbx::graphics
