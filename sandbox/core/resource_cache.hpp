#ifndef SBX_CORE_RESOURCE_CACHE_HPP_
#define SBX_CORE_RESOURCE_CACHE_HPP_

#include <unordered_map>
#include <memory>
#include <string>
#include <cassert>

#include <types/primitives.hpp>

#include <utils/type_id.hpp>

#include "logger.hpp"

namespace sbx {

/**
 * @brief Provides a global cache for resources.
 *        Resources are stored by type and can be retrieved by name.
 */
class resource_cache {

public:

  /**
   * @brief Construct a new resource cache object
   */
  resource_cache();

  /**
   * @brief Destroy the resource cache object
   */
  ~resource_cache();

  /**
   * @brief Loads a new resource of a given type, constructs it with the given arguments and stores it with the given name.
   * 
   * @tparam Resource Type of the resource to load
   * @tparam Args     Types of the arguments to pass to the constructor of the resource
   * @param name      Name identifier of the resource
   * @param args      Arguments to pass to the constructor of the resource
   */
  template<typename Resource, typename... Args>
  void load(const std::string& name, Args&&... args) {
    constexpr auto id = type_id<Resource>();

    // [NOTE] KAJ 2021-11-11 21:58 - Make sure we have a cache for this type
    if (_resources_by_type.find(id) == _resources_by_type.cend()) {
      _resources_by_type.emplace(id, std::unordered_map<std::string, std::shared_ptr<void>>());
    }

    auto& resources = _resources_by_type[id];

    assert(resources.find(name) == resources.cend()); // Resource with same type and name already exists

    resources.emplace(name, std::make_shared<Resource>(std::forward<Args>(args)...));
  }

  /**
   * @brief Retrieves a resource of a given type by its name.
   * 
   * @tparam Resource Type of the resource to retrieve 
   * @param name      Name identifier of the resource
   * 
   * @return std::shared_ptr<Resource> Pointer to the resource
   */
  template<typename Resource>
  std::shared_ptr<Resource> get(const std::string& name) {
    // [TODO] KAJ 2021-11-09 23:08 - Think about shared_ptr vs weak_ptr
    constexpr auto id = type_id<Resource>();

    assert(_resources_by_type.find(id) != _resources_by_type.cend()); // Resources of this type do not exit

    auto resources = _resources_by_type[id];

    assert(resources.find(name) != resources.cend()); // Resource with this name does not exist

    return std::static_pointer_cast<Resource>(resources[name]);
  }

private:

  std::unordered_map<uint32, std::unordered_map<std::string, std::shared_ptr<void>>> _resources_by_type{};

}; // class resource_cache

} // namespace sbx

#endif // SBX_CORE_RESOURCE_CACHE_HPP_
