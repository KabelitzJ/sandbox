#include <libsbx/mesh/mesh_source.hpp>

#include <fstream>
#include <bit>

#include <fmt/format.h>

#include <libbase64.h>

#include <nlohmann/json.hpp>

#include <libsbx/io/read_file.hpp>

#include <libsbx/math/vector2.hpp>  
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/quaternion.hpp>

namespace sbx::mesh {

mesh_source::mesh_source(const std::filesystem::path& path) {
  
}

} // namespace sbx::mesh
