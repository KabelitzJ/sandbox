#ifndef LIBSBX_MODELS_MESH_HPP_
#define LIBSBX_MODELS_MESH_HPP_

#include <filesystem>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

#include <tiny_obj_loader.h>

#include <libsbx/utility/hash.hpp>

#include <libsbx/assets/asset.hpp>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/vector4.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/graphics/pipeline/pipeline.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/graphics/buffer/uniform_handler.hpp>

#include <libsbx/models/vertex3d.hpp>
#include <libsbx/models/buffer.hpp>

namespace sbx::models {

class mesh : public assets::asset<assets::asset_type::mesh> {

public:

  mesh(const std::filesystem::path& path);

  ~mesh() override;

  auto render(graphics::command_buffer& command_buffer) -> void {
    command_buffer.bind_vertex_buffer(0, *_vertex_buffer);
    command_buffer.bind_index_buffer(*_index_buffer, 0, VK_INDEX_TYPE_UINT32);

    command_buffer.draw_indexed(static_cast<std::uint32_t>(_index_buffer->size()), 1, 0, 0, 0);
  }

private:

  std::unique_ptr<vertex_buffer> _vertex_buffer{};
  std::unique_ptr<index_buffer> _index_buffer{};

}; // class mesh

} // namespace sbx::models

#endif // LIBSBX_MODELS_MESH_HPP_
