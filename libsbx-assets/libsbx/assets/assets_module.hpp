#ifndef LIBSBX_ASSETS_ASSETS_MODULE_HPP_
#define LIBSBX_ASSETS_ASSETS_MODULE_HPP_

#include <memory>
#include <typeindex>
#include <filesystem>
#include <unordered_map>

#include <libsbx/utility/hashed_string.hpp>

#include <libsbx/async/async_module.hpp>

#include <libsbx/core/module.hpp>

#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/assets/asset.hpp>
#include <libsbx/assets/storage.hpp>

namespace sbx::assets {

struct asset_metadata {
  asset_id id;
}; // struct asset_metadata

class assets_module final : public core::module<assets_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<async::async_module>{});

public:

  assets_module();

  ~assets_module();

  auto update() -> void override;

  template<typename Asset, typename... Args>
  requires (std::is_base_of_v<asset<Asset::type>, Asset> && std::is_constructible_v<Asset, const std::filesystem::path&, Args...>)
  auto load_asset(const std::filesystem::path& path, Args&&... args) -> asset_id {
    auto& storage = _get_or_create_storage<Asset>(Asset::type);

    auto asset = std::make_unique<Asset>(path, std::forward<Args>(args)...);

    const auto id = asset->id();

    _metadata.insert({path, asset_metadata{id}});

    storage.insert(id, std::move(asset));

    return id;
  }

  template<typename Asset>
  requires (std::is_base_of_v<asset<Asset::type>, Asset>)
  [[nodiscard]] auto get_asset(const asset_id id) -> Asset& {
    if (auto storage = _try_get_storage<Asset>(Asset::type); storage) {
      if (auto entry = storage->find(id); entry != storage->end()) {
        return *entry->second;
      }
    }

    throw std::runtime_error{fmt::format("Failed to find asset")};
  }

  template<typename Asset>
  requires (std::is_base_of_v<asset<Asset::type>, Asset>)
  [[nodiscard]] auto get_asset(const std::filesystem::path& path) -> Asset& {
    if (auto entry = _metadata.find(path); entry != _metadata.end()) {
      return get_asset<Asset>(entry->second.id);
    }

    throw std::runtime_error{fmt::format("Failed to find asset '{}'", path.string())};
  }

  template<typename Asset>
  requires (std::is_base_of_v<asset<Asset::type>, Asset>)
  [[nodiscard]] auto try_get_asset_id(const std::filesystem::path& path) -> std::optional<asset_id> {
    if (auto entry = _metadata.find(path); entry != _metadata.end()) {
      return entry->second.id;
    }

    return std::nullopt;
  }

  auto unload_assets() -> void {
    // [Note] KAJ 2023-08-14 16:50 - Unfortunately mesh and pipeline assets need be unloaded in a fixed order since meshes hold indirect references to pipelines.
    _storages.erase(asset_type::mesh);
    _storages.erase(asset_type::pipeline);

    // [Note] KAJ 2023-08-14 16:50 - To ensure we dont introduce bugs when adding new asset types we clear the rest out here
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
  std::unordered_map<std::filesystem::path, asset_metadata> _metadata;

}; // class assets_module

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_ASSETS_MODULE_HPP_
