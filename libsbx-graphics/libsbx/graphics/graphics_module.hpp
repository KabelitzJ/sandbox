#ifndef LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_
#define LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_

#include <memory>
#include <unordered_map>
#include <vector>
#include <typeindex>

#include <vk_mem_alloc.h>

#include <libsbx/core/module.hpp>
#include <libsbx/core/delegate.hpp>

#include <libsbx/math/uuid.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/utility/hash.hpp>
#include <libsbx/utility/concepts.hpp>

#include <libsbx/graphics/devices/instance.hpp>
#include <libsbx/graphics/devices/physical_device.hpp>
#include <libsbx/graphics/devices/logical_device.hpp>
#include <libsbx/graphics/devices/surface.hpp>

#include <libsbx/graphics/commands/command_pool.hpp>
#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/render_pass/swapchain.hpp>

#include <libsbx/graphics/pipeline/pipeline.hpp>
#include <libsbx/graphics/pipeline/shader.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>
#include <libsbx/graphics/pipeline/compute_pipeline.hpp>

#include <libsbx/graphics/buffers/buffer.hpp>

#include <libsbx/graphics/renderer.hpp>
#include <libsbx/graphics/render_stage.hpp>

#include <libsbx/graphics/resource_storage.hpp>

namespace sbx::graphics {

/**
 * @brief Checks the @ref VkResult and throws an exception if it is an error
 * @param result 
 * @throws @see std::runtime_error 
 */
auto validate(VkResult result) -> void;

template<typename VkEnum, typename Enum>
requires ((std::is_enum_v<VkEnum> || std::is_same_v<VkEnum, VkFlags>) && std::is_enum_v<Enum>)
constexpr auto to_vk_enum(Enum value) -> VkEnum {
  return static_cast<VkEnum>(value);
}

/**
 * @brief Module for managing rendering specific tasks
 * 
 * @extends @ref sbx::core::module<T>
 */
class graphics_module final : public core::module<graphics_module> {

  inline static const auto is_registered = register_module(stage::rendering, dependencies<devices::devices_module>{});

  inline static constexpr auto max_deletion_queue_size = std::size_t{16u};

public:

  graphics_module();

  ~graphics_module() override;

  auto update() -> void override;

  auto instance() -> instance&;

  auto physical_device() -> physical_device&;

  auto logical_device() -> logical_device&;

  auto surface() -> surface&;

  auto command_pool(VkQueueFlagBits queue_type = VK_QUEUE_GRAPHICS_BIT, const std::thread::id& thread_id = std::this_thread::get_id()) -> const std::shared_ptr<command_pool>&;

  auto swapchain() -> swapchain&;

  template<utility::implements<renderer> Renderer, typename... Args>
  requires (std::is_constructible_v<Renderer, Args...>)
  auto set_renderer(Args&&... args) -> void {
    _renderer = std::make_unique<Renderer>(std::forward<Args>(args)...);
    _reset_render_stages();
    _renderer->initialize();
  }

  auto render_stage(const pipeline::stage& stage) -> graphics::render_stage&;
  
  auto current_frame() const noexcept -> std::uint32_t {
    return _current_frame;
  }

  auto attachment(const std::string& name) const -> const descriptor&;

  template<typename Type, typename... Args>
  auto add_asset(Args&&... args) -> math::uuid {
    const auto id = math::uuid{};
    const auto type = std::type_index{typeid(Type)};

    auto container = _asset_containers.find(type);

    if (container == _asset_containers.end()) {
      container = _asset_containers.insert({type, std::make_unique<asset_container<Type>>()}).first;
    }

    static_cast<asset_container<Type>*>(container->second.get())->add(id, std::forward<Args>(args)...);

    return id;
  }

  template<typename Type>
  auto add_asset(std::unique_ptr<Type>&& asset) -> math::uuid {
    const auto id = math::uuid{};
    const auto type = std::type_index{typeid(Type)};

    auto container = _asset_containers.find(type);

    if (container == _asset_containers.end()) {
      container = _asset_containers.insert({type, std::make_unique<asset_container<Type>>()}).first;
    }

    static_cast<asset_container<Type>*>(container->second.get())->add(id, std::move(asset));

    return id;
  }

