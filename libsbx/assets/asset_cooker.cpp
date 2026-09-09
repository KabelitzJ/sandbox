// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/assets/asset_cooker.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iterator>
#include <numeric>
#include <string_view>
#include <system_error>
#include <unordered_map>

#include <yaml-cpp/yaml.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/math.hpp>

#include <meshoptimizer.h>

#include <libsbx/utility/iterator.hpp>
#include <libsbx/utility/fourcc.hpp>
#include <libsbx/utility/logger.hpp>

#include <libsbx/math/matrix_cast.hpp>

#include <libsbx/core/engine.hpp>

namespace sbx::assets {

inline constexpr auto texture_magic = utility::fourcc_v<"SBTX">;  // 'SBTX'
inline constexpr auto mesh_magic = utility::fourcc_v<"SBSH">;   // 'SBSH'
inline constexpr auto material_magic = utility::fourcc_v<"SBMT">; // 'SBMT'
inline constexpr auto environment_magic = utility::fourcc_v<"SBEN">; // 'SBEN'
inline constexpr auto skeleton_magic = utility::fourcc_v<"SBSK">; // 'SBSK'
inline constexpr auto animation_magic = utility::fourcc_v<"SBAN">; // 'SBAN'
inline constexpr auto font_magic = utility::fourcc_v<"SBFN">; // 'SBFN'

// The pixel height a font's atlas is rasterized at; every glyph metric is stored normalized by
// this (i.e. per one unit of ui_text::font_size), so one atlas serves any font_size at runtime.
inline constexpr auto font_reference_size = 48.0f;
inline constexpr auto font_sdf_padding = 4;
inline constexpr auto font_sdf_onedge_value = 128u;
inline constexpr auto font_sdf_pixel_dist_scale = 32.0f; // onedge_value / padding
inline constexpr auto font_first_codepoint = std::uint32_t{32u};
inline constexpr auto font_codepoint_count = std::uint32_t{224u}; // 32..255: printable ASCII + Latin-1

struct texture_header {
  std::uint32_t magic;
  std::uint32_t version;
  std::uint32_t width;
  std::uint32_t height;
  std::uint32_t channels;   // always 4 (RGBA) for now
  std::uint32_t data_size;  // bytes of pixel data following the header
}; // struct texture_header

struct font_header {
  std::uint32_t magic;
  std::uint32_t version;
  std::uint32_t atlas_width;
  std::uint32_t atlas_height;
  std::uint32_t glyph_count;
  std::uint32_t first_codepoint;
  std::float_t line_height;
  std::float_t ascent;
  std::float_t descent;
  std::uint32_t atlas_data_size; // bytes of single-channel atlas pixel data following the glyph table
}; // struct font_header

struct font_glyph_record {
  std::float_t uv_x0;
  std::float_t uv_y0;
  std::float_t uv_x1;
  std::float_t uv_y1;
  std::float_t width;
  std::float_t height;
  std::float_t bearing_x;
  std::float_t bearing_y;
  std::float_t advance;
}; // struct font_glyph_record

struct mesh_file_header {
  std::uint32_t magic;
  std::uint32_t version;
  std::uint32_t vertex_count;      // logical count after decoding vertex_data
  std::uint32_t index_count;       // logical count after decoding index_data (spans every submesh's LOD chain, not just LOD0)
  std::uint32_t submesh_count;
  std::float_t bounds_min[3];
  std::float_t bounds_max[3];
  std::uint32_t vertex_data_size;  // bytes of meshopt-encoded vertex buffer following the header
  std::uint32_t index_data_size;   // bytes of meshopt-encoded index buffer following the vertex data
  std::uint32_t flags;             // bit 0 = has_skin_data
  std::uint32_t skin_vertex_data_size; // bytes of *raw* (unencoded) skin_vertex array following the index data; 0 when unskinned
  std::uint32_t animation_clip_count;  // clips cooked as a side effect, resolvable via derive_animation_clip_uuid(id, 0..count)
}; // struct mesh_file_header

inline constexpr auto mesh_flag_has_skin_data = std::uint32_t{1u << 0u};

struct submesh_file_record {
  std::uint32_t index_offset;
  std::uint32_t index_count;
  std::float_t bounds_min[3];
  std::float_t bounds_max[3];
  std::uint64_t material_uuid; // 0 = none
  std::uint32_t lod_count;     // submesh_lod_record entries immediately following this record
}; // struct submesh_file_record

struct submesh_lod_record {
  std::uint32_t index_offset;
  std::uint32_t index_count;
  std::float_t error;
}; // struct submesh_lod_record

// Texture slots are variable-length path strings (assets-directory-relative, empty = none), not
// fixed uuid64s -- see material_description's doc comment for why. name and the five slot strings
// follow this header back to back, each preceded by nothing (lengths are all up front here) in
// the fixed order: name, albedo, normal, metallic_roughness, occlusion, emissive.
struct material_file_header {
  std::uint32_t magic;
  std::uint32_t version;
  std::float_t base_color_factor[4];
  std::float_t emissive_factor[3];
  std::float_t metallic_factor;
  std::float_t roughness_factor;
  std::uint32_t alpha_mode;
  std::float_t alpha_cutoff;
  std::uint32_t is_double_sided;
  std::float_t normal_scale;
  std::float_t occlusion_strength;
  std::float_t emissive_strength;
  std::float_t ior;
  std::uint32_t name_length;
  std::uint32_t albedo_path_length;
  std::uint32_t normal_path_length;
  std::uint32_t metallic_roughness_path_length;
  std::uint32_t occlusion_path_length;
  std::uint32_t emissive_path_length;
}; // struct material_file_header

struct skeleton_file_header {
  std::uint32_t magic;
  std::uint32_t version;
  std::uint32_t joint_count;
}; // struct skeleton_file_header

// Immediately followed by name_length bytes of the joint's name.
struct skeleton_joint_record {
  std::int32_t parent_index; // -1 = root; always < this joint's own index (topologically sorted)
  std::float_t inverse_bind_matrix[16]; // column-major, matches math::matrix4x4's layout
  std::float_t bind_translation[3];
  std::float_t bind_rotation[4]; // x, y, z, w
  std::float_t bind_scale[3];
  std::uint32_t name_length;
}; // struct skeleton_joint_record

struct animation_clip_file_header {
  std::uint32_t magic;
  std::uint32_t version;
  std::float_t duration;
  std::uint32_t channel_count;
  std::uint32_t name_length; // name bytes immediately follow this header
}; // struct animation_clip_file_header

// Immediately followed by translation_key_count vector3_key_records, then rotation_key_count
// quaternion_key_records, then scale_key_count vector3_key_records.
struct animation_channel_record {
  std::uint32_t joint_index;
  std::uint32_t translation_key_count;
  std::uint32_t rotation_key_count;
  std::uint32_t scale_key_count;
  std::uint32_t translation_interpolation;
  std::uint32_t rotation_interpolation;
  std::uint32_t scale_interpolation;
}; // struct animation_channel_record

struct vector3_key_record {
  std::float_t time;
  std::float_t value[3];
}; // struct vector3_key_record

struct quaternion_key_record {
  std::float_t time;
  std::float_t value[4]; // x, y, z, w
}; // struct quaternion_key_record

// "type"+"value" tag pair -- animation_parameter_value's alternative *is* its type, so this is
// purely a persistence detail (the runtime API never switches on a type enum, see
// animation_graph.hpp's doc comment). Mirrored by save_animation_parameter_value in
// asset_residency.cpp (save_animation_graph is a synchronous, editor-only write path -- not part
// of this refactor).
static auto load_animation_parameter_value(const YAML::Node& node) -> animation_parameter_value {
  const auto type = node["type"] ? node["type"].as<std::string>() : std::string{"float"};

  if (type == "bool") {
    return animation_parameter_value{node["value"] ? node["value"].as<bool>() : false};
  }

  if (type == "int") {
    return animation_parameter_value{node["value"] ? node["value"].as<std::int32_t>() : std::int32_t{0}};
  }

  if (type == "trigger") {
    return animation_parameter_value{animation_trigger{}};
  }

  return animation_parameter_value{node["value"] ? node["value"].as<std::float_t>() : 0.0f};
}

static auto load_animation_condition_comparator(const std::string& value) -> animation_condition_comparator {
  if (value == "not_equals") return animation_condition_comparator::not_equals;
  if (value == "greater") return animation_condition_comparator::greater;
  if (value == "greater_or_equal") return animation_condition_comparator::greater_or_equal;
  if (value == "less") return animation_condition_comparator::less;
  if (value == "less_or_equal") return animation_condition_comparator::less_or_equal;
  return animation_condition_comparator::equals;
}

auto asset_cooker::cooked_path(const math::uuid& id, std::string_view extension) -> std::filesystem::path {
  const auto& project = core::engine::project();

  return project.library_directory() / fmt::format("{}{}", id.value(), extension);
}

auto asset_cooker::resolve_cooked_material(const math::uuid& id) -> std::optional<material_description> {
  const auto cooked = cooked_path(id, ".sbxmat");

  if (!std::filesystem::exists(cooked)) {
    return std::nullopt;
  }

  auto in = std::ifstream{cooked, std::ios::binary};

  if (!in) {
    utility::logger<"assets">::warn("Could not open cooked material '{}'", cooked.generic_string());
    return std::nullopt;
  }

  auto header = material_file_header{};
  in.read(reinterpret_cast<char*>(&header), sizeof(header));

  if (!in || header.magic != material_magic || header.version != material_cook_version) {
    utility::logger<"assets">::warn("Invalid cooked material '{}'", cooked.generic_string());
    return std::nullopt;
  }

  const auto read_string = [&in](std::uint32_t length) -> std::optional<std::string> {
    auto value = std::string(length, '\0');

    if (length > 0u) {
      in.read(value.data(), static_cast<std::streamsize>(length));

      if (!in) {
        return std::nullopt;
      }
    }

    return value;
  };

  const auto name = read_string(header.name_length);
  const auto albedo = read_string(header.albedo_path_length);
  const auto normal = read_string(header.normal_path_length);
  const auto metallic_roughness = read_string(header.metallic_roughness_path_length);
  const auto occlusion = read_string(header.occlusion_path_length);
  const auto emissive = read_string(header.emissive_path_length);

  if (!name || !albedo || !normal || !metallic_roughness || !occlusion || !emissive) {
    return std::nullopt;
  }

  auto description = material_description{};
  description.name = name->empty() ? std::string{"material"} : *name;
  description.base_color_factor = math::color{header.base_color_factor[0], header.base_color_factor[1], header.base_color_factor[2], header.base_color_factor[3]};
  description.emissive_factor = math::vector3{header.emissive_factor[0], header.emissive_factor[1], header.emissive_factor[2]};
  description.metallic_factor = header.metallic_factor;
  description.roughness_factor = header.roughness_factor;
  description.alpha = static_cast<alpha_mode>(header.alpha_mode);
  description.alpha_cutoff = header.alpha_cutoff;
  description.is_double_sided = header.is_double_sided != 0u;
  description.normal_scale = header.normal_scale;
  description.occlusion_strength = header.occlusion_strength;
  description.emissive_strength = header.emissive_strength;
  description.ior = header.ior;
  description.albedo = *albedo;
  description.normal = *normal;
  description.metallic_roughness = *metallic_roughness;
  description.occlusion = *occlusion;
  description.emissive = *emissive;

  return description;
}

auto asset_cooker::derive_material_uuid(const math::uuid& mesh, std::size_t index) -> math::uuid {
  // splitmix64 over (mesh uuid, index) — deterministic so re-cooking is stable.
  auto x = mesh.value() ^ (0x9e3779b97f4a7c15ull * (static_cast<std::uint64_t>(index) + 1ull));
  x ^= x >> 30; x *= 0xbf58476d1ce4e5b9ull;
  x ^= x >> 27; x *= 0x94d049bb133111ebull;
  x ^= x >> 31;

  return math::uuid::from_value(x == 0ull ? 1ull : x); // never nil
}

auto asset_cooker::derive_skeleton_uuid(const math::uuid& mesh) -> math::uuid {
  // Same splitmix64 shape as derive_material_uuid, salted differently so a mesh's skeleton uuid
  // never collides with one of its material uuids.
  auto x = mesh.value() ^ 0xff51afd7ed558ccdull;
  x ^= x >> 30; x *= 0xbf58476d1ce4e5b9ull;
  x ^= x >> 27; x *= 0x94d049bb133111ebull;
  x ^= x >> 31;

  return math::uuid::from_value(x == 0ull ? 1ull : x); // never nil
}

auto asset_cooker::derive_animation_clip_uuid(const math::uuid& mesh, std::size_t index) -> math::uuid {
  auto x = mesh.value() ^ (0xc2b2ae3d27d4eb4full * (static_cast<std::uint64_t>(index) + 1ull));
  x ^= x >> 30; x *= 0xbf58476d1ce4e5b9ull;
  x ^= x >> 27; x *= 0x94d049bb133111ebull;
  x ^= x >> 31;

  return math::uuid::from_value(x == 0ull ? 1ull : x); // never nil
}

auto asset_cooker::resolve_texture(const std::filesystem::path& source, const std::filesystem::path& cooked, bool needs_cook, bool& did_cook) -> std::optional<pixel_data> {
  did_cook = false;

  if (needs_cook) {
    if (!_cook_texture(source, cooked)) {
      utility::logger<"assets">::warn("Could not cook texture '{}'", source.generic_string());
      return std::nullopt;
    }
    did_cook = true;
  }

  auto data = pixel_data{};

  // If the blob is unreadable/out-of-date (e.g. cooker version bumped), recook once.
  if (!_load_cooked_texture(cooked, data.pixels, data.width, data.height)) {
    if (!_cook_texture(source, cooked) || !_load_cooked_texture(cooked, data.pixels, data.width, data.height)) {
      utility::logger<"assets">::warn("Could not load cooked texture '{}'", cooked.generic_string());
      return std::nullopt;
    }
    did_cook = true;
  }

  return data;
}

auto asset_cooker::resolve_environment(const std::filesystem::path& source, const std::filesystem::path& cooked, bool needs_cook, bool& did_cook) -> std::optional<pixel_data> {
  did_cook = false;

  if (needs_cook) {
    if (!_cook_environment_map(source, cooked)) {
      utility::logger<"assets">::warn("Could not cook environment map '{}'", source.generic_string());
      return std::nullopt;
    }
    did_cook = true;
  }

  auto data = pixel_data{};

  if (!_load_cooked_environment_map(cooked, data.pixels, data.width, data.height)) {
    if (!_cook_environment_map(source, cooked) || !_load_cooked_environment_map(cooked, data.pixels, data.width, data.height)) {
      utility::logger<"assets">::warn("Could not load cooked environment map '{}'", cooked.generic_string());
      return std::nullopt;
    }
    did_cook = true;
  }

  return data;
}

auto asset_cooker::resolve_font(const std::filesystem::path& source, const std::filesystem::path& cooked, bool needs_cook, bool& did_cook) -> std::optional<cooked_font_data> {
  did_cook = false;

  if (needs_cook) {
    if (!_cook_font(source, cooked)) {
      utility::logger<"assets">::warn("Could not cook font '{}'", source.generic_string());
      return std::nullopt;
    }
    did_cook = true;
  }

  auto data = cooked_font_data{};

  if (!_load_cooked_font(cooked, data)) {
    if (!_cook_font(source, cooked) || !_load_cooked_font(cooked, data)) {
      utility::logger<"assets">::warn("Could not load cooked font '{}'", cooked.generic_string());
      return std::nullopt;
    }
    did_cook = true;
  }

  return data;
}

auto asset_cooker::resolve_mesh(const std::filesystem::path& source, const math::uuid& id, const std::filesystem::path& cooked, bool needs_cook, bool& did_cook) -> std::optional<cooked_mesh_data> {
  did_cook = false;

  if (needs_cook) {
    if (!_cook_mesh(source, id, cooked)) {
      return std::nullopt;
    }
    did_cook = true;
  }

  auto data = cooked_mesh_data{};
  auto animation_clip_count = std::uint32_t{0u};

  if (!_load_cooked_mesh(cooked, data.vertices, data.indices, data.submeshes, data.bounds, data.skin_vertices, animation_clip_count)) {
    if (!_cook_mesh(source, id, cooked) || !_load_cooked_mesh(cooked, data.vertices, data.indices, data.submeshes, data.bounds, data.skin_vertices, animation_clip_count)) {
      utility::logger<"assets">::warn("Could not load cooked mesh '{}'", cooked.generic_string());
      return std::nullopt;
    }
    did_cook = true;
  }

  if (data.vertices.empty() || data.indices.empty()) {
    utility::logger<"assets">::warn("Mesh '{}' has no drawable geometry", source.generic_string());
    return std::nullopt;
  }

  if (!data.skin_vertices.empty()) {
    data.skeleton = derive_skeleton_uuid(id);

    data.animation_clips.reserve(animation_clip_count);

    for (auto index = std::uint32_t{0u}; index < animation_clip_count; ++index) {
      data.animation_clips.push_back(derive_animation_clip_uuid(id, index));
    }
  }

  return data;
}

auto asset_cooker::resolve_skeleton(const math::uuid& id) -> std::optional<std::vector<skeleton::joint>> {
  auto joints = std::vector<skeleton::joint>{};

  if (!_load_cooked_skeleton(id, joints)) {
    return std::nullopt;
  }

  return joints;
}

auto asset_cooker::resolve_animation_clip(const math::uuid& id) -> std::optional<animation_clip_data> {
  auto data = animation_clip_data{};

  if (!_load_cooked_animation_clip(id, data)) {
    return std::nullopt;
  }

  return data;
}

auto asset_cooker::parse_material_file(const std::filesystem::path& source) -> std::optional<material_description> {
  auto root = YAML::Node{};

  try {
    root = YAML::LoadFile(source.string());
  } catch (const std::exception& exception) {
    utility::logger<"assets">::warn("Could not parse material '{}' ({})", source.generic_string(), exception.what());
    return std::nullopt;
  }

  auto description = material_description{};

  if (root["name"]) description.name = root["name"].as<std::string>();
  if (root["base_color_factor"]) description.base_color_factor = root["base_color_factor"].as<math::color>();
  if (root["emissive_factor"]) description.emissive_factor = root["emissive_factor"].as<math::vector3>();
  if (root["metallic_factor"]) description.metallic_factor = root["metallic_factor"].as<std::float_t>();
  if (root["roughness_factor"]) description.roughness_factor = root["roughness_factor"].as<std::float_t>();
  if (root["alpha_mode"]) {
    const auto mode = root["alpha_mode"].as<std::string>();
    description.alpha = (mode == "blend") ? alpha_mode::blend : (mode == "mask") ? alpha_mode::mask : alpha_mode::opaque;
  }
  if (root["alpha_cutoff"]) description.alpha_cutoff = root["alpha_cutoff"].as<std::float_t>();
  if (root["is_double_sided"]) description.is_double_sided = root["is_double_sided"].as<bool>();
  if (root["casts_shadow"]) description.casts_shadow = root["casts_shadow"].as<bool>();
  if (root["receives_shadow"]) description.receives_shadow = root["receives_shadow"].as<bool>();
  if (root["normal_scale"]) description.normal_scale = root["normal_scale"].as<std::float_t>();
  if (root["occlusion_strength"]) description.occlusion_strength = root["occlusion_strength"].as<std::float_t>();
  if (root["emissive_strength"]) description.emissive_strength = root["emissive_strength"].as<std::float_t>();
  if (root["ior"]) description.ior = root["ior"].as<std::float_t>();

  const auto path_slot = [&](const char* key) -> std::string {
    if (const auto node = root[key]) {
      return node.as<std::string>();
    }
    return {};
  };

  description.albedo = path_slot("albedo");
  description.normal = path_slot("normal");
  description.metallic_roughness = path_slot("metallic_roughness");
  description.occlusion = path_slot("occlusion");
  description.emissive = path_slot("emissive");

  return description;
}

auto asset_cooker::parse_particle_effect_file(const std::filesystem::path& source) -> std::optional<particle_effect_description> {
  auto root = YAML::Node{};

  try {
    root = YAML::LoadFile(source.string());
  } catch (const std::exception& exception) {
    utility::logger<"assets">::warn("Could not parse particle_effect '{}' ({})", source.generic_string(), exception.what());
    return std::nullopt;
  }

  auto description = particle_effect_description{};

  if (root["name"]) description.name = root["name"].as<std::string>();

  const auto load_curve = [](const YAML::Node& node) -> curve {
    auto result = curve{};

    if (!node) {
      return result;
    }

    for (const auto key_node : node) {
      if (result.keys.is_full()) {
        break;
      }

      auto key = curve_key{};

      if (key_node["time"]) key.time = key_node["time"].as<std::float_t>();
      if (key_node["value"]) key.value = key_node["value"].as<std::float_t>();

      result.keys.push_back(key);
    }

    return result;
  };

  const auto load_gradient = [](const YAML::Node& node) -> gradient {
    auto result = gradient{};

    if (!node) {
      return result;
    }

    if (const auto color_keys_node = node["color_keys"]) {
      for (const auto key_node : color_keys_node) {
        if (result.color_keys.is_full()) {
          break;
        }

        auto key = gradient_color_key{};

        if (key_node["time"]) key.time = key_node["time"].as<std::float_t>();
        if (key_node["color"]) key.color = key_node["color"].as<math::color>();

        result.color_keys.push_back(key);
      }
    }

    if (const auto alpha_keys_node = node["alpha_keys"]) {
      for (const auto key_node : alpha_keys_node) {
        if (result.alpha_keys.is_full()) {
          break;
        }

        auto key = gradient_alpha_key{};

        if (key_node["time"]) key.time = key_node["time"].as<std::float_t>();
        if (key_node["alpha"]) key.alpha = key_node["alpha"].as<std::float_t>();

        result.alpha_keys.push_back(key);
      }
    }

    return result;
  };

  if (const auto emitters = root["emitters"]) {
    description.emitters.reserve(emitters.size());

    for (const auto emitter_node : emitters) {
      auto emitter = particle_emitter_description{};

      if (emitter_node["name"]) emitter.name = emitter_node["name"].as<std::string>();

      if (emitter_node["blend_mode"]) {
        const auto mode = emitter_node["blend_mode"].as<std::string>();
        emitter.blend_mode = (mode == "alpha_blend") ? emitter_blend_mode::alpha_blend : emitter_blend_mode::additive;
      }

      if (emitter_node["simulation_mode"]) {
        const auto mode = emitter_node["simulation_mode"].as<std::string>();
        emitter.simulation_mode = (mode == "gpu") ? particle_simulation_mode::gpu : particle_simulation_mode::cpu;
      }

      if (emitter_node["emission_rate"]) emitter.emission_rate = emitter_node["emission_rate"].as<std::float_t>();
      if (emitter_node["burst_count"]) emitter.burst_count = emitter_node["burst_count"].as<std::uint32_t>();

      if (emitter_node["shape"]) {
        const auto shape = emitter_node["shape"].as<std::string>();
        emitter.shape = (shape == "sphere") ? emitter_shape::sphere : (shape == "box") ? emitter_shape::box : (shape == "cone") ? emitter_shape::cone : emitter_shape::point;
      }

      if (emitter_node["shape_extents"]) emitter.shape_extents = emitter_node["shape_extents"].as<math::vector3>();

      if (const auto cone_node = emitter_node["cone"]) {
        if (cone_node["angle_degrees"]) emitter.cone.angle = math::degree{cone_node["angle_degrees"].as<std::float_t>()};
        if (cone_node["radius"]) emitter.cone.radius = cone_node["radius"].as<std::float_t>();
        if (cone_node["emit_from_volume"]) emitter.cone.emit_from_volume = cone_node["emit_from_volume"].as<std::float_t>();
      }

      if (emitter_node["velocity_min"]) emitter.velocity_min = emitter_node["velocity_min"].as<math::vector3>();
      if (emitter_node["velocity_max"]) emitter.velocity_max = emitter_node["velocity_max"].as<math::vector3>();
      if (emitter_node["lifetime_min"]) emitter.lifetime_min = emitter_node["lifetime_min"].as<std::float_t>();
      if (emitter_node["lifetime_max"]) emitter.lifetime_max = emitter_node["lifetime_max"].as<std::float_t>();
      if (emitter_node["start_color"]) emitter.start_color = emitter_node["start_color"].as<math::color>();
      if (emitter_node["end_color"]) emitter.end_color = emitter_node["end_color"].as<math::color>();
      if (emitter_node["color_over_lifetime"]) emitter.color_over_lifetime = load_gradient(emitter_node["color_over_lifetime"]);
      if (emitter_node["size_min"]) emitter.size_min = emitter_node["size_min"].as<std::float_t>();
      if (emitter_node["size_max"]) emitter.size_max = emitter_node["size_max"].as<std::float_t>();
      if (emitter_node["size_over_lifetime"]) emitter.size_over_lifetime = load_curve(emitter_node["size_over_lifetime"]);
      if (emitter_node["rotation_min"]) emitter.rotation_min = emitter_node["rotation_min"].as<std::float_t>();
      if (emitter_node["rotation_max"]) emitter.rotation_max = emitter_node["rotation_max"].as<std::float_t>();
      if (emitter_node["rotation_over_lifetime"]) emitter.rotation_over_lifetime = load_curve(emitter_node["rotation_over_lifetime"]);

      if (const auto velocity_curve_node = emitter_node["velocity_over_lifetime"]) {
        emitter.velocity_over_lifetime.x = load_curve(velocity_curve_node["x"]);
        emitter.velocity_over_lifetime.y = load_curve(velocity_curve_node["y"]);
        emitter.velocity_over_lifetime.z = load_curve(velocity_curve_node["z"]);
      }

      if (emitter_node["force_over_lifetime_min"]) emitter.force_over_lifetime_min = emitter_node["force_over_lifetime_min"].as<math::vector3>();
      if (emitter_node["force_over_lifetime_max"]) emitter.force_over_lifetime_max = emitter_node["force_over_lifetime_max"].as<math::vector3>();

      if (emitter_node["gravity"]) emitter.gravity = emitter_node["gravity"].as<std::float_t>();
      if (emitter_node["drag"]) emitter.drag = emitter_node["drag"].as<std::float_t>();

      if (emitter_node["texture"]) {
        emitter.texture = emitter_node["texture"].as<std::string>();
      }

      if (emitter_node["render_mode"]) {
        emitter.render_mode = (emitter_node["render_mode"].as<std::string>() == "mesh") ? particle_render_mode::mesh : particle_render_mode::billboard;
      }

      if (emitter_node["render_mesh"]) {
        emitter.render_mesh = emitter_node["render_mesh"].as<std::string>();
      }

      if (emitter_node["render_material"]) {
        emitter.render_material = emitter_node["render_material"].as<std::string>();
      }

      if (const auto collision_node = emitter_node["collision"]) {
        auto& collision = emitter.collision;

        if (collision_node["mode"]) {
          const auto mode = collision_node["mode"].as<std::string>();
          collision.mode = (mode == "planes") ? particle_collision_mode::planes : (mode == "world") ? particle_collision_mode::world : particle_collision_mode::none;
        }

        if (collision_node["bounce"]) collision.bounce = collision_node["bounce"].as<std::float_t>();
        if (collision_node["lifetime_loss"]) collision.lifetime_loss = collision_node["lifetime_loss"].as<std::float_t>();
        if (collision_node["dampen"]) collision.dampen = collision_node["dampen"].as<std::float_t>();
        if (collision_node["radius_scale"]) collision.radius_scale = collision_node["radius_scale"].as<std::float_t>();
        if (collision_node["max_collisions_per_particle"]) collision.max_collisions_per_particle = collision_node["max_collisions_per_particle"].as<std::uint32_t>();

        if (const auto planes_node = collision_node["planes"]) {
          for (const auto plane_node : planes_node) {
            if (collision.planes.size() >= collision_max_planes) {
              break;
            }

            auto plane = collision_plane{};

            if (plane_node["normal"]) plane.normal = plane_node["normal"].as<math::vector3>();
            if (plane_node["distance"]) plane.distance = plane_node["distance"].as<std::float_t>();

            collision.planes.push_back(plane);
          }
        }
      }

      if (const auto sub_emitters_node = emitter_node["sub_emitters"]) {
        for (const auto binding_node : sub_emitters_node) {
          auto binding = particle_emitter_description::sub_emitter_description{};

          if (binding_node["event"]) {
            const auto event = binding_node["event"].as<std::string>();
            binding.event = (event == "death") ? sub_emitter_event::death : (event == "collision") ? sub_emitter_event::collision : sub_emitter_event::birth;
          }

          if (binding_node["effect"]) {
            binding.effect = binding_node["effect"].as<std::string>();
          }

          if (binding_node["probability"]) binding.probability = binding_node["probability"].as<std::float_t>();
          if (binding_node["inherit_velocity"]) binding.inherit_velocity = binding_node["inherit_velocity"].as<bool>();

          emitter.sub_emitters.push_back(binding);
        }
      }

      if (const auto trail_node = emitter_node["trail"]) {
        auto& trail = emitter.trail;

        if (trail_node["enabled"]) trail.enabled = trail_node["enabled"].as<bool>();
        if (trail_node["min_vertex_distance"]) trail.min_vertex_distance = trail_node["min_vertex_distance"].as<std::float_t>();
        if (trail_node["lifetime"]) trail.lifetime = trail_node["lifetime"].as<std::float_t>();
        if (trail_node["width"]) trail.width = trail_node["width"].as<std::float_t>();
        if (trail_node["color_over_trail"]) trail.color_over_trail = load_gradient(trail_node["color_over_trail"]);
        if (trail_node["die_with_particle"]) trail.die_with_particle = trail_node["die_with_particle"].as<bool>();
      }

      description.emitters.push_back(std::move(emitter));
    }
  }

  return description;
}

auto asset_cooker::parse_animation_graph_file(const std::filesystem::path& source) -> std::optional<animation_graph::create_info> {
  auto root = YAML::Node{};

  try {
    root = YAML::LoadFile(source.string());
  } catch (const std::exception& exception) {
    utility::logger<"assets">::warn("Could not parse animation_graph '{}' ({})", source.generic_string(), exception.what());
    return std::nullopt;
  }

  auto info = animation_graph::create_info{};

  if (root["name"]) info.name = root["name"].as<std::string>();
  if (root["entry_state_id"]) info.entry_state_id = root["entry_state_id"].as<std::uint32_t>();

  if (const auto parameters = root["parameters"]) {
    info.parameters.reserve(parameters.size());

    for (const auto parameter_node : parameters) {
      auto parameter = animation_parameter{};

      if (parameter_node["name"]) parameter.name = parameter_node["name"].as<std::string>();
      parameter.default_value = load_animation_parameter_value(parameter_node);

      info.parameters.push_back(parameter);
    }
  }

  if (const auto states = root["states"]) {
    info.states.reserve(states.size());

    for (const auto state_node : states) {
      auto state = animation_state{};

      if (state_node["id"]) state.id = state_node["id"].as<std::uint32_t>();
      if (state_node["name"]) state.name = state_node["name"].as<std::string>();
      if (state_node["clip_name"]) state.clip_name = state_node["clip_name"].as<std::string>();
      if (state_node["speed"]) state.speed = state_node["speed"].as<std::float_t>();
      if (state_node["loop"]) state.loop = state_node["loop"].as<bool>();

      if (const auto position_node = state_node["editor_position"]) {
        if (position_node["x"]) state.editor_position.x() = position_node["x"].as<std::float_t>();
        if (position_node["y"]) state.editor_position.y() = position_node["y"].as<std::float_t>();
      }

      info.states.push_back(state);
    }
  }

  if (const auto transitions = root["transitions"]) {
    info.transitions.reserve(transitions.size());

    for (const auto transition_node : transitions) {
      auto transition = animation_transition{};

      if (transition_node["from_state"]) transition.from_state = transition_node["from_state"].as<std::uint32_t>();
      if (transition_node["to_state"]) transition.to_state = transition_node["to_state"].as<std::uint32_t>();
      if (transition_node["duration"]) transition.duration = transition_node["duration"].as<std::float_t>();
      if (transition_node["has_exit_time"]) transition.has_exit_time = transition_node["has_exit_time"].as<bool>();
      if (transition_node["exit_time"]) transition.exit_time = transition_node["exit_time"].as<std::float_t>();

      if (const auto conditions_node = transition_node["conditions"]) {
        for (const auto condition_node : conditions_node) {
          auto condition = animation_condition{};

          if (condition_node["parameter_name"]) condition.parameter_name = condition_node["parameter_name"].as<std::string>();
          if (condition_node["comparator"]) condition.comparator = load_animation_condition_comparator(condition_node["comparator"].as<std::string>());
          condition.expected = load_animation_parameter_value(condition_node);

          transition.conditions.push_back(condition);
        }
      }

      info.transitions.push_back(transition);
    }
  }

  return info;
}

auto asset_cooker::_cook_texture(const std::filesystem::path& source, const std::filesystem::path& cooked) -> bool {
  auto width = std::int32_t{0};
  auto height = std::int32_t{0};
  auto channels = std::int32_t{0};

  auto* data = stbi_load(source.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

  if (data == nullptr) {
    utility::logger<"assets">::warn("Cook: could not decode '{}'", source.generic_string());
    return false;
  }

  const auto data_size = static_cast<std::uint32_t>(width) * static_cast<std::uint32_t>(height) * 4u;

  const auto header = texture_header{
    texture_magic,
    texture_cook_version,
    static_cast<std::uint32_t>(width),
    static_cast<std::uint32_t>(height),
    4u,
    data_size
  };

  auto error = std::error_code{};
  std::filesystem::create_directories(cooked.parent_path(), error);

  auto out = std::ofstream{cooked, std::ios::binary};

  if (!out) {
    utility::logger<"assets">::warn("Cook: could not write '{}'", cooked.generic_string());
    stbi_image_free(data);
    return false;
  }

  out.write(reinterpret_cast<const char*>(&header), sizeof(header));
  out.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(data_size));

  stbi_image_free(data);

  utility::logger<"assets">::debug("Cooked texture '{}' -> '{}'", source.generic_string(), cooked.generic_string());

  return true;
}

auto asset_cooker::_load_cooked_texture(const std::filesystem::path& cooked, std::vector<std::byte>& pixels, std::uint32_t& width, std::uint32_t& height) -> bool {
  auto in = std::ifstream{cooked, std::ios::binary};

  if (!in) {
    return false;
  }

  auto header = texture_header{};
  in.read(reinterpret_cast<char*>(&header), sizeof(header));

  if (!in || header.magic != texture_magic || header.version != texture_cook_version) {
    return false; // missing / corrupt / stale format -> caller recooks
  }

  pixels.resize(header.data_size);
  in.read(reinterpret_cast<char*>(pixels.data()), static_cast<std::streamsize>(header.data_size));

  if (!in) {
    return false;
  }

  width = header.width;
  height = header.height;

  return true;
}

auto asset_cooker::_cook_font(const std::filesystem::path& source, const std::filesystem::path& cooked) -> bool {
  auto in = std::ifstream{source, std::ios::binary};

  if (!in) {
    utility::logger<"assets">::warn("Cook: could not open '{}'", source.generic_string());
    return false;
  }

  const auto buffer = std::vector<unsigned char>{std::istreambuf_iterator<char>{in}, std::istreambuf_iterator<char>{}};

  auto font_info = stbtt_fontinfo{};

  if (!stbtt_InitFont(&font_info, buffer.data(), stbtt_GetFontOffsetForIndex(buffer.data(), 0))) {
    utility::logger<"assets">::warn("Cook: could not parse font '{}'", source.generic_string());
    return false;
  }

  const auto scale = stbtt_ScaleForPixelHeight(&font_info, font_reference_size);

  auto ascent_units = std::int32_t{0};
  auto descent_units = std::int32_t{0};
  auto line_gap_units = std::int32_t{0};

  stbtt_GetFontVMetrics(&font_info, &ascent_units, &descent_units, &line_gap_units);

  struct baked_glyph {
    std::vector<std::uint8_t> bitmap{};
    std::int32_t width{0};
    std::int32_t height{0};
    std::int32_t xoff{0};
    std::int32_t yoff{0};
    std::float_t advance{0.0f};
  }; // struct baked_glyph

  auto baked = std::vector<baked_glyph>(font_codepoint_count);

  auto max_width = std::int32_t{1};
  auto max_height = std::int32_t{1};

  for (auto index = std::uint32_t{0u}; index < font_codepoint_count; ++index) {
    const auto codepoint = static_cast<std::int32_t>(font_first_codepoint + index);

    auto advance_units = std::int32_t{0};
    auto left_side_bearing_units = std::int32_t{0};

    stbtt_GetCodepointHMetrics(&font_info, codepoint, &advance_units, &left_side_bearing_units);

    auto& glyph = baked[index];
    glyph.advance = static_cast<std::float_t>(advance_units) * scale;

    auto width = std::int32_t{0};
    auto height = std::int32_t{0};
    auto xoff = std::int32_t{0};
    auto yoff = std::int32_t{0};

    auto* bitmap = stbtt_GetCodepointSDF(&font_info, scale, codepoint, font_sdf_padding, static_cast<unsigned char>(font_sdf_onedge_value), font_sdf_pixel_dist_scale, &width, &height, &xoff, &yoff);

    if (bitmap != nullptr) {
      glyph.width = width;
      glyph.height = height;
      glyph.xoff = xoff;
      glyph.yoff = yoff;
      glyph.bitmap.assign(bitmap, bitmap + static_cast<std::size_t>(width) * static_cast<std::size_t>(height));

      stbtt_FreeSDF(bitmap, nullptr);

      max_width = std::max(max_width, width);
      max_height = std::max(max_height, height);
    }
  }

  const auto columns = static_cast<std::int32_t>(std::ceil(std::sqrt(static_cast<std::float_t>(font_codepoint_count))));
  const auto rows = (static_cast<std::int32_t>(font_codepoint_count) + columns - 1) / columns;

  const auto atlas_width = static_cast<std::uint32_t>(columns * max_width);
  const auto atlas_height = static_cast<std::uint32_t>(rows * max_height);

  auto atlas = std::vector<std::uint8_t>(static_cast<std::size_t>(atlas_width) * static_cast<std::size_t>(atlas_height), std::uint8_t{0u});

  auto records = std::vector<font_glyph_record>(font_codepoint_count);

  for (auto index = std::uint32_t{0u}; index < font_codepoint_count; ++index) {
    const auto& glyph = baked[index];

    auto& record = records[index];
    record.advance = glyph.advance / font_reference_size;

    if (glyph.width == 0 || glyph.height == 0) {
      record.uv_x0 = 0.0f;
      record.uv_y0 = 0.0f;
      record.uv_x1 = 0.0f;
      record.uv_y1 = 0.0f;
      record.width = 0.0f;
      record.height = 0.0f;
      record.bearing_x = 0.0f;
      record.bearing_y = 0.0f;
      continue;
    }

    const auto column = static_cast<std::int32_t>(index) % columns;
    const auto row = static_cast<std::int32_t>(index) / columns;

    const auto origin_x = static_cast<std::uint32_t>(column * max_width);
    const auto origin_y = static_cast<std::uint32_t>(row * max_height);

    for (auto y = std::int32_t{0}; y < glyph.height; ++y) {
      const auto destination_offset = (origin_y + static_cast<std::uint32_t>(y)) * atlas_width + origin_x;
      const auto source_offset = static_cast<std::size_t>(y) * static_cast<std::size_t>(glyph.width);

      std::copy_n(glyph.bitmap.begin() + static_cast<std::ptrdiff_t>(source_offset), glyph.width, atlas.begin() + static_cast<std::ptrdiff_t>(destination_offset));
    }

    record.uv_x0 = static_cast<std::float_t>(origin_x) / static_cast<std::float_t>(atlas_width);
    record.uv_y0 = static_cast<std::float_t>(origin_y) / static_cast<std::float_t>(atlas_height);
    record.uv_x1 = static_cast<std::float_t>(origin_x + static_cast<std::uint32_t>(glyph.width)) / static_cast<std::float_t>(atlas_width);
    record.uv_y1 = static_cast<std::float_t>(origin_y + static_cast<std::uint32_t>(glyph.height)) / static_cast<std::float_t>(atlas_height);
    record.width = static_cast<std::float_t>(glyph.width) / font_reference_size;
    record.height = static_cast<std::float_t>(glyph.height) / font_reference_size;
    record.bearing_x = static_cast<std::float_t>(glyph.xoff) / font_reference_size;
    record.bearing_y = static_cast<std::float_t>(glyph.yoff) / font_reference_size;
  }

  auto error = std::error_code{};
  std::filesystem::create_directories(cooked.parent_path(), error);

  auto out = std::ofstream{cooked, std::ios::binary};

  if (!out) {
    utility::logger<"assets">::warn("Cook: could not write '{}'", cooked.generic_string());
    return false;
  }

  const auto header = font_header{
    font_magic,
    font_cook_version,
    atlas_width,
    atlas_height,
    font_codepoint_count,
    font_first_codepoint,
    (static_cast<std::float_t>(ascent_units - descent_units + line_gap_units) * scale) / font_reference_size,
    (static_cast<std::float_t>(ascent_units) * scale) / font_reference_size,
    (static_cast<std::float_t>(descent_units) * scale) / font_reference_size,
    static_cast<std::uint32_t>(atlas.size())
  };

  out.write(reinterpret_cast<const char*>(&header), sizeof(header));
  out.write(reinterpret_cast<const char*>(records.data()), static_cast<std::streamsize>(records.size() * sizeof(font_glyph_record)));
  out.write(reinterpret_cast<const char*>(atlas.data()), static_cast<std::streamsize>(atlas.size()));

  utility::logger<"assets">::debug("Cooked font '{}' -> '{}' ({}x{} atlas, {} glyphs)", source.generic_string(), cooked.generic_string(), atlas_width, atlas_height, font_codepoint_count);

  return true;
}

auto asset_cooker::_load_cooked_font(const std::filesystem::path& cooked, cooked_font_data& data) -> bool {
  auto in = std::ifstream{cooked, std::ios::binary};

  if (!in) {
    return false;
  }

  auto header = font_header{};
  in.read(reinterpret_cast<char*>(&header), sizeof(header));

  if (!in || header.magic != font_magic || header.version != font_cook_version) {
    return false; // missing / corrupt / stale format -> caller recooks
  }

  auto records = std::vector<font_glyph_record>(header.glyph_count);
  in.read(reinterpret_cast<char*>(records.data()), static_cast<std::streamsize>(records.size() * sizeof(font_glyph_record)));

  if (!in) {
    return false;
  }

  data.atlas.pixels.resize(header.atlas_data_size);
  in.read(reinterpret_cast<char*>(data.atlas.pixels.data()), static_cast<std::streamsize>(header.atlas_data_size));

  if (!in) {
    return false;
  }

  data.atlas.width = header.atlas_width;
  data.atlas.height = header.atlas_height;
  data.first_codepoint = header.first_codepoint;
  data.line_height = header.line_height;
  data.ascent = header.ascent;
  data.descent = header.descent;

  data.glyphs.clear();
  data.glyphs.reserve(records.size());

  for (const auto& record : records) {
    data.glyphs.push_back(font::glyph{
      math::vector4{record.uv_x0, record.uv_y0, record.uv_x1, record.uv_y1},
      record.width,
      record.height,
      record.bearing_x,
      record.bearing_y,
      record.advance
    });
  }

  return true;
}

auto asset_cooker::_generate_normals(std::vector<vertex>& vertices, const std::vector<std::uint32_t>& indices, std::size_t vertex_start, std::size_t vertex_count, std::size_t index_start, std::size_t index_count) -> void {
  // Area-weighted face-normal accumulation: a cross product's length is proportional to twice its
  // triangle's area, so summing it directly (before normalizing) naturally weights larger
  // triangles more, same idea as _generate_tangents' Lengyel accumulation below.
  auto normal_sum = std::vector<math::vector3>(vertex_count, math::vector3::zero);

  for (auto i = std::size_t{0u}; i + 2u < index_count; i += 3u) {
    const auto i0 = indices[index_start + i];
    const auto i1 = indices[index_start + i + 1u];
    const auto i2 = indices[index_start + i + 2u];

    const auto& v0 = vertices[i0];
    const auto& v1 = vertices[i1];
    const auto& v2 = vertices[i2];

    const auto face_normal = math::vector3::cross(v1.position - v0.position, v2.position - v0.position);

    for (const auto index : {i0, i1, i2}) {
      const auto local = index - static_cast<std::uint32_t>(vertex_start);
      normal_sum[local] = normal_sum[local] + face_normal;
    }
  }

  for (auto local = std::size_t{0u}; local < vertex_count; ++local) {
    auto& current = vertices[vertex_start + local];

    // Degenerate (isolated point / zero-area triangles only) falls back to a fixed up vector
    // rather than a zero normal -- NaN-ing every downstream lighting calculation is worse than a
    // wrong-but-finite normal on the handful of vertices this could ever affect.
    const auto normal = (normal_sum[local].length_squared() > 1e-12f) ? math::vector3::normalized(normal_sum[local]) : math::vector3{0.0f, 1.0f, 0.0f};

    current.normal[0] = normal.x();
    current.normal[1] = normal.y();
    current.normal[2] = normal.z();
  }
}

auto asset_cooker::_generate_tangents(std::vector<vertex>& vertices, const std::vector<std::uint32_t>& indices, std::size_t vertex_start, std::size_t vertex_count, std::size_t index_start, std::size_t index_count) -> void {
  // Lengyel's method: accumulate tangent/bitangent per vertex from referencing triangles, then
  // orthogonalize against the normal and derive handedness from the bitangent sum.
  auto tangent_sum = std::vector<math::vector3>(vertex_count, math::vector3::zero);
  auto bitangent_sum = std::vector<math::vector3>(vertex_count, math::vector3::zero);

  for (auto i = std::size_t{0u}; i + 2u < index_count; i += 3u) {
    const auto i0 = indices[index_start + i];
    const auto i1 = indices[index_start + i + 1u];
    const auto i2 = indices[index_start + i + 2u];

    const auto& v0 = vertices[i0];
    const auto& v1 = vertices[i1];
    const auto& v2 = vertices[i2];

    const auto edge1 = v1.position - v0.position;
    const auto edge2 = v2.position - v0.position;

    const auto delta_uv1 = v1.uv - v0.uv;
    const auto delta_uv2 = v2.uv - v0.uv;

    const auto denom = delta_uv1.x() * delta_uv2.y() - delta_uv2.x() * delta_uv1.y();
    const auto f = (std::abs(denom) > 1e-8f) ? (1.0f / denom) : 0.0f;

    const auto triangle_tangent = f * (edge1 * delta_uv2.y() - edge2 * delta_uv1.y());
    const auto triangle_bitangent = f * (edge2 * delta_uv1.x() - edge1 * delta_uv2.x());

    for (const auto index : {i0, i1, i2}) {
      const auto local = index - static_cast<std::uint32_t>(vertex_start);
      tangent_sum[local] = tangent_sum[local] + triangle_tangent;
      bitangent_sum[local] = bitangent_sum[local] + triangle_bitangent;
    }
  }

  for (auto local = std::size_t{0u}; local < vertex_count; ++local) {
    auto& current = vertices[vertex_start + local];

    const auto n = current.normal;
    auto t = tangent_sum[local] - n * math::vector3::dot(n, tangent_sum[local]);

    t = (t.length_squared() < 1e-12f) ? math::vector3::orthogonal(n) : math::vector3::normalized(t);

    const auto handedness = (math::vector3::dot(math::vector3::cross(n, t), bitangent_sum[local]) < 0.0f) ? -1.0f : 1.0f;

    current.tangent[0] = t.x();
    current.tangent[1] = t.y();
    current.tangent[2] = t.z();
    current.tangent[3] = handedness;
  }
}

auto asset_cooker::_optimize_and_generate_lods(std::vector<vertex>& vertices, std::vector<std::uint32_t>& indices, std::size_t vertex_start, std::size_t vertex_count, std::size_t index_start, std::size_t index_count, std::vector<skin_vertex>* skin_vertices) -> std::vector<mesh_lod> {
  auto lods = std::vector<mesh_lod>{};

  if (index_count == 0u || vertex_count == 0u) {
    return lods;
  }

  // meshopt works in a 0-based local index space, not indices' mesh-global one — translate this
  // submesh's slice down to local, optimize, then translate back before writing to the shared arrays.
  auto local = std::vector<std::uint32_t>(index_count);

  for (auto i = std::size_t{0u}; i < index_count; ++i) {
    local[i] = indices[index_start + i] - static_cast<std::uint32_t>(vertex_start);
  }

  // Standard GPU-friendly ordering trio: vertex cache (post-transform reuse), overdraw (front-to-back
  // triangle order), vertex fetch (pre-transform cache locality — reorders the vertex buffer itself).
  meshopt_optimizeVertexCache(local.data(), local.data(), index_count, vertex_count);
  meshopt_optimizeOverdraw(local.data(), local.data(), index_count, &vertices[vertex_start].position.x(), vertex_count, sizeof(vertex), 1.05f);

  // Computed as an explicit remap (rather than calling meshopt_optimizeVertexFetch directly) so the
  // same permutation can also be applied to skin_vertices -- a second, parallel vertex stream that
  // function has no way to know about (see its own doc comment on multiple vertex streams).
  auto remap = std::vector<unsigned int>(vertex_count);
  meshopt_optimizeVertexFetchRemap(remap.data(), local.data(), index_count, vertex_count);

  auto reordered = std::vector<vertex>(vertex_count);
  meshopt_remapVertexBuffer(reordered.data(), &vertices[vertex_start], vertex_count, sizeof(vertex), remap.data());
  std::ranges::copy(reordered, vertices.begin() + static_cast<std::ptrdiff_t>(vertex_start));

  if (skin_vertices != nullptr) {
    auto reordered_skin = std::vector<skin_vertex>(vertex_count);
    meshopt_remapVertexBuffer(reordered_skin.data(), &(*skin_vertices)[vertex_start], vertex_count, sizeof(skin_vertex), remap.data());
    std::ranges::copy(reordered_skin, skin_vertices->begin() + static_cast<std::ptrdiff_t>(vertex_start));
  }

  for (auto i = std::size_t{0u}; i < index_count; ++i) {
    local[i] = remap[local[i]];
    indices[index_start + i] = local[i] + static_cast<std::uint32_t>(vertex_start);
  }

  // Coarser LOD chain, each level targeting half the previous one's triangle budget; stops once
  // meshopt_simplify stalls (topology-locked) or the mesh is already too small to bother.
  auto previous = local;
  constexpr auto max_levels = std::size_t{4u};
  constexpr auto min_triangle_count = std::size_t{8u};

  for (auto level = std::size_t{0u}; level < max_levels; ++level) {
    const auto target_index_count = std::max((previous.size() / 2u) / 3u * 3u, min_triangle_count * 3u);

    if (target_index_count >= previous.size()) {
      break;
    }

    auto simplified = std::vector<std::uint32_t>(previous.size());
    auto result_error = 0.0f;

    const auto simplified_count = meshopt_simplify(
      simplified.data(), previous.data(), previous.size(),
      &vertices[vertex_start].position.x(), vertex_count, sizeof(vertex),
      target_index_count, 1e-2f, 0u, &result_error
    );

    // Less than ~10% reduction means the chain has bottomed out (topology constraints, etc).
    if (simplified_count == 0u || simplified_count >= (previous.size() * 9u) / 10u) {
      break;
    }

    simplified.resize(simplified_count);
    meshopt_optimizeVertexCache(simplified.data(), simplified.data(), simplified_count, vertex_count);

    const auto lod_offset = indices.size();
    indices.reserve(lod_offset + simplified_count);

    for (const auto index : simplified) {
      indices.push_back(index + static_cast<std::uint32_t>(vertex_start));
    }

    lods.push_back(mesh_lod{static_cast<std::uint32_t>(lod_offset), static_cast<std::uint32_t>(simplified_count), result_error});

    previous = std::move(simplified);
  }

  return lods;
}

auto asset_cooker::_cook_mesh(const std::filesystem::path& source, const math::uuid& id, const std::filesystem::path& cooked) -> bool {
  auto data = fastgltf::GltfDataBuffer::FromPath(source);

  if (data.error() != fastgltf::Error::None) {
    utility::logger<"assets">::warn("Cook: could not open mesh '{}'", source.generic_string());
    return false;
  }

  auto parser = fastgltf::Parser{fastgltf::Extensions::KHR_materials_emissive_strength | fastgltf::Extensions::KHR_materials_ior};

  auto loaded = parser.loadGltf(data.get(), source.parent_path(), fastgltf::Options::LoadExternalBuffers | fastgltf::Options::GenerateMeshIndices);

  if (loaded.error() != fastgltf::Error::None) {
    utility::logger<"assets">::warn("Cook: could not parse mesh '{}'", source.generic_string());
    return false;
  }

  auto& gltf = loaded.get();

  // Referenced glTF images become a material_description texture-slot *path* (assets-directory-
  // relative), not a uuid -- resolving a path to a stable uuid is asset_manifest::import's job, and
  // asset_manifest is main-thread-only (see its own doc comment for why). asset_residency's mesh
  // finalize step turns this path into a handle via load_texture(path, ...) exactly the same way it
  // already does for a hand-authored `.material` file's texture slots.
  const auto& project = core::engine::project();

  const auto texture_path = [&](std::size_t texture_index) -> std::string {
    const auto& gltf_texture = gltf.textures[texture_index];

    if (!gltf_texture.imageIndex.has_value()) {
      return {};
    }

    const auto& image = gltf.images[gltf_texture.imageIndex.value()];

    if (const auto* uri = std::get_if<fastgltf::sources::URI>(&image.data)) {
      const auto absolute = source.parent_path() / std::filesystem::path{std::string{uri->uri.path()}};
      return std::filesystem::relative(absolute, project.assets_directory()).generic_string();
    }

    utility::logger<"assets">::warn("Cook: mesh '{}' has a non-file image, using default", source.generic_string());
    return {};
  };

  auto material_uuids = std::vector<math::uuid>{};
  material_uuids.reserve(gltf.materials.size());

  for (const auto& gltf_material : gltf.materials) {
    const auto& pbr = gltf_material.pbrData;

    auto description = material_description{};
    description.name = gltf_material.name.empty() ? std::string{"material"} : std::string{gltf_material.name.begin(), gltf_material.name.end()};
    description.base_color_factor = math::color{pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2], pbr.baseColorFactor[3]};
    description.emissive_factor = math::vector3{gltf_material.emissiveFactor[0], gltf_material.emissiveFactor[1], gltf_material.emissiveFactor[2]};
    description.metallic_factor = pbr.metallicFactor;
    description.roughness_factor = pbr.roughnessFactor;
    description.alpha = (gltf_material.alphaMode == fastgltf::AlphaMode::Blend) ? alpha_mode::blend : (gltf_material.alphaMode == fastgltf::AlphaMode::Mask) ? alpha_mode::mask : alpha_mode::opaque;
    description.alpha_cutoff = gltf_material.alphaCutoff;
    description.is_double_sided = gltf_material.doubleSided;
    description.normal_scale = gltf_material.normalTexture.has_value() ? gltf_material.normalTexture->scale : 1.0f;
    description.occlusion_strength = gltf_material.occlusionTexture.has_value() ? gltf_material.occlusionTexture->strength : 1.0f;
    description.emissive_strength = gltf_material.emissiveStrength;
    description.ior = gltf_material.ior;

    if (pbr.baseColorTexture.has_value())         description.albedo             = texture_path(pbr.baseColorTexture->textureIndex);
    if (pbr.metallicRoughnessTexture.has_value()) description.metallic_roughness = texture_path(pbr.metallicRoughnessTexture->textureIndex);
    if (gltf_material.normalTexture.has_value())  description.normal             = texture_path(gltf_material.normalTexture->textureIndex);
    if (gltf_material.occlusionTexture.has_value()) description.occlusion        = texture_path(gltf_material.occlusionTexture->textureIndex);
    if (gltf_material.emissiveTexture.has_value()) description.emissive          = texture_path(gltf_material.emissiveTexture->textureIndex);

    // Every embedded material is cooked as a self-contained, resolvable side-effect blob here --
    // same idea as a skinned mesh's skeleton/animation clips below. Whether this ends up being what
    // the submesh actually uses, or gets superseded by a hand-editable extracted `.material` file,
    // is decided later by asset_residency's main-thread mesh finalize step (mesh_import_options::
    // extract_materials) -- see cooked_submesh::material's doc comment.
    const auto material_uuid = derive_material_uuid(id, material_uuids.size());

    if (!_cook_material(material_uuid, description)) {
      return false;
    }

    material_uuids.push_back(material_uuid);
  }

