#include <cassert>
#include <cmath>
#include <iomanip>
#include <string>

namespace sbx {

template<arithmetic Type>
inline constexpr basic_vector3<Type>::basic_vector3() noexcept
: x{value_type{0}}, 
  y{value_type{0}}, 
  z{value_type{0}} { }

template<arithmetic Type>
inline constexpr basic_vector3<Type>::basic_vector3(const value_type value) noexcept
: x{value},
  y{value},
  z{value} { }

template<arithmetic Type>
inline constexpr basic_vector3<Type>::basic_vector3(const value_type _x, const value_type _y, const value_type _z) noexcept
: x{_x},
  y{_y},
  z{_z} { }

template<arithmetic Type>
inline constexpr basic_vector3<Type>::basic_vector3(const basic_vector2<Type>& vector, const Type _z) noexcept
: x{vector.x},
  y{vector.y},
  z{_z} { }

template<arithmetic Type>
template<arithmetic Other>
inline constexpr basic_vector3<Type>::basic_vector3(const basic_vector3<Other>& other) noexcept
: x{static_cast<value_type>(other.x)},
  y{static_cast<value_type>(other.y)},
  z{static_cast<value_type>(other.z)} { }

template<arithmetic Type>
inline constexpr basic_vector3<Type> basic_vector3<Type>::normalized(const basic_vector3& vector) noexcept {
  const auto length = vector.length();
  return length == value_type{0} ? basic_vector3<Type>{} : vector / length;
}

template<arithmetic Type>
inline constexpr Type basic_vector3<Type>::dot(const basic_vector3& lhs, const basic_vector3& rhs) noexcept {
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> basic_vector3<Type>::cross(const basic_vector3& lhs, const basic_vector3& rhs) noexcept {
  constexpr auto x = lhs.y * rhs.z - lhs.z * rhs.y;
  constexpr auto y = lhs.z * rhs.x - lhs.x * rhs.z;
  constexpr auto z = lhs.x * rhs.y - lhs.y * rhs.x;

  return basic_vector3<Type>{x, y, z};
}

template<arithmetic Type>
template<arithmetic Other>
constexpr basic_vector3<Type>& basic_vector3<Type>::operator=(const basic_vector3<Other>& other) noexcept {
  x = static_cast<value_type>(other.x);
  y = static_cast<value_type>(other.y);
  z = static_cast<value_type>(other.z);

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator-() noexcept {
  x = -x;
  y = -y;
  z = -z;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator+=(const basic_vector3<Type>& other) noexcept {
  x += other.x;
  y += other.y;
  z += other.z;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator-=(const basic_vector3<Type>& other) noexcept {
  x -= other.x;
  y -= other.y;
  z -= other.z;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator*=(const Type scalar) noexcept {
  x *= scalar;
  y *= scalar;
  z *= scalar;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator/=(const Type scalar) {
  if (scalar == value_type{0}) {
    throw std::domain_error("Division by zero");
  }

  x /= scalar;
  y /= scalar;
  z /= scalar;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>::reference basic_vector3<Type>::operator[](const index_type index) noexcept {
  assert(index < 3);

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
  }
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>::const_reference basic_vector3<Type>::operator[](const index_type index) const noexcept {
  assert(index < 3);

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
  }
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>::length_type basic_vector3<Type>::length() const noexcept {
  return std::sqrt(x * x + y * y + z * z);
}

template<arithmetic Type>
inline constexpr void basic_vector3<Type>::normalize() noexcept {
  const auto length = this->length();

  if (length != length_type{0}) {
    x /= length;
    y /= length;
    z /= length;
  }
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>::pointer basic_vector3<Type>::data() noexcept {
  return &x;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>::const_pointer basic_vector3<Type>::data() const noexcept {
  return &x;
}

template<arithmetic Type>
inline constexpr bool operator==(const basic_vector3<Type>& lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> operator+(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs += rhs;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> operator-(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs -= rhs;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> operator*(basic_vector3<Type> lhs, const Type rhs) noexcept {
  return lhs *= rhs;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> operator/(basic_vector3<Type> lhs, const Type rhs) {
  return lhs /= rhs;
}

template<arithmetic Type>
inline constexpr std::ostream& operator<<(std::ostream& output_stream, const basic_vector3<Type>& vector) {
  auto default_state = std::ios{nullptr};
  default_state.copyfmt(output_stream);

  output_stream << std::setprecision(3) << std::fixed;

  output_stream << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";

  output_stream.copyfmt(default_state);

  return output_stream; 
}

template<arithmetic Type>
inline constexpr std::ofstream& operator<<(std::ofstream& output_stream, const basic_vector3<Type>& vector) {
  auto json = nlohmann::json::object();

  to_json(json, vector);

  output_stream << json;

  return output_stream;
}

template<arithmetic Type, output_stream<Type> OutputStream>
inline constexpr OutputStream& operator<<(OutputStream& output_stream, const basic_vector3<Type>& vector) {
  // [TODO] KAJ 2022-01-31 09:48 - Find a suitable format for vectors and implement a parser for that format.
  return output_stream;
}

template<arithmetic Type>
inline constexpr std::istream& operator>>(std::istream& input_stream, basic_vector3<Type>& vector) {
  // [TODO] KAJ 2022-01-31 09:48 - Find a suitable format for vectors and implement a parser for that format.
  return input_stream;
}

template<arithmetic Type, input_stream<Type> InputStream>
inline constexpr InputStream& operator>>(InputStream& input_stream, basic_vector3<Type>& vector) {
  // [TODO] KAJ 2022-01-31 09:48 - Find a suitable format for vectors and implement a parser for that format.
  return input_stream;
}

template<arithmetic Type>
void to_json(nlohmann::json& json, const basic_vector3<Type>& vector) {
  json = nlohmann::json::object({
    {"x", vector.x},
    {"y", vector.y},
    {"z", vector.z}
  });
}

template<arithmetic Type>
void from_json(const nlohmann::json& json, basic_vector3<Type>& vector) {
  json.at("x").get_to(vector.x);
  json.at("y").get_to(vector.y);
  json.at("z").get_to(vector.z);
}

} // namespace sbx

template<sbx::arithmetic Type>
inline std::size_t std::hash<sbx::basic_vector3<Type>>::operator()(const sbx::basic_vector3<Type>& vector) const noexcept {
  auto seed = std::size_t{0};
  sbx::hash_combine(seed, vector.x);
  sbx::hash_combine(seed, vector.y);
  sbx::hash_combine(seed, vector.z);
  return seed;
}
