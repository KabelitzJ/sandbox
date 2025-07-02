#ifndef LIBSBX_MESH_MESH_SOURCE_HPP_
#define LIBSBX_MESH_MESH_SOURCE_HPP_

#include <filesystem>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

#include <libsbx/utility/hashed_string.hpp>

namespace sbx::mesh {

class mesh_source {

public:

  mesh_source(const std::filesystem::path& path);

  

private:

  struct attribute {
    std::string name;
    std::string type;
    std::uint32_t component_count;
    std::uint32_t stride;
  }; // struct attribute

  struct entry {
    std::vector<std::uint8_t> data;
    attribute attribute;
  }; // struct entry

  std::unordered_map<std::string, entry> _attributes;

}; // class mesh_source

} // namespace sbx::mesh

#endif // LIBSBX_MESH_MESH_SOURCE_HPP_
