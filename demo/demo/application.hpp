#ifndef DEMO_APPLICATION_HPP_
#define DEMO_APPLICATION_HPP_

#include <libsbx/units/units.hpp>
#include <libsbx/utility/utility.hpp>
#include <libsbx/io/io.hpp>
#include <libsbx/math/math.hpp>
#include <libsbx/memory/memory.hpp>
#include <libsbx/signals/signals.hpp>
#include <libsbx/ecs/ecs.hpp>
#include <libsbx/core/core.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp>
#include <libsbx/models/models.hpp>
#include <libsbx/scenes/scenes.hpp>
#include <libsbx/assets/assets.hpp>
#include <libsbx/ui/ui.hpp>
#include <libsbx/physics/physics.hpp>

#include <demo/camera_controller.hpp>
#include <demo/player_controller.hpp>

#include <demo/terrain/mesh.hpp>

namespace demo {

class tank {

public:

  tank(
    const sbx::math::transform& transform,
    std::unordered_map<sbx::utility::hashed_string, sbx::math::uuid>& mesh_ids, 
    std::unordered_map<sbx::utility::hashed_string, sbx::graphics::image_handle>& image_ids
  ) {
    auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();
    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    auto& tank_mesh = assets_module.get_asset<sbx::models::mesh>(mesh_ids["bmp"]);

    auto tank_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
    tank_submeshes.push_back(sbx::scenes::static_mesh::submesh{tank_mesh.submesh_index("turret"), sbx::math::color::white(), sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.1f, 0.8f}, image_ids["bmp_body1_albedo"]});
    tank_submeshes.push_back(sbx::scenes::static_mesh::submesh{tank_mesh.submesh_index("gun_primary"), sbx::math::color::white(), sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.1f, 0.8f}, image_ids["bmp_body1_albedo"]});
    tank_submeshes.push_back(sbx::scenes::static_mesh::submesh{tank_mesh.submesh_index("gun_secondary"), sbx::math::color::white(), sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.1f, 0.8f}, image_ids["bmp_body1_albedo"]});

    tank_submeshes.push_back(sbx::scenes::static_mesh::submesh{tank_mesh.submesh_index("hull"), sbx::math::color::white(), sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.1f, 0.8f}, image_ids["bmp_body2_albedo"]});

    tank_submeshes.push_back(sbx::scenes::static_mesh::submesh{tank_mesh.submesh_index("track_l"), sbx::math::color::white(), sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.1f, 0.8f}, image_ids["bmp_tracks_albedo"]});
    tank_submeshes.push_back(sbx::scenes::static_mesh::submesh{tank_mesh.submesh_index("track_r"), sbx::math::color::white(), sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.1f, 0.8f}, image_ids["bmp_tracks_albedo"]});

    const auto selection_tag = sbx::scenes::selection_tag{};

    _root = scene.create_node("Tank", sbx::math::transform{}, selection_tag);
    scene.get_component<sbx::math::transform>(_root) = transform;

    _hull = scene.create_child_node(_root, "Hull", sbx::math::transform{tank_mesh.submesh_local_transform("hull") * sbx::math::vector4{0, 0, 0, 1}}, selection_tag);
    scene.add_component<sbx::scenes::static_mesh>(_hull, mesh_ids["bmp"], sbx::utility::subrange(tank_submeshes, sbx::utility::offset_count{3u, 1u}));

    _turret = scene.create_child_node(_root, "Turret", sbx::math::transform{tank_mesh.submesh_local_transform("turret") * sbx::math::vector4{0, 0, 0, 1}}, selection_tag);
    scene.add_component<sbx::scenes::static_mesh>(_turret, mesh_ids["bmp"], sbx::utility::subrange(tank_submeshes, sbx::utility::offset_count{0u, 1u}));

    _gun_primary = scene.create_child_node(_turret, "GunPrimary", sbx::math::transform{tank_mesh.submesh_local_transform("gun_primary") * sbx::math::vector4{0, 0, 0, 1}}, selection_tag);
    scene.add_component<sbx::scenes::static_mesh>(_gun_primary, mesh_ids["bmp"], sbx::utility::subrange(tank_submeshes, sbx::utility::offset_count{1u, 1u}));

    _gun_secondary = scene.create_child_node(_turret, "GunSecondary", sbx::math::transform{tank_mesh.submesh_local_transform("gun_secondary") * sbx::math::vector4{0, 0, 0, 1}}, selection_tag);
    scene.add_component<sbx::scenes::static_mesh>(_gun_secondary, mesh_ids["bmp"], sbx::utility::subrange(tank_submeshes, sbx::utility::offset_count{2u, 1u}));

    _track_l = scene.create_child_node(_root, "TrackL", sbx::math::transform{tank_mesh.submesh_local_transform("track_l") * sbx::math::vector4{0, 0, 0, 1}}, selection_tag);
    scene.add_component<sbx::scenes::static_mesh>(_track_l, mesh_ids["bmp"], sbx::utility::subrange(tank_submeshes, sbx::utility::offset_count{4u, 1u}));


    _track_r = scene.create_child_node(_root, "TrackR", sbx::math::transform{tank_mesh.submesh_local_transform("track_r") * sbx::math::vector4{0, 0, 0, 1}}, selection_tag);
    scene.add_component<sbx::scenes::static_mesh>(_track_r, mesh_ids["bmp"], sbx::utility::subrange(tank_submeshes, sbx::utility::offset_count{5u, 1u}));


    _gun_primary_tip = scene.create_child_node(_gun_primary, "GunPrimaryTip", sbx::math::transform{sbx::math::vector3{-10.5f, -0.1f, -0.9f}, sbx::math::quaternion::identity, sbx::math::vector3{0.5f, 0.5f, 0.5f}}, selection_tag);
    scene.add_component<sbx::scenes::static_mesh>(_gun_primary_tip, mesh_ids["cube"], 0u, sbx::math::color::red());
  }

  ~tank() {

  }

  auto update() {
    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    const auto delta_time = sbx::core::engine::delta_time();

    auto gun_elevation_direction = 0;

    if (sbx::devices::input::is_key_down(sbx::devices::key::up)) {
      gun_elevation_direction = 1;
    } else if (sbx::devices::input::is_key_down(sbx::devices::key::down)) {
      gun_elevation_direction = -1;
    }

    _gun_elevation += sbx::math::degree{90} * delta_time * gun_elevation_direction;

    if (_gun_elevation > sbx::math::degree{45}) {
      _gun_elevation = sbx::math::degree{45};
    }

    if (_gun_elevation < sbx::math::degree{-5}) {
      _gun_elevation = sbx::math::degree{-5};
    }

    auto turret_rotation_direction = 0;

    if (sbx::devices::input::is_key_down(sbx::devices::key::left)) {
      turret_rotation_direction = 1;
    } else if (sbx::devices::input::is_key_down(sbx::devices::key::right)) {
      turret_rotation_direction = -1;
    }

    _turret_rotation += sbx::math::degree{90} * delta_time * turret_rotation_direction;

    auto tank_transform = scene.get_component<sbx::math::transform>(_root);
    tank_transform.move_by(sbx::math::vector3{1, 0, 0} * delta_time * 5.0f);

    auto& tank_turret_transform = scene.get_component<sbx::math::transform>(_turret);
    tank_turret_transform.set_rotation(sbx::math::vector3::up, _turret_rotation);

    auto& tank_gun_primary_transform = scene.get_component<sbx::math::transform>(_gun_primary);
    tank_gun_primary_transform.set_rotation(sbx::math::vector3::forward, _gun_elevation);

    auto& tank_gun_secondary_transform = scene.get_component<sbx::math::transform>(_gun_secondary);
    tank_gun_secondary_transform.set_rotation(sbx::math::vector3::forward, _gun_elevation);
  }

