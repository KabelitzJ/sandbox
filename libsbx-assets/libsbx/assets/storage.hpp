#ifndef LIBSBX_ASSETS_STORAGE_HPP_
#define LIBSBX_ASSETS_STORAGE_HPP_

#include <unordered_map>
#include <memory>
#include <typeindex>

#include <libsbx/assets/asset.hpp>

namespace sbx::assets {

struct storage_base {
  virtual ~storage_base() = default;
}; // class storage_base

template<typename Type>
class storage : public storage_base {

  using storage_type = std::unordered_map<asset::handle_type, Type>;

public:

  using value_type = Type;
  using reference = Type&;
  using const_reference = const Type&;
  using pointer = Type*;
  using const_pointer = const Type*;
  using iterator = typename storage_type::iterator;
  using const_iterator = typename storage_type::const_iterator;

  storage() = default;

  ~storage() = default;

  template<typename... Args>
  auto emplace(Args&&... args) -> Type& {
    auto asset = Type{std::forward<Args>(args)...};

    auto handle = asset.handle();

    auto entry = _storage.insert({handle, std::move(asset)});

    return entry.first->second;
  }

  auto erase(asset::handle_type handle) -> void {
    _storage.erase(handle);
  }

  auto find(asset::handle_type handle) -> iterator {
    return _storage.find(handle);
  }

  auto find(asset::handle_type handle) const -> const_iterator {
    return _storage.find(handle);
  }

  auto begin() -> iterator {
    return _storage.begin();
  }

  auto begin() const -> const_iterator {
    return _storage.begin();
  }

  auto cbegin() const -> const_iterator {
    return _storage.cbegin();
  }

  auto end() -> iterator {
    return _storage.end();
  }

  auto end() const -> const_iterator {
    return _storage.end();
  }

  auto cend() const -> const_iterator {
    return _storage.cend();
  }

  auto operator[](asset::handle_type handle) -> Type& {
    return _storage[handle];
  }

  auto operator[](asset::handle_type handle) const -> const Type& {
    return _storage[handle];
  }

  auto size() const -> std::size_t {
    return _storage.size();
  }

  auto is_empty() const -> bool {
    return _storage.empty();
  }

  auto clear() -> void {
    _storage.clear();
  }

private:

  storage_type _storage;

}; // class storage

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_STORAGE_HPP_
