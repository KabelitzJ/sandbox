#ifndef DEMO_ASSET_MANAGER_HPP_
#define DEMO_ASSET_MANAGER_HPP_

#include <vector>
#include <memory>

#include <utils/noncopyable.hpp>

#include "asset_container.hpp"

namespace demo {

class asset_manager : sbx::noncopyable {

  using asset_container_type = std::vector<std::unique_ptr<asset_container_base>>;

public:

  asset_manager()
  : _containers{},
    _current_asset_id{} { }

  ~asset_manager() = default;

  template<typename Asset, typename... Args>
  Asset& add(const hashed_string& name, Args&&... args) {
    const auto asset_id = _asset_id<Asset>();

    auto& container = *static_cast<asset_container<Asset>*>(_containers[asset_id].get());

    return container.add(name, std::forward<Args>(args)...);
  }

  template<typename Asset>
  Asset& get(const hashed_string& name) {
    const auto asset_id = _asset_id<Asset>();

    auto& container = *static_cast<asset_container<Asset>*>(_containers[asset_id].get());

    return container.get(name);
  }

  template<typename Asset>
  bool contains(const hashed_string& name) const {
    const auto asset_id = _asset_id<Asset>();

    const auto& container = *static_cast<const asset_container<Asset>*>(_containers[asset_id].get());

    return container.contains(name);
  }

  template<typename Asset>
  void remove(const hashed_string& name) {
    const auto asset_id = _asset_id<Asset>();

    _containers[asset_id]->remove(name);
  }

  template<typename Asset>
  void clear() {
    const auto asset_id = _asset_id<Asset>();

    _containers[asset_id]->clear();
  }

  void clear() {
    for (auto& container : _containers) {
      container->clear();
    }
  }


private:

  template<typename Asset>
  std::size_t _asset_id() {
    static const std::size_t id = _register_asset<Asset>();
    return id;
  }

  template<typename Asset>
  std::size_t _register_asset() {
    _containers.emplace_back(std::make_unique<asset_container<Asset>>());
    return _current_asset_id++;
  }

  asset_container_type _containers{};
  std::size_t _current_asset_id{};

}; // class asset_manager

} // namespace demo

#endif // DEMO_ASSET_MANAGER_HPP_
