#include <libsbx/graphics/mesh.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include <tiny_obj_loader.h>

#include <libsbx/core/logger.hpp>

#include <libsbx/utility/timer.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

mesh::mesh(const std::filesystem::path& path) {
  const auto& logical_device = graphics_module::get().logical_device();

  auto timer = utility::timer{};

  auto vertices = std::vector<vertex>{};
  auto indices = std::vector<std::uint32_t>{};

  // [NOTE] KAJ 2023-05-29 : Old tinyobjloader API.
  auto attributes = tinyobj::attrib_t{};
  auto shapes = std::vector<tinyobj::shape_t>{};
  auto materials = std::vector<tinyobj::material_t>{};
  auto error = std::string{};
  auto warning = std::string{};

  const auto result = tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.string().c_str(), path.parent_path().string().c_str());

  if (!warning.empty()) {
    core::logger::warn("sbx::graphics", "{}", warning);
  }

  if (!error.empty()) {
    core::logger::error("sbx::graphics", "{}", error);
  }

  if (!result) {
    throw std::runtime_error{fmt::format("Failed to load mesh '{}'", path.string())};
  }

  _name = path.stem().string();

  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      auto new_vertex = vertex{};

      new_vertex.position.x = attributes.vertices[3 * index.vertex_index + 0];
      new_vertex.position.y = attributes.vertices[3 * index.vertex_index + 1];
      new_vertex.position.z = attributes.vertices[3 * index.vertex_index + 2];

      new_vertex.normal.x = attributes.normals[3 * index.normal_index + 0];
      new_vertex.normal.y = attributes.normals[3 * index.normal_index + 1];
      new_vertex.normal.z = attributes.normals[3 * index.normal_index + 2];

      new_vertex.uv.x = attributes.texcoords[2 * index.texcoord_index + 0];
      new_vertex.uv.y = attributes.texcoords[2 * index.texcoord_index + 1];

      new_vertex.color = math::color{0.87f, 0.21f, 0.12f, 1.0f};

      vertices.push_back(new_vertex);
      indices.push_back(indices.size());
    }
  }

  // [NOTE] KAJ 2023-05-29 : Maybe use YAML instead of tinyobjloader?
  // auto file = YAML::LoadFile(path.string());
  
  // _name = file["name"].as<std::string>();
  // _version = file["version"].as<std::string>();

  // auto positions = file["positions"].as<std::vector<math::vector3>>();
  // auto colors = file["colors"].as<std::vector<math::color>>();
  // auto normals = file["normals"].as<std::vector<math::vector3>>();
  // auto uvs = file["uvs"].as<std::vector<math::vector2>>();

  // auto vertices = std::vector<vertex>{};

  // for (const auto& node : file["vertices"]) {
  //   const auto position_index = node["position"].as<std::size_t>();
  //   const auto& position = positions[position_index];

  //   const auto color_index = node["color"].as<std::size_t>();
  //   const auto& color = colors[color_index];

  //   const auto normal_index = node["normal"].as<std::size_t>();
  //   const auto& normal = normals[normal_index];

  //   const auto uv_index = node["uv"].as<std::size_t>();
  //   const auto& uv = uvs[uv_index];

  //   vertices.push_back(vertex{position, color, normal, uv});
  // }

  // auto indices = file["indices"].as<std::vector<std::uint32_t>>();

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

  core::logger::debug("sbx::graphics", "Mesh '{}' with {} vertices and {} indices created in {}ms", _name, vertices.size(), indices.size(), units::quantity_cast<units::millisecond>(timer.elapsed()).value());
}

} // namespace sbx::graphics