  template<typename Type>
  auto get_asset(const math::uuid& id) const -> const Type& {
    const auto type = std::type_index{typeid(Type)};

    auto container = _asset_containers.find(type);

    if (container == _asset_containers.end()) {
      throw std::runtime_error{"Asset does not exist"};
    }

    return static_cast<const asset_container<Type>*>(container->second.get())->get(id);
  }

  template<typename Type>
  auto get_asset(const math::uuid& id) -> Type& {
    const auto type = std::type_index{typeid(Type)};

    auto container = _asset_containers.find(type);

    if (container == _asset_containers.end()) {
      throw std::runtime_error{"Asset does not exist"};
    }

    return static_cast<asset_container<Type>*>(container->second.get())->get(id);
  }

  template<typename Type, typename... Args>
  requires (std::is_constructible_v<Type, Args...>)
  auto add_resource(Args&&... args) -> resource_handle<Type> {
    return _storage<Type>().emplace(std::forward<Args>(args)...);
  }

  template<typename Type>
  auto get_resource(const resource_handle<Type>& handle) -> Type& {
    return _storage<Type>().get(handle);
  }

  template<typename Type>
  auto get_resource(const resource_handle<Type>& handle) const -> const Type& {
    return _storage<Type>().get(handle);
  }

  auto allocator() const noexcept -> VmaAllocator {
    return _allocator;
  }

private:

  auto _start_render_pass(graphics::render_stage& render_stage, graphics::command_buffer& command_buffer) -> void;

  auto _end_render_pass(graphics::render_stage& render_stage, graphics::command_buffer& command_buffer) -> void;

  auto _reset_render_stages() -> void;

  auto _recreate_swapchain() -> void;

  auto _recreate_per_frame_data() -> void;

  auto _recreate_per_image_data() -> void;

  auto _recreate_command_buffers() -> void;

  auto _recreate_attachments() -> void;

  struct per_frame_data {
    // graphics
    VkSemaphore image_available_semaphore{};
    VkFence graphics_in_flight_fence{};
    // compute
    VkSemaphore compute_finished_semaphore{};
    VkFence compute_in_flight_fence{};
  }; // struct per_frame_data
  
  struct per_image_data {
    VkSemaphore render_finished_semaphore{};
  }; // struct per_image_data

  struct command_pool_key {
    VkQueueFlagBits queue_type;
    std::thread::id thread_id;
  }; // struct command_pool_key

  struct command_pool_key_hash {
    auto operator()(const command_pool_key& key) const noexcept -> std::size_t {
      auto hast = std::size_t{0};
      utility::hash_combine(hast, key.queue_type, key.thread_id);
      return hast;
    }
  }; // struct command_pool_key_hash

  struct command_pool_key_equality {
    auto operator()(const command_pool_key& lhs, const command_pool_key& rhs) const noexcept -> bool {
      return lhs.queue_type == rhs.queue_type && lhs.thread_id == rhs.thread_id;
    }
  }; // struct command_pool_key_equal


  static_assert(std::is_class_v<graphics::graphics_pipeline>, "graphics_pipeline is not a class in sbx::graphics");

  template<typename Type>
  auto _storage() -> resource_storage<Type>& {
    if constexpr (std::is_same_v<Type, shader>) {
      return _shaders;
    } else if constexpr (std::is_same_v<Type, graphics_pipeline>) {
      return _graphics_pipelines;
    } else if constexpr (std::is_same_v<Type, compute_pipeline>) {
      return _compute_pipelines;
    } else if constexpr (std::is_same_v<Type, buffer>) {
      return _buffers;
    } else if constexpr (std::is_same_v<Type, image2d>) {
      return _buffers;
    } else if constexpr (std::is_same_v<Type, shader>) {
      return _buffers;
    } else if constexpr (std::is_same_v<Type, graphics_pipeline>) {
      return _buffers;
    } else if constexpr (std::is_same_v<Type, compute_pipeline>) {
      return _buffers;
    }

    utility::assert_that(false, "Invalid resource type");
  }

