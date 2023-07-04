#ifndef LIBSBX_ASSETS_ASSETS_MODULE_HPP_
#define LIBSBX_ASSETS_ASSETS_MODULE_HPP_

#include <unordered_map>
#include <typeindex>
#include <filesystem>

#include <libsbx/core/module.hpp>

#include <libsbx/async/async_module.hpp>

#include <libsbx/assets/asset_storage.hpp>

namespace sbx::assets {

class assets_module final : public core::module<assets_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<async::async_module>{});

public:

  assets_module() = default;

  ~assets_module() override = default;

  auto update() -> void override {

  }

  template<typename Type>
  auto load_asset(const std::filesystem::path& path) -> asset_handle<Type> {
    return asset_handle<Type>{};
  }

  template<typename Type>
  auto get_asset(const asset_handle<Type>& handle) -> Type& {
    return static_cast<Type&>(_get_storage<Type>().get(handle));
  }

private:

  template<typename Type>
  auto _get_storage() -> asset_storage<Type>& {
    auto type = std::type_index(typeid(Type));

    if (auto storage = _storages.find(type); storage != _storages.end()) {
      return *static_cast<asset_storage<Type>*>(storage->second.get());
    }

    auto storage = _storages.insert({type, std::make_unique<asset_storage<Type>>()});

    return *static_cast<asset_storage<Type>*>(storage.first->second.get());
  }

  std::unordered_map<std::type_index, std::unique_ptr<asset_storage_base>> _storages;

}; // class assets_module

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_ASSETS_MODULE_HPP_
