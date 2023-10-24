#include <libsbx/models/mesh.hpp>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/utility/timer.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::models {

mesh::mesh(const std::filesystem::path& path)
: graphics::mesh<vertex3d>{} {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();

  const auto extension = path.extension().string();

  const auto entry = _loaders().find(extension);

  if (entry == _loaders().end()) {
    throw std::runtime_error{"No loader found for extension: " + extension};
  }

  auto& loader = entry->second;

  auto timer = utility::timer{};

  auto [vertices, indices] = std::invoke(loader, path);

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

  core::logger::debug("Loaded mesh: {}, vertices: {}, indices: {} in {} ms", path.string(), vertices.size(), indices.size(), units::quantity_cast<units::millisecond>(timer.elapsed()).value());
}

mesh::~mesh() {

}

} // namespace sbx::models
