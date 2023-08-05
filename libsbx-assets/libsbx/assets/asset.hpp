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

class asset_id {

public:

  using value_type = math::uuid;

  asset_id() noexcept = default;

  ~asset_id() = default;

  operator value_type() const noexcept {
    return _value;
  }

  auto operator==(const asset_id& other) const noexcept -> bool {
    return _value == other._value;
  }

private:

  value_type _value;

}; // class asset_id

template<asset_type Type>
class asset {

public:

  using id_type = asset_id;

  inline static constexpr auto type = Type;

  virtual ~asset() = default;

  auto id() const noexcept -> const id_type& {
    return _id;
  }

private:

  id_type _id;

}; // class asset

} // namespace sbx::assets

template<>
struct std::hash<sbx::assets::asset_id> {
  auto operator()(const sbx::assets::asset_id& id) const noexcept -> std::size_t {
    return static_cast<sbx::assets::asset_id::value_type>(id);
  }
}; // struct std::hash

#endif // LIBSBX_ASSETS_ASSET_HPP_
