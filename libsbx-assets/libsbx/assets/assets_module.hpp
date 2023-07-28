#ifndef LIBSBX_ASSETS_ASSETS_MODULE_HPP_
#define LIBSBX_ASSETS_ASSETS_MODULE_HPP_

#include <memory>
#include <typeindex>

#include <libsbx/core/module.hpp>

#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/assets/asset.hpp>
#include <libsbx/assets/storage.hpp>

namespace sbx::assets {

class assets_module final : public core::module<assets_module> {

  inline static const auto is_registered = register_module(stage::normal);

public:

  assets_module() = default;

  ~assets_module() override = default;

  auto update() -> void override {

  }

  template<typename Asset, typename... Args>
  auto add_asset(Args&&... args) -> asset::handle_type {
    auto& storage = _get_or_create_storage<Asset>();

    auto& asset = storage.emplace(std::forward<Args>(args)...);

    return asset.handle();
  }

  template<typename Asset>
  auto get_asset(asset::handle_type handle) -> Asset& {
    if (auto storage = _try_get_storage<Asset>(); storage) {
      if (auto entry = storage->find(handle); entry != storage->end()) {
        return entry->second;
      }
    }

    throw std::runtime_error{"Asset not found."};
  }
  

private:

  template<typename Asset>
  auto _get_or_create_storage() -> storage<Asset>& {
    const auto type = std::type_index{typeid(Asset)};

    if (auto entry = _storages.find(type); entry != _storages.end()) {
      return *static_cast<storage<Asset>*>(entry->second.get());
    }

    auto storage = _storages.insert({type, std::make_unique<assets::storage<Asset>>()});

    return *static_cast<assets::storage<Asset>*>(storage.first->second.get());
  }

  template<typename Asset>
  auto _try_get_storage() -> memory::observer_ptr<storage<Asset>> {
    const auto type = std::type_index{typeid(Asset)};

    if (auto entry = _storages.find(type); entry != _storages.end()) {
      return memory::make_observer<storage<Asset>>(static_cast<storage<Asset>*>(entry->second.get()));
    }

    return memory::observer_ptr<storage<Asset>>{};
  }

  std::unordered_map<std::type_index, std::unique_ptr<storage_base>> _storages;

}; // class assets_module

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_ASSETS_MODULE_HPP_
