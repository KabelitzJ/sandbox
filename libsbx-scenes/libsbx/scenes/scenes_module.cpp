#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scenes/scene.hpp>

namespace sbx::scenes {

scenes_module::scenes_module()
: _scene{std::nullopt} { }

scenes_module::~scenes_module() {

}

auto scenes_module::update() -> void {

}

auto scenes_module::load_scene(const std::filesystem::path& path) -> scenes::scene& {
  _scene.emplace(path);

  return *_scene;
}

auto scenes_module::scene() -> scenes::scene& {
  return *_scene;
}

} // namespace sbx::scenes
