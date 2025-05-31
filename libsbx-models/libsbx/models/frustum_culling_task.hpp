#ifndef LIBSBX_MODELS_FRUSTUM_CULLING_TASK_HPP_
#define LIBSBX_MODELS_FRUSTUM_CULLING_TASK_HPP_

#include <libsbx/graphics/task.hpp>
#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/pipeline/compute_pipeline.hpp>

#include <libsbx/graphics/buffers/uniform_handler.hpp>
#include <libsbx/graphics/buffers/push_handler.hpp>
#include <libsbx/graphics/buffers/storage_handler.hpp>

#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

namespace sbx::models {

class frustum_culling_task final : public graphics::task {

  using base = graphics::task;

  struct frustum_buffer {
    sbx::math::vector4 planes[6];
    sbx::math::vector4 corners[8];
    std::uint32_t num_meshes_to_cull;
    std::uint32_t num_visible_meshes;
  }; // struct frustum_buffer

  inline static constexpr auto usage = (VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
  inline static constexpr auto properties = (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

public:

  frustum_culling_task(const std::filesystem::path& path)
  : _pipeline{path},
    _push_handler{_pipeline},
    _bounding_box_buffer{std::make_optional<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)},
    _draw_data_buffer{std::make_optional<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)},
    _frustum_buffer{std::make_optional<graphics::storage_buffer>(std::max(sizeof(frustum_buffer), graphics::storage_buffer::min_size.value()), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)} {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    _draw_commands_buffer = graphics_module.add_resource<graphics::buffer>(graphics::storage_buffer::min_size, usage, properties);
  }

  ~frustum_culling_task() override = default;

  auto draw_commands_buffer() -> graphics::resource_handle<graphics::buffer> {
    return _draw_commands_buffer;
  }

  auto execute(graphics::command_buffer& command_buffer) -> void override {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    auto& logical_device = graphics_module.logical_device();

    auto& draw_commands_buffer = graphics_module.get_resource<graphics::buffer>(_draw_commands_buffer);

    _pipeline.bind(command_buffer);

    // _uniform_handler.push("model", math::matrix4x4::identity);
    // _uniform_handler.push("subdivisions", 6u);

    // _descriptor_handler.push("uniform_parameters", _uniform_handler);
    // _descriptor_handler.push("buffer_out_vertices", _output_vertex_storage_handler);
    // _descriptor_handler.push("buffer_out_indices", _output_index_storage_handler);

    _push_handler.push("draw_commands", draw_commands_buffer.address());
    _push_handler.push("draw_data", _draw_data_buffer->address());
    _push_handler.push("bounding_boxes", _bounding_box_buffer->address());
    _push_handler.push("frustum", _frustum_buffer->address());

    // if (!_descriptor_handler.update(_pipeline)) {
    //   return;
    // }

    // _descriptor_handler.bind_descriptors(command_buffer);
    _push_handler.bind(command_buffer, _pipeline);

    _pipeline.dispatch(command_buffer, {32u, 1u, 1u});

    const auto buffer_memory_barrier = graphics::command_buffer::release_ownership_data{
      .src_stage_mask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      .src_access_mask = VK_ACCESS_SHADER_WRITE_BIT,
      .src_queue_family = logical_device.queue<graphics::queue::type::compute>().family(),
      .dst_queue_family = logical_device.queue<graphics::queue::type::graphics>().family(),
      .buffer = draw_commands_buffer
    };

    command_buffer.release_ownership({buffer_memory_barrier});
  }

private:

  graphics::compute_pipeline _pipeline;

  // sbx::graphics::uniform_handler _uniform_handler;
  std::optional<graphics::storage_buffer> _bounding_box_buffer;
  std::optional<graphics::storage_buffer> _draw_data_buffer;
  // std::optional<graphics::storage_buffer> _draw_command_buffer;
  graphics::resource_handle<graphics::buffer> _draw_commands_buffer;
  std::optional<graphics::storage_buffer> _frustum_buffer;
  // sbx::graphics::storage_handler _output_index_storage_handler;
  graphics::push_handler _push_handler;
  // graphics::descriptor_handler _descriptor_handler;

}; // class frustum_culling_task

} // namespace sbx::models

#endif // LIBSBX_MODELS_FRUSTUM_CULLING_TASK_HPP_
