#ifndef LIBSBX_GRAPHICS_DESCRIPTOR_DESCRIPTOR_HANDLER_HPP_
#define LIBSBX_GRAPHICS_DESCRIPTOR_DESCRIPTOR_HANDLER_HPP_

#include <memory>
#include <map>
#include <vector>
#include <optional>

#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/graphics/buffers/uniform_handler.hpp>
#include <libsbx/graphics/buffers/storage_handler.hpp>
#include <libsbx/graphics/buffers/push_handler.hpp>

#include <libsbx/graphics/pipeline/pipeline.hpp>

#include <libsbx/graphics/descriptor/descriptor.hpp>
#include <libsbx/graphics/descriptor/descriptor_set.hpp>
#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

class descriptor_handler {

public:

  descriptor_handler() = default;

  explicit descriptor_handler(const pipeline& pipeline)
  : _pipeline{&pipeline} {
    _descriptor_sets.resize(graphics::swapchain::max_frames_in_flight);

    for (auto& descriptor_set : _descriptor_sets) {
      descriptor_set = std::make_unique<graphics::descriptor_set>(pipeline);
    }
  }

  ~descriptor_handler() {
    _descriptor_sets.clear();
  }

  template<typename Descriptor>
  requires (std::is_base_of_v<descriptor, Descriptor>)
  auto push(const std::string& name, const Descriptor& descriptor) -> void {
    if (_pipeline) {
      _push(name, descriptor);
    }
  }

  auto push(const std::string& name, uniform_handler& uniform_handler) -> void {
    if (_pipeline) {
      uniform_handler.update(_pipeline->descriptor_block(name));
      _push(name, uniform_handler.uniform_buffer());
    }
  }

  auto push(const std::string& name, storage_handler& storage_handler) -> void {
    if (_pipeline) {
      storage_handler.update(_pipeline->descriptor_block(name));
      _push(name, storage_handler.storage_buffer());
    }
  }

  auto push(const std::string& name, push_handler& push_handler) -> void {
    if (_pipeline) {
      push_handler.update(_pipeline->descriptor_block(name));
    }
  }

  auto bind_descriptors(command_buffer& command_buffer) -> void {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    const auto current_frame = graphics_module.current_frame();

    _descriptor_sets[current_frame]->bind(command_buffer);
  }

  auto descriptor_set() const noexcept -> VkDescriptorSet {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    const auto current_frame = graphics_module.current_frame();

    return *_descriptor_sets[current_frame];
  }

  auto update(const pipeline& pipeline) -> bool {
    if (_pipeline.get() != &pipeline) {
      _pipeline = &pipeline;

      _descriptors.clear();

      _descriptor_sets.resize(graphics::swapchain::max_frames_in_flight);

      for (auto& descriptor_set : _descriptor_sets) {
        descriptor_set = std::make_unique<graphics::descriptor_set>(pipeline);
      }

      return false;
    }

    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    const auto current_frame = graphics_module.current_frame();

    auto& descriptor_set = _descriptor_sets[current_frame];

    auto write_descriptor_sets = std::vector<VkWriteDescriptorSet>{};
    write_descriptor_sets.reserve(_descriptors.size());

    for (const auto& [name, descriptor] : _descriptors) {
      auto write_descriptor_set = descriptor.write_descriptor_set.handle();
      write_descriptor_set.dstSet = *descriptor_set;

      write_descriptor_sets.push_back(write_descriptor_set);
    }

    descriptor_set->update(write_descriptor_sets);

    _descriptors.clear();

    return true;
  }

private:

  struct descriptor_entry {
    memory::observer_ptr<const graphics::descriptor> descriptor;
    graphics::write_descriptor_set write_descriptor_set;
    std::uint32_t binding;
  }; // struct descriptor_entry

  template<typename Descriptor>
  requires (std::is_base_of_v<descriptor, Descriptor>)
  auto _push(const std::string& name, const Descriptor& descriptor) -> void {
    auto binding = _pipeline->find_descriptor_binding(name);

    if (!binding) {
      throw std::runtime_error(fmt::format("Failed to find descriptor binding for descriptor '{}'", name));
    }

    auto descriptor_type = _pipeline->find_descriptor_type_at_binding(*binding);

    if (!descriptor_type) {
      throw std::runtime_error(fmt::format("Failed to find descriptor type for descriptor '{}'", name));
    }

    auto write_descriptor_set = descriptor.write_descriptor_set(*binding, *descriptor_type);

    _descriptors.insert({name, descriptor_entry{std::addressof(descriptor), std::move(write_descriptor_set), *binding}});
  }

  memory::observer_ptr<const pipeline> _pipeline;

  std::vector<std::unique_ptr<graphics::descriptor_set>> _descriptor_sets{};

  std::map<std::string, descriptor_entry> _descriptors{};

}; // class descriptor_handler

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DESCRIPTOR_DESCRIPTOR_HANDLER_HPP_
