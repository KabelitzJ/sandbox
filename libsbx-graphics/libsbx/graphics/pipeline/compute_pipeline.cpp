#include <libsbx/graphics/pipeline/compute_pipeline.hpp>

#include <libsbx/utility/timer.hpp>
#include <libsbx/utility/logger.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/buffers/uniform_buffer.hpp>
#include <libsbx/graphics/buffers/storage_buffer.hpp>
#include <libsbx/graphics/images/image.hpp>

namespace sbx::graphics {

compute_pipeline::compute_pipeline(const std::filesystem::path& path)
: _bind_point{VK_PIPELINE_BIND_POINT_COMPUTE} {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();

  auto timer = utility::timer{};

  _name = path.filename().string();

  const auto binary_path = path / "bin";

  if (!std::filesystem::exists(binary_path) && !std::filesystem::is_directory(binary_path)) {
    throw std::runtime_error{fmt::format("Path '{}' does not exist", binary_path.string())};
  }

  for (const auto& entry : std::filesystem::directory_iterator(binary_path)) {
    if (entry.is_regular_file()) {
      const auto& file = entry.path();
      
      const auto& stem = file.stem().string();

      const auto stage = _get_stage_from_name(stem);

      if (stage == VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM) {
        utility::logger<"graphics">::warn("Unsupported shader stage '{}' in compute pipeline '{}'", stem, _name);
        continue;
      }

      _shader = std::make_unique<shader>(file, stage);
      
      break;
    }
  }

  if (!_shader) {
    throw std::runtime_error{"Required compute shader not found"};
  }

  auto shader_stage = VkPipelineShaderStageCreateInfo{};

  shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shader_stage.stage = _shader->stage();
  shader_stage.module = *_shader;
  shader_stage.pName = "main";

  for (const auto& [name, uniform] : _shader->uniforms()) {
    if (auto entry = _uniforms.find(name); entry != _uniforms.end()) {
      entry->second.add_stage_flag(uniform.stage_flags());
    } else {
      _uniforms.insert({name, uniform});
    }
  }

  for (const auto& [name, uniform_block] : _shader->uniform_blocks()) {
    if (auto entry = _uniform_blocks.find(name); entry != _uniform_blocks.end()) {
      entry->second.add_stage_flag(uniform_block.stage_flags());
    } else {
      _uniform_blocks.insert({name, uniform_block});
    }
  }

  auto descriptor_set_layout_bindings = std::map<std::uint32_t, std::vector<VkDescriptorSetLayoutBinding>>{};

  for (const auto& [name, uniform_block] : _uniform_blocks) {
    switch (uniform_block.buffer_type()) {
      case shader::uniform_block::type::uniform: {
        descriptor_set_layout_bindings[uniform_block.set()].push_back(uniform_buffer::create_descriptor_set_layout_binding(uniform_block.binding(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniform_block.stage_flags()));
        break;
      }
      case shader::uniform_block::type::storage: {
        descriptor_set_layout_bindings[uniform_block.set()].push_back(storage_buffer::create_descriptor_set_layout_binding(uniform_block.binding(), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, uniform_block.stage_flags()));
        break;
      }
      case shader::uniform_block::type::push: {
        // [NOTE] KAJ 2024-01-19 : We dont need descriptor sets for push constants but we still want to add them the the bindings and sizes
        break;
      }
      default: {
        utility::logger<"graphics">::warn("Unsupported uniform block type (sbx::graphics::shader::uniform_block::type): {}", uniform_block.buffer_type());
        continue;
      }
    }

    _descriptor_bindings.emplace(name, descriptor_id{uniform_block.set(), uniform_block.binding()});
    _descriptor_sizes.insert({name, uniform_block.size()});
  }

  for (const auto& [name, uniform] : _uniforms) {
    switch (uniform.type()) {
      case shader::data_type::storage_image: {
        descriptor_set_layout_bindings[uniform.set()].push_back(image::create_descriptor_set_layout_binding(uniform.binding(), VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, uniform.stage_flags()));
        break;
      }
      default: {
        utility::logger<"graphics">::warn("Unsupported uniform type (sbx::graphics::shader::data_type): {}", uniform.type());
        continue;
      }
    }
  }

  for (auto& [set, descriptors] : descriptor_set_layout_bindings) {
    std::ranges::sort(descriptors, [](const auto& lhs, const auto& rhs) {
      return lhs.binding < rhs.binding;
    });
  }

  const auto biggest_set = descriptor_set_layout_bindings.rbegin()->first;

  _descriptor_count_in_set.resize(biggest_set + 1u);
  
  for (auto& [set, descriptors] : descriptor_set_layout_bindings) {
    for (const auto& descriptor : descriptors) {
      _descriptor_type_at_binding.emplace(descriptor_id{set, descriptor.binding}, descriptor.descriptorType);
      _descriptor_count_in_set[set].push_back(descriptor.descriptorCount);
    }
  }

  _descriptor_set_layouts.resize(biggest_set, VK_NULL_HANDLE);

  for (const auto& [set, descriptors] : descriptor_set_layout_bindings) {
    auto descriptor_set_layout_create_info = VkDescriptorSetLayoutCreateInfo{};
    descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_create_info.bindingCount = static_cast<std::uint32_t>(descriptors.size());
    descriptor_set_layout_create_info.pBindings = descriptors.data();
  
    validate(vkCreateDescriptorSetLayout(logical_device, &descriptor_set_layout_create_info, nullptr, &_descriptor_set_layouts[set]));
  }

  auto descriptor_pool_sizes = std::vector<VkDescriptorPoolSize>{
    VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2048},
    VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 2048},
    VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2048}
  };

  auto descriptor_pool_create_info = VkDescriptorPoolCreateInfo{};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  descriptor_pool_create_info.poolSizeCount = static_cast<std::uint32_t>(descriptor_pool_sizes.size());
  descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.data();
  descriptor_pool_create_info.maxSets = 8192;

  validate(vkCreateDescriptorPool(logical_device, &descriptor_pool_create_info, nullptr, &_descriptor_pool));

  auto push_constant_ranges = std::vector<VkPushConstantRange>{};

  auto current_offset = std::uint32_t{0u};

  for (const auto& [name, uniform] : _uniform_blocks) {
    if (uniform.buffer_type() != shader::uniform_block::type::push) {
      continue;
    }

    auto push_constant_range = VkPushConstantRange{};
    push_constant_range.stageFlags = uniform.stage_flags();
    push_constant_range.offset = current_offset;
    push_constant_range.size = uniform.size();

    push_constant_ranges.push_back(push_constant_range);

    current_offset += push_constant_range.size;
  }

  auto pipeline_layout_create_info = VkPipelineLayoutCreateInfo{};
  pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_create_info.setLayoutCount = static_cast<std::uint32_t>(_descriptor_set_layouts.size());
  pipeline_layout_create_info.pSetLayouts = _descriptor_set_layouts.data();
  pipeline_layout_create_info.pushConstantRangeCount = static_cast<std::uint32_t>(push_constant_ranges.size());
  pipeline_layout_create_info.pPushConstantRanges = push_constant_ranges.data();

  validate(vkCreatePipelineLayout(logical_device, &pipeline_layout_create_info, nullptr, &_layout));

  auto pipeline_create_info = VkComputePipelineCreateInfo{};
  pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  pipeline_create_info.stage = shader_stage;
  pipeline_create_info.layout = _layout;
  pipeline_create_info.basePipelineIndex = -1;
  pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;

  validate(vkCreateComputePipelines(logical_device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &_handle));

  utility::logger<"graphics">::debug("Pipeline '{}' created in {:.2f}ms", _name, units::quantity_cast<units::millisecond>(timer.elapsed()).value());
}

compute_pipeline::~compute_pipeline() {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();

  _shader.reset();

  logical_device.wait_idle();

  vkDestroyDescriptorPool(logical_device, _descriptor_pool, nullptr);
  
  for (const auto& layout : _descriptor_set_layouts) {
    vkDestroyDescriptorSetLayout(logical_device, layout, nullptr);
  }

  vkDestroyPipelineLayout(logical_device, _layout, nullptr);

  vkDestroyPipeline(logical_device, _handle, nullptr);
}

auto compute_pipeline::_get_stage_from_name(const std::string& name) const noexcept -> VkShaderStageFlagBits {
  if (name == "compute") {
    return VK_SHADER_STAGE_COMPUTE_BIT;
  }
  
  return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

} // namespace sbx::graphics {
