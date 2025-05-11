#ifndef DEMO_DEBUG_SUBRENDERER_HPP_
#define DEMO_DEBUG_SUBRENDERER_HPP_

#include <libsbx/graphics/subrenderer.hpp>

namespace demo {

class debug_subrenderer final : public sbx::graphics::subrenderer {

public:

  debug_subrenderer(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
  : sbx::graphics::subrenderer{stage} {

  }

  ~debug_subrenderer() override {

  }

  auto render(sbx::graphics::command_buffer& command_buffer) -> void override {
    
  }

}; // class debug_subrenderer

} // namespace demo

#endif // DEMO_DEBUG_SUBRENDERER_HPP_
