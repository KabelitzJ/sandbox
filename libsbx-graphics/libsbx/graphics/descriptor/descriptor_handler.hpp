#ifndef LIBSBX_GRAPHICS_DESCRIPTOR_DESCRIPTOR_HANDLER_HPP_
#define LIBSBX_GRAPHICS_DESCRIPTOR_DESCRIPTOR_HANDLER_HPP_

#include <memory>
#include <map>
#include <vector>
#include <optional>

#include <range/v3/all.hpp>

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

  explicit descriptor_handler(pipeline& pipeline)
  : _pipeline{&pipeline},
    _was_changed{true} {
    _recreate_descriptor_sets();
  }

  ~descriptor_handler() {
    for (auto& descriptor_sets : _descriptor_sets_per_frame) {
      descriptor_sets.clear();
    }
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

  auto bind_descriptors(command_buffer& command_buffer, const std::uint32_t set) -> void {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    const auto current_frame = graphics_module.swapchain().active_image_index();

    auto& descriptor_sets = _descriptor_sets_per_frame[current_frame];

    descriptor_sets[set]->bind(command_buffer);
  }

  auto descriptor_set(const std::uint32_t set) const noexcept -> VkDescriptorSet {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    const auto current_frame = graphics_module.swapchain().active_image_index();

    auto& descriptor_sets = _descriptor_sets_per_frame[current_frame];

    return *descriptor_sets[set];
  }

  auto update_pipeline(pipeline& pipeline) -> bool {
    if (_pipeline.get() != &pipeline) {
      _pipeline = &pipeline;

      _recreate_descriptor_sets();

      _was_changed = false;

      return false;
    }

    return true;
  }

  auto update_set(const std::uint32_t set) -> bool {
    if (_was_changed) {
      auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

      const auto current_frame = graphics_module.swapchain().active_image_index();

      auto& descriptor_sets = _descriptor_sets_per_frame[current_frame];

      auto& descriptor_set = descriptor_sets[set];

      auto write_descriptor_sets = std::vector<VkWriteDescriptorSet>{};
      write_descriptor_sets.reserve(_descriptors[set].size());

      for (const auto& [name, descriptor] : _descriptors[set]) {
        auto write_descriptor_set = descriptor.write_descriptor_set.handle();
        write_descriptor_set.dstSet = *descriptor_set;

        write_descriptor_sets.push_back(write_descriptor_set);
      }

      descriptor_set->update(write_descriptor_sets);

      _was_changed = false;
    }

    // _descriptors[set].clear();

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
    auto descriptor_id = _pipeline->find_descriptor_binding(name);

    if (!descriptor_id) {
      throw std::runtime_error(fmt::format("Failed to find descriptor binding for descriptor '{}'", name));
    }

    auto entry = _descriptors[descriptor_id->set].find(name);

    if (entry != _descriptors[descriptor_id->set].end()) {
      if (entry->second.descriptor.get() == std::addressof(descriptor)) {
        return;
      }

      _descriptors[descriptor_id->set].erase(entry);
    }

    auto descriptor_type = _pipeline->find_descriptor_type_at_binding(*descriptor_id);

    if (!descriptor_type) {
      throw std::runtime_error(fmt::format("Failed to find descriptor type for descriptor '{}'", name));
    }

    auto write_descriptor_set = descriptor.write_descriptor_set(descriptor_id->binding, *descriptor_type);

    _descriptors[descriptor_id->set].emplace(name, descriptor_entry{std::addressof(descriptor), std::move(write_descriptor_set), descriptor_id->binding});

    _was_changed = true;
  }

  auto _recreate_descriptor_sets() -> void {
    const auto& descriptor_set_layouts = _pipeline->descriptor_set_layouts();

    _descriptors.clear();
    _descriptors.resize(descriptor_set_layouts.size());

    for (auto& descriptor_sets : _descriptor_sets_per_frame) {

      for (auto&& [id, set] : ranges::views::enumerate(descriptor_set_layouts)) {
        if (set == VK_NULL_HANDLE) {
          descriptor_sets.push_back(nullptr);
        } else {
          descriptor_sets.push_back(std::make_unique<graphics::descriptor_set>(*_pipeline, id));
        }
      }
    }
  }

  memory::observer_ptr<pipeline> _pipeline;

  std::array<std::vector<std::unique_ptr<graphics::descriptor_set>>, 3u> _descriptor_sets_per_frame{};

  std::vector<std::map<std::string, descriptor_entry>> _descriptors;

  bool _was_changed;

}; // class descriptor_handler

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DESCRIPTOR_DESCRIPTOR_HANDLER_HPP_
