#ifndef LIBSBX_ASSETS_ASSET_HPP_
#define LIBSBX_ASSETS_ASSET_HPP_

#include <cinttypes>
#include <typeindex>
#include <utility>

#include <libsbx/utility/hash.hpp>

#include <libsbx/math/uuid.hpp>

namespace sbx::assets {

template<typename Type>
class handle {

  friend class std::hash<handle<Type>>;

public:

  handle()
  : _id{math::uuid{}},
    _type{std::type_index{typeid(Type)}.hash_code()} { }

private:

  math::uuid _id;
  std::uint32_t _type;

}; // class handle

template<typename Derived>
class asset {

public:

  using handle_type = handle<Derived>;

  virtual ~asset() = default;

private:

}; // class asset

} // namespace sbx::assets

template<typename Derived>
struct std::hash<sbx::assets::handle<Derived>> {
  auto operator()(const sbx::assets::handle<Derived>& handle) const noexcept -> std::size_t {
    auto seed = std::size_t{0};
    sbx::utility::hash_combine(seed, handle._id, handle._type);
    return seed;
  }
}; // struct std::hash<handle<Derived>>

#endif // LIBSBX_ASSETS_ASSET_HPP_
