// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/utility/profiler.hpp>

#include <libsbx/scenes/scene_serializer.hpp>

namespace sbx::scenes {

scenes_module::scenes_module() {

}

scenes_module::~scenes_module() { 

}

auto scenes_module::late_update() -> void {
  SBX_PROFILE_SCOPE("scenes_module::late_update");

  _simulation_time += simulation_delta_time();

  _scene.update();

  // Resyncs any prefab_instance whose source has been edited since this scene last saw it — see
  // scene_serializer::sync_prefab_instances. Runs every frame, editor and runtime alike, same as
  // the transform update above.
  scene_serializer::sync_prefab_instances(_scene);
}

} // namespace sbx::scenes
