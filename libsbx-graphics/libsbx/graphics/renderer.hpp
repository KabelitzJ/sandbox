#ifndef LIBSBX_GRAPHICS_RENDERER_HPP_
#define LIBSBX_GRAPHICS_RENDERER_HPP_

#include <memory>
#include <vector>
#include <typeindex>

#include <easy/profiler.h>

#include <libsbx/utility/noncopyable.hpp>
#include <libsbx/utility/concepts.hpp>
#include <libsbx/utility/hash.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/pipeline/pipeline.hpp>

#include <libsbx/graphics/task.hpp>
#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/render_stage.hpp>
#include <libsbx/graphics/render_graph.hpp>

namespace sbx::graphics {

class renderer : utility::noncopyable {

public:

  renderer() = default;

  virtual ~renderer() = default;

  virtual auto initialize() -> void = 0;

  auto render(const pipeline::stage& stage, command_buffer& command_buffer) -> void {
    const auto stage_name = fmt::format("Render Stage: {}.{}", stage.renderpass, stage.subpass);
    EASY_BLOCK(stage_name.c_str(), profiler::colors::LightBlue);
    for (auto& [render_stage, subrenderer] : _subrenderers) {
      if (render_stage == stage) {
        subrenderer->render(command_buffer);
      }
    }
    EASY_END_BLOCK;
  }

  auto execute_tasks(command_buffer& command_buffer) -> void {
    for (const auto& task : _tasks) {
      task->execute(command_buffer);
    }
  }

  auto add_render_stage(std::vector<attachment>&& attachments, std::vector<subpass_binding>&& subpass_bindings, const viewport& viewport = graphics::viewport::window()) -> void {
    _render_stages.push_back(std::make_unique<graphics::render_stage>(std::move(attachments), std::move(subpass_bindings), viewport));
  }

  auto render_stages() const noexcept -> const std::vector<std::unique_ptr<graphics::render_stage>>& {
    return _render_stages;
  }

  auto render_stage(const pipeline::stage& stage) -> graphics::render_stage& {
    return *_render_stages.at(stage.renderpass);
  }

protected:

  template<typename Type, typename... Args>
  requires (std::is_constructible_v<Type, const std::filesystem::path&, const pipeline::stage&, Args...>)
  auto add_subrenderer(const std::filesystem::path& path, const pipeline::stage& stage, Args&&... args) -> Type& {
    auto result = _subrenderers.insert({stage, std::make_unique<Type>(path, stage, std::forward<Args>(args)...)});

    return *static_cast<Type*>(result->second.get());
  }

  template<typename Type, typename... Args>
  requires (std::is_constructible_v<Type, std::filesystem::path, Args...>)
  auto add_task(const std::filesystem::path& path, Args&&... args) -> Type& {
    _tasks.push_back(std::make_unique<Type>(path, std::forward<Args>(args)...));

    return *static_cast<Type*>(_tasks.back().get());
  }

private:

  std::vector<std::unique_ptr<graphics::task>> _tasks;

  std::vector<std::unique_ptr<graphics::render_stage>> _render_stages;

  std::multimap<pipeline::stage, std::unique_ptr<subrenderer>> _subrenderers;

  render_graph _graph;

}; // class renderer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_RENDERER_HPP_
