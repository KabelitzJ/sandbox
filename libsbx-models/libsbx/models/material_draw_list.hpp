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

class material_draw_list final : public graphics::draw_list {

public:

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

  material_draw_list() {
    create_buffer(transform_data_buffer_name, graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    create_buffer(material_data_buffer_name, graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
  }

  ~material_draw_list() override {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    for (const auto& [key, data] : _pipeline_data) {  
      graphics_module.remove_resource<graphics::storage_buffer>(data.draw_commands_buffer);
      graphics_module.remove_resource<graphics::storage_buffer>(data.instance_data_buffer);
    }
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

    auto static_mesh_query = scene.query<const scenes::static_mesh>();

    for (auto&& [node, static_mesh] : static_mesh_query.each()) {
      const auto transform_index = static_cast<std::uint32_t>(_transform_data.size());

      _transform_data.emplace_back(scene.world_transform(node), scene.world_normal(node));

      for (const auto& submesh : static_mesh.submeshes()) {
        const auto& material = assets_module.get_asset<models::material>(submesh.material);

        auto& pipeline_data = _get_or_create_pipeline_data(material);

        auto [entry, was_created] = material_indices.try_emplace(submesh.material, static_cast<std::uint32_t>(_material_data.size()));

        if (was_created) {
          const auto albedo_index = add_image(material.albedo);
          const auto normal_index = add_image(material.normal);
          const auto mrao_index = add_image(material.mrao);

          auto material_data = models::material_data{};

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

          _material_buckets[material].insert(_classify_bucket(material));

          if (_submits_to_shadow(material)) {
            _material_buckets[material].insert(bucket::shadow);
          }
        }

        // if (_material_bucket.find(submesh.material) == _material_bucket.end()) {
        //   _material_bucket.emplace(submesh.material, _classify_bucket(material));
        // }

        _submit_mesh(pipeline_data, static_mesh.mesh_id(), submesh.index, transform_index, entry->second);
      }
    }

    update_buffer(_transform_data, transform_data_buffer_name);
    update_buffer(_material_data, material_data_buffer_name);

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

  static inline auto _classify_bucket(const models::material& material) -> bucket {
    if (material.alpha == models::alpha_mode::blend) {
      return bucket::transparent;
    } 

    return bucket::opaque;
  }

  static inline auto _submits_to_shadow(const models::material& material) -> bool {
    return material.features.has(models::material_feature::cast_shadow);
  }

  auto _get_or_create_pipeline_data(const material_key& key) -> pipeline_data& {
    if (auto entry = _pipeline_data.find(key); entry != _pipeline_data.end()) {
      return entry->second;
    }

    auto [entry, inserted] = _pipeline_data.emplace(key, pipeline_data{});

    return entry->second;
  }

  auto _submit_mesh(pipeline_data& pipeline_data, const math::uuid& mesh_id, std::uint32_t submesh_index, std::uint32_t transform_index, std::uint32_t material_index) -> void {
    auto& per_mesh = pipeline_data.submesh_instances[mesh_id];

    per_mesh.resize(std::max(per_mesh.size(), static_cast<std::size_t>(submesh_index + 1u)));

    per_mesh[submesh_index].push_back(models::instance_data{transform_index, material_index, 0u, 0u});
  }

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

  auto _build_draw_commands(const material_key& key, pipeline_data& pipeline_data) -> void {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto draw_commands = std::vector<VkDrawIndexedIndirectCommand>{};  
    auto instance_data = std::vector<models::instance_data>{};
    auto base_instance = std::uint32_t{0u};

    // const auto entry = _material_bucket.find(key);
    const auto& buckets = _material_buckets.at(key);

    for (auto&& [mesh_id, submesh_vectors] : pipeline_data.submesh_instances) {
      auto& mesh = assets_module.get_asset<models::mesh>(mesh_id);

      auto range = graphics::draw_command_range{};
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
        const auto hash = material_key_hash{}(key);

        push_draw_command_range(hash, mesh_id, range);

        for (const auto& bucket : buckets) {
          auto& entry = _bucket_ranges[magic_enum::enum_underlying(bucket)][key];

          entry.draw_commands_buffer = pipeline_data.draw_commands_buffer;
          entry.instance_data_buffer = pipeline_data.instance_data_buffer;
          entry.ranges.push_back(range_reference{ .mesh_id = mesh_id, .range = range });

          // .push_back(range_reference{
          //   .mesh_id = mesh_id,
          //   .range   = range
          // });
        }
      }
    }

    if (!draw_commands.empty()) {
      _update_buffer(pipeline_data.draw_commands_buffer, draw_commands);
      _update_buffer(pipeline_data.instance_data_buffer, instance_data);
    }
  }

  std::vector<transform_data> _transform_data;
  std::vector<material_data> _material_data;

  std::unordered_map<material_key, pipeline_data, material_key_hash> _pipeline_data;

  std::array<bucket_map, magic_enum::enum_count<bucket>()> _bucket_ranges;

  inline static auto _material_buckets = std::unordered_map<material_key, std::unordered_set<bucket>, material_key_hash>{};

}; // class material_draw_list

} // namespace sbx::models

#endif // LIBSBX_MODELS_MATERIAL_DRAW_LIST_HPP_