  const auto material_uuid_for = [&](fastgltf::Optional<std::size_t> index) -> math::uuid {
    if (index.has_value() && index.value() < material_uuids.size()) {
      return material_uuids[index.value()];
    }

    return math::uuid::nil();
  };

  auto vertices = std::vector<vertex>{};
  auto skin_vertices = std::vector<skin_vertex>{};
  auto indices = std::vector<std::uint32_t>{};
  auto submeshes = std::vector<cooked_submesh>{};
  auto mesh_volume = math::volume{};

  // Skinning: one skeleton per cooked mesh -- the first skinned node's skin wins; any other skin
  // encountered later only warns. A node's world transform is *not* baked into a skinned
  // primitive's vertices (see the traversal below) -- glTF skinning requires vertices to stay in
  // the space the skin's inverse-bind matrices were authored against, with placement coming
  // entirely from the joint hierarchy instead.
  auto joints = std::vector<skeleton::joint>{};
  auto joint_remap = std::vector<std::uint32_t>{}; // skin-local (JOINTS_0) index -> joints' topologically-sorted index
  auto primary_skin_index = std::optional<std::size_t>{};
  auto node_to_joint = std::unordered_map<std::size_t, std::size_t>{}; // glTF node index -> joints index, for animation cooking below

  if (!gltf.scenes.empty()) {
    const auto scene_index = gltf.defaultScene.value_or(std::size_t{0});

    fastgltf::iterateSceneNodes(gltf, scene_index, fastgltf::math::fmat4x4{}, [&](const fastgltf::Node& node, const fastgltf::math::fmat4x4&) {
      if (!node.meshIndex.has_value() || !node.skinIndex.has_value()) {
        return;
      }

      if (!primary_skin_index.has_value()) {
        primary_skin_index = node.skinIndex.value();
      } else if (*primary_skin_index != node.skinIndex.value()) {
        utility::logger<"assets">::warn("Cook: mesh '{}' references multiple skins; only the first is used", source.generic_string());
      }
    });
  }

  if (primary_skin_index.has_value()) {
    const auto& skin = gltf.skins[*primary_skin_index];
    const auto joint_count = skin.joints.size();

    auto node_to_skin_local = std::unordered_map<std::size_t, std::uint32_t>{};
    node_to_skin_local.reserve(joint_count);

    for (auto index = std::size_t{0u}; index < joint_count; ++index) {
      node_to_skin_local.emplace(skin.joints[index], static_cast<std::uint32_t>(index));
    }

    auto node_parent = std::unordered_map<std::size_t, std::size_t>{};

    for (auto node_index = std::size_t{0u}; node_index < gltf.nodes.size(); ++node_index) {
      for (const auto child : gltf.nodes[node_index].children) {
        node_parent.emplace(child, node_index);
      }
    }

    // Parent, in the *original* skin.joints order (still to be topo-sorted below); -1 if the
    // parent node isn't itself a joint of this skin (i.e. this is the skin's effective root).
    auto skin_local_parent = std::vector<std::int32_t>(joint_count, -1);

    for (auto index = std::size_t{0u}; index < joint_count; ++index) {
      if (const auto parent_entry = node_parent.find(skin.joints[index]); parent_entry != node_parent.end()) {
        if (const auto joint_entry = node_to_skin_local.find(parent_entry->second); joint_entry != node_to_skin_local.end()) {
          skin_local_parent[index] = static_cast<std::int32_t>(joint_entry->second);
        }
      }
    }

    // Topological sort by depth from root -- a joint's parent always has a strictly smaller
    // depth, so a stable sort on depth alone guarantees parent-before-child.
    auto depth = std::vector<std::uint32_t>(joint_count, 0u);

    for (auto index = std::size_t{0u}; index < joint_count; ++index) {
      auto current = skin_local_parent[index];

      while (current >= 0) {
        ++depth[index];
        current = skin_local_parent[static_cast<std::size_t>(current)];
      }
    }

    auto order = std::vector<std::uint32_t>(joint_count);
    std::iota(order.begin(), order.end(), std::uint32_t{0u});
    std::stable_sort(order.begin(), order.end(), [&](std::uint32_t a, std::uint32_t b) { return depth[a] < depth[b]; });

    joint_remap.resize(joint_count);

    for (auto new_index = std::uint32_t{0u}; new_index < joint_count; ++new_index) {
      joint_remap[order[new_index]] = new_index;
    }

    auto inverse_binds = std::vector<fastgltf::math::fmat4x4>(joint_count, fastgltf::math::fmat4x4{});

    if (skin.inverseBindMatrices.has_value()) {
      fastgltf::iterateAccessorWithIndex<fastgltf::math::fmat4x4>(gltf, gltf.accessors[skin.inverseBindMatrices.value()], [&](fastgltf::math::fmat4x4 value, std::size_t index) {
        inverse_binds[index] = value;
      });
    }

    joints.resize(joint_count);

    for (auto old_index = std::size_t{0u}; old_index < joint_count; ++old_index) {
      const auto new_index = joint_remap[old_index];
      const auto node_index = skin.joints[old_index];
      const auto& node = gltf.nodes[node_index];

      node_to_joint.emplace(node_index, new_index);

      auto& joint = joints[new_index];
      joint.name = node.name.empty() ? fmt::format("joint_{}", new_index) : std::string{node.name.begin(), node.name.end()};
      joint.parent_index = (skin_local_parent[old_index] < 0) ? -1 : static_cast<std::int32_t>(joint_remap[static_cast<std::size_t>(skin_local_parent[old_index])]);

      if (const auto* trs = std::get_if<fastgltf::TRS>(&node.transform)) {
        joint.bind_local_translation = math::vector3{trs->translation.x(), trs->translation.y(), trs->translation.z()};
        joint.bind_local_rotation = math::quaternion::wxyz(trs->rotation.w(), trs->rotation.x(), trs->rotation.y(), trs->rotation.z());
        joint.bind_local_scale = math::vector3{trs->scale.x(), trs->scale.y(), trs->scale.z()};
      } else if (const auto* node_matrix = std::get_if<fastgltf::math::fmat4x4>(&node.transform)) {
        auto local = math::matrix4x4::identity;

        for (auto column = std::size_t{0u}; column < 4u; ++column) {
          for (auto row = std::size_t{0u}; row < 4u; ++row) {
            local[column][row] = (*node_matrix)[column][row];
          }
        }

        const auto decomposed = math::decompose(local);
        joint.bind_local_translation = decomposed.position;
        joint.bind_local_rotation = decomposed.rotation;
        joint.bind_local_scale = decomposed.scale;
      }

      const auto& inverse_bind = inverse_binds[old_index];

      for (auto column = std::size_t{0u}; column < 4u; ++column) {
        for (auto row = std::size_t{0u}; row < 4u; ++row) {
          joint.inverse_bind_matrix[column][row] = inverse_bind[column][row];
        }
      }
    }
  }

