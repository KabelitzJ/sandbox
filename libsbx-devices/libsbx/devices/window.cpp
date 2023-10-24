#include <libsbx/devices/window.hpp>

#include <stb_image.h>

#include <libsbx/core/engine.hpp>

#include <libsbx/assets/assets_module.hpp>

namespace sbx::devices {

auto window::set_icon(const std::filesystem::path& path) -> void {
  auto& assets_module = core::engine::get_module<assets::assets_module>();

  const auto actual_path = assets_module.asset_path(path);

  auto width = 0;
  auto height = 0;

  auto* pixels = stbi_load(actual_path.string().c_str(), &width, &height, nullptr, STBI_rgb_alpha);

  if (!pixels) {
    throw std::runtime_error{fmt::format("Failed to load window icon: {}", actual_path.string())};
  }

  auto icon = GLFWimage{};
  icon.width = width;
  icon.height = height;
  icon.pixels = pixels;

  glfwSetWindowIcon(_handle, 1, &icon);

  stbi_image_free(pixels);
}

} // namespace sbx::devices
