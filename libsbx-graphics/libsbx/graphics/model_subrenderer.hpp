#ifndef LIBSBX_GRAPHICS_MODEL_SUBRENDERER_HPP_
#define LIBSBX_GRAPHICS_MODEL_SUBRENDERER_HPP_

#include <libsbx/graphics/subrenderer.hpp>

namespace sbx::graphics {

class model_subrenderer : public subrenderer {

public:

  model_subrenderer(const pipeline::stage& stage)
  : subrenderer{stage} { }

  ~model_subrenderer() override = default;

  auto render(command_buffer& command_buffer, [[maybe_unused]] std::float_t delta_time) -> void override {
    
  }

}; // class model_subrenderer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_MODEL_SUBRENDERER_HPP_
