#include <libsbx/models/mesh.hpp>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/utility/timer.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::models {

struct mesh_data {
  std::vector<vertex3d> vertices;
  std::vector<std::uint32_t> indices;
}; // struct mesh_data

static auto _load_mesh_data(const std::filesystem::path& path) -> mesh_data {
  auto data = mesh_data{};

  // [NOTE] KAJ 2023-05-29 : Old tinyobjloader API.
  auto attributes = tinyobj::attrib_t{};
  auto shapes = std::vector<tinyobj::shape_t>{};
  auto materials = std::vector<tinyobj::material_t>{};
  auto error = std::string{};
  auto warning = std::string{};

  const auto result = tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.string().c_str(), path.parent_path().string().c_str());

  if (!warning.empty()) {
    core::logger::warn("{}", warning);
  }

  if (!error.empty()) {
    core::logger::error("{}", error);
  }

  if (!result) {
    throw std::runtime_error{fmt::format("Failed to load mesh '{}'", path.string())};
  }

  auto unique_vertices = std::unordered_map<vertex3d, std::uint32_t>{};

  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      auto vertex = vertex3d{};

      vertex.position.x = attributes.vertices[3 * index.vertex_index + 0];
      vertex.position.y = attributes.vertices[3 * index.vertex_index + 1];
      vertex.position.z = attributes.vertices[3 * index.vertex_index + 2];

      vertex.normal.x = attributes.normals[3 * index.normal_index + 0];
      vertex.normal.y = attributes.normals[3 * index.normal_index + 1];
      vertex.normal.z = attributes.normals[3 * index.normal_index + 2];

      vertex.uv.x = attributes.texcoords[2 * index.texcoord_index + 0];
      vertex.uv.y = attributes.texcoords[2 * index.texcoord_index + 1];

      if (auto entry = unique_vertices.find(vertex); entry != unique_vertices.end()) {
        data.indices.push_back(entry->second);
      } else {
        unique_vertices.insert({vertex, data.vertices.size()});
        data.indices.push_back(data.vertices.size());
        data.vertices.push_back(vertex);
      }
    }
  }

  return data;
}

mesh::mesh(const std::filesystem::path& path)
: graphics::mesh<vertex3d>{} {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();

  const auto [vertices, indices] = _load_mesh_data(path);

  auto fence_create_info = VkFenceCreateInfo{};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

  auto fence = VkFence{};

  graphics::validate(vkCreateFence(logical_device, &fence_create_info, nullptr, &fence));

  graphics::validate(vkResetFences(logical_device, 1, &fence));

  auto vertex_buffer_size = sizeof(vertex3d) * vertices.size();
  auto index_buffer_size = sizeof(std::uint32_t) * indices.size();

  auto staging_buffer_size = vertex_buffer_size + index_buffer_size;

  auto staging_buffer = graphics::buffer{staging_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

  staging_buffer.write(vertices.data(), vertex_buffer_size);
  staging_buffer.write(indices.data(), index_buffer_size, vertex_buffer_size);

  _vertex_buffer = std::make_unique<vertex_buffer_type>(vertices.size(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  _index_buffer = std::make_unique<index_buffer_type>(indices.size(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT);

  auto command_buffer = graphics::command_buffer{true, VK_QUEUE_TRANSFER_BIT};

  {
    auto copy_region = VkBufferCopy{};
    copy_region.size = vertex_buffer_size;
    copy_region.dstOffset = 0;
    copy_region.srcOffset = 0;

    command_buffer.copy_buffer(staging_buffer, *_vertex_buffer, copy_region);
  }

  {
    auto copy_region = VkBufferCopy{};
    copy_region.size = index_buffer_size;
    copy_region.dstOffset = 0;
    copy_region.srcOffset = vertex_buffer_size;

    command_buffer.copy_buffer(staging_buffer, *_index_buffer, copy_region);
  }

  command_buffer.submit(nullptr, nullptr, fence);

  // [TODO] KAJ 2023-03-20 20:07 - This forces the CPU to wait for the GPU to finish copying the data to the device local buffer.
  graphics::validate(vkWaitForFences(logical_device, 1, &fence, true, std::numeric_limits<std::uint64_t>::max()));

	vkDestroyFence(logical_device, fence, nullptr);
}

mesh::~mesh() {

}

} // namespace sbx::models
