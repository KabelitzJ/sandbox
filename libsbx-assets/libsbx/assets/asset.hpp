#ifndef LIBSBX_ASSETS_ASSET_HPP_
#define LIBSBX_ASSETS_ASSET_HPP_

#include <cinttypes>
#include <typeindex>
#include <utility>

#include <libsbx/utility/hash.hpp>

#include <libsbx/math/uuid.hpp>

namespace sbx::assets {

class asset {

public:

  using handle_type = math::uuid;

  asset()
  : _handle{} { }

  virtual ~asset() = default;

  auto handle() const noexcept -> const handle_type& {
    return _handle;
  }

private:

  handle_type _handle;

}; // class asset

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_ASSET_HPP_
