#ifndef LIBSBX_ASSETS_ASSET_HPP_
#define LIBSBX_ASSETS_ASSET_HPP_

#include <libsbx/assets/asset_handle.hpp>

namespace sbx::assets {

struct asset_base {
  virtual ~asset_base() = default;
}; // class asset_base

template<typename Derived>
class asset : public asset_base {

public:

  using handle_type = asset_handle<Derived>;

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
