#ifndef LIBSBX_POST_FXAA_FILTER_HPP_
#define LIBSBX_POST_FXAA_FILTER_HPP_

#include <libsbx/post/filter.hpp>

namespace sbx::post {

class fxaa_filter final : public filter {

  using base_type = filter;

public:

  fxaa_filter(const std::filesystem::path& path, const graphics::pipeline::stage& stage, const std::string& in_image, const std::string& out_image)
  : base_type{path, stage},
    _in_image{in_image},
    _out_image{out_image} { }

  ~fxaa_filter() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& pipeline = base_type::pipeline();
    auto& descriptor_handler = base_type::descriptor_handler();

    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    pipeline.bind(command_buffer);

    descriptor_handler.push("in_image", graphics_module.attachment(_in_image));
    descriptor_handler.push("out_image", graphics_module.attachment(_out_image));

    if (!descriptor_handler.update(pipeline)) {
      return;
    }

    descriptor_handler.bind_descriptors(command_buffer);

    command_buffer.draw(3, 1, 0, 0);
  }

private:

  std::string _in_image;
  std::string _out_image;

}; // class blur_filter

} // namespace sbx::post 

#endif // LIBSBX_POST_FXAA_FILTER_HPP_