private:

  sbx::scenes::node _root;
  sbx::scenes::node _hull;
  sbx::scenes::node _turret;
  sbx::scenes::node _gun_primary;
  sbx::scenes::node _gun_primary_tip;
  sbx::scenes::node _gun_secondary;
  sbx::scenes::node _track_l;
  sbx::scenes::node _track_r;

  sbx::math::degree _turret_rotation;
  sbx::math::degree _gun_elevation;

}; // class tank

class application : public sbx::core::application {

public:

  application();

  ~application() override = default;

  auto update() -> void override;

  auto fixed_update() -> void override;

private:

  // auto _generate_icosphere(const std::float_t radius, const std::uint32_t subdivisions) -> std::unique_ptr<sbx::models::mesh>;

  std::unordered_map<sbx::utility::hashed_string, sbx::graphics::image_handle> _image_ids;
  std::unordered_map<sbx::utility::hashed_string, sbx::graphics::cube_image_handle> _cube_image_ids;
  std::unordered_map<sbx::utility::hashed_string, sbx::math::uuid> _mesh_ids;

  // sbx::units::second _time;
  // std::uint32_t _frames;

  // sbx::memory::observer_ptr<sbx::ui::label> _fps_label;
  // sbx::memory::observer_ptr<sbx::ui::label> _delta_time_label;

  sbx::math::angle _rotation;

  sbx::scenes::node _player;

  camera_controller _camera_controller;
  // player_controller _player_controller;

  sbx::graphics::storage_buffer_handle _selection_buffer;

  std::vector<tank> _tanks;

}; // class application

} // namespace demo

#endif // DEMO_APPLICATION_HPP_