  const auto has_skin_data = !joints.empty();

  const auto append = [&](const fastgltf::Mesh& gltf_mesh, const fastgltf::math::fmat4x4& world, bool is_skinned) {
    for (const auto& primitive : gltf_mesh.primitives) {
      const auto* position = primitive.findAttribute("POSITION");

      if (position == primitive.attributes.end()) {
        continue;
      }

      const auto vertex_start = vertices.size();

      const auto& position_accessor = gltf.accessors[position->accessorIndex];
      vertices.resize(vertex_start + position_accessor.count);

      // A mixed skinned/static file still keeps skin_vertices parallel to vertices for every
      // primitive -- entries a primitive doesn't overwrite below default to a rigid bind to
      // joints[0], a safe fallback for e.g. static decoration meshes sharing a skinned character file.
      if (has_skin_data) {
        skin_vertices.resize(vertex_start + position_accessor.count, skin_vertex{{0u, 0u, 0u, 0u}, math::vector4{1.0f, 0.0f, 0.0f, 0.0f}});
      }

      auto submesh_volume = math::volume{};

      fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(gltf, position_accessor, [&](fastgltf::math::fvec3 value, std::size_t index) {
        const auto world_position = world * fastgltf::math::fvec4{value[0], value[1], value[2], 1.0f};
        const auto point = math::vector3{world_position[0], world_position[1], world_position[2]};

        auto& current = vertices[vertex_start + index];
        current.position[0] = point.x();
        current.position[1] = point.y();
        current.position[2] = point.z();

        submesh_volume.include(point);
        mesh_volume.include(point);
      });

      const auto* normal = primitive.findAttribute("NORMAL");
      const auto has_explicit_normal = normal != primitive.attributes.end();

      if (has_explicit_normal) {
        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(gltf, gltf.accessors[normal->accessorIndex], [&](fastgltf::math::fvec3 value, std::size_t index) {
          const auto world_normal = world * fastgltf::math::fvec4{value[0], value[1], value[2], 0.0f};

          auto length = std::sqrt(world_normal[0] * world_normal[0] + world_normal[1] * world_normal[1] + world_normal[2] * world_normal[2]);
          length = (length > 0.0f) ? length : 1.0f;

          auto& current = vertices[vertex_start + index];
          current.normal[0] = world_normal[0] / length;
          current.normal[1] = world_normal[1] / length;
          current.normal[2] = world_normal[2] / length;
        });
      }

      const auto* tangent = primitive.findAttribute("TANGENT");
      const auto has_explicit_tangent = tangent != primitive.attributes.end();

      if (has_explicit_tangent) {
        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(gltf, gltf.accessors[tangent->accessorIndex], [&](fastgltf::math::fvec4 value, std::size_t index) {
          const auto world_tangent = world * fastgltf::math::fvec4{value[0], value[1], value[2], 0.0f};

          auto length = std::sqrt(world_tangent[0] * world_tangent[0] + world_tangent[1] * world_tangent[1] + world_tangent[2] * world_tangent[2]);
          length = (length > 0.0f) ? length : 1.0f;

          auto& current = vertices[vertex_start + index];
          current.tangent[0] = world_tangent[0] / length;
          current.tangent[1] = world_tangent[1] / length;
          current.tangent[2] = world_tangent[2] / length;
          current.tangent[3] = value[3];
        });
      }

      if (const auto* uv = primitive.findAttribute("TEXCOORD_0"); uv != primitive.attributes.end()) {
        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(gltf, gltf.accessors[uv->accessorIndex], [&](fastgltf::math::fvec2 value, std::size_t index) {
          vertices[vertex_start + index].uv[0] = value[0];
          vertices[vertex_start + index].uv[1] = value[1];
        });
      }

      if (is_skinned) {
        const auto* joints0 = primitive.findAttribute("JOINTS_0");
        const auto* weights0 = primitive.findAttribute("WEIGHTS_0");

        if (joints0 != primitive.attributes.end()) {
          fastgltf::iterateAccessorWithIndex<fastgltf::math::u32vec4>(gltf, gltf.accessors[joints0->accessorIndex], [&](fastgltf::math::u32vec4 value, std::size_t index) {
            auto& current = skin_vertices[vertex_start + index];

            for (auto component = std::size_t{0u}; component < 4u; ++component) {
              const auto skin_local = value[component];
              current.joint_indices[component] = (skin_local < joint_remap.size()) ? joint_remap[skin_local] : std::uint32_t{0u};
            }
          });
        }

        if (weights0 != primitive.attributes.end()) {
          fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(gltf, gltf.accessors[weights0->accessorIndex], [&](fastgltf::math::fvec4 value, std::size_t index) {
            const auto sum = value[0] + value[1] + value[2] + value[3];
            const auto inverse_sum = (sum > 0.0f) ? (1.0f / sum) : 0.0f;

            skin_vertices[vertex_start + index].weights = math::vector4{value[0] * inverse_sum, value[1] * inverse_sum, value[2] * inverse_sum, value[3] * inverse_sum};
          });
        }
      }

      if (!primitive.indicesAccessor.has_value()) {
        continue;
      }

      const auto& index_accessor = gltf.accessors[primitive.indicesAccessor.value()];
      const auto index_start = indices.size();
      indices.reserve(index_start + index_accessor.count);

      fastgltf::iterateAccessor<std::uint32_t>(gltf, index_accessor, [&](std::uint32_t index) {
        indices.push_back(static_cast<std::uint32_t>(vertex_start) + index);
      });

      // Positions are already in world space by this point (baked above, like the explicit-NORMAL
      // path already accounts for), so the generated face normals need no further transform.
      if (!has_explicit_normal) {
        _generate_normals(vertices, indices, vertex_start, position_accessor.count, index_start, index_accessor.count);
      }

      if (!has_explicit_tangent) {
        _generate_tangents(vertices, indices, vertex_start, position_accessor.count, index_start, index_accessor.count);
      }

      auto lods = _optimize_and_generate_lods(vertices, indices, vertex_start, position_accessor.count, index_start, index_accessor.count, has_skin_data ? &skin_vertices : nullptr);

      submeshes.push_back(cooked_submesh{
        static_cast<std::uint32_t>(index_start),
        static_cast<std::uint32_t>(index_accessor.count),
        submesh_volume,
        material_uuid_for(primitive.materialIndex),
        std::move(lods)
      });
    }
  };

