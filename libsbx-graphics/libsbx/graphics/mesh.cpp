#include <libsbx/graphics/mesh.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

mesh::mesh(const std::filesystem::path& path) {
  const auto& logical_device = graphics_module::get().logical_device();

  auto file = YAML::LoadFile(path.string());

  _name = file["name"].as<std::string>();
  _version = file["version"].as<std::string>();

  auto vertices = std::vector<vertex>{};

  for (const auto& node : file["vertices"]) {
    vertices.push_back(node.as<vertex>());
  }

  auto indices = std::vector<std::uint32_t>{};

  for (const auto& index : file["indices"]) {
    indices.push_back(index.as<std::uint32_t>());
  }

  auto fence_create_info = VkFenceCreateInfo{};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

  auto fence = VkFence{};

  validate(vkCreateFence(logical_device, &fence_create_info, nullptr, &fence));

  validate(vkResetFences(logical_device, 1, &fence));

  auto vertex_buffer_size = sizeof(vertex) * vertices.size();
  auto index_buffer_size = sizeof(std::uint32_t) * indices.size();

  auto staging_buffer_size = vertex_buffer_size + index_buffer_size;

  auto staging_buffer = buffer{staging_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

  staging_buffer.write(vertices.data(), vertex_buffer_size);
  staging_buffer.write(indices.data(), index_buffer_size, vertex_buffer_size);

  _vertex_buffer = std::make_unique<buffer>(vertex_buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  _index_buffer = std::make_unique<buffer>(index_buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  auto command_buffer = graphics::command_buffer{};

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

  validate(vkWaitForFences(logical_device, 1, &fence, true, std::numeric_limits<uint64_t>::max()));

	vkDestroyFence(logical_device, fence, nullptr);
}

} // namespace sbx::graphics
