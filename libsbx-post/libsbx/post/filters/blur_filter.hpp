#ifndef LIBSBX_POST_FILTERS_BLUR_FILTER_HPP_
#define LIBSBX_POST_FILTERS_BLUR_FILTER_HPP_

#include <libsbx/math/vector2.hpp>

#include <libsbx/post/filter.hpp>

namespace sbx::post {

template<graphics::vertex Vertex>
class blur_filter final : public filter<Vertex> {

  using base_type = filter<Vertex>;

public:

  using vertex_type = base_type::vertex_type;

  blur_filter(const std::filesystem::path& path, const graphics::pipeline::stage& stage, const std::string& attachment_name, const math::vector2& direction)
  : base_type{path, stage},
    _attachment_name{attachment_name},
    _direction{direction} { }

  ~blur_filter() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& pipeline = base_type::pipeline();
    auto& descriptor_handler = base_type::descriptor_handler();

    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
    
    _push_handler.push("direction", _direction);

    descriptor_handler.push("data", _push_handler);
    descriptor_handler.push("image", graphics_module.attachment(_attachment_name));

    if (!descriptor_handler.update(pipeline)) {
      return;
    }

    pipeline.bind(command_buffer);

    descriptor_handler.bind_descriptors(command_buffer);
    _push_handler.bind(command_buffer, pipeline);

    command_buffer.draw(6, 1, 0, 0);
  }

private:

  graphics::push_handler _push_handler;

  std::string _attachment_name;
  math::vector2 _direction;

}; // class blur_filter

} // namespace sbx::post

#endif // LIBSBX_POST_FILTERS_BLUR_FILTER_HPP_