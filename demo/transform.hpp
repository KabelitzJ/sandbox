#ifndef DEMO_TRANSFORM_HPP_
#define DEMO_TRANSFORM_HPP_

#include <nlohmann/json.hpp>
#include <utility>

#include <utils/hash.hpp>

#include <math/vector3.hpp>
#include <math/quaternion.hpp>
#include <math/matrix4x4.hpp>

namespace demo {

class transform {

public:

  sbx::vector3 position{};
  sbx::quaternion rotation{};
  sbx::vector3 scale{};

}; // class transform

void to_json(nlohmann::json& json, const transform& transform) {
  json = nlohmann::json::object({
    {"position", transform.position},
    {"rotation", transform.rotation},
    {"scale", transform.scale}
  });
}

void from_json(const nlohmann::json& json, transform& transform) {
  json.at("position").get_to(transform.position);
  json.at("rotation").get_to(transform.rotation);
  json.at("scale").get_to(transform.scale);
}

} // namespace demo

template<>
struct std::hash<demo::transform> {
  std::size_t operator()(const demo::transform& transform) const noexcept {
    auto seed = std::size_t{0};
    sbx::hash_combine(seed, transform.position);
    sbx::hash_combine(seed, transform.rotation);
    sbx::hash_combine(seed, transform.scale);
    return seed;
  }
}; // struct std::hash

#endif // DEMO_TRANSFORM_HPP_
