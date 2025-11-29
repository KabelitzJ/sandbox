#ifndef LIBSBX_POST_FILTERS_TONEMAP_FILTER_HPP_
#define LIBSBX_POST_FILTERS_TONEMAP_FILTER_HPP_

#include <libsbx/utility/enum.hpp>

#include <libsbx/math/vector2.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/buffers/push_handler.hpp>

#include <libsbx/post/filter.hpp>

namespace sbx::post {

class tonemap_filter final : public filter {

  using base = filter;

public:

  tonemap_filter(const graphics::render_graph::graphics_pass& pass, const std::filesystem::path& path, std::vector<std::pair<std::string, std::string>>&& attachment_names, std::float_t exposure = 1.0f, std::float_t bloom_mix = 0.4f)
  : base{pass, path, base::default_pipeline_definition},
    _push_handler{base::pipeline()},
    _attachment_names{std::move(attachment_names)},
    _exposure{exposure},
    _bloom_mix{bloom_mix} { }

  ~tonemap_filter() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& pipeline = base::pipeline();
    auto& descriptor_handler = base::descriptor_handler();

    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
    
    pipeline.bind(command_buffer);

    _push_handler.push("exposure", _exposure);
    _push_handler.push("bloom_mix", _bloom_mix);

    for (const auto& [name, attachment] : _attachment_names) {
      descriptor_handler.push(name, graphics_module.attachment(attachment));
    }

    if (!descriptor_handler.update(pipeline)) {
      return;
    }

    descriptor_handler.bind_descriptors(command_buffer);
    _push_handler.bind(command_buffer);

    command_buffer.draw(3, 1, 0, 0);
  }

private:

  graphics::push_handler _push_handler;
  std::vector<std::pair<std::string, std::string>> _attachment_names;
  std::float_t _exposure;
  std::float_t _bloom_mix;

}; // class tonemap_filter

} // namespace sbx::post

#endif // LIBSBX_POST_FILTERS_TONEMAP_FILTER_HPP_
