#ifndef LIBSBX_SCENES_SCENE_MODULE_HPP_
#define LIBSBX_SCENES_SCENE_MODULE_HPP_

#include <memory>
#include <optional>
#include <utility>
#include <filesystem>

#include <libsbx/core/module.hpp>

#include <libsbx/scenes/scene.hpp>

namespace sbx::scenes {

class scenes_module final : public core::module<scenes_module> {

  friend class scene;

  inline static const auto is_registered = register_module(stage::normal);

public:

  scenes_module();

  ~scenes_module() override;

  auto update() -> void override;

  auto load_scene(const std::filesystem::path& path) -> scenes::scene&;

  auto scene() -> scenes::scene&;

private:

  std::optional<scenes::scene> _scene;

}; // class scene_modules

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_MODULE_HPP_
