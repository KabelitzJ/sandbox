#include "sparse_set.hpp"

namespace sbx {

sparse_set::sparse_set(sparse_set&& other) noexcept
: _sparse{std::move(other._sparse)},
  _dense{std::move(other._dense)} { }

sparse_set& sparse_set::operator=(sparse_set&& other) noexcept {
  _sparse = std::move(other._sparse);
  _dense = std::move(other._dense);

  return *this;
}

} // namespace sbx
