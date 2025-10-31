#ifndef LIBSBX_MODELS_MATERIAL_DRAW_LIST_HPP_
#define LIBSBX_MODELS_MATERIAL_DRAW_LIST_HPP_

#include <magic_enum/magic_enum.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/draw_list.hpp>

#include <libsbx/graphics/buffers/storage_buffer.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/components/static_mesh.hpp>

#include <libsbx/models/material.hpp>

namespace sbx::models {

template<typename Traits>
class basic_material_draw_list final : public graphics::draw_list {

  using traits_type = Traits;

public:

  using component_type = typename traits_type::component_type;
  using instance_payload = typename traits_type::instance_payload;

  enum class bucket : std::uint8_t {
    opaque,
    transparent,
    shadow
  }; // enum class bucket

  struct range_reference {
    math::uuid mesh_id;
    graphics::draw_command_range range;
  }; // struct range_reference

  struct bucket_entry {
    graphics::storage_buffer_handle draw_commands_buffer{};
    graphics::storage_buffer_handle instance_data_buffer{};
    std::vector<range_reference> ranges;
  }; // struct bucket_entry

  using bucket_map = std::unordered_map<material_key, bucket_entry, material_key_hash>;

  inline static const auto transform_data_buffer_name = utility::hashed_string{"transform_data"};
  inline static const auto material_data_buffer_name = utility::hashed_string{"material_data"};

  basic_material_draw_list() {
    create_buffer(transform_data_buffer_name, graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    create_buffer(material_data_buffer_name, graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);

    traits_type::create_shared_buffers(*this);
  }

  ~basic_material_draw_list() override {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    for (const auto& [key, data] : _pipeline_data) {  
      graphics_module.remove_resource<graphics::storage_buffer>(data.draw_commands_buffer);
      graphics_module.remove_resource<graphics::storage_buffer>(data.instance_data_buffer);
    }

    traits_type::destroy_shared_buffers(*this);
  }

  auto update() -> void override {
    _transform_data.clear();
    _material_data.clear();

    for (auto& [key, pipeline_data] : _pipeline_data) {
      pipeline_data.submesh_instances.clear();
    }

    for (auto& buckets : _bucket_ranges) {
      buckets.clear();
    }

    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();


    auto material_indices = std::unordered_map<math::uuid, std::uint32_t>{};

    traits_type::for_each_submission(scene, [&](const component_type& component, const math::uuid& mesh_id, std::uint32_t submesh_index, const math::uuid& material_id, const transform_data& transform, const instance_payload& payload) {
      const auto transform_index = static_cast<std::uint32_t>(_transform_data.size());
      _transform_data.push_back(transform);

      const auto& material = assets_module.get_asset<models::material>(material_id);

      auto& pipeline = _get_or_create_pipeline_data(material);

      auto [entry, created] = material_indices.try_emplace(material_id, static_cast<std::uint32_t>(_material_data.size()));

      if (created) {
        _push_material(material);
      }

      const auto instance = traits_type::make_instance_data(transform_index, entry->second, payload);

      auto& per_mesh = pipeline.submesh_instances[mesh_id];

      per_mesh.resize(std::max(per_mesh.size(), static_cast<std::size_t>(submesh_index + 1u)));
      per_mesh[submesh_index].push_back(instance);
    });

    update_buffer(_transform_data, transform_data_buffer_name);
    update_buffer(_material_data, material_data_buffer_name);

    traits_type::update_shared_buffers(*this);

    for (auto& [key, pipeline_data] : _pipeline_data) {
      if (pipeline_data.submesh_instances.empty()) {
        continue;
      }

      _build_draw_commands(key, pipeline_data);
    }
  }

  auto ranges(const bucket bucket) const -> const bucket_map& {
    return _bucket_ranges[magic_enum::enum_underlying(bucket)];
  }

private:

  struct pipeline_data {

    std::unordered_map<math::uuid, std::vector<std::vector<instance_data>>> submesh_instances;

    graphics::storage_buffer_handle draw_commands_buffer;
    graphics::storage_buffer_handle instance_data_buffer;

    pipeline_data() {
      auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
      
      draw_commands_buffer = graphics_module.add_resource<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);
      instance_data_buffer = graphics_module.add_resource<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    }

  }; // struct pipeline_data

  static auto _classify_bucket(const models::material& material) -> bucket {
    if (material.alpha == models::alpha_mode::blend) {
      return bucket::transparent;
    }

    return bucket::opaque;
  }

  static auto _submits_to_shadow(const models::material& material) -> bool {
    return material.features.has(models::material_feature::cast_shadow);
  }

  auto _get_or_create_pipeline_data(const material_key& key) -> pipeline_data& {
    if (auto it = _pipeline_data.find(key); it != _pipeline_data.end()) {
      return it->second;
    }

    auto [entry, inserted] = _pipeline_data.emplace(key, pipeline_data{});

    return entry->second;
  }

