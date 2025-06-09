#ifndef LIBSBX_MODELS_FOLIAGE_TASK_HPP_
#define LIBSBX_MODELS_FOLIAGE_TASK_HPP_

#include <libsbx/graphics/task.hpp>
#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/math/transform.hpp>

#include <libsbx/graphics/pipeline/compute_pipeline.hpp>

#include <libsbx/graphics/buffers/uniform_handler.hpp>
#include <libsbx/graphics/buffers/push_handler.hpp>
#include <libsbx/graphics/buffers/storage_handler.hpp>

#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/global_transform.hpp>

namespace sbx::models {

class foliage_task final : public graphics::task {

  using base = graphics::task;

  inline static constexpr auto usage = (VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
  inline static constexpr auto properties = (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

public:

  foliage_task(const std::filesystem::path& path)
  : _pipeline{path},
    _push_handler{_pipeline} {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    _draw_commands_buffer = graphics_module.add_resource<graphics::buffer>(graphics::storage_buffer::min_size, usage, properties);
    // _bounding_box_buffer = graphics_module.add_resource<graphics::buffer>(graphics::storage_buffer::min_size, usage, properties);
    // _draw_data_buffer = graphics_module.add_resource<graphics::buffer>(graphics::storage_buffer::min_size, usage, properties);
    // _frustum_buffer = graphics_module.add_resource<graphics::buffer>(sizeof(frustum_buffer), usage, properties);
  }

  ~foliage_task() override = default;

  auto draw_commands_buffer() -> graphics::resource_handle<graphics::buffer> {
    return _draw_commands_buffer;
  }

  auto execute(graphics::command_buffer& command_buffer) -> void override {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& logical_device = graphics_module.logical_device();

    auto& draw_commands_buffer = graphics_module.get_resource<graphics::buffer>(_draw_commands_buffer);
    // auto& bounding_box_buffer = graphics_module.get_resource<graphics::buffer>(_bounding_box_buffer);
    // auto& draw_data_buffer = graphics_module.get_resource<graphics::buffer>(_draw_data_buffer);
    // auto& frustum_buffer = graphics_module.get_resource<graphics::buffer>(_frustum_buffer);

    // auto& scene = scenes_module.scene();

    // auto camera_node = scene.camera();

    // auto& transform = scene.get_component<math::transform>(camera_node);
    // auto& global_transform = scene.get_component<scenes::global_transform>(camera_node);

    // const auto view = math::matrix4x4::inverted(global_transform.model);

    // auto& camera = scene.get_component<scenes::camera>(camera_node);

    // auto frustum = camera.view_frustum(view);

    _pipeline.bind(command_buffer);

    // _uniform_handler.push("model", math::matrix4x4::identity);
    // _uniform_handler.push("subdivisions", 6u);

    // _descriptor_handler.push("uniform_parameters", _uniform_handler);
    // _descriptor_handler.push("buffer_out_vertices", _output_vertex_storage_handler);
    // _descriptor_handler.push("buffer_out_indices", _output_index_storage_handler);

    _push_handler.push("draw_commands", draw_commands_buffer.address());
    // _push_handler.push("draw_data", draw_data_buffer.address());
    // _push_handler.push("bounding_boxes", bounding_box_buffer.address());
    // _push_handler.push("frustum", frustum_buffer.address());

    _push_handler.bind(command_buffer);

    _pipeline.dispatch(command_buffer, {32u, 1u, 1u});
  }

private:

  graphics::compute_pipeline _pipeline;

  // graphics::buffer_handle _bounding_box_buffer;
  // graphics::buffer_handle _draw_data_buffer;
  graphics::buffer_handle _draw_commands_buffer;
  // graphics::buffer_handle _frustum_buffer;

  graphics::push_handler _push_handler;

}; // class frustum_culling_task

} // namespace sbx::models

#endif // LIBSBX_MODELS_FOLIAGE_TASK_HPP_
