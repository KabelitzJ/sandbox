#include <cassert>
#include <cmath>

#include <utils/hash.hpp>

namespace sbx {

template<std::floating_point Type>
inline constexpr basic_quaternion<Type>::basic_quaternion() noexcept
: x{value_type{0}}, 
  y{value_type{0}},
  z{value_type{0}}, 
  w{value_type{0}} { }

template<std::floating_point Type>
inline constexpr basic_quaternion<Type>::basic_quaternion(const Type _x, const Type _y, const Type _z, const Type _w) noexcept
: x{_x},
  y{_y},
  z{_z},
  w{_w} { }

template<std::floating_point Type>
constexpr basic_quaternion<Type>::basic_quaternion(const basic_vector3<Type>& axis, const angle<Type>& angle) noexcept
: x{axis.x},
  y{axis.y},
  z{axis.z},
  w{angle.to_degrees()} { }

template<std::floating_point Type>
inline constexpr basic_quaternion<Type>::basic_quaternion(const basic_vector3<Type>& euler_angles) noexcept {
  const auto cosign = basic_vector3<value_type>{
    std::cos(euler_angles.x * value_type{0.5}),
    std::cos(euler_angles.y * value_type{0.5}),
    std::cos(euler_angles.z * value_type{0.5})
  };

  const auto sign = basic_vector3<value_type>{
    std::sin(euler_angles.x * value_type{0.5}),
    std::sin(euler_angles.y * value_type{0.5}),
    std::sin(euler_angles.z * value_type{0.5})
  };

  x = sign.x * cosign.y * cosign.z - cosign.x * sign.y * sign.z;
  y = cosign.x * sign.y * cosign.z + sign.x * cosign.y * sign.z;
  z = cosign.x * cosign.y * sign.z - sign.x * sign.y * cosign.z;
  w = cosign.x * cosign.y * cosign.z + sign.x * sign.y * sign.z;
}

template<std::floating_point Type>
template<std::floating_point Other>
inline constexpr basic_quaternion<Type>::basic_quaternion(const basic_quaternion<Other>& other) noexcept
: x{static_cast<value_type>(other.x)},
  y{static_cast<value_type>(other.y)},
  z{static_cast<value_type>(other.z)},
  w{static_cast<value_type>(other.w)} { }

template<std::floating_point Type>
template<std::floating_point Other>
inline constexpr basic_quaternion<Type>& basic_quaternion<Type>::operator=(const basic_quaternion<Other>& other) noexcept {
  x = static_cast<value_type>(other.x);
  y = static_cast<value_type>(other.y);
  z = static_cast<value_type>(other.z);
  w = static_cast<value_type>(other.w);

  return *this;
}

template<std::floating_point Type>
inline constexpr basic_quaternion<Type>::reference basic_quaternion<Type>::operator[](const index_type index) noexcept {
  assert(index < 4);

  switch (index) {
    default:
    case 0: {
      return x;
    }
    case 1: {
      return y;
    }
    case 2: {
      return z;
    }
    case 3: {
      return w;
    }
  }
}

template<std::floating_point Type>
inline constexpr basic_quaternion<Type>::const_reference basic_quaternion<Type>::operator[](const index_type index) const noexcept {
  assert(index < 4);

  switch (index) {
    default:
    case 0: {
      return x;
    }
    case 1: {
      return y;
    }
    case 2: {
      return z;
    }
    case 3: {
      return w;
    }
  }
}

template<std::floating_point Type>
inline constexpr basic_quaternion<Type>::pointer basic_quaternion<Type>::data() noexcept {
  return &x;
}

template<std::floating_point Type>
inline constexpr basic_quaternion<Type>::const_pointer basic_quaternion<Type>::data() const noexcept {
  return &x;
}

template<std::floating_point Type>
inline constexpr bool operator==(const basic_quaternion<Type>& lhs, const basic_quaternion<Type>& rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}

template<std::floating_point Type>
void to_json(nlohmann::json& json, const basic_quaternion<Type>& quat) {
  json = nlohmann::json::object({
    {"x", quat.x},
    {"y", quat.y},
    {"z", quat.z},
    {"w", quat.w}
  });
}

template<std::floating_point Type>
void from_json(const nlohmann::json& json, basic_quaternion<Type>& quat) {
  json.at("x").get_to(quat.x);
  json.at("y").get_to(quat.y);
  json.at("z").get_to(quat.z);
  json.at("w").get_to(quat.w);
}

} // namespace sbx

template<std::floating_point Type>
std::size_t std::hash<sbx::basic_quaternion<Type>>::operator()(const sbx::basic_quaternion<Type>& quat) const noexcept {
  auto seed = std::size_t{0};
  sbx::hash_combine(seed, quat.x);
  sbx::hash_combine(seed, quat.y);
  sbx::hash_combine(seed, quat.z);
  sbx::hash_combine(seed, quat.w);
  return seed;
}
