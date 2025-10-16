#ifndef LIBSBX_MODELS_MATERIAL_SUBRENDERER_HPP_
#define LIBSBX_MODELS_MATERIAL_SUBRENDERER_HPP_

#include <cstddef>
#include <filesystem>
#include <unordered_set>
#include <ranges>
#include <algorithm>

#include <easy/profiler.h>

#include <fmt/format.h>

#include <tsl/robin_map.h>
#include <tsl/robin_set.h>

#include <range/v3/view/enumerate.hpp>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/enum.hpp>

#include <libsbx/containers/octree.hpp>

#include <libsbx/math/color.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/volume.hpp>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/timer.hpp>
#include <libsbx/utility/layout.hpp>
#include <libsbx/utility/iterator.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/draw_list.hpp>

#include <libsbx/graphics/pipeline/pipeline.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/graphics/buffers/uniform_handler.hpp>
#include <libsbx/graphics/buffers/storage_handler.hpp>
#include <libsbx/graphics/buffers/storage_buffer.hpp>

#include <libsbx/graphics/images/image2d.hpp>
#include <libsbx/graphics/images/separate_image2d_array.hpp>
#include <libsbx/graphics/images/separate_sampler.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/scene.hpp>
#include <libsbx/scenes/node.hpp>

#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/components/point_light.hpp>
#include <libsbx/scenes/components/global_transform.hpp>
#include <libsbx/scenes/components/selection_tag.hpp>

#include <libsbx/models/vertex3d.hpp>
#include <libsbx/models/mesh.hpp>
#include <libsbx/models/static_mesh_draw_list.hpp>

namespace sbx::models::prototype {

enum class alpha_mode : std::uint8_t { 
  opaque, 
  mask,
  blend
}; // enum class alpha_mode

enum class material_feature : std::uint8_t {
  emission    = utility::bit_v<0>,
  normal_map  = utility::bit_v<1>, 
  occlusion   = utility::bit_v<2>, 
  height      = utility::bit_v<3>, 
  clearcoat   = utility::bit_v<4>, 
  anisotropy  = utility::bit_v<5>
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
    key.feature_mask = features.underlying();

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

struct draw_command_range {
  std::uint32_t offset;
  std::uint32_t count;
}; // struct draw_command_range

class static_mesh {

public:

  struct submesh {
    std::uint32_t index;
    math::uuid material;
  }; // struct submesh

  static_mesh(const math::uuid mesh_id, const math::uuid material)
  : _mesh_id{mesh_id},
    _submeshes{{0, material}} { }

  static_mesh(const math::uuid mesh_id, const std::vector<submesh>& submeshes)
  : _mesh_id{mesh_id},
    _submeshes{submeshes} { }

  static_mesh(const math::uuid mesh_id, std::initializer_list<submesh> submeshes)
  : _mesh_id{mesh_id},
    _submeshes{submeshes} { }

  auto mesh_id() const noexcept -> math::uuid {
    return _mesh_id;
  }

  auto submeshes() const noexcept -> const std::vector<submesh>& {
    return _submeshes;
  }

private:

  math::uuid _mesh_id;
  std::vector<submesh> _submeshes;

}; // class static_mesh

class material_subrenderer final : public graphics::subrenderer {

  inline static const auto pipeline_definition = graphics::pipeline_definition{
    .depth = graphics::depth::read_write,
    .uses_transparency = false,
    .rasterization_state = graphics::rasterization_state{
      .polygon_mode = graphics::polygon_mode::fill,
      .cull_mode = graphics::cull_mode::back,
      .front_face = graphics::front_face::counter_clockwise
    }
  };

public:

  material_subrenderer(const graphics::render_graph::graphics_pass& pass, const std::filesystem::path& base_pipeline)
  : graphics::subrenderer{pass},
    _base_pipeline{base_pipeline} {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
      
    _transform_buffer = graphics_module.add_resource<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    _material_buffer = graphics_module.add_resource<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
  }

  ~material_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    EASY_FUNCTION();

    SBX_SCOPED_TIMER("material_subrenderer");

    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    _transform_data.clear();
    _material_data.clear();
    _images.clear();

    for (auto& [key, pipeline_data] : _pipeline_cache) {
      pipeline_data.submesh_instances.clear();
      pipeline_data.draw_ranges.clear();
    }

    auto material_indices = std::unordered_map<math::uuid, std::uint32_t>{};

    auto static_mesh_query = scene.query<const models::prototype::static_mesh>();

    for (auto&& [node, static_mesh] : static_mesh_query.each()) {
      const auto transform_index = static_cast<std::uint32_t>(_transform_data.size());

      _transform_data.emplace_back(scene.world_transform(node), scene.world_normal(node));

      for (const auto& submesh : static_mesh.submeshes()) {
        const auto& material = assets_module.get_asset<models::prototype::material>(submesh.material);

        auto& pipeline_data = _get_or_create_pipeline(material, pass());

        auto [entry, was_created] = material_indices.try_emplace(submesh.material, static_cast<std::uint32_t>(_material_data.size()));

        if (was_created) {
          const auto albedo_index = _images.push_back(material.albedo);
          const auto normal_index = _images.push_back(material.normal);
          const auto mrao_index = _images.push_back(material.mrao);

          auto material_data = prototype::material_data{};

          material_data.albedo_index = albedo_index;
          material_data.normal_index = normal_index;
          material_data.mrao_index = mrao_index;
          material_data.emissive_index = graphics::separate_image2d_array::max_size;

          material_data.base_color = material.base_color;
          material_data.emissive_color = material.emissive_color;
          material_data.metallic = material.metallic;
          material_data.roughness = material.roughness;
          material_data.occlusion = material.occlusion;
          material_data.emissive_strength = material.emissive_strength;

          material_data.alpha_cutoff = material.alpha_cutoff;
          material_data.normal_scale = 1.0f;
          material_data.flags = material.features.underlying();

          _material_data.push_back(material_data);
        }

        _submit_mesh(pipeline_data, static_mesh.mesh_id(), submesh.index, transform_index, entry->second);
      }
    }

    _update_buffer(_transform_buffer, _transform_data);
    _update_buffer(_material_buffer,  _material_data);

    auto& transform_data_buffer = graphics_module.get_resource<graphics::storage_buffer>(_transform_buffer);
    auto& material_data_buffer = graphics_module.get_resource<graphics::storage_buffer>(_material_buffer);

    for (auto& [key, pipeline_data] : _pipeline_cache) {
      if (pipeline_data.submesh_instances.empty()) {
        continue;
      }

      _build_draw_commands(pipeline_data);

      auto& pipeline = graphics_module.get_resource<graphics::graphics_pipeline>(pipeline_data.pipeline);

      pipeline.bind(command_buffer);

      pipeline_data.scene_descriptor_handler.push("scene", scene.uniform_handler());
      pipeline_data.scene_descriptor_handler.push("images_sampler", _sampler);
      pipeline_data.scene_descriptor_handler.push("images", _images);

      if (!pipeline_data.scene_descriptor_handler.update(pipeline)) {
        return;
      }

      pipeline_data.scene_descriptor_handler.bind_descriptors(command_buffer);

      auto& instance_data_buffer = graphics_module.get_resource<graphics::storage_buffer>(pipeline_data.instance_data_buffer);

      pipeline_data.push_handler.push("transform_data_buffer", transform_data_buffer.address());
      pipeline_data.push_handler.push("material_data_buffer", material_data_buffer.address());
      pipeline_data.push_handler.push("instance_data_buffer", instance_data_buffer.address());

      for (const auto& [mesh_id, range] : pipeline_data.draw_ranges) {
        auto& mesh = assets_module.get_asset<models::mesh>(mesh_id);
        
        mesh.bind(command_buffer);
        
        pipeline_data.push_handler.push("vertex_buffer", mesh.address());

        pipeline_data.push_handler.bind(command_buffer);

        auto& draw_commands_buffer = graphics_module.get_resource<graphics::storage_buffer>(pipeline_data.draw_commands_buffer);

        command_buffer.draw_indexed_indirect(draw_commands_buffer, range.offset, range.count);
      }
    }
  }

private:

  struct pipeline_data {
    graphics::graphics_pipeline_handle pipeline;
    graphics::push_handler push_handler;
    graphics::descriptor_handler scene_descriptor_handler;

    std::unordered_map<math::uuid, draw_command_range> draw_ranges;
    std::unordered_map<math::uuid, std::vector<std::vector<instance_data>>> submesh_instances;

    graphics::storage_buffer_handle draw_commands_buffer;
    graphics::storage_buffer_handle instance_data_buffer;

    pipeline_data(const graphics::graphics_pipeline_handle& handle)
    : pipeline{handle},
      push_handler{pipeline},
      scene_descriptor_handler{pipeline, 0u} {
      auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
      
      draw_commands_buffer = graphics_module.add_resource<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);
      instance_data_buffer = graphics_module.add_resource<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    }

  }; // struct pipeline_data

  template<typename Type>
  static auto _update_buffer(graphics::storage_buffer_handle handle, const std::vector<Type>& buffer) -> void {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    auto& storage_buffer = graphics_module.get_resource<graphics::storage_buffer>(handle);

    const auto required_size = static_cast<std::uint32_t>(buffer.size() * sizeof(Type));

    if (storage_buffer.size() < required_size) {
      storage_buffer.resize(static_cast<std::size_t>(static_cast<std::float_t>(required_size) * 1.5f));
    }

    storage_buffer.update(buffer.data(), required_size);
  }

