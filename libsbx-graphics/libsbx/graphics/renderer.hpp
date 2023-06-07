#ifndef LIBSBX_GRAPHICS_RENDERER_HPP_
#define LIBSBX_GRAPHICS_RENDERER_HPP_

#include <memory>
#include <vector>

#include <libsbx/utility/noncopyable.hpp>
#include <libsbx/utility/concepts.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/subrenderer.hpp>

namespace sbx::graphics {

class renderer : utility::noncopyable {

public:

  renderer() = default;

  virtual ~renderer() = default;

  virtual auto render(command_buffer& command_buffer, std::float_t delta_time) -> void = 0;

protected:

  template<utility::implements<subrenderer> Type, typename... Args>
  auto add_subrenderer(Args&&... args) -> void {
    _subrenderers.push_back(std::make_unique<Type>(std::forward<Args>(args)...));
  }

private:

  std::vector<std::unique_ptr<subrenderer>> _subrenderers;

}; // class renderer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_RENDERER_HPP_