  if (!gltf.scenes.empty()) {
    const auto scene_index = gltf.defaultScene.value_or(std::size_t{0});

    fastgltf::iterateSceneNodes(gltf, scene_index, fastgltf::math::fmat4x4{}, [&](const fastgltf::Node& node, const fastgltf::math::fmat4x4& world) {
      if (!node.meshIndex.has_value()) {
        return;
      }

      const auto is_skinned = has_skin_data && node.skinIndex.has_value() && node.skinIndex.value() == *primary_skin_index;

      // Skinned vertices stay in bind-pose space -- see the comment above joints' construction --
      // so a skinned node's world transform is never baked in, unlike every other node's.
      append(gltf.meshes[node.meshIndex.value()], is_skinned ? fastgltf::math::fmat4x4{} : world, is_skinned);
    });
  } else {
    for (const auto& gltf_mesh : gltf.meshes) {
      append(gltf_mesh, fastgltf::math::fmat4x4{}, false);
    }
  }

  if (vertices.empty() || indices.empty()) {
    utility::logger<"assets">::warn("Cook: mesh '{}' has no drawable geometry", source.generic_string());
    return false;
  }

  auto animation_clip_count = std::uint32_t{0u};

  if (has_skin_data) {
    if (!_cook_skeleton(derive_skeleton_uuid(id), joints)) {
      return false;
    }

    for (const auto& gltf_animation : gltf.animations) {
      auto channels = std::vector<animation_joint_channel>{};

      const auto find_or_create_channel = [&](std::uint32_t joint_index) -> animation_joint_channel& {
        for (auto& existing : channels) {
          if (existing.joint_index == joint_index) {
            return existing;
          }
        }

        auto& created = channels.emplace_back();
        created.joint_index = joint_index;
        return created;
      };

      for (const auto& gltf_channel : gltf_animation.channels) {
        if (!gltf_channel.nodeIndex.has_value()) {
          continue;
        }

        const auto joint_entry = node_to_joint.find(gltf_channel.nodeIndex.value());

        if (joint_entry == node_to_joint.end()) {
          continue; // targets a node that isn't one of this skin's joints -- not skinning-relevant
        }

        const auto& sampler = gltf_animation.samplers[gltf_channel.samplerIndex];

        const auto interpolation = (sampler.interpolation == fastgltf::AnimationInterpolation::Step) ? animation_interpolation::step
          : (sampler.interpolation == fastgltf::AnimationInterpolation::CubicSpline) ? animation_interpolation::cubic_spline
          : animation_interpolation::linear;

        if (interpolation == animation_interpolation::cubic_spline) {
          utility::logger<"assets">::warn("Cook: mesh '{}' animation '{}' uses CUBICSPLINE interpolation, unsupported -- skipping channel", source.generic_string(), std::string{gltf_animation.name.begin(), gltf_animation.name.end()});
          continue;
        }

        auto& channel = find_or_create_channel(static_cast<std::uint32_t>(joint_entry->second));

        auto times = std::vector<std::float_t>{};
        fastgltf::iterateAccessor<std::float_t>(gltf, gltf.accessors[sampler.inputAccessor], [&](std::float_t value) {
          times.push_back(value);
        });

        switch (gltf_channel.path) {
          case fastgltf::AnimationPath::Translation: {
            channel.translation_interpolation = interpolation;
            auto index = std::size_t{0u};
            fastgltf::iterateAccessor<fastgltf::math::fvec3>(gltf, gltf.accessors[sampler.outputAccessor], [&](fastgltf::math::fvec3 value) {
              if (index < times.size()) {
                channel.translation_keys.push_back({times[index], math::vector3{value[0], value[1], value[2]}});
              }
              ++index;
            });
            break;
          }
          case fastgltf::AnimationPath::Rotation: {
            channel.rotation_interpolation = interpolation;
            auto index = std::size_t{0u};
            fastgltf::iterateAccessor<fastgltf::math::fvec4>(gltf, gltf.accessors[sampler.outputAccessor], [&](fastgltf::math::fvec4 value) {
              if (index < times.size()) {
                channel.rotation_keys.push_back({times[index], math::quaternion::wxyz(value[3], value[0], value[1], value[2])});
              }
              ++index;
            });
            break;
          }
          case fastgltf::AnimationPath::Scale: {
            channel.scale_interpolation = interpolation;
            auto index = std::size_t{0u};
            fastgltf::iterateAccessor<fastgltf::math::fvec3>(gltf, gltf.accessors[sampler.outputAccessor], [&](fastgltf::math::fvec3 value) {
              if (index < times.size()) {
                channel.scale_keys.push_back({times[index], math::vector3{value[0], value[1], value[2]}});
              }
              ++index;
            });
            break;
          }
          default:
            break; // Weights (morph targets) -- not applicable to skeletal skinning
        }
      }

      if (channels.empty()) {
        continue; // e.g. an animation that only targets morph-target weights
      }

      auto duration = 0.0f;

      for (const auto& channel : channels) {
        if (!channel.translation_keys.empty()) duration = std::max(duration, channel.translation_keys.back().time);
        if (!channel.rotation_keys.empty()) duration = std::max(duration, channel.rotation_keys.back().time);
        if (!channel.scale_keys.empty()) duration = std::max(duration, channel.scale_keys.back().time);
      }

      auto clip_data = animation_clip_data{};
      clip_data.name = gltf_animation.name.empty() ? fmt::format("clip_{}", animation_clip_count) : std::string{gltf_animation.name.begin(), gltf_animation.name.end()};
      clip_data.duration = duration;
      clip_data.channels = std::move(channels);

      if (!_cook_animation_clip(derive_animation_clip_uuid(id, animation_clip_count), clip_data)) {
        return false;
      }

      ++animation_clip_count;
    }
  }

