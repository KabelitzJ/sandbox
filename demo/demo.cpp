#include <vector>
#include <string>

#include "engine.hpp"

int main(int argc, char** argv) {
  auto engine = demo::engine{"demo/config/app.json", std::vector<std::string>{argv, argv + argc}};

  return engine.start();

  return 0;
}
