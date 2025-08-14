// #ifndef DEMO_LINE_HPP_
// #define DEMO_LINE_HPP_

// #include <unordered_map>

// #include <libsbx/math/math.hpp>

// #include <libsbx/graphics/graphics.hpp>

// namespace demo {

// struct line_vertex3d {
//   sbx::math::vector3 position;
//   sbx::math::color color;
// }; // struct vertex

// constexpr auto operator==(const line_vertex3d& lhs, const line_vertex3d& rhs) noexcept -> bool {
//   return lhs.position == rhs.position && lhs.color == rhs.color;
// }

// } // namespace demo

// template<>
// struct sbx::graphics::vertex_input<demo::line_vertex3d> {
//   static auto description() -> sbx::graphics::vertex_input_description {
//     auto binding_descriptions = std::vector<VkVertexInputBindingDescription>{};

//     binding_descriptions.push_back(VkVertexInputBindingDescription{
//       .binding = 0,
//       .stride = sizeof(demo::line_vertex3d),
//       .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
//     });

//     auto attribute_descriptions = std::vector<VkVertexInputAttributeDescription>{};

//     attribute_descriptions.push_back(VkVertexInputAttributeDescription{
//       .location = 0,
//       .binding = 0,
//       .format = VK_FORMAT_R32G32B32_SFLOAT,
//       .offset = offsetof(demo::line_vertex3d, position)
//     });

//     attribute_descriptions.push_back(VkVertexInputAttributeDescription{
//       .location = 1,
//       .binding = 0,
//       .format = VK_FORMAT_R32G32B32A32_SFLOAT,
//       .offset = offsetof(demo::line_vertex3d, color)
//     });

//     return sbx::graphics::vertex_input_description{std::move(binding_descriptions), std::move(attribute_descriptions)};
//   }
// }; // struct sbx::graphics::vertex_input

// template<>
// struct std::hash<demo::line_vertex3d> {
//   auto operator()(const demo::line_vertex3d& vertex) const noexcept -> std::size_t {
//     auto hash = std::size_t{0};
//     sbx::utility::hash_combine(hash, vertex.position, vertex.color);
//     return hash;
//   }
// }; // struct std::hash<vertex3d>

// namespace demo {

// class line_mesh : public sbx::graphics::mesh<line_vertex3d> {

//   using base = sbx::graphics::mesh<line_vertex3d>;

// public:

//   line_mesh(std::vector<base::vertex_type>&& vertices, std::vector<base::index_type>&& indices)
//   : base{std::move(vertices), std::move(indices)} { }

//   ~line_mesh() override = default;

// }; // class mesh

// struct primitive {
//   sbx::math::uuid mesh_id;
// }; // struct primitive

// auto generate_grid(const sbx::math::vector2u& count, const sbx::math::vector2& size) -> std::unique_ptr<line_mesh>;

// class pipeline : public sbx::graphics::graphics_pipeline<line_vertex3d> {

//   inline static const auto pipeline_definition = sbx::graphics::pipeline_definition{
//     .depth = graphics::depth::read_write,
//     .uses_transparency = false,
//     .rasterization_state = sbx::graphics::rasterization_state{
//       .polygon_mode = sbx::graphics::polygon_mode::line,
//       .line_width = 2.0f,
//       .cull_mode = sbx::graphics::cull_mode::back,
//       .front_face = sbx::graphics::front_face::counter_clockwise
//     },
//     .primitive_topology = sbx::graphics::primitive_topology::line_list
//   };

//   using base = sbx::graphics::graphics_pipeline<line_vertex3d>;

// public:

//   pipeline(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
//   : base{path, stage, pipeline_definition} { }

//   ~pipeline() override = default;

// }; // class pipeline

// class line_subrenderer : public sbx::graphics::subrenderer {

//   using base = sbx::graphics::subrenderer;

// public:

//   line_subrenderer(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
//   : base{stage},
//     _pipeline{path, stage} { }

//   ~line_subrenderer() override = default;

//   auto render(sbx::graphics::command_buffer& command_buffer) -> void override {
//     auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

//     auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
//     auto& scene = scenes_module.scene();

//     auto camera_node = scene.camera();

//     auto& camera = camera_node.get_component<sbx::scenes::camera>();

//     _scene_uniform_handler.push("projection", camera.projection());

//     const auto& camera_transform = camera_node.get_component<sbx::scenes::transform>();

//     _scene_uniform_handler.push("view", sbx::math::matrix4x4::inverted(camera_transform.as_matrix()));

//     for (auto entry = _uniform_data.begin(); entry != _uniform_data.end();) {
//       if (_used_uniforms.contains(entry->first)) {
//         ++entry;
//       } else {
//         entry = _uniform_data.erase(entry);
//       }
//     }

//     _used_uniforms.clear();

//     auto primitive_nodes = scene.query<primitive>();

//     for (const auto& node : primitive_nodes) {
//       auto& primitive = node.get_component<demo::primitive>();
//       auto& id = node.get_component<sbx::scenes::id>();
//       auto& transform = node.get_component<sbx::scenes::transform>();

//       _used_uniforms.insert(id);

//       _pipeline.bind(command_buffer);

//       auto& [push_handler, descriptor_handler] = _uniform_data[id];

//       push_handler.push("model", transform.as_matrix());

//       auto& mesh = assets_module.get_asset<line_mesh>(primitive.mesh_id);

//       descriptor_handler.push("data", push_handler);
//       descriptor_handler.push("uniform_scene", _scene_uniform_handler);

//       if (!descriptor_handler.update(_pipeline)) {
//         continue;
//       }

//       descriptor_handler.bind_descriptors(command_buffer);
//       push_handler.bind(command_buffer);

//       mesh.render(command_buffer);
//     }
//   }

// private:

//   struct uniform_data {
//     sbx::graphics::push_handler push_handler;
//     sbx::graphics::descriptor_handler descriptor_handler;
//   }; // struct uniform_data

//   pipeline _pipeline;

//   std::unordered_map<sbx::math::uuid, uniform_data> _uniform_data;
//   std::unordered_set<sbx::math::uuid> _used_uniforms;

//   sbx::graphics::uniform_handler _scene_uniform_handler;

// }; // class line_subrenderer

// } // namespace demo

// #endif // DEMO_LINE_HPP_