  auto error = std::error_code{};
  std::filesystem::create_directories(cooked.parent_path(), error);

  auto out = std::ofstream{cooked, std::ios::binary};

  if (!out) {
    utility::logger<"assets">::warn("Cook: could not write '{}'", cooked.generic_string());
    return false;
  }

  // meshopt-compress both buffers for the on-disk cache (smaller files, less I/O); decoded back to
  // flat vertex/index vectors on read, transparent to everything downstream of _load_cooked_mesh.
  auto encoded_vertices = std::vector<unsigned char>(meshopt_encodeVertexBufferBound(vertices.size(), sizeof(vertex)));
  const auto vertex_data_size = meshopt_encodeVertexBuffer(encoded_vertices.data(), encoded_vertices.size(), vertices.data(), vertices.size(), sizeof(vertex));
  encoded_vertices.resize(vertex_data_size);

  auto encoded_indices = std::vector<unsigned char>(meshopt_encodeIndexBufferBound(indices.size(), vertices.size()));
  const auto index_data_size = meshopt_encodeIndexBuffer(encoded_indices.data(), encoded_indices.size(), indices.data(), indices.size());
  encoded_indices.resize(index_data_size);

  auto header = mesh_file_header{};
  header.magic = mesh_magic;
  header.version = mesh_cook_version;
  header.vertex_count = static_cast<std::uint32_t>(vertices.size());
  header.index_count = static_cast<std::uint32_t>(indices.size());
  header.submesh_count = static_cast<std::uint32_t>(submeshes.size());
  header.bounds_min[0] = mesh_volume.min().x();
  header.bounds_min[1] = mesh_volume.min().y();
  header.bounds_min[2] = mesh_volume.min().z();
  header.bounds_max[0] = mesh_volume.max().x();
  header.bounds_max[1] = mesh_volume.max().y();
  header.bounds_max[2] = mesh_volume.max().z();
  header.vertex_data_size = static_cast<std::uint32_t>(vertex_data_size);
  header.index_data_size = static_cast<std::uint32_t>(index_data_size);
  header.flags = has_skin_data ? mesh_flag_has_skin_data : 0u;
  header.skin_vertex_data_size = static_cast<std::uint32_t>(skin_vertices.size() * sizeof(skin_vertex));
  header.animation_clip_count = animation_clip_count;

