#ifndef LIBSBX_ASSETS_ASSET_STORAGE_HPP_
#define LIBSBX_ASSETS_ASSET_STORAGE_HPP_

#include <unordered_map>
#include <memory>

#include <libsbx/assets/asset.hpp>
#include <libsbx/assets/asset_handle.hpp>

namespace sbx::assets {

struct asset_storage_base {
  virtual ~asset_storage_base() = default;
}; // struct asset_storage_base

template<typename Type>
class asset_storage : public asset_storage_base {

  using container_type = std::unordered_map<asset_handle<Type>, std::unique_ptr<asset<Type>>>;

public:

  using handle_type = asset_handle<Type>;

  using iterator = typename container_type::iterator;

  asset_storage() = default;

  auto get(const handle_type& handle) const noexcept -> const asset<Type>& {
    return *_assets.at(handle);
  }

  auto get(const handle_type& handle) noexcept -> asset<Type>& {
    return *_assets.at(handle);
  }

  template<typename... Args>
  auto add(Args&&... args) noexcept -> const handle_type& {
    auto handle = handle_type{};
    _assets.emplace(handle, std::make_unique<asset<Type>>(std::forward<Args>(args)...));
    return handle;
  }

  auto remove(const handle_type& handle) noexcept -> void {
    _assets.erase(handle);
  }

  auto begin() noexcept -> iterator {
    return _assets.begin();
  }

  auto end() noexcept -> iterator {
    return _assets.end();
  }

  auto find(const handle_type& handle) noexcept -> iterator {
    return _assets.find(handle);
  }

private:
  
  container_type _assets;

}; // class asset_storage

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_ASSET_STORAGE_HPP_
