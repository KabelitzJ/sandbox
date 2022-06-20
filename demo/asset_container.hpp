#ifndef DEMO_ASSET_CONTAINER_HPP_
#define DEMO_ASSET_CONTAINER_HPP_

#include <unordered_map>
#include <memory>

#include <fmt/core.h>

#include <utils/noncopyable.hpp>

#include <utils/type_name.hpp>

#include "hashed_string.hpp"

namespace demo {

class asset_container_base {

public:

  virtual ~asset_container_base() = default;

  virtual bool contains(const hashed_string& name) const = 0;

  virtual void remove(const hashed_string& name) = 0;

  virtual void clear() = 0;

}; // class asset_container_base

template<typename Type>
class asset_container : public asset_container_base, public sbx::noncopyable {

public:

  asset_container() = default;

  ~asset_container() override = default;

  template<typename... Args>
  Type& add(const hashed_string& name, Args&&... args) {
    if (_assets.contains(name.value())) {
      const auto type_name = sbx::type_name<Type>::value();
      throw std::runtime_error{fmt::format("Asset of type '{}' with name '{}' already exists", type_name, name.to_string())};
    }
    
    auto asset = std::make_unique<Type>(std::forward<Args>(args)...);
    const auto entry = _assets.emplace(name.value(), std::move(asset)).first;

    return *entry->second;
  }

  Type& get(const hashed_string& name) {
    const auto entry = _assets.find(name.value());

    if (entry == _assets.cend()) {
      const auto type_name = sbx::type_name<Type>::value();
      throw std::runtime_error{fmt::format("Asset of type '{}' with name '{}' does not exist", type_name, name.to_string())};
    }

    return *entry->second;
  }

  bool contains(const hashed_string& name) const override {
    return _assets.contains(name.value());
  } 

  void remove(const hashed_string& name) override {
    if (const auto entry = _assets.find(name.value()); entry != _assets.cend()) {
      _assets.erase(entry);
    }
  }

  void clear() override {
    _assets.clear();
  }

private:

  std::unordered_map<std::size_t, std::unique_ptr<Type>> _assets;

}; // class asset_container

} // namespace demo

#endif // DEMO_ASSET_CONTAINER_HPP_
