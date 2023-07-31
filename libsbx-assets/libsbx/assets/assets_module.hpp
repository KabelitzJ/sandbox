#ifndef LIBSBX_ASSETS_ASSETS_MODULE_HPP_
#define LIBSBX_ASSETS_ASSETS_MODULE_HPP_

#include <memory>
#include <typeindex>
#include <unordered_map>

#include <libsbx/utility/hashed_string.hpp>

#include <libsbx/async/async_module.hpp>

#include <libsbx/core/module.hpp>

#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/assets/asset.hpp>
#include <libsbx/assets/storage.hpp>

namespace sbx::assets {

class assets_module final : public core::module<assets_module> {

  inline static const auto is_registered = register_module(stage::normal, core::dependencies<async/async_module>{});

public:

  assets_module() = default;

  ~assets_module() override = default;

  auto update() -> void override {

  }

  template<typename Asset, typename... Args>
  requires (std::is_base_of_v<asset<Asset::type>, Asset> && std::is_constructible_v<Asset, Args...>)
  auto load_asset(const utility::hashed_string& key, Args&&... args) -> Asset& {
    auto& storage = _get_or_create_storage<Asset>(Asset::type);

    return storage.insert(key, std::make_unique<Asset>(std::forward<Args>(args)...));
  }

  template<typename Asset>
  requires (std::is_base_of_v<asset<Asset::type>, Asset>)
  [[nodiscard]] auto get_asset(const utility::hashed_string& key) -> Asset& {
    if (auto storage = _try_get_storage<Asset>(Asset::type); storage) {
      if (auto entry = storage->find(key); entry != storage->end()) {
        return *entry->second;
      }
    }

    throw std::runtime_error{fmt::format("Failed to find asset '{}'", key.c_str())};
  }

  auto unload_assets() -> void {
    for (auto& [type, storage] : _storages) {
      storage->clear();
    }
  }

private:

  template<typename Type>
  auto _get_or_create_storage(asset_type type) -> storage<Type>& {
    if (auto entry = _storages.find(type); entry != _storages.end()) {
      return *static_cast<storage<Type>*>(entry->second.get());
    }

    auto entry = _storages.insert({type, std::make_unique<storage<Type>>()});

    return *static_cast<storage<Type>*>(entry.first->second.get());
  }

  template<typename Type>
  auto _try_get_storage(asset_type type) -> memory::observer_ptr<storage<Type>> {
    if (auto entry = _storages.find(type); entry != _storages.end()) {
      return memory::observer_ptr{static_cast<storage<Type>*>(entry->second.get())};
    }

    return memory::observer_ptr<storage<Type>>{};
  }

  std::unordered_map<asset_type, std::unique_ptr<storage_base>> _storages;

}; // class assets_module

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_ASSETS_MODULE_HPP_
