#include <libsbx/graphics/model.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include <tiny_obj_loader.h>

#include <libsbx/core/logger.hpp>

#include <libsbx/utility/timer.hpp>

namespace sbx::graphics {

model::model(const std::filesystem::path& path) {
  auto timer = utility::timer{};

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

  core::logger::debug("sbx::graphics", "meshes: {}", shapes.size());
  core::logger::debug("sbx::graphics", "materials: {}", materials.size());

  _mesh = std::make_unique<graphics::mesh>(attributes, shapes[0]);
  _material = std::make_unique<graphics::material>(attributes, materials[0]);

  core::logger::debug("sbx::graphics", "Loaded mesh '{}' in {} ms", path.string(), timer.elapsed().value());
}

} // namespace sbx::graphics