  auto _get_or_create_pipeline(const material_key& key, const graphics::render_graph::graphics_pass& pass) -> pipeline_data& {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    if (auto entry = _pipeline_cache.find(key); entry != _pipeline_cache.end()) {
      return entry->second;
    }

    auto definition = pipeline_definition;
    definition.depth = graphics::depth::read_write;
    definition.rasterization_state.cull_mode = key.is_double_sided ? graphics::cull_mode::none : graphics::cull_mode::back;
    definition.uses_transparency = (static_cast<alpha_mode>(key.alpha) == alpha_mode::blend);

    auto& compiler = graphics_module.compiler();

    const auto alpha_policy = std::unordered_map<alpha_mode, std::string>{
      {alpha_mode::opaque, "opaque_alpha_policy"},
      {alpha_mode::mask, "mask_alpha_policy"},
      {alpha_mode::blend, "blend_alpha_policy"}
    };

    const auto request = graphics::compiler::compile_request{
      .path = _base_pipeline,
      .specializations = {
        {SLANG_STAGE_FRAGMENT, {alpha_policy.at(static_cast<alpha_mode>(key.alpha)), "opaque_fs_out"}}
      }
    };

    const auto result = compiler.compile(request);

    auto compiled_shaders = graphics::graphics_pipeline::compiled_shaders{_base_pipeline.filename().string(), result.code};

    auto pipeline = graphics_module.add_resource<graphics::graphics_pipeline>(compiled_shaders, pass, definition);

    auto [entry, inserted] = _pipeline_cache.emplace(key, pipeline);

    return entry->second;
  }

  auto _submit_mesh(pipeline_data& pipeline_data, const math::uuid& mesh_id, std::uint32_t submesh_index, std::uint32_t transform_index, std::uint32_t material_index) -> void {
    auto& per_mesh = pipeline_data.submesh_instances[mesh_id];

    per_mesh.resize(std::max(per_mesh.size(), static_cast<std::size_t>(submesh_index + 1u)));

    per_mesh[submesh_index].push_back(models::prototype::instance_data{transform_index, material_index, 0u, 0u});
  }

  auto _build_draw_commands(pipeline_data& pipeline_data) -> void {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto draw_commands = std::vector<VkDrawIndexedIndirectCommand>{};  
    auto instance_data = std::vector<models::prototype::instance_data>{};
    auto base_instance = std::uint32_t{0u};

    pipeline_data.draw_ranges.clear();

    for (auto&& [mesh_id, submesh_vectors] : pipeline_data.submesh_instances) {
      auto& mesh = assets_module.get_asset<models::mesh>(mesh_id);

      auto range = draw_command_range{};
      range.offset = static_cast<std::uint32_t>(draw_commands.size());
      range.count = 0u;

      for (auto&& [submesh_index, instances] : ranges::views::enumerate(submesh_vectors)) {
        if (instances.empty()) {
          continue;
        }

        const auto& submesh = mesh.submesh(submesh_index);

        auto command = VkDrawIndexedIndirectCommand{};
        command.indexCount    = submesh.index_count;
        command.instanceCount = static_cast<std::uint32_t>(instances.size());
        command.firstIndex    = submesh.index_offset;
        command.vertexOffset  = submesh.vertex_offset;
        command.firstInstance = base_instance;

        draw_commands.push_back(command);

        utility::append(instance_data, std::move(instances));

        base_instance += command.instanceCount;
        range.count++;
      }

      if (range.count > 0) {
        pipeline_data.draw_ranges.emplace(mesh_id, range);
      }
    }

    if (!draw_commands.empty()) {
      _update_buffer(pipeline_data.draw_commands_buffer, draw_commands);
      _update_buffer(pipeline_data.instance_data_buffer, instance_data);
    }
  }

  std::filesystem::path _base_pipeline;

  graphics::separate_image2d_array _images;
  graphics::separate_sampler _sampler;
  
  graphics::storage_buffer_handle _transform_buffer;
  graphics::storage_buffer_handle _material_buffer;

  std::vector<transform_data> _transform_data;
  std::vector<material_data> _material_data;

  std::unordered_map<material_key, pipeline_data, material_key_hash> _pipeline_cache;

}; // class material_subrenderer

} // namespace sbx::models::prototype

template<>
struct sbx::utility::enum_mapping<sbx::models::prototype::material_feature> {

  using entry_type = sbx::utility::entry<sbx::models::prototype::material_feature>;

  static constexpr auto values = std::array<entry_type, 6u>{
    entry_type{sbx::models::prototype::material_feature::emission, "emission"},
    entry_type{sbx::models::prototype::material_feature::normal_map, "normal_map"},
    entry_type{sbx::models::prototype::material_feature::occlusion, "occlusion"},
    entry_type{sbx::models::prototype::material_feature::height, "height"},
    entry_type{sbx::models::prototype::material_feature::clearcoat, "clearcoat"},
    entry_type{sbx::models::prototype::material_feature::anisotropy, "anisotropy"},
  };

}; // struct sbx::utility::enum_mapping

#endif // LIBSBX_MODELS_MATERIAL_SUBRENDERER_HPP_