  auto _push_material(const models::material& material) -> void {
    const auto albedo_index   = add_image(material.albedo);
    const auto normal_index   = add_image(material.normal);
    const auto mrao_index     = add_image(material.mrao);

    auto data = models::material_data{};
    data.albedo_index = albedo_index;
    data.normal_index = normal_index;
    data.mrao_index = mrao_index;
    data.emissive_index = graphics::separate_image2d_array::max_size;

    data.base_color = material.base_color;
    data.emissive_color = material.emissive_color;
    data.metallic = material.metallic;
    data.roughness = material.roughness;
    data.occlusion = material.occlusion;
    data.emissive_strength = material.emissive_strength;

    data.alpha_cutoff = material.alpha_cutoff;
    data.normal_scale = 1.0f;
    data.flags = material.features.underlying();

    _material_data.push_back(data);

    auto& buckets = _material_buckets[material];
    buckets.insert(_classify_bucket(material));

    if (_submits_to_shadow(material)) {
      buckets.insert(bucket::shadow);
    }
  }


  auto _build_draw_commands(const material_key& key, pipeline_data& pipeline) -> void {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto draw_commands = std::vector<VkDrawIndexedIndirectCommand>{};
    auto instance_data = std::vector<models::instance_data>{};
    auto base_instance = std::uint32_t{0u};

    const auto& buckets = _material_buckets.at(key);

    for (auto& [mesh_id, submesh_vectors] : pipeline.submesh_instances) {
      auto& mesh = assets_module.get_asset<models::mesh>(mesh_id);

      auto range = graphics::draw_command_range{};
      range.offset = static_cast<std::uint32_t>(draw_commands.size());
      range.count  = 0u;

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
        const auto hash = material_key_hash{}(key);

        push_draw_command_range(hash, mesh_id, range);

        for (const auto& bucket_type : buckets) {
          auto& entry = _bucket_ranges[magic_enum::enum_underlying(bucket_type)][key];

          entry.draw_commands_buffer = pipeline.draw_commands_buffer;
          entry.instance_data_buffer = pipeline.instance_data_buffer;
          entry.ranges.push_back(range_reference{ .mesh_id = mesh_id, .range = range });
        }
      }
    }

    if (!draw_commands.empty()) {
      _update_buffer(pipeline.draw_commands_buffer, draw_commands);
      _update_buffer(pipeline.instance_data_buffer, instance_data);
    }
  }


  template <typename Type>
  static auto _update_buffer(graphics::storage_buffer_handle handle, const std::vector<Type>& data) -> void {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
    auto& buffer = graphics_module.get_resource<graphics::storage_buffer>(handle);

    const auto required_size = static_cast<std::uint32_t>(data.size() * sizeof(Type));

    if (buffer.size() < required_size) {
      buffer.resize(static_cast<std::size_t>(static_cast<std::float_t>(required_size) * 1.5f));
    }

    if (required_size > 0) {
      buffer.update(data.data(), required_size);
    }
  }

  std::vector<transform_data> _transform_data;
  std::vector<material_data> _material_data;

  std::unordered_map<material_key, pipeline_data, material_key_hash> _pipeline_data;

  std::array<bucket_map, magic_enum::enum_count<bucket>()> _bucket_ranges;

  inline static auto _material_buckets = std::unordered_map<material_key, std::unordered_set<bucket>, material_key_hash>{};

}; // class material_draw_list

struct static_mesh_traits {

  using component_type = scenes::static_mesh;
  struct instance_payload { };

  template<typename DarwList>
  static auto create_shared_buffers([[maybe_unused]] DarwList& draw_list) -> void {

  }

  template<typename DarwList>
  static auto destroy_shared_buffers([[maybe_unused]] DarwList& draw_list) -> void {

  }

  template<typename DarwList>
  static auto update_shared_buffers([[maybe_unused]] DarwList& draw_list) -> void {

  }

  template<class Callable>
  static void for_each_submission(scenes::scene& scene, Callable&& callable) {
    auto query = scene.query<const component_type>();

    for (auto&& [node, component] : query.each()) {
      const auto transform_data = models::transform_data{ scene.world_transform(node), scene.world_normal(node) };

      for (const auto& submesh : component.submeshes()) {
        std::invoke(callable, component, component.mesh_id(), submesh.index, submesh.material, transform_data, instance_payload{});
      }
    }
  }

  static auto make_instance_data(std::uint32_t transform_index, std::uint32_t material_index, const instance_payload& payload) -> instance_data {
    return instance_data{transform_index, material_index, 0u, 0u};
  }
}; // static_mesh_traits

using static_mesh_material_draw_list = basic_material_draw_list<static_mesh_traits>;

} // namespace sbx::models

#endif // LIBSBX_MODELS_MATERIAL_DRAW_LIST_HPP_
