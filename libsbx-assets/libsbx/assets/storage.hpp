#ifndef LIBSBX_ASSETS_STORAGE_HPP_
#define LIBSBX_ASSETS_STORAGE_HPP_

#include <unordered_map>
#include <memory>
#include <typeindex>

#include <libsbx/utility/hashed_string.hpp>

#include <libsbx/assets/asset.hpp>

namespace sbx::assets {

struct storage_base {
  virtual ~storage_base() = default;
  virtual auto is_empty() const -> bool = 0;
  virtual auto clear() -> void = 0;
};

template<typename Type>
class storage : public storage_base {

  using storage_type = std::unordered_map<asset_id, std::unique_ptr<Type>>;

public:

  using value_type = Type;
  using reference = Type&;
  using iterator = typename storage_type::iterator;

  storage() {

  }

  ~storage() override {
    clear();
  }

  auto insert(const asset_id id, std::unique_ptr<Type>&& value) -> reference {
    auto entry = _storage.insert({id, std::move(value)});

    return *entry.first->second;
  }

  auto find(const asset_id id) -> iterator {
    return _storage.find(id);
  }

  auto begin() -> iterator {
    return _storage.begin();
  }

  auto end() -> iterator {
    return _storage.end();
  }

  auto is_empty() const -> bool override {
    return _storage.empty();
  }

  auto clear() -> void override {
    _storage.clear();
  }

private:

  storage_type _storage;

}; // class storage

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_STORAGE_HPP_
