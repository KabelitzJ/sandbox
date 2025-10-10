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
#include <libsbx/graphics/render_graph.hpp>
#include <libsbx/graphics/draw_list.hpp>
#include <libsbx/graphics/viewport.hpp>

namespace sbx::graphics {

class renderer : utility::noncopyable {

public:

  renderer() = default;

  virtual ~renderer() = default;

  // virtual auto initialize() -> void = 0;

  auto render(command_buffer& command_buffer, const swapchain& swapchain) -> void {
    _graph.execute(command_buffer, swapchain, [this, &command_buffer](const auto& pass_name) {
      if (auto entry = _subrenderers.find(pass_name); entry != _subrenderers.end()) {
        for (auto& subrenderer : entry->second) {
          subrenderer->render(command_buffer);
        }
      }
    });
  }

  auto execute_tasks(command_buffer& command_buffer) -> void {
    for (const auto& task : _tasks) {
      task->execute(command_buffer);
    }
  }

  auto resize(const viewport::type flags) -> void {
    _graph.resize(flags);
  }

  auto attachment(const std::string& name) const -> const descriptor& {
    return _graph.attachment(name);
  }

protected:

  template<typename Type, typename... Args>
  requires (std::is_constructible_v<Type, const render_graph::graphics_pass&, Args...>)
  auto add_subrenderer(const render_graph::graphics_pass& pass, Args&&... args) -> Type& {
    auto& subrenderers = _subrenderers[pass.name()];

    subrenderers.emplace_back(std::make_unique<Type>(pass, std::forward<Args>(args)...));

    return *static_cast<Type*>(subrenderers.back().get());
  }

  template<typename Type, typename... Args>
  requires (std::is_constructible_v<Type, Args...>)
  auto add_draw_list(const utility::hashed_string& name, Args&&... args) -> Type& {
    return _graph.add_draw_list<Type>(name, std::forward<Args>(args)...);
  }

  template<typename... Callables>
  requires (sizeof...(Callables) > 1u)
  auto create_graph(Callables&&... callables) -> decltype(auto) {
    auto passes = _graph.emplace(std::forward<Callables>(callables)...);

    _graph.build();

    return passes;
  }

  // template<typename Type, typename... Args>
  // requires (std::is_constructible_v<Type, std::filesystem::path, Args...>)
  // auto add_task(const std::filesystem::path& path, Args&&... args) -> Type& {
  //   _tasks.push_back(std::make_unique<Type>(path, std::forward<Args>(args)...));

  //   return *static_cast<Type*>(_tasks.back().get());
  // }

private:

  std::vector<std::unique_ptr<graphics::task>> _tasks;

  std::unordered_map<utility::hashed_string, std::vector<std::unique_ptr<subrenderer>>> _subrenderers;

  render_graph _graph;

}; // class renderer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_RENDERER_HPP_
