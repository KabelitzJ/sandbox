#ifndef LIBSBX_PHYSICS_POST_TRANSFORM_MODULE_HPP_
#define LIBSBX_PHYSICS_POST_TRANSFORM_MODULE_HPP_

#include <easy/profiler.h>

#include <libsbx/utility/timer.hpp>

#include <libsbx/core/module.hpp>
#include <libsbx/core/engine.hpp>
#include <libsbx/core/profiler.hpp>

#include <libsbx/math/transform.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scenes/components/hierarchy.hpp>
#include <libsbx/scenes/components/global_transform.hpp>

#include <libsbx/physics/physics_module.hpp>

namespace sbx::physics {

class post_transform_module final : public core::module<post_transform_module> {

  friend class scene;

  inline static const auto is_registered = register_module(stage::post_fixed);

  struct stack_entry {
    scenes::node node;
    math::matrix4x4 parent_world;
    bool is_parent_dirty;
  }; // struct stack_entry

public:

  post_transform_module() {
    
  }

  ~post_transform_module() override = default;

  auto update() -> void override {
    SBX_SCOPED_TIMER("post_transform_module");

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    auto root = scene.root();

    EASY_BLOCK("updating global transforms");

    auto stack = std::vector<stack_entry>{};
    stack.reserve(256u);

    stack.push_back({root, math::matrix4x4::identity, false});

    while (!stack.empty()) {
      const auto [current, parent_matrix, is_parent_dirty] = stack.back();
      stack.pop_back();

      auto& transform = scene.get_component<math::transform>(current);
      auto& global_transform = scene.get_component<scenes::global_transform>(current);
      const auto& hierarchy = scene.get_component<const scenes::hierarchy>(current);

      const auto is_dirty = is_parent_dirty || transform.is_dirty();

      if (is_dirty) {
        global_transform.parent = parent_matrix;
        global_transform.model = parent_matrix * math::matrix_cast<4, 4>(transform);
        global_transform.normal = math::matrix4x4::transposed(math::matrix4x4::inverted(global_transform.model));
        transform.clear_is_dirty();
      }

      auto child = hierarchy.first_child;

      while (child != scenes::node::null) {
        stack.push_back({child, global_transform.model, is_dirty});
        child = scene.get_component<const scenes::hierarchy>(child).next_sibling;
      }
    }

    EASY_END_BLOCK;
  }

}; // class post_transform_module

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_POST_TRANSFORM_MODULE_HPP_
