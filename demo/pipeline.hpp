#ifndef DEMO_PIPELINE_HPP_
#define DEMO_PIPELINE_HPP_

#include <filesystem>
#include <unordered_set>
#include <string>

#include <vulkan/vulkan.hpp>

#include <io/file_reader.hpp>
#include <types/primitives.hpp>
#include <utils/noncopyable.hpp>

#include "logical_device.hpp"
#include "swapchain.hpp"


namespace demo {

class pipeline : sbx::noncopyable {

public:

  pipeline(const std::filesystem::path& path, logical_device* logical_device, swapchain* swapchain)
  : _logical_device{logical_device},
    _swapchain{swapchain},
    _pipeline_layout{},
    _handle{} {
    const auto absolute_path = std::filesystem::absolute(path);
    _initialize(absolute_path);
  }

  ~pipeline() {
    _terminate();
  }

  [[nodiscard]] const VkPipelineLayout& layout() const noexcept {
    return _pipeline_layout;
  }

  [[nodiscard]] VkPipeline handle() const noexcept {
    return _handle;
  }

private:

  void _initialize(const std::filesystem::path& path) {      
    const auto shader_stage_create_infos = _create_shader_stage_create_infos(path);

    if (shader_stage_create_infos.empty()) {
      throw std::runtime_error{"Shader has no shader modules: " + path.string()};
    }

    const auto vertex_input_stage_create_info = VkPipelineVertexInputStateCreateInfo {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .vertexBindingDescriptionCount = 0,
      .pVertexBindingDescriptions = nullptr,
      .vertexAttributeDescriptionCount = 0,
      .pVertexAttributeDescriptions = nullptr
    };

    const auto input_assembly_stage_create_info = VkPipelineInputAssemblyStateCreateInfo {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = false
    };

    const auto& swapchain_extent = _swapchain->extent();

    const auto viewport_create_info = VkViewport {
      .x = 0.0f,
      .y = 0.0f,
      .width = static_cast<sbx::float32>(swapchain_extent.width),
      .height = static_cast<sbx::float32>(swapchain_extent.height),
      .minDepth = 0.0f,
      .maxDepth = 1.0f
    };

    const auto scissor_create_info = VkRect2D {
      .offset = {0, 0},
      .extent = swapchain_extent
    };

    const auto viewport_state_create_info = VkPipelineViewportStateCreateInfo {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .viewportCount = 1,
      .pViewport = &viewport_create_info,
      .scissorCount = 1,
      .pScissors = &scissor_create_info
    };

    const auto rasterization_stage_create_info = VkPipelineRasterizationStateCreateInfo {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .depthClampEnable = false,
      .rasterizerDiscardEnable = false,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = VK_CULL_MODE_BACK_BIT,
      .frontFace = VK_FRONT_FACE_CLOCKWISE,
      .depthBiasEnable = false,
      .depthBiasConstantFactor = 0.0f,
      .depthBiasClamp = 0.0f,
      .depthBiasSlopeFactor = 0.0f,
      .lineWidth = 1.0f
    };

    const auto multisample_stage_create_info = VkPipelineMultisampleStateCreateInfo {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = false,
      .minSampleShading = 1.0f,
      .pSampleMask = nullptr,
      .alphaToCoverageEnable = false,
      .alphaToOneEnable = false
    };

    const auto color_blend_attachment_description = VkPipelineColorBlendAttachmentState {
      .blendEnable = false,
      .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
      .colorBlendOp = VK_BLEND_OP_ADD,
      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
      .alphaBlendOp = VK_BLEND_OP_ADD,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };

    const auto color_blend_state_create_info = VkPipelineColorBlendStateCreateInfo {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .logicOpEnable = false,
      .logicOp = VK_LOGIC_OP_COPY,
      .attachmentCount = 1,
      .pAttachments = &color_blend_attachment_description,
      .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
    };

    const auto pipeline_layout_create_info = VkPipelineLayoutCreateInfo {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .setLayoutCount = 0,
      .pSetLayouts = nullptr,
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = nullptr
    };

    if (vkCreatePipelineLayout(_logical_device->handle(), &pipeline_layout_create_info, nullptr, &_pipeline_layout) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create pipeline layout"};
    }

    const auto pipeline_create_info = VkGraphicsPipelineCreateInfo {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stageCount = static_cast<sbx::uint32>(shader_stage_create_infos.size()),
      .pStages = shader_stage_create_infos.data(),
      .pVertexInputState = &vertex_input_stage_create_info,
      .pInputAssemblyState = &input_assembly_stage_create_info,
      .pTessellationState = nullptr,
      .pViewportState = &viewport_state_create_info,
      .pRasterizationState = &rasterization_stage_create_info,
      .pMultisampleState = &multisample_stage_create_info,
      .pDepthStencilState = nullptr,
      .pColorBlendState = &color_blend_state_create_info,
      .pDynamicState = nullptr,
      .layout = _pipeline_layout,
      .renderPass = _swapchain->render_pass(),
      .subpass = 0,
      .basePipelineHandle = nullptr,
      .basePipelineIndex = 0
    };

    if (vkCreateGraphicsPipelines(_logical_device->handle(), nullptr, 1, &pipeline_create_info, nullptr, &_handle) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create graphics pipeline"};
    }

    for (const auto& shader_stage_create_info : shader_stage_create_infos) {
      vkDestroyShaderModule(_logical_device->handle(), shader_stage_create_info.module, nullptr);
    }
  }

