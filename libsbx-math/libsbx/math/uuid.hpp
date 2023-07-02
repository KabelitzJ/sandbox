#ifndef LIBSBX_MATH_UUID_HPP_
#define LIBSBX_MATH_UUID_HPP_

#include <cinttypes>

#include <libsbx/math/random.hpp>

namespace sbx::math {

class uuid {

public:

  using value_type = std::uint64_t;

  uuid()
  : _value{random::next<value_type>()} { }

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

} // namespace sbx::math

template<>
struct std::hash<sbx::math::uuid> {
  auto operator()(const sbx::math::uuid& uuid) const noexcept -> std::size_t {
    auto hasher = std::hash<sbx::math::uuid::value_type>{};
    return hasher(uuid.value());
  }
}; // struct std::hash<sbx::math::uuid>

#endif // LIBSBX_MATH_UUID_HPP_

