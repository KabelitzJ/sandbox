#include "mesh_loader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

namespace sbx {

basic_mesh load_basic_mesh(const std::filesystem::path& path) {
  std::ifstream ifs(path);

  if (!ifs.is_open()) {
    std::stringstream ss;

    ss << "[Error] Could not open mesh file: '" << path << "'!\n";

    throw std::runtime_error(ss.str());
  }

  basic_mesh mesh;
  std::string line;
  std::istringstream iss;

  while (std::getline(ifs, line)) {
    iss.str(line);
    iss.clear();
    if (line.find("#region", 0) == 0) {
      std::string dump;
      std::string region;
      unsigned int value_count;
      unsigned int row_count;

  	  iss >> dump >> region >> value_count >> row_count;

      for (unsigned int row = 0; row < row_count; ++row) {
        std::getline(ifs, line);
        iss.str(line);
        iss.clear();
        
        for (unsigned int index = 0; index < value_count; ++index) {
          if (region == "vertices") {
            GLfloat value;
            iss >> value;
            mesh.vertices.push_back(value);
          } else if (region == "indices") {
            GLuint value;
            iss >> value;
            mesh.indices.push_back(value);
          }

        }
      }
    }
  }

  return mesh;
}

} // namespace sbx
