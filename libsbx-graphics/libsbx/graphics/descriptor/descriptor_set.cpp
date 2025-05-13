#include <libsbx/graphics/descriptor/descriptor_set.hpp>

#include <array>
#include <cinttypes>
#include <ranges>

#include <range/v3/all.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>
#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

namespace sbx::graphics {

descriptor_set::descriptor_set(const pipeline& pipeline, const std::uint32_t set) noexcept
: _set{set},
  _pipeline_layout{pipeline.layout()},
  _pipeline_bind_point{pipeline.bind_point()},
  _descriptor_pool{pipeline.descriptor_pool()} {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto& logical_device = graphics_module.logical_device();

  const auto descriptor_set_layout = pipeline.descriptor_set_layout(_set);

  const auto layouts = std::array<VkDescriptorSetLayout, 1u>{descriptor_set_layout};

  const auto& descriptor_counts = pipeline.descriptor_counts(_set);

  const auto max_descriptor_count = !descriptor_counts.empty() ? std::ranges::max(descriptor_counts) : 1u;

  // const auto has_variable_descriptors = ranges::any_of(descriptor_counts, [](const auto& entry){ return entry > 1u; });


  auto descriptor_set_variable_descriptor_count_allocate_info = VkDescriptorSetVariableDescriptorCountAllocateInfo{};
  descriptor_set_variable_descriptor_count_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
  descriptor_set_variable_descriptor_count_allocate_info.descriptorSetCount = 1u;
  descriptor_set_variable_descriptor_count_allocate_info.pDescriptorCounts = &max_descriptor_count;

  auto descriptor_set_allocate_info = VkDescriptorSetAllocateInfo{};
  descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descriptor_set_allocate_info.pNext = (max_descriptor_count > 1u ) ? &descriptor_set_variable_descriptor_count_allocate_info : nullptr;
  descriptor_set_allocate_info.descriptorPool = _descriptor_pool;
  descriptor_set_allocate_info.descriptorSetCount = 1u;
  descriptor_set_allocate_info.pSetLayouts = &descriptor_set_layout;

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
