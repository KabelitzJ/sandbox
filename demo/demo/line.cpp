// #include <demo/line.cpp>

// namespace demo {

// auto generate_grid(const sbx::math::vector2u& count, const sbx::math::vector2& size) -> std::unique_ptr<line_mesh> {
//   auto vertices = std::vector<line_mesh::vertex_type>{};
//   auto indices = std::vector<line_mesh::index_type>{};

//   const auto vertex_count = count.x() * count.y();
//   const auto face_count = (count.x() - 1u) * (count.y() - 1u) * 2;

//   const auto width = size.x() * count.x();
//   const auto depth = size.y() * count.y();

//   const auto half_width = width * 0.5f;
//   const auto half_depth = depth * 0.5f;

//   const auto dx = width / (count.y() - 1u);
//   const auto dz = depth / (count.x() - 1u);

//   vertices.reserve(vertex_count);

//   for (unsigned int i = 0; i < count.x(); ++i) {
//     float z = half_depth - i * dz;

//     for (unsigned int j = 0; j < count.y(); ++j) {
//       float x = -half_width + j * dx;

//       const auto position = sbx::math::vector3{x, 0.0f, z};
//       const auto color = sbx::math::color::white;

//       vertices.push_back(line_vertex3d{position, color});
//     }
//   }

//   indices.reserve(face_count * 4u);

//   for (unsigned int i = 0; i < count.x() - 1; ++i) {
//     for (unsigned int j = 0; j < count.y() - 1; ++j) {
//       indices.push_back(i * count.y() + j);
//       indices.push_back(i * count.y() + (j + 1));
//       indices.push_back(i * count.y() + (j + 1));
//       indices.push_back((i + 1) * count.y() + j + 1);

//       indices.push_back((i + 1) * count.y() + j + 1);
//       indices.push_back((i + 1) * count.y() + j);
//       indices.push_back((i + 1) * count.y() + j);
//       indices.push_back(i * count.y() + j);
//     }
//   }

//   return std::make_unique<line_mesh>(std::move(vertices), std::move(indices)); 
// }

// } // namespace demo
