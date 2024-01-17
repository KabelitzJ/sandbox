#ifndef LIBSBX_POST_FILTERS_DEFAULT_FILTER_HPP_
#define LIBSBX_POST_FILTERS_DEFAULT_FILTER_HPP_

#include <string>

#include <libsbx/post/filter.hpp>

namespace sbx::post {

template<graphics::vertex Vertex>
class default_filter final : public filter<Vertex> {

  using base_type = filter<Vertex>;

public:

  using vertex_type = base_type::vertex_type;
  using pipeline_type = base_type::pipeline_type;

  default_filter(const std::filesystem::path& path, const graphics::pipeline::stage& stage, const std::string& attachment_name)
  : base_type{path, stage},
    _attachment_name{attachment_name} { }

  ~default_filter() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& descriptor_handler = base_type::descriptor_handler();
    auto& pipeline = base_type::pipeline();

    pipeline.bind(command_buffer);

    if (!descriptor_handler.update(pipeline)) {
      return;
    }

    descriptor_handler.bind_descriptors(command_buffer);

    command_buffer.draw(6, 1, 0, 0);
  }

private:

  std::string _attachment_name;

}; // class default_filter

} // namespace sbx::post

#endif // LIBSBX_POST_FILTERS_DEFAULT_FILTER_HPP_
