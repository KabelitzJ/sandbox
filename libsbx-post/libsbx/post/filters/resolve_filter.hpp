#ifndef LIBSBX_POST_FILTERS_RESOLVE_FILTER_HPP_
#define LIBSBX_POST_FILTERS_RESOLVE_FILTER_HPP_

#include <string>
#include <unordered_map>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/post/filter.hpp>

namespace sbx::post {

template<graphics::vertex Vertex>
class resolve_filter final : public filter<Vertex> {

  using base_type = filter<Vertex>;

public:

  using vertex_type = base_type::vertex_type;
  using pipeline_type = base_type::pipeline_type;

  resolve_filter(const std::filesystem::path& path, const graphics::pipeline::stage& stage, std::unordered_map<std::string, std::string>&& attachment_names)
  : base_type{path, stage},
    _attachment_names{std::move(attachment_names)} { }

  ~resolve_filter() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& pipeline = base_type::pipeline();
    auto& descriptor_handler = base_type::descriptor_handler();

    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    pipeline.bind(command_buffer);

    // descriptor_handler.push("image", graphics_module.attachment(_attachment_name));

    for (const auto& [name, attachment] : _attachment_names) {
      descriptor_handler.push(name, graphics_module.attachment(attachment));
    }

    if (!descriptor_handler.update(pipeline)) {
      return;
    }

    descriptor_handler.bind_descriptors(command_buffer);

    command_buffer.draw(6, 1, 0, 0);
  }

private:

  std::unordered_map<std::string, std::string> _attachment_names;

}; // class resolve_filter

} // namespace sbx::post

#endif // LIBSBX_POST_FILTERS_RESOLVE_FILTER_HPP_
