#include "model_loader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

namespace sbx {

basic_model load_basic_model(const std::filesystem::path& path) {
  std::ifstream file_stream(path);

  if (!file_stream.is_open()) {
    std::stringstream ss;

    ss << "[Error] Could not open mesh file: '" << path << "'!\n";

    throw std::runtime_error(ss.str());
  }

  basic_model model;
  std::string line;

  while (std::getline(file_stream, line)) {
    if (line[0] == 'v') {
      std::istringstream line_stream(line);

      char tag;
      GLfloat x, y, z;

      line_stream >> tag >> x >> y >> z;

      model.vertices.push_back(x);
      model.vertices.push_back(y);
      model.vertices.push_back(z);
    } else if (line[0] == 'f') {
      std::istringstream line_stream(line);

      char tag;
      GLuint first, second, third;

      line_stream >> tag >> first >> second >> third;

      // Blender indices are 1 indexed...
      model.indices.push_back(first - 1);
      model.indices.push_back(second - 1);
      model.indices.push_back(third - 1);
    }
  }

  return model;
}

} // namespace sbx
