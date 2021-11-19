#ifndef SBX_CORE_RESOURCE_CACHE_HPP_
#define SBX_CORE_RESOURCE_CACHE_HPP_

#include <unordered_map>
#include <memory>
#include <string>
#include <cassert>
#include <utility>
#include <algorithm>

#include <types/primitives.hpp>

#include <utils/type_id.hpp>

#include "logger.hpp"
#include "resource_key.hpp"

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

    const auto key = resource_key{id, name};

    if (_resources.find(key) != _resources.cend()) {
      logger::error("Resource with name '{}' and type '{}' already exists in the cache.", name, id);
      assert(false); // Duplicate resource loaded
    }

    _resources.emplace(key, std::make_shared<Resource>(std::forward<Args>(args)...));
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
    constexpr auto id = type_id<Resource>();

    const auto key = resource_key{id, name};

    const auto entry = _resources.find(key);

    if (entry == _resources.cend()) {
      logger::error("Resource with name '{}' and type '{}' does not exist in the cache.", name, id);
      assert(false); // Resource not found
    }

    return std::static_pointer_cast<Resource>(entry->second);
  }

private:

  std::unordered_map<resource_key, std::shared_ptr<void>> _resources{};

}; // class resource_cache

} // namespace sbx

#endif // SBX_CORE_RESOURCE_CACHE_HPP_
