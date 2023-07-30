#ifndef LIBSBX_ASSETS_ASSET_HPP_
#define LIBSBX_ASSETS_ASSET_HPP_

#include <cinttypes>
#include <typeindex>
#include <utility>

#include <libsbx/utility/hash.hpp>

#include <libsbx/math/uuid.hpp>

namespace sbx::assets {

enum class asset_type : std::uint32_t {
  texture,
  mesh,
}; // enum class asset_type

template<asset_type Type>
class asset {

public:

  using id_type = math::uuid;

  inline static constexpr auto type = Type;

  virtual ~asset() = default;

  auto id() const noexcept -> const id_type& {
    return _id;
  }

private:

  id_type _id;

}; // class asset

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_ASSET_HPP_
