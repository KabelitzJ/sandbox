#ifndef LIBSBX_ASSETS_ASSET_HANDLE_HPP_
#define LIBSBX_ASSETS_ASSET_HANDLE_HPP_

#include <typeindex>

#include <libsbx/math/uuid.hpp>

#include <libsbx/utility/hash.hpp>

namespace sbx::assets {

template<typename Type>
class asset_handle {

  friend struct std::hash<asset_handle<Type>>;

public:

  asset_handle()
  : _type{std::type_index{typeid(Type)}.hash_code()} { }

  auto operator==(const asset_handle& other) const noexcept -> bool {
    return _type == other._type && _id == other._id;
  }

private:

  std::size_t _type;
  math::uuid _id;

}; // class asset_handle

} // namespace sbx::assets

template<typename Type>
struct std::hash<sbx::assets::asset_handle<Type>> {
  auto operator()(const sbx::assets::asset_handle<Type>& handle) const noexcept -> std::size_t {
    auto seed = std::size_t{0};
    sbx::utility::hash_combine(seed, handle._type, handle._id);
    return seed;
  }
}; // struct std::hash<sbx::assets::asset_handle<Type>>

#endif // LIBSBX_ASSETS_ASSET_HANDLE_HPP_