  out.write(reinterpret_cast<const char*>(&header), sizeof(header));
  out.write(reinterpret_cast<const char*>(encoded_vertices.data()), static_cast<std::streamsize>(vertex_data_size));
  out.write(reinterpret_cast<const char*>(encoded_indices.data()), static_cast<std::streamsize>(index_data_size));

  // Raw (unencoded) -- meshopt's vertex codec targets quantizable floats, not packed joint indices.
  if (has_skin_data) {
    out.write(reinterpret_cast<const char*>(skin_vertices.data()), static_cast<std::streamsize>(header.skin_vertex_data_size));
  }

  for (const auto& submesh : submeshes) {
    auto record = submesh_file_record{};
    record.index_offset = submesh.index_offset;
    record.index_count = submesh.index_count;
    record.bounds_min[0] = submesh.bounds.min().x();
    record.bounds_min[1] = submesh.bounds.min().y();
    record.bounds_min[2] = submesh.bounds.min().z();
    record.bounds_max[0] = submesh.bounds.max().x();
    record.bounds_max[1] = submesh.bounds.max().y();
    record.bounds_max[2] = submesh.bounds.max().z();
    record.material_uuid = submesh.material.value();
    record.lod_count = static_cast<std::uint32_t>(submesh.lods.size());

    out.write(reinterpret_cast<const char*>(&record), sizeof(record));

    for (const auto& lod : submesh.lods) {
      auto lod_record = submesh_lod_record{lod.index_offset, lod.index_count, lod.error};
      out.write(reinterpret_cast<const char*>(&lod_record), sizeof(lod_record));
    }
  }

  if (has_skin_data) {
    utility::logger<"assets">::debug("Cooked mesh '{}' -> '{}' ({} joints, {} animation clips)", source.generic_string(), cooked.generic_string(), joints.size(), animation_clip_count);
  } else {
    utility::logger<"assets">::debug("Cooked mesh '{}' -> '{}'", source.generic_string(), cooked.generic_string());
  }

  return true;
}

auto asset_cooker::_load_cooked_mesh(const std::filesystem::path& cooked, std::vector<vertex>& vertices, std::vector<std::uint32_t>& indices, std::vector<cooked_submesh>& submeshes, math::volume& bounds, std::vector<skin_vertex>& skin_vertices, std::uint32_t& animation_clip_count) -> bool {
  auto in = std::ifstream{cooked, std::ios::binary};

  if (!in) {
    return false;
  }

  auto header = mesh_file_header{};
  in.read(reinterpret_cast<char*>(&header), sizeof(header));

  if (!in || header.magic != mesh_magic || header.version != mesh_cook_version) {
    return false; // missing / corrupt / stale format -> caller recooks
  }

  auto encoded_vertices = std::vector<unsigned char>(header.vertex_data_size);
  in.read(reinterpret_cast<char*>(encoded_vertices.data()), static_cast<std::streamsize>(header.vertex_data_size));

  vertices.resize(header.vertex_count);

  if (!in || meshopt_decodeVertexBuffer(vertices.data(), header.vertex_count, sizeof(vertex), encoded_vertices.data(), encoded_vertices.size()) != 0) {
    return false; // corrupt / truncated -> caller recooks
  }

  auto encoded_indices = std::vector<unsigned char>(header.index_data_size);
  in.read(reinterpret_cast<char*>(encoded_indices.data()), static_cast<std::streamsize>(header.index_data_size));

  indices.resize(header.index_count);

  if (!in || meshopt_decodeIndexBuffer(indices.data(), header.index_count, sizeof(std::uint32_t), encoded_indices.data(), encoded_indices.size()) != 0) {
    return false;
  }

  // Raw (unencoded), immediately after the index data -- matches _cook_mesh's write order exactly;
  // must be read before the submesh records below, not after.
  skin_vertices.clear();

  if ((header.flags & mesh_flag_has_skin_data) != 0u) {
    skin_vertices.resize(header.vertex_count);
    in.read(reinterpret_cast<char*>(skin_vertices.data()), static_cast<std::streamsize>(header.skin_vertex_data_size));

    if (!in) {
      return false;
    }
  }

  submeshes.clear();
  submeshes.reserve(header.submesh_count);

  for (auto i = std::uint32_t{0u}; i < header.submesh_count; ++i) {
    auto record = submesh_file_record{};
    in.read(reinterpret_cast<char*>(&record), sizeof(record));

    if (!in) {
      return false;
    }

    auto lods = std::vector<mesh_lod>{};
    lods.reserve(record.lod_count);

    for (auto l = std::uint32_t{0u}; l < record.lod_count; ++l) {
      auto lod_record = submesh_lod_record{};
      in.read(reinterpret_cast<char*>(&lod_record), sizeof(lod_record));

      if (!in) {
        return false;
      }

      lods.push_back(mesh_lod{lod_record.index_offset, lod_record.index_count, lod_record.error});
    }

    submeshes.push_back(cooked_submesh{
      record.index_offset,
      record.index_count,
      math::volume{math::vector3{record.bounds_min[0], record.bounds_min[1], record.bounds_min[2]}, math::vector3{record.bounds_max[0], record.bounds_max[1], record.bounds_max[2]}},
      math::uuid::from_value(record.material_uuid),
      std::move(lods)
    });
  }

  if (!in) {
    return false;
  }

  bounds = math::volume{math::vector3{header.bounds_min[0], header.bounds_min[1], header.bounds_min[2]}, math::vector3{header.bounds_max[0], header.bounds_max[1], header.bounds_max[2]}};

  animation_clip_count = header.animation_clip_count;

  return true;
}

auto asset_cooker::write_cooked_mesh(const std::filesystem::path& cooked, const std::vector<vertex>& vertices, const std::vector<std::uint32_t>& indices, const std::vector<cooked_submesh>& submeshes, const math::volume& bounds) -> bool {
  auto error = std::error_code{};
  std::filesystem::create_directories(cooked.parent_path(), error);

  auto out = std::ofstream{cooked, std::ios::binary};

  if (!out) {
    utility::logger<"assets">::warn("Cook: could not write '{}'", cooked.generic_string());
    return false;
  }

  auto encoded_vertices = std::vector<unsigned char>(meshopt_encodeVertexBufferBound(vertices.size(), sizeof(vertex)));
  const auto vertex_data_size = meshopt_encodeVertexBuffer(encoded_vertices.data(), encoded_vertices.size(), vertices.data(), vertices.size(), sizeof(vertex));
  encoded_vertices.resize(vertex_data_size);

  auto encoded_indices = std::vector<unsigned char>(meshopt_encodeIndexBufferBound(indices.size(), vertices.size()));
  const auto index_data_size = meshopt_encodeIndexBuffer(encoded_indices.data(), encoded_indices.size(), indices.data(), indices.size());
  encoded_indices.resize(index_data_size);

  auto header = mesh_file_header{};
  header.magic = mesh_magic;
  header.version = mesh_cook_version;
  header.vertex_count = static_cast<std::uint32_t>(vertices.size());
  header.index_count = static_cast<std::uint32_t>(indices.size());
  header.submesh_count = static_cast<std::uint32_t>(submeshes.size());
  header.bounds_min[0] = bounds.min().x();
  header.bounds_min[1] = bounds.min().y();
  header.bounds_min[2] = bounds.min().z();
  header.bounds_max[0] = bounds.max().x();
  header.bounds_max[1] = bounds.max().y();
  header.bounds_max[2] = bounds.max().z();
  header.vertex_data_size = static_cast<std::uint32_t>(vertex_data_size);
  header.index_data_size = static_cast<std::uint32_t>(index_data_size);
  header.flags = 0u;
  header.skin_vertex_data_size = 0u;
  header.animation_clip_count = 0u;

  out.write(reinterpret_cast<const char*>(&header), sizeof(header));
  out.write(reinterpret_cast<const char*>(encoded_vertices.data()), static_cast<std::streamsize>(vertex_data_size));
  out.write(reinterpret_cast<const char*>(encoded_indices.data()), static_cast<std::streamsize>(index_data_size));

  for (const auto& submesh : submeshes) {
    auto record = submesh_file_record{};
    record.index_offset = submesh.index_offset;
    record.index_count = submesh.index_count;
    record.bounds_min[0] = submesh.bounds.min().x();
    record.bounds_min[1] = submesh.bounds.min().y();
    record.bounds_min[2] = submesh.bounds.min().z();
    record.bounds_max[0] = submesh.bounds.max().x();
    record.bounds_max[1] = submesh.bounds.max().y();
    record.bounds_max[2] = submesh.bounds.max().z();
    record.material_uuid = submesh.material.value();
    record.lod_count = static_cast<std::uint32_t>(submesh.lods.size());

    out.write(reinterpret_cast<const char*>(&record), sizeof(record));

    for (const auto& lod : submesh.lods) {
      auto lod_record = submesh_lod_record{lod.index_offset, lod.index_count, lod.error};
      out.write(reinterpret_cast<const char*>(&lod_record), sizeof(lod_record));
    }
  }

  utility::logger<"assets">::debug("Wrote generated mesh '{}'", cooked.generic_string());

  return true;
}

auto asset_cooker::write_cooked_material(const math::uuid& id, const material_description& description) -> bool {
  const auto cooked = cooked_path(id, ".sbxmat");

  auto error = std::error_code{};
  std::filesystem::create_directories(cooked.parent_path(), error);

  auto out = std::ofstream{cooked, std::ios::binary};

  if (!out) {
    utility::logger<"assets">::warn("Cook: could not write material '{}'", cooked.generic_string());
    return false;
  }

  auto header = material_file_header{};
  header.magic = material_magic;
  header.version = material_cook_version;
  header.base_color_factor[0] = description.base_color_factor.r();
  header.base_color_factor[1] = description.base_color_factor.g();
  header.base_color_factor[2] = description.base_color_factor.b();
  header.base_color_factor[3] = description.base_color_factor.a();
  header.emissive_factor[0] = description.emissive_factor.x();
  header.emissive_factor[1] = description.emissive_factor.y();
  header.emissive_factor[2] = description.emissive_factor.z();
  header.metallic_factor = description.metallic_factor;
  header.roughness_factor = description.roughness_factor;
  header.alpha_mode = static_cast<std::uint32_t>(description.alpha);
  header.alpha_cutoff = description.alpha_cutoff;
  header.is_double_sided = description.is_double_sided ? 1u : 0u;
  header.normal_scale = description.normal_scale;
  header.occlusion_strength = description.occlusion_strength;
  header.emissive_strength = description.emissive_strength;
  header.ior = description.ior;
  header.name_length = static_cast<std::uint32_t>(description.name.size());
  header.albedo_path_length = static_cast<std::uint32_t>(description.albedo.size());
  header.normal_path_length = static_cast<std::uint32_t>(description.normal.size());
  header.metallic_roughness_path_length = static_cast<std::uint32_t>(description.metallic_roughness.size());
  header.occlusion_path_length = static_cast<std::uint32_t>(description.occlusion.size());
  header.emissive_path_length = static_cast<std::uint32_t>(description.emissive.size());

  out.write(reinterpret_cast<const char*>(&header), sizeof(header));
  out.write(description.name.data(), static_cast<std::streamsize>(description.name.size()));
  out.write(description.albedo.data(), static_cast<std::streamsize>(description.albedo.size()));
  out.write(description.normal.data(), static_cast<std::streamsize>(description.normal.size()));
  out.write(description.metallic_roughness.data(), static_cast<std::streamsize>(description.metallic_roughness.size()));
  out.write(description.occlusion.data(), static_cast<std::streamsize>(description.occlusion.size()));
  out.write(description.emissive.data(), static_cast<std::streamsize>(description.emissive.size()));

  utility::logger<"assets">::debug("Wrote generated material '{}'", cooked.generic_string());

  return true;
}

