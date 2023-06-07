#include <libsbx/graphics/mesh.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/utility/timer.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

mesh::mesh(const tinyobj::attrib_t& attributes, const tinyobj::shape_t& shape) {
  const auto& logical_device = graphics_module::get().logical_device();

  auto unique_vertices = std::unordered_map<vertex3d, std::uint32_t>{};

  auto vertices = std::vector<vertex3d>{};
  auto indices = std::vector<std::uint32_t>{};

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
      indices.push_back(entry->second);
    } else {
      unique_vertices.insert({vertex, vertices.size()});
      indices.push_back(vertices.size());
      vertices.push_back(vertex);
    }
  }

  auto fence_create_info = VkFenceCreateInfo{};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

  auto fence = VkFence{};

  validate(vkCreateFence(logical_device, &fence_create_info, nullptr, &fence));

  validate(vkResetFences(logical_device, 1, &fence));

  auto vertex_buffer_size = sizeof(vertex3d) * vertices.size();
  auto index_buffer_size = sizeof(std::uint32_t) * indices.size();

  auto staging_buffer_size = vertex_buffer_size + index_buffer_size;

  auto staging_buffer = buffer{staging_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

  staging_buffer.write(vertices.data(), vertex_buffer_size);
  staging_buffer.write(indices.data(), index_buffer_size, vertex_buffer_size);

  _vertex_buffer = std::make_unique<buffer>(vertex_buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false);
  _index_buffer = std::make_unique<buffer>(index_buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false);

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
  validate(vkWaitForFences(logical_device, 1, &fence, true, std::numeric_limits<std::uint64_t>::max()));

	vkDestroyFence(logical_device, fence, nullptr);
}

} // namespace sbx::graphics
