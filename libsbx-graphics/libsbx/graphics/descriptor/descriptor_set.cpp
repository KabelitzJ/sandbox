#include <libsbx/graphics/descriptor/descriptor_set.hpp>

#include <array>
#include <cinttypes>

#include <range/v3/all.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>
#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

namespace sbx::graphics {

descriptor_set::descriptor_set(const pipeline& pipeline, std::uint32_t set) noexcept
: _set{set},
  _pipeline_layout{pipeline.layout()},
  _pipeline_bind_point{pipeline.bind_point()},
  _descriptor_pool{pipeline.descriptor_pool()} {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto& logical_device = graphics_module.logical_device();

  auto descriptor_set_layouts = std::array<VkDescriptorSetLayout, 1u>{};
  descriptor_set_layouts[0] = pipeline.descriptor_set_layout(_set);

  const auto& descriptor_counts = pipeline.descriptor_counts(_set);

  const auto has_variable_descriptors = ranges::any_of(descriptor_counts, [](const auto& counts){ return counts > 1u; });

  auto counts = std::array<std::uint32_t, 1u>{};
  counts[0] = 32u;

  auto descriptor_set_variable_descriptor_count_allocate_info = VkDescriptorSetVariableDescriptorCountAllocateInfo{};
  descriptor_set_variable_descriptor_count_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
  descriptor_set_variable_descriptor_count_allocate_info.descriptorSetCount = static_cast<std::uint32_t>(counts.size());
  descriptor_set_variable_descriptor_count_allocate_info.pDescriptorCounts = counts.data();

  auto descriptor_set_allocate_info = VkDescriptorSetAllocateInfo{};
  descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descriptor_set_allocate_info.pNext = has_variable_descriptors ? &descriptor_set_variable_descriptor_count_allocate_info : nullptr;
  descriptor_set_allocate_info.descriptorPool = _descriptor_pool;
  descriptor_set_allocate_info.descriptorSetCount = static_cast<std::uint32_t>(descriptor_set_layouts.size());
  descriptor_set_allocate_info.pSetLayouts = descriptor_set_layouts.data();

  validate(vkAllocateDescriptorSets(logical_device, &descriptor_set_allocate_info, &_descriptor_set));
}

descriptor_set::~descriptor_set() {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto& logical_device = graphics_module.logical_device();

  logical_device.wait_idle();

  vkFreeDescriptorSets(logical_device, _descriptor_pool, 1, &_descriptor_set);
}

auto descriptor_set::update(const std::vector<VkWriteDescriptorSet>& write_descriptor_sets) -> void {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto& logical_device = graphics_module.logical_device();

  vkUpdateDescriptorSets(logical_device, static_cast<std::uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);
}

auto descriptor_set::handle() const noexcept -> VkDescriptorSet {
  return _descriptor_set;
}

descriptor_set::operator VkDescriptorSet() const noexcept {
  return _descriptor_set;
}

auto descriptor_set::bind(command_buffer& command_buffer) const noexcept -> void {
  vkCmdBindDescriptorSets(command_buffer, _pipeline_bind_point, _pipeline_layout, _set, 1, &_descriptor_set, 0, nullptr);
}

} // namespace sbx::graphics