auto asset_cooker::_cook_material(const math::uuid& id, const material_description& description) -> bool {
  const auto cooked = cooked_path(id, ".sbxmat");

  auto error = std::error_code{};
  std::filesystem::create_directories(cooked.parent_path(), error);

  auto out = std::ofstream{cooked, std::ios::binary};

  if (!out) {
    utility::logger<"assets">::warn("Cook: could not write material '{}'", cooked.generic_string());
    return false;
  }

  auto header = material_file_header{};
  header.magic = material_magic;
  header.version = material_cook_version;
  header.base_color_factor[0] = description.base_color_factor.r();
  header.base_color_factor[1] = description.base_color_factor.g();
  header.base_color_factor[2] = description.base_color_factor.b();
  header.base_color_factor[3] = description.base_color_factor.a();
  header.emissive_factor[0] = description.emissive_factor.x();
  header.emissive_factor[1] = description.emissive_factor.y();
  header.emissive_factor[2] = description.emissive_factor.z();
  header.metallic_factor = description.metallic_factor;
  header.roughness_factor = description.roughness_factor;
  header.alpha_mode = static_cast<std::uint32_t>(description.alpha);
  header.alpha_cutoff = description.alpha_cutoff;
  header.is_double_sided = description.is_double_sided ? 1u : 0u;
  header.normal_scale = description.normal_scale;
  header.occlusion_strength = description.occlusion_strength;
  header.emissive_strength = description.emissive_strength;
  header.ior = description.ior;
  header.name_length = static_cast<std::uint32_t>(description.name.size());
  header.albedo_path_length = static_cast<std::uint32_t>(description.albedo.size());
  header.normal_path_length = static_cast<std::uint32_t>(description.normal.size());
  header.metallic_roughness_path_length = static_cast<std::uint32_t>(description.metallic_roughness.size());
  header.occlusion_path_length = static_cast<std::uint32_t>(description.occlusion.size());
  header.emissive_path_length = static_cast<std::uint32_t>(description.emissive.size());

  out.write(reinterpret_cast<const char*>(&header), sizeof(header));
  out.write(description.name.data(), static_cast<std::streamsize>(description.name.size()));
  out.write(description.albedo.data(), static_cast<std::streamsize>(description.albedo.size()));
  out.write(description.normal.data(), static_cast<std::streamsize>(description.normal.size()));
  out.write(description.metallic_roughness.data(), static_cast<std::streamsize>(description.metallic_roughness.size()));
  out.write(description.occlusion.data(), static_cast<std::streamsize>(description.occlusion.size()));
  out.write(description.emissive.data(), static_cast<std::streamsize>(description.emissive.size()));

  return true;
}

auto asset_cooker::_cook_environment_map(const std::filesystem::path& source, const std::filesystem::path& cooked) -> bool {
  auto width = std::int32_t{0};
  auto height = std::int32_t{0};
  auto channels = std::int32_t{0};

  // source is already fully resolved -- same as _cook_texture's source.string() above.
  auto* data = stbi_loadf(source.string().c_str(), &width, &height, &channels, 4);

  if (data == nullptr) {
    utility::logger<"assets">::warn("Cook: could not decode HDR '{}'", source.generic_string());
    return false;
  }

  const auto data_size = static_cast<std::uint32_t>(width) * static_cast<std::uint32_t>(height) * 4u * static_cast<std::uint32_t>(sizeof(std::float_t));

  const auto header = texture_header{environment_magic, environment_cook_version, static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height), 4u, data_size};

  auto error = std::error_code{};
  std::filesystem::create_directories(cooked.parent_path(), error);

  auto out = std::ofstream{cooked, std::ios::binary};

  if (!out) {
    utility::logger<"assets">::warn("Cook: could not write '{}'", cooked.generic_string());
    stbi_image_free(data);
    return false;
  }

  out.write(reinterpret_cast<const char*>(&header), sizeof(header));
  out.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(data_size));

  stbi_image_free(data);

  utility::logger<"assets">::debug("Cooked environment '{}' -> '{}'", source.generic_string(), cooked.generic_string());
  return true;
}

auto asset_cooker::_load_cooked_environment_map(const std::filesystem::path& cooked, std::vector<std::byte>& pixels, std::uint32_t& width, std::uint32_t& height) -> bool {
  auto in = std::ifstream{cooked, std::ios::binary};

  if (!in) {
    return false;
  }

  auto header = texture_header{};
  in.read(reinterpret_cast<char*>(&header), sizeof(header));

  if (!in || header.magic != environment_magic || header.version != environment_cook_version) {
    return false;
  }

  pixels.resize(header.data_size);
  in.read(reinterpret_cast<char*>(pixels.data()), static_cast<std::streamsize>(header.data_size));

  if (!in) {
    return false;
  }

  width = header.width;
  height = header.height;

  return true;
}

auto asset_cooker::_cook_skeleton(const math::uuid& id, const std::vector<skeleton::joint>& joints) -> bool {
  const auto cooked = cooked_path(id, ".sbxskl");

  auto error = std::error_code{};
  std::filesystem::create_directories(cooked.parent_path(), error);

  auto out = std::ofstream{cooked, std::ios::binary};

  if (!out) {
    utility::logger<"assets">::warn("Cook: could not write skeleton '{}'", cooked.generic_string());
    return false;
  }

  auto header = skeleton_file_header{};
  header.magic = skeleton_magic;
  header.version = skeleton_cook_version;
  header.joint_count = static_cast<std::uint32_t>(joints.size());

  out.write(reinterpret_cast<const char*>(&header), sizeof(header));

  for (const auto& joint : joints) {
    auto record = skeleton_joint_record{};
    record.parent_index = joint.parent_index;

    for (auto column = std::size_t{0u}; column < 4u; ++column) {
      for (auto row = std::size_t{0u}; row < 4u; ++row) {
        record.inverse_bind_matrix[column * 4u + row] = joint.inverse_bind_matrix[column][row];
      }
    }

    record.bind_translation[0] = joint.bind_local_translation.x();
    record.bind_translation[1] = joint.bind_local_translation.y();
    record.bind_translation[2] = joint.bind_local_translation.z();
    record.bind_rotation[0] = joint.bind_local_rotation.x();
    record.bind_rotation[1] = joint.bind_local_rotation.y();
    record.bind_rotation[2] = joint.bind_local_rotation.z();
    record.bind_rotation[3] = joint.bind_local_rotation.w();
    record.bind_scale[0] = joint.bind_local_scale.x();
    record.bind_scale[1] = joint.bind_local_scale.y();
    record.bind_scale[2] = joint.bind_local_scale.z();
    record.name_length = static_cast<std::uint32_t>(joint.name.size());

    out.write(reinterpret_cast<const char*>(&record), sizeof(record));
    out.write(joint.name.data(), static_cast<std::streamsize>(joint.name.size()));
  }

  return true;
}

auto asset_cooker::_load_cooked_skeleton(const math::uuid& id, std::vector<skeleton::joint>& joints) -> bool {
  const auto cooked = cooked_path(id, ".sbxskl");

  auto in = std::ifstream{cooked, std::ios::binary};

  if (!in) {
    return false;
  }

  auto header = skeleton_file_header{};
  in.read(reinterpret_cast<char*>(&header), sizeof(header));

  if (!in || header.magic != skeleton_magic || header.version != skeleton_cook_version) {
    utility::logger<"assets">::warn("Invalid cooked skeleton '{}'", cooked.generic_string());
    return false;
  }

  joints.clear();
  joints.reserve(header.joint_count);

  for (auto i = std::uint32_t{0u}; i < header.joint_count; ++i) {
    auto record = skeleton_joint_record{};
    in.read(reinterpret_cast<char*>(&record), sizeof(record));

    if (!in) {
      return false;
    }

    auto name = std::string(record.name_length, '\0');

    if (record.name_length > 0u) {
      in.read(name.data(), static_cast<std::streamsize>(record.name_length));

      if (!in) {
        return false;
      }
    }

    auto joint = skeleton::joint{};
    joint.name = std::move(name);
    joint.parent_index = record.parent_index;

    for (auto column = std::size_t{0u}; column < 4u; ++column) {
      for (auto row = std::size_t{0u}; row < 4u; ++row) {
        joint.inverse_bind_matrix[column][row] = record.inverse_bind_matrix[column * 4u + row];
      }
    }

    joint.bind_local_translation = math::vector3{record.bind_translation[0], record.bind_translation[1], record.bind_translation[2]};
    joint.bind_local_rotation = math::quaternion::wxyz(record.bind_rotation[3], record.bind_rotation[0], record.bind_rotation[1], record.bind_rotation[2]);
    joint.bind_local_scale = math::vector3{record.bind_scale[0], record.bind_scale[1], record.bind_scale[2]};

    joints.push_back(std::move(joint));
  }

  return true;
}

auto asset_cooker::_cook_animation_clip(const math::uuid& id, const animation_clip_data& data) -> bool {
  const auto cooked = cooked_path(id, ".sbxanm");

  auto error = std::error_code{};
  std::filesystem::create_directories(cooked.parent_path(), error);

  auto out = std::ofstream{cooked, std::ios::binary};

  if (!out) {
    utility::logger<"assets">::warn("Cook: could not write animation clip '{}'", cooked.generic_string());
    return false;
  }

  auto header = animation_clip_file_header{};
  header.magic = animation_magic;
  header.version = animation_cook_version;
  header.duration = data.duration;
  header.channel_count = static_cast<std::uint32_t>(data.channels.size());
  header.name_length = static_cast<std::uint32_t>(data.name.size());

  out.write(reinterpret_cast<const char*>(&header), sizeof(header));
  out.write(data.name.data(), static_cast<std::streamsize>(data.name.size()));

  for (const auto& channel : data.channels) {
    auto record = animation_channel_record{};
    record.joint_index = channel.joint_index;
    record.translation_key_count = static_cast<std::uint32_t>(channel.translation_keys.size());
    record.rotation_key_count = static_cast<std::uint32_t>(channel.rotation_keys.size());
    record.scale_key_count = static_cast<std::uint32_t>(channel.scale_keys.size());
    record.translation_interpolation = static_cast<std::uint32_t>(channel.translation_interpolation);
    record.rotation_interpolation = static_cast<std::uint32_t>(channel.rotation_interpolation);
    record.scale_interpolation = static_cast<std::uint32_t>(channel.scale_interpolation);

    out.write(reinterpret_cast<const char*>(&record), sizeof(record));

    for (const auto& key : channel.translation_keys) {
      const auto key_record = vector3_key_record{key.time, {key.value.x(), key.value.y(), key.value.z()}};
      out.write(reinterpret_cast<const char*>(&key_record), sizeof(key_record));
    }

    for (const auto& key : channel.rotation_keys) {
      const auto key_record = quaternion_key_record{key.time, {key.value.x(), key.value.y(), key.value.z(), key.value.w()}};
      out.write(reinterpret_cast<const char*>(&key_record), sizeof(key_record));
    }

    for (const auto& key : channel.scale_keys) {
      const auto key_record = vector3_key_record{key.time, {key.value.x(), key.value.y(), key.value.z()}};
      out.write(reinterpret_cast<const char*>(&key_record), sizeof(key_record));
    }
  }

  return true;
}

auto asset_cooker::_load_cooked_animation_clip(const math::uuid& id, animation_clip_data& data) -> bool {
  const auto cooked = cooked_path(id, ".sbxanm");

  auto in = std::ifstream{cooked, std::ios::binary};

  if (!in) {
    return false;
  }

  auto header = animation_clip_file_header{};
  in.read(reinterpret_cast<char*>(&header), sizeof(header));

  if (!in || header.magic != animation_magic || header.version != animation_cook_version) {
    utility::logger<"assets">::warn("Invalid cooked animation clip '{}'", cooked.generic_string());
    return false;
  }

  auto name = std::string(header.name_length, '\0');

  if (header.name_length > 0u) {
    in.read(name.data(), static_cast<std::streamsize>(header.name_length));

    if (!in) {
      return false;
    }
  }

  data.name = std::move(name);
  data.duration = header.duration;
  data.channels.clear();
  data.channels.reserve(header.channel_count);

  for (auto i = std::uint32_t{0u}; i < header.channel_count; ++i) {
    auto record = animation_channel_record{};
    in.read(reinterpret_cast<char*>(&record), sizeof(record));

    if (!in) {
      return false;
    }

    auto channel = animation_joint_channel{};
    channel.joint_index = record.joint_index;
    channel.translation_interpolation = static_cast<animation_interpolation>(record.translation_interpolation);
    channel.rotation_interpolation = static_cast<animation_interpolation>(record.rotation_interpolation);
    channel.scale_interpolation = static_cast<animation_interpolation>(record.scale_interpolation);

    channel.translation_keys.reserve(record.translation_key_count);

    for (auto k = std::uint32_t{0u}; k < record.translation_key_count; ++k) {
      auto key_record = vector3_key_record{};
      in.read(reinterpret_cast<char*>(&key_record), sizeof(key_record));

      if (!in) {
        return false;
      }

      channel.translation_keys.push_back({key_record.time, math::vector3{key_record.value[0], key_record.value[1], key_record.value[2]}});
    }

    channel.rotation_keys.reserve(record.rotation_key_count);

    for (auto k = std::uint32_t{0u}; k < record.rotation_key_count; ++k) {
      auto key_record = quaternion_key_record{};
      in.read(reinterpret_cast<char*>(&key_record), sizeof(key_record));

      if (!in) {
        return false;
      }

      channel.rotation_keys.push_back({key_record.time, math::quaternion::wxyz(key_record.value[3], key_record.value[0], key_record.value[1], key_record.value[2])});
    }

    channel.scale_keys.reserve(record.scale_key_count);

    for (auto k = std::uint32_t{0u}; k < record.scale_key_count; ++k) {
      auto key_record = vector3_key_record{};
      in.read(reinterpret_cast<char*>(&key_record), sizeof(key_record));

      if (!in) {
        return false;
      }

      channel.scale_keys.push_back({key_record.time, math::vector3{key_record.value[0], key_record.value[1], key_record.value[2]}});
    }

    data.channels.push_back(std::move(channel));
  }

  return true;
}

} // namespace sbx::assets
