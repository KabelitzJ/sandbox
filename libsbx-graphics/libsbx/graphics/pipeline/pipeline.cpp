#include <libsbx/graphics/pipeline/pipeline.hpp>

#include <unordered_set>

#include <fmt/format.h>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/scoped_timer.hpp>

#include <libsbx/utility/timer.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/mesh.hpp>

#include <libsbx/graphics/pipeline/push_constant.hpp>

namespace sbx::graphics {

pipeline::pipeline(const std::filesystem::path& path) {
  const auto& logical_device = graphics_module::get().logical_device();
  const auto& render_pass = graphics_module::get().render_pass();

  SBX_DEBUG_SCOPED_TIMER([this](const units::second& duration){ core::logger::debug("sbx::graphics", "Loaded pipeline '{}' in {}ms", _name, units::quantity_cast<units::millisecond>(duration).value()); });

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
        throw std::runtime_error{fmt::format("Unknown shader stage '{}'", stem)};
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
  rasterization_state.cullMode = VK_CULL_MODE_NONE;
  rasterization_state.frontFace = VK_FRONT_FACE_CLOCKWISE;
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

  auto vertex_binding_description = std::vector<VkVertexInputBindingDescription>{};

  vertex_binding_description.push_back(VkVertexInputBindingDescription{
    .binding = 0,
    .stride = sizeof(vertex),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
  });

  auto vertex_attribute_descriptions = std::vector<VkVertexInputAttributeDescription>{};

  vertex_attribute_descriptions.push_back(VkVertexInputAttributeDescription{
    .location = 0,
    .binding = 0,
    .format = VK_FORMAT_R32G32_SFLOAT,
    .offset = offsetof(vertex, position)
  });

  vertex_attribute_descriptions.push_back(VkVertexInputAttributeDescription{
    .location = 1,
    .binding = 0,
    .format = VK_FORMAT_R32G32B32A32_SFLOAT,
    .offset = offsetof(vertex, color)
  });

  auto vertex_input_state = VkPipelineVertexInputStateCreateInfo{};
  vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_state.vertexBindingDescriptionCount = static_cast<std::uint32_t>(vertex_binding_description.size());
  vertex_input_state.pVertexBindingDescriptions = vertex_binding_description.data();
  vertex_input_state.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(vertex_attribute_descriptions.size());
  vertex_input_state.pVertexAttributeDescriptions = vertex_attribute_descriptions.data();

  auto input_assembly_state = VkPipelineInputAssemblyStateCreateInfo{};
  input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_state.primitiveRestartEnable = false;

  auto push_constant_range = VkPushConstantRange{};
  push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  push_constant_range.offset = 0;
  push_constant_range.size = sizeof(push_constant);

  auto pipeline_layout_create_info = VkPipelineLayoutCreateInfo{};
  pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_create_info.setLayoutCount = 0;
  pipeline_layout_create_info.pSetLayouts = nullptr;
  pipeline_layout_create_info.pushConstantRangeCount = 1;
  pipeline_layout_create_info.pPushConstantRanges = &push_constant_range;

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
  pipeline_create_info.renderPass = render_pass;
  pipeline_create_info.subpass = subpass;

  pipeline_create_info.basePipelineIndex = -1;
  pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;

  validate(vkCreateGraphicsPipelines(logical_device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &_handle));
}

pipeline::~pipeline() {
  const auto& logical_device = graphics_module::get().logical_device();

  _shaders.clear();

  vkDestroyPipelineLayout(logical_device, _layout, nullptr);

  vkDestroyPipeline(logical_device, _handle, nullptr);
}

auto pipeline::handle() const noexcept -> const VkPipeline& {
  return _handle;
}

pipeline::operator const VkPipeline&() const noexcept {
  return _handle;
}

auto pipeline::layout() const noexcept -> const VkPipelineLayout& {
  return _layout;
}

auto pipeline::_get_stage_from_name(const std::string& name) const noexcept -> VkShaderStageFlagBits {
  if (name == "vertex") {
    return VK_SHADER_STAGE_VERTEX_BIT;
  } else if (name == "fragment") {
    return VK_SHADER_STAGE_FRAGMENT_BIT;
  } 
  
  return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

} // namespace sbx::graphics
