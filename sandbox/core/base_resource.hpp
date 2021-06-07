#ifndef SBX_CORE_BASE_RESOURCE_HPP_
#define SBX_CORE_BASE_RESOURCE_HPP_

#include <filesystem>

namespace sbx {

struct base_resource_data {

  virtual ~base_resource_data() = default;

  std::filesystem::path path;

};

class base_resource {

public:
  base_resource() = default;
  virtual ~base_resource() = default;

protected:
  virtual base_resource_data* _load(const std::filesystem::path& path) = 0;
  virtual void _initialize(base_resource_data* resource_data) = 0;

}; // class base_resource

} // namespace sbx

#endif // SBX_CORE_BASE_RESOURCE_HPP_