  template<typename Type>
  auto _storage() const -> const resource_storage<Type>& {
    if constexpr (std::is_same_v<Type, shader>) {
      return _shaders;
    } else if constexpr (std::is_same_v<Type, graphics_pipeline>) {
      return _graphics_pipelines;
    } else if constexpr (std::is_same_v<Type, compute_pipeline>) {
      return _compute_pipelines;
    } else if constexpr (std::is_same_v<Type, buffer>) {
      return _buffers;
    } else if constexpr (std::is_same_v<Type, image2d>) {
      return _buffers;
    } else if constexpr (std::is_same_v<Type, shader>) {
      return _buffers;
    } else if constexpr (std::is_same_v<Type, graphics_pipeline>) {
      return _buffers;
    } else if constexpr (std::is_same_v<Type, compute_pipeline>) {
      return _buffers;
    }

    utility::assert_that(false, "Invalid resource type");
  }

  std::unique_ptr<graphics::instance> _instance{};
  std::unique_ptr<graphics::physical_device> _physical_device{};
  std::unique_ptr<graphics::logical_device> _logical_device{};

  std::unordered_map<command_pool_key, std::shared_ptr<graphics::command_pool>, command_pool_key_hash, command_pool_key_equality> _command_pools{};

  std::map<std::string, memory::observer_ptr<const descriptor>> _attachments{};

  std::unique_ptr<graphics::surface> _surface{};

  std::unique_ptr<graphics::swapchain> _swapchain{};

  std::vector<per_frame_data> _per_frame_data{};
  std::vector<per_image_data> _per_image_data{};
  // std::vector<std::unique_ptr<graphics::command_buffer>> _graphics_command_buffers{};
  // std::vector<std::unique_ptr<graphics::command_buffer>> _compute_command_buffers{};
  std::vector<graphics::command_buffer> _graphics_command_buffers{};
  std::vector<graphics::command_buffer> _compute_command_buffers{};

  std::unique_ptr<graphics::renderer> _renderer{};

  VmaAllocator _allocator;

  resource_storage<graphics::shader> _shaders;
  resource_storage<graphics::graphics_pipeline> _graphics_pipelines;
  resource_storage<graphics::compute_pipeline> _compute_pipelines;
  resource_storage<graphics::buffer> _buffers;
  resource_storage<graphics::image2d> _images;

  struct asset_container_base {
    virtual ~asset_container_base() = default;
    virtual auto remove(const math::uuid& id) -> void = 0;
    virtual auto clear() -> void = 0;
  };

  template<typename Type>
  class asset_container : public asset_container_base {

  public:

    asset_container() {

    }

    ~asset_container() override {

    }

    auto remove(const math::uuid& id) -> void override {
      _assets.erase(id);
    }

    auto clear() -> void override {
      _assets.clear();
    }

    template<typename... Args>
    auto add(const math::uuid& id, Args&&... args) -> void {
      _assets.insert({id, std::make_unique<Type>(std::forward<Args>(args)...)});
    }

    auto add(const math::uuid& id, std::unique_ptr<Type>&& asset) -> void {
      _assets.insert({id, std::move(asset)});
    }

    auto get(const math::uuid& id) const -> const Type& {
      return *_assets.at(id);
    }

    auto get(const math::uuid& id) -> Type& {
      return *_assets.at(id);
    }

  private:

    std::unordered_map<math::uuid, std::unique_ptr<Type>> _assets;

  };

  std::unordered_map<std::type_index, std::unique_ptr<asset_container_base>> _asset_containers;

  std::uint32_t _current_frame{};
  bool _is_framebuffer_resized{};

}; // class graphics_module

} // namespace sbx::graphics

template<typename Type>
struct std::hash<sbx::graphics::resource_handle<Type>> {
  auto operator()(const sbx::graphics::resource_handle<Type>& handle) const noexcept -> std::size_t {
    auto hash = std::size_t{0};
    sbx::utility::hash_combine(hash, handle.handle(), handle.generation());
    return hash;
  }
};

#endif // LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_