  void _terminate() {
    vkDestroyPipeline(_logical_device->handle(), _handle, nullptr);
    vkDestroyPipelineLayout(_logical_device->handle(), _pipeline_layout, nullptr);
  }

  std::vector<VkPipelineShaderStageCreateInfo> _create_shader_stage_create_infos(const std::filesystem::path& path) {
    if (!std::filesystem::is_directory(path)) {
      throw std::runtime_error{"Shader does not exist: " + path.string()};
    }

    const auto shader_binary_path = path / "bin";

    if (!std::filesystem::exists(shader_binary_path)) {
      throw std::runtime_error{"Shader has not been compiled: " + path.string()};
    }

    auto shader_stage_create_infos = std::vector<VkPipelineShaderStageCreateInfo>{};

    for (const auto& entry : std::filesystem::directory_iterator{shader_binary_path}) {
      if (!entry.is_regular_file()) {
        continue;
      }

      if (!entry.path().has_extension() || entry.path().extension() != ".spv") {
        continue;
      }
      
      const auto shader_code = sbx::get_file_contents(entry.path());

      const auto shader_module = _create_shader_module(shader_code);

      const auto stage = _get_shader_stage_flag_from_file_name(entry.path()); 

      const auto shader_stage_create_info = VkPipelineShaderStageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = stage,
        .module = shader_module,
        .pName = "main",
        .pSpecializationInfo = nullptr
      };

      shader_stage_create_infos.push_back(shader_stage_create_info);
    }

    return shader_stage_create_infos;
  }

  VkShaderModule _create_shader_module(const std::vector<char>& shader_code) {
    const auto shader_module_create_info = VkShaderModuleCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .codeSize = shader_code.size(),
      .pCode = reinterpret_cast<const sbx::uint32*>(shader_code.data())
    };

    auto shader_module = VkShaderModule{};

    if (vkCreateShaderModule(_logical_device->handle(), &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create shader module"};
    }

    return shader_module;
  }

  VkShaderStageFlagBits _get_shader_stage_flag_from_file_name(const std::filesystem::path& path) {
    const auto file_name = path.stem().string();

    if (file_name[0] == 'v') {
      return VK_SHADER_STAGE_VERTEX_BIT;
    } else if (file_name[0] == 'f') {
      return VK_SHADER_STAGE_FRAGMENT_BIT;
    } else if (file_name[0] == 't') {
      // Here we unfortunately have to check the full name of the shader.
      if (file_name == "tessellation_evaluation") {
        return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
      } else if (file_name == "tessellation_control") {
        return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
      } else {
        throw std::runtime_error{"Unknown shader stage: " + file_name};
      }
    } else if (file_name[0] == 'g') {
      return VK_SHADER_STAGE_GEOMETRY_BIT;
    } else if (file_name[0] == 'c') {
      return VK_SHADER_STAGE_COMPUTE_BIT;
    } else {
      throw std::runtime_error{"Unknown shader stage: " + file_name};
    }
  }

  logical_device* _logical_device{};
  swapchain* _swapchain{};

  VkPipelineLayout _pipeline_layout{};
  VkPipeline _handle{};

}; // class pipeline

} // namespace demo

#endif // DEMO_PIPELINE_HPP_
