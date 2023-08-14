#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <unordered_set>

#include <fmt/format.h>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/utility/timer.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/buffer/uniform_buffer.hpp>

#include <libsbx/graphics/render_pass/swapchain.hpp>

namespace sbx::graphics {

graphics_pipeline::graphics_pipeline(const std::filesystem::path& path, const pipeline::stage& stage, const vertex_input_description& vertex_input_description)
: _bind_point{VK_PIPELINE_BIND_POINT_GRAPHICS},
  _stage{stage} {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();
  const auto& render_stage = graphics_module.render_stage(stage);

  auto timer = utility::timer{};

  _name = path.filename().string();

  const auto binary_path = path / "bin";

  if (!std::filesystem::exists(binary_path) && !std::filesystem::is_directory(binary_path)) {
    throw std::runtime_error{"Path does not exist"};
  }

  for (const auto& entry : std::filesystem::directory_iterator(binary_path)) {
    if (entry.is_regular_file()) {
      const auto& file = entry.path();
      
      const auto& stem = file.stem().string();

      const auto stage = _get_stage_from_name(stem);

      if (stage == VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM) {
        throw std::runtime_error{fmt::format("Unsupported shader stage '{}'", stem)};
      }

      _shaders.insert({stage, std::make_unique<shader>(file, stage)});
    }
  }

  if (!_shaders.contains(VK_SHADER_STAGE_VERTEX_BIT)) {
    throw std::runtime_error{"Required vertex shader not found"};
  }

  if (!_shaders.contains(VK_SHADER_STAGE_FRAGMENT_BIT)) {
    throw std::runtime_error{"Required fragment shader not found"};
  }

  auto shader_stages = std::vector<VkPipelineShaderStageCreateInfo>{};

  for (const auto& [stage, shader] : _shaders) {
    auto shader_stage = VkPipelineShaderStageCreateInfo{};

    shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage.stage = stage;
    shader_stage.module = *shader;
    shader_stage.pName = "main";

    shader_stages.push_back(shader_stage);

    const auto& uniforms = shader->uniforms();
    _uniforms.insert(uniforms.begin(), uniforms.end());

    const auto& uniform_blocks = shader->uniform_blocks();
    _uniform_blocks.insert(uniform_blocks.begin(), uniform_blocks.end());
  }

  auto descriptor_set_layout_bindings = std::vector<VkDescriptorSetLayoutBinding>{};
  auto descriptor_pool_sizes_by_type = std::map<VkDescriptorType, std::uint32_t>{};

  for (const auto& [name, uniform_block] : _uniform_blocks) {
    auto descriptor_type = VK_DESCRIPTOR_TYPE_MAX_ENUM;

    switch (uniform_block.buffer_type()) {
      case shader::uniform_block::type::uniform: {
        descriptor_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_set_layout_bindings.push_back(uniform_buffer::create_descriptor_set_layout_binding(uniform_block.binding(), descriptor_type, uniform_block.stage_flags()));
        break;
      }
      case shader::uniform_block::type::push: {
        throw std::runtime_error{"Push constants are not supported yet"};
      }
      case shader::uniform_block::type::storage: {
        throw std::runtime_error{"Storage buffers are not supported yet"};
      }
      default: {
        throw std::runtime_error{"Invalid uniform block type"};
      }
    }

    if (descriptor_type != VK_DESCRIPTOR_TYPE_MAX_ENUM) {
      descriptor_pool_sizes_by_type[descriptor_type] += swapchain::max_frames_in_flight; // ??? 3 ???
    }

    _descriptor_bindings.insert({name, uniform_block.binding()});
    _descriptor_sizes.insert({name, uniform_block.size()});
  }

  for (const auto& [name, uniform] : _uniforms) {
    if (uniform.type() != shader::data_type::sampler2d) {
      throw std::runtime_error{"Unsupported uniform type"};
    }

    const auto descriptor_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    descriptor_set_layout_bindings.push_back(image::create_descriptor_set_layout_binding(uniform.binding(), descriptor_type, uniform.stage_flags()));
    descriptor_pool_sizes_by_type[descriptor_type] += swapchain::max_frames_in_flight; // ??? 3 ???

    _descriptor_bindings.insert({name, uniform.binding()});
  }

  std::ranges::sort(descriptor_set_layout_bindings, [](const auto& lhs, const auto& rhs) {
    return lhs.binding < rhs.binding;
  });

  for (const auto& descriptor : descriptor_set_layout_bindings) {
    _descriptor_type_at_binding.insert({descriptor.binding, descriptor.descriptorType});
  }

  auto descriptor_pool_sizes = std::vector<VkDescriptorPoolSize>{};

  for (const auto& [type, count] : descriptor_pool_sizes_by_type) {
    auto pool_size = VkDescriptorPoolSize{};
    pool_size.type = type;
    pool_size.descriptorCount = count;

    descriptor_pool_sizes.push_back(pool_size);
  }

  auto viewport_state = VkPipelineViewportStateCreateInfo{};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.scissorCount = 1;

  auto rasterization_state = VkPipelineRasterizationStateCreateInfo{};
  rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterization_state.depthClampEnable = false;
  rasterization_state.rasterizerDiscardEnable = false;
  rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
  rasterization_state.lineWidth = 1.0f;
  rasterization_state.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterization_state.depthBiasEnable = false;

  auto multisample_state = VkPipelineMultisampleStateCreateInfo{};
  multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisample_state.sampleShadingEnable = false;
  multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  auto color_blend_attachment = VkPipelineColorBlendAttachmentState{};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = false;

  auto color_blend_state = VkPipelineColorBlendStateCreateInfo{};
  color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_state.logicOpEnable = false;
  color_blend_state.attachmentCount = 1;
  color_blend_state.pAttachments = &color_blend_attachment;

  auto dynamic_states = std::array<VkDynamicState, 2>{
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
  };

  auto dynamic_state = VkPipelineDynamicStateCreateInfo{};
  dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state.dynamicStateCount = static_cast<std::uint32_t>(dynamic_states.size());
  dynamic_state.pDynamicStates = dynamic_states.data();

  auto depth_stencil_state = VkPipelineDepthStencilStateCreateInfo{};
  depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil_state.depthTestEnable = true;
  depth_stencil_state.depthWriteEnable = true;
  depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil_state.depthBoundsTestEnable = false;
  depth_stencil_state.stencilTestEnable = false;

  const auto& binding_descriptions = vertex_input_description.binding_descriptions();

  const auto& attribute_descriptions = vertex_input_description.attribute_descriptions();

  auto vertex_input_state = VkPipelineVertexInputStateCreateInfo{};
  vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_state.vertexBindingDescriptionCount = static_cast<std::uint32_t>(binding_descriptions.size());
  vertex_input_state.pVertexBindingDescriptions = binding_descriptions.data();
  vertex_input_state.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(attribute_descriptions.size());
  vertex_input_state.pVertexAttributeDescriptions = attribute_descriptions.data();

  auto input_assembly_state = VkPipelineInputAssemblyStateCreateInfo{};
  input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_state.primitiveRestartEnable = false;

  auto descriptor_set_layout_create_info = VkDescriptorSetLayoutCreateInfo{};
  descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptor_set_layout_create_info.bindingCount = static_cast<std::uint32_t>(descriptor_set_layout_bindings.size());
  descriptor_set_layout_create_info.pBindings = descriptor_set_layout_bindings.data();

  validate(vkCreateDescriptorSetLayout(logical_device, &descriptor_set_layout_create_info, nullptr, &_descriptor_set_layout));

  auto descriptor_pool_create_info = VkDescriptorPoolCreateInfo{};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  descriptor_pool_create_info.poolSizeCount = static_cast<std::uint32_t>(descriptor_pool_sizes.size());
  descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.data();
  descriptor_pool_create_info.maxSets = swapchain::max_frames_in_flight;

  validate(vkCreateDescriptorPool(logical_device, &descriptor_pool_create_info, nullptr, &_descriptor_pool));

  auto pipeline_layout_create_info = VkPipelineLayoutCreateInfo{};
  pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_create_info.setLayoutCount = 1;
  pipeline_layout_create_info.pSetLayouts = &_descriptor_set_layout;
  pipeline_layout_create_info.pushConstantRangeCount = 0;
  pipeline_layout_create_info.pPushConstantRanges = nullptr;

  validate(vkCreatePipelineLayout(logical_device, &pipeline_layout_create_info, nullptr, &_layout));

  auto subpass = std::uint32_t{0};
  
  auto pipeline_create_info = VkGraphicsPipelineCreateInfo{};
  pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_create_info.stageCount = static_cast<std::uint32_t>(shader_stages.size());
  pipeline_create_info.pStages = shader_stages.data();
  pipeline_create_info.pVertexInputState = &vertex_input_state;
  pipeline_create_info.pInputAssemblyState = &input_assembly_state;
  pipeline_create_info.pViewportState = &viewport_state;
  pipeline_create_info.pRasterizationState = &rasterization_state;
  pipeline_create_info.pMultisampleState = &multisample_state;
  pipeline_create_info.pDepthStencilState = &depth_stencil_state;
  pipeline_create_info.pColorBlendState = &color_blend_state;
  pipeline_create_info.pDynamicState = &dynamic_state;

  pipeline_create_info.layout = _layout;
  pipeline_create_info.renderPass = render_stage.render_pass();
  pipeline_create_info.subpass = subpass;

  pipeline_create_info.basePipelineIndex = -1;
  pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;

  validate(vkCreateGraphicsPipelines(logical_device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &_handle));

  core::logger::debug("sbx::graphics", "Pipeline '{}' created in {}ms", _name, units::quantity_cast<units::millisecond>(timer.elapsed()).value());
}

graphics_pipeline::~graphics_pipeline() {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();

  _shaders.clear();

  logical_device.wait_idle();

  vkDestroyDescriptorPool(logical_device, _descriptor_pool, nullptr);
  vkDestroyDescriptorSetLayout(logical_device, _descriptor_set_layout, nullptr);

  vkDestroyPipelineLayout(logical_device, _layout, nullptr);

  vkDestroyPipeline(logical_device, _handle, nullptr);
}

auto graphics_pipeline::handle() const noexcept -> const VkPipeline& {
  return _handle;
}

auto graphics_pipeline::descriptor_set_layout() const noexcept -> const VkDescriptorSetLayout& {
  return _descriptor_set_layout;
}

auto graphics_pipeline::descriptor_pool() const noexcept -> const VkDescriptorPool& {
  return _descriptor_pool;
}

auto graphics_pipeline::layout() const noexcept -> const VkPipelineLayout& {
  return _layout;
}

auto graphics_pipeline::bind_point() const noexcept -> VkPipelineBindPoint {
  return _bind_point;
}

auto graphics_pipeline::_get_stage_from_name(const std::string& name) const noexcept -> VkShaderStageFlagBits {
  if (name == "vertex") {
    return VK_SHADER_STAGE_VERTEX_BIT;
  } else if (name == "fragment") {
    return VK_SHADER_STAGE_FRAGMENT_BIT;
  } 
  
  return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

} // namespace sbx::graphics
