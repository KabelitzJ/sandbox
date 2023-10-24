#ifndef LIBSBX_SHADOWS_SHADOW_SUBRENDERER_HPP_
#define LIBSBX_SHADOWS_SHADOW_SUBRENDERER_HPP_

#include <libsbx/graphics/subrenderer.hpp>

#include <libsbx/shadows/vertex3d.hpp>
#include <libsbx/shadows/pipeline.hpp>

namespace sbx::shadows {

class shadow_subrenderer : public graphics::subrenderer {

public:

  shadow_subrenderer(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : graphics::subrenderer{stage},
    _pipeline{path, stage} { }

  ~shadow_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {

  }

private:

  pipeline _pipeline;

}; // class shadow_subrenderer

} // namespace sbx::shadows

#endif // LIBSBX_SHADOWS_SHADOW_SUBRENDERER_HPP_
