#ifndef LIBSBX_ASSETS_ASSET_HPP_
#define LIBSBX_ASSETS_ASSET_HPP_

#include <cinttypes>
#include <typeindex>
#include <utility>

#include <libsbx/utility/hash.hpp>

#include <libsbx/math/uuid.hpp>

namespace sbx::assets {

enum class asset_type : std::uint32_t {
  mesh,
  texture,
  font,
  sound
}; // enum class asset_type

class asset_id final : public math::uuid {

public:

  using super = math::uuid;

  template<typename... Args>
  asset_id(Args&&... args)
  : super{std::forward<Args>(args)...} { }

}; // class asset_id

template<asset_type Type>
class asset {

public:

  inline static constexpr auto type = Type;

  virtual ~asset() = default;

  auto id() const noexcept -> asset_id {
    return _id;
  }

private:

  asset_id _id;

}; // class asset

} // namespace sbx::assets

template<>
struct std::hash<sbx::assets::asset_id> {
  auto operator()(const sbx::assets::asset_id& id) const noexcept -> std::size_t {
    return static_cast<sbx::assets::asset_id::value_type>(id);
  }
}; // struct std::hash

#endif // LIBSBX_ASSETS_ASSET_HPP_
