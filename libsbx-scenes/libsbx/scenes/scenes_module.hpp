#ifndef LIBSBX_SCENES_SCENE_MODULE_HPP_
#define LIBSBX_SCENES_SCENE_MODULE_HPP_

#include <memory>
#include <optional>
#include <utility>
#include <filesystem>

#include <libsbx/math/vector4.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/core/module.hpp>

#include <libsbx/scenes/scene.hpp>

namespace sbx::scenes {

class scenes_module final : public core::module<scenes_module> {

  friend class scene;

  inline static const auto is_registered = register_module(stage::normal);

public:

  struct line {
    sbx::math::vector4 position;
    sbx::math::color color;
  }; // struct line

  scenes_module();

  ~scenes_module() override;

  auto update() -> void override;

  auto load_scene(const std::filesystem::path& path) -> scenes::scene&;

  auto scene() -> scenes::scene&;

  auto debug_lines() const -> const std::vector<line>& {
    return _debug_lines;
  }

  auto clear_debug_lines() -> void {
    _debug_lines.clear();
  }

  auto add_debug_line(const sbx::math::vector3& start, const sbx::math::vector3& end, const sbx::math::color& color) -> void {
    _debug_lines.push_back(line{
      .position = sbx::math::vector4{start, 1.0f},
      .color = color
    });

    _debug_lines.push_back(line{
      .position = sbx::math::vector4{end, 1.0f},
      .color = color
    });
  }

private:

  std::optional<scenes::scene> _scene;

  std::vector<line> _debug_lines{};

}; // class scene_modules

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_MODULE_HPP_
