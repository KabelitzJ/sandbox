#ifndef LIBSBX_SCENES_COMPONENTS_UUID_HPP_
#define LIBSBX_SCENES_COMPONENTS_UUID_HPP_

#include <cinttypes>

#include <libsbx/math/random.hpp>

namespace sbx::scenes {

class uuid {

public:

  using value_type = std::uint64_t;

  uuid()
  : _value{math::random::next<value_type>()} { }

  ~uuid() = default;

  auto value() const noexcept -> value_type {
    return _value;
  }

  operator value_type() const noexcept {
    return _value;
  }

  auto operator==(const uuid& other) const noexcept -> bool {
    return _value == other._value;
  }

private:

  value_type _value;

}; // class uuid

} // namespace sbx::scenes

template<>
struct std::hash<sbx::scenes::uuid> {
  auto operator()(const sbx::scenes::uuid& uuid) const noexcept -> std::size_t {
    auto hasher = std::hash<sbx::scenes::uuid::value_type>{};
    return hasher(uuid.value());
  }
}; // struct std::hash<sbx::scenes::uuid>

#endif // LIBSBX_SCENES_COMPONENTS_UUID_HPP_

