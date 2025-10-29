#ifndef LIBSBX_MODELS_MATERIAL_HPP_
#define LIBSBX_MODELS_MATERIAL_HPP_

#include <cinttypes>
#include <cmath>

#include <libsbx/math/uuid.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/graphics/images/image2d.hpp>

namespace sbx::models {

enum class alpha_mode : std::uint8_t { 
  opaque, 
  mask,
  blend
}; // enum class alpha_mode

enum class material_feature : std::uint8_t {
  none           = 0,
  emission       = utility::bit_v<0>,
  normal_map     = utility::bit_v<1>, 
  occlusion      = utility::bit_v<2>, 
  height         = utility::bit_v<3>, 
  clearcoat      = utility::bit_v<4>, 
  anisotropy     = utility::bit_v<5>,
  cast_shadow    = utility::bit_v<6>,
  receive_shadow = utility::bit_v<7>
}; // struct material_feature

struct alignas(16) material_data {
  std::uint32_t albedo_index;
  std::uint32_t normal_index;
  std::uint32_t mrao_index;
  std::uint32_t emissive_index;

  sbx::math::color base_color;
  sbx::math::color emissive_color;

  std::float_t metallic;
  std::float_t roughness;
  std::float_t occlusion;
  std::float_t emissive_strength;

  std::float_t alpha_cutoff;
  std::float_t normal_scale;
  std::uint32_t flags;
  std::uint32_t _pad0;
}; // struct material_data

static_assert(sizeof(material_data) <= 256u);
static_assert(alignof(material_data) == 16u);

struct material_key {
  std::uint16_t alpha           : 2;
  std::uint16_t is_double_sided : 1;
  std::uint16_t _pad0           : 5;
  std::uint16_t feature_mask    : 8;

  material_key() {
    std::memset(this, 0, sizeof(material_key));
  }

}; // struct material_key

static_assert(sizeof(material_key) == sizeof(std::uint16_t));
static_assert(alignof(material_key) == alignof(std::uint16_t));

inline auto operator==(const material_key& lhs, const material_key& rhs) -> bool { 
  return std::memcmp(&lhs, &rhs, sizeof(material_key)) == 0; 
}

struct material_key_hash {
  auto operator()(const material_key& key) const noexcept -> std::size_t {
    return utility::djb2_hash{}({reinterpret_cast<const std::uint8_t*>(&key), sizeof(material_key)});
  }
}; // struct material_key_hash

struct material {
  math::color base_color{math::color::white()};
  std::float_t metallic{0.0f};
  std::float_t roughness{0.5f};
  std::float_t occlusion{1.0f};
  math::color emissive_color{0, 0, 0, 1};
  std::float_t emissive_strength{0.0f};
  std::float_t alpha_cutoff{0.9f};

  graphics::image2d_handle albedo{};
  graphics::image2d_handle normal{};
  graphics::image2d_handle mrao{};

  alpha_mode alpha{alpha_mode::opaque};
  bool is_double_sided{false};

  utility::bit_field<material_feature> features;

  operator material_key() const {
    auto key = material_key{};

    key.alpha = static_cast<std::uint64_t>(alpha);
    key.is_double_sided = is_double_sided;
    // key.feature_mask = features.underlying();

    return key;
  }

}; // struct material

struct alignas(16) transform_data {
  math::matrix4x4 model;
  math::matrix4x4 normal;
}; // struct transform_data

struct alignas(16) instance_data {
  std::uint32_t transform_index;
  std::uint32_t material_index;
  std::uint32_t object_id_upper;
  std::uint32_t object_id_lower;
}; // struct instance_data
  
} // namespace sbx::models

#endif // LIBSBX_MODELS_MATERIAL_HPP_