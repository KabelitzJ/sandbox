#ifndef LIBSBX_GRAPHICS_RENDERER_HPP_
#define LIBSBX_GRAPHICS_RENDERER_HPP_

#include <memory>
#include <vector>
#include <typeindex>

#include <libsbx/utility/noncopyable.hpp>
#include <libsbx/utility/concepts.hpp>
#include <libsbx/utility/hash.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/pipeline/pipeline.hpp>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/render_stage.hpp>

namespace sbx::graphics {

class renderer : utility::noncopyable {

public:

  renderer() = default;

  virtual ~renderer() = default;

  virtual auto initialize() -> void = 0;

  auto render(const pipeline::stage& stage, command_buffer& command_buffer) -> void {
    for (const auto& [render_stage, type] : _subrenderer_stages) {
      if (render_stage == stage) {
        _subrenderers[type]->render(command_buffer);
      }
    }
  }

  auto add_render_stage(std::vector<attachment>&& attachments, std::vector<subpass_binding>&& subpass_bindings, const viewport viewport = graphics::viewport{}) -> void {
    _render_stages.push_back(std::make_unique<graphics::render_stage>(std::move(attachments), std::move(subpass_bindings), viewport));
  }

  auto render_stages() const noexcept -> const std::vector<std::unique_ptr<graphics::render_stage>>& {
    return _render_stages;
  }

  auto render_stage(const pipeline::stage& stage) -> graphics::render_stage& {
    return *_render_stages.at(stage.renderpass);
  }

protected:

  template<utility::implements<subrenderer> Type, typename... Args>
  requires (std::is_constructible_v<Type, const pipeline::stage&, Args...>)
  auto add_subrenderer(const pipeline::stage& stage, Args&&... args) -> void {
    const auto type = std::type_index{typeid(Type)};

    _subrenderer_stages.insert({stage, type});

    _subrenderers.insert({type, std::make_unique<Type>(stage, std::forward<Args>(args)...)});
  }

private:

  std::vector<std::unique_ptr<graphics::render_stage>> _render_stages;

  std::unordered_map<std::type_index, std::unique_ptr<subrenderer>> _subrenderers;
  std::multimap<pipeline::stage, std::type_index> _subrenderer_stages;

}; // class renderer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_RENDERER_HPP_
