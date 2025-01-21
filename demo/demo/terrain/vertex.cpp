#include <demo/terrain/vertex.hpp>

namespace demo {

auto operator==(const vertex& lhs, const vertex& rhs) noexcept -> bool {
  return lhs.position == rhs.position && lhs.height_index == rhs.height_index && lhs.normal == rhs.normal && lhs.uv == rhs.uv;
}

} // namespace demo
