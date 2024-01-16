#ifndef LIBSBX_POST_FILTER_HPP_
#define LIBSBX_POST_FILTER_HPP_

#include <string>
#include <map>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/graphics/descriptor/descriptor_handler.hpp>


namespace sbx::post {

template<graphics::vertex Vertex>
class filter : public graphics::subrenderer {

public:

  using vertex_type = Vertex;
  using pipeline_type = graphics::graphics_pipeline<vertex_type>;

  filter(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : graphics::subrenderer{stage},
    _pipeline{path, stage} { }

  virtual ~filter() = default;

  auto descriptor_handler() noexcept -> graphics::descriptor_handler& {
    return _descriptor_handler;
  }

  auto pipeline() noexcept -> pipeline_type& {
    return _pipeline;
  }

  auto attachment(const std::string& descriptor_name, const graphics::descriptor& descriptor) -> const graphics::descriptor& {
    if (auto it = _descriptors.find(descriptor_name); it != _descriptors.end()) {
      return *(it->second);
    }

    return descriptor;
  }

  auto attachment(const std::string& descriptor_name, const std::string& render_attachment) -> const graphics::descriptor& {
    if (auto it = _descriptors.find(descriptor_name); it != _descriptors.end()) {
      return *(it->second);
    }

    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    return graphics_module.attachment(render_attachment);
  }

  auto set_attachment(const std::string& descriptor_name, const graphics::descriptor& descriptor) -> void {
    _descriptors.insert_or_assign(descriptor_name, &descriptor);
  }

  auto remove_attachment(const std::string& descriptor_name) -> bool {
    if (auto it = _descriptors.find(descriptor_name); it != _descriptors.end()) {
      _descriptors.erase(it);
      return true;
    }

    return false;
  }

protected:

  // auto add_descriptor(const std::string& name, const graphics::descriptor* descriptor) -> void {
  //   _descriptors.insert_or_assign(name, descriptor);
  // }

  // auto get_descriptor(const std::string& name) const -> const graphics::descriptor* {
  //   if (auto it = _descriptors.find(name); it != _descriptors.end()) {
  //     return it->second;
  //   }
    
  //   return nullptr;
  // }

private:

  std::map<std::string, memory::observer_ptr<const graphics::descriptor>> _descriptors;
  pipeline_type _pipeline;
  graphics::descriptor_handler _descriptor_handler;

}; // class filter

} // namespace sbx::post

#endif // LIBSBX_POST_FILTER_HPP_
