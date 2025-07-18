#ifndef LIBSBX_ASSETS_ASSETS_MODULE_HPP_
#define LIBSBX_ASSETS_ASSETS_MODULE_HPP_

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>

#include <libsbx/utility/compression.hpp>

#include <libsbx/math/uuid.hpp>

#include <libsbx/core/module.hpp>

#include <libsbx/assets/thread_pool.hpp>
#include <libsbx/assets/metadata.hpp>

namespace sbx::assets {

class assets_module : public core::module<assets_module> {

  inline static const auto is_registered = register_module(stage::post);

public:

  assets_module()
  : _thread_pool{std::thread::hardware_concurrency()} { }

  ~assets_module() override {
    for (const auto& [type, container] : _containers) {
      container->clear();
    }
  }

  auto update() -> void override {
    
  }

  template<typename Function, typename... Args>
  requires (std::is_invocable_v<Function, Args...>)
  auto submit(Function&& function, Args&&... args) -> std::future<std::invoke_result_t<Function, Args...>> {
    return _thread_pool.submit(std::forward<Function>(function), std::forward<Args>(args)...);
  }

  template<typename Type, typename... Args>
  auto add_asset(Args&&... args) -> math::uuid {
    const auto id = math::uuid{};
    const auto type = std::type_index{typeid(Type)};

    auto entry = _containers.find(type);

    if (entry == _containers.end()) {
      entry = _containers.insert({type, std::make_unique<container<Type>>()}).first;
    }

    static_cast<container<Type>*>(entry->second.get())->add(id, std::forward<Args>(args)...);

    return id;
  }

  template<typename Type>
  auto add_asset(std::unique_ptr<Type>&& asset) -> math::uuid {
    const auto id = math::uuid{};
    const auto type = std::type_index{typeid(Type)};

    auto entry = _containers.find(type);

    if (entry == _containers.end()) {
      entry = _containers.insert({type, std::make_unique<container<Type>>()}).first;
    }

    static_cast<container<Type>*>(entry->second.get())->add(id, std::move(asset));

    return id;
  }

  template<typename Type>
  auto get_asset(const math::uuid& id) const -> const Type& {
    const auto type = std::type_index{typeid(Type)};

    auto entry = _containers.find(type);

    if (entry == _containers.end()) {
      throw std::runtime_error{"Asset does not exist"};
    }

    return static_cast<const container<Type>*>(entry->second.get())->get(id);
  }

  template<typename Type>
  auto get_asset(const math::uuid& id) -> Type& {
    const auto type = std::type_index{typeid(Type)};

    auto entry = _containers.find(type);

    if (entry == _containers.end()) {
      throw std::runtime_error{"Asset does not exist"};
    }

    return static_cast<container<Type>*>(entry->second.get())->get(id);
  }

private:

  thread_pool _thread_pool;

  struct container_base {
    virtual ~container_base() = default;
    virtual auto remove(const math::uuid& id) -> void = 0;
    virtual auto clear() -> void = 0;
  };

  template<typename Type>
  class container : public container_base {

  public:

    container() {

    }

    ~container() override {

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

  std::unordered_map<std::type_index, std::unique_ptr<container_base>> _containers;

}; // class assets_module

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_ASSETS_MODULE_HPP_
