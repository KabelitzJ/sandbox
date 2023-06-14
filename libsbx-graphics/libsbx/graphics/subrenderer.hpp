#ifndef LIBSBX_GRAPHICS_SUBRENDERER_HPP_
#define LIBSBX_GRAPHICS_SUBRENDERER_HPP_

#include <cmath>

#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/pipeline/pipeline.hpp>

namespace sbx::graphics {

class subrenderer {

public:

  subrenderer(const pipeline::stage& stage);

  virtual ~subrenderer() = default;

  virtual auto render(command_buffer& command_buffer, std::float_t delta_time) -> void = 0;

  auto stage() const noexcept -> const pipeline::stage& {
    return _stage;
  }

private:

  pipeline::stage _stage{};

}; // class subrenderer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_SUBRENDERER_HPP_
