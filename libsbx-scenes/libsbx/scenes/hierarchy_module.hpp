#ifndef LIBSBX_SCENES_HIERARCHY_MODULE_HPP_
#define LIBSBX_SCENES_HIERARCHY_MODULE_HPP_

#include <easy/profiler.h>

#include <libsbx/core/module.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/math/transform.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scenes/components/hierarchy.hpp>
#include <libsbx/scenes/components/global_transform.hpp>

namespace sbx::scenes {

class hierarchy_module final : public core::module<hierarchy_module> {

  friend class scene;

  inline static const auto is_registered = register_module(stage::post);

  struct stack_entry {
    scenes::node node;
    math::matrix4x4 parent_world;
    bool is_parent_dirty;
  }; // struct stack_entry

public:

  hierarchy_module() {
    
  }

  ~hierarchy_module() override = default;

  auto update() -> void override {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    auto root = scene.root();

    EASY_BLOCK("updating global transforms");

    auto stack = std::vector<stack_entry>{};
    stack.reserve(256u);

    stack.push_back({root, math::matrix4x4::identity, false});

    // auto query = scene.query<const scenes::hierarchy, const math::transform, scenes::global_transform>();

    // for (const auto node : query) {
    //   auto [hierarchy, transform, global_transform] = query.get(node);
    // }

    while (!stack.empty()) {
      const auto [current, parent_matrix, is_parent_dirty] = stack.back();
      stack.pop_back();

      auto& transform = scene.get_component<math::transform>(current);
      auto& global_transform = scene.get_component<scenes::global_transform>(current);
      const auto& hierarchy = scene.get_component<const scenes::hierarchy>(current);

      const auto is_dirty = is_parent_dirty || transform.is_dirty();

      if (is_dirty) {
        global_transform.matrix = parent_matrix * transform.as_matrix();
        transform.clear_is_dirty();
      }

      auto child = hierarchy.first_child;

      while (child != node::null) {
        stack.push_back({child, global_transform.matrix, is_dirty});
        child = scene.get_component<const scenes::hierarchy>(child).next_sibling;
      }
    }

    EASY_END_BLOCK;
  }

}; // class hierarchy_module

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_HIERARCHY_MODULE_HPP_
