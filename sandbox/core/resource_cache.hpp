#ifndef SBX_CORE_RESOURCE_CACHE_HPP_
#define SBX_CORE_RESOURCE_CACHE_HPP_

#include <unordered_map>
#include <memory>
#include <string>
#include <cassert>

#include <types/primitives.hpp>

#include <utils/type_id.hpp>

namespace sbx {

class resource_cache {

public:

  resource_cache();
  ~resource_cache();

  template<typename Resource, typename... Args>
  void load(const std::string& name, Args&&... args) {
    constexpr auto id = type_id<Resource>();

    if (_resources_by_type.find(id) == _resources_by_type.cend()) {
      _resources_by_type.emplace(id, std::unordered_map<std::string, std::shared_ptr<void>>());
    }

    auto& resources = _resources_by_type.at(id);

    assert(resources.find(name) == resources.cend()); 

    resources.emplace(name, std::make_shared<Resource>(std::forward<Args>(args)...));
  }

  template<typename Resource>
  std::shared_ptr<Resource> get(const std::string& name) {
    // [TODO] KAJ 2021-11-09 23:08 - Think about shared_ptr vs weak_ptr
    constexpr auto id = type_id<Resource>();

    assert(_resources_by_type.find(id) != _resources_by_type.cend()); // Resources of this type do not exit

    auto resources = _resources_by_type.at(id);

    assert(resources.find(name) != resources.cend()); // Resource with this name does not exist

    return std::static_pointer_cast<Resource>(resources.at(name));
  }

private:

  std::unordered_map<uint32, std::unordered_map<std::string, std::shared_ptr<void>>> _resources_by_type{};

}; // class resource_cache

} // namespace sbx

#endif // SBX_CORE_RESOURCE_CACHE_HPP_
