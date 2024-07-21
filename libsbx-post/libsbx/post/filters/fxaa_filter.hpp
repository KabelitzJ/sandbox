#ifndef LIBSBX_POST_FXAA_FILTER_HPP_
#define LIBSBX_POST_FXAA_FILTER_HPP_

#include <libsbx/post/filter.hpp>

namespace sbx::post {

template<graphics::vertex Vertex>
class fxaa_filter final : public filter<Vertex> {

  using base_type = filter<Vertex>;

public:

  using vertex_type = base_type::vertex_type;

  fxaa_filter(const std::filesystem::path& path, const graphics::pipeline::stage& stage, const std::string& attachment_name)
  : base_type{path, stage},
    _attachment_name{attachment_name} { }

  ~fxaa_filter() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& pipeline = base_type::pipeline();
    auto& descriptor_handler = base_type::descriptor_handler();

    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    descriptor_handler.push("image", graphics_module.attachment(_attachment_name));

    if (!descriptor_handler.update(pipeline)) {
      return;
    }

    pipeline.bind(command_buffer);

    descriptor_handler.bind_descriptors(command_buffer);

    command_buffer.draw(6, 1, 0, 0);
  }

private:

  std::string _attachment_name;

}; // class blur_filter

} // namespace sbx::post 

#endif // LIBSBX_POST_FXAA_FILTER_HPP_
