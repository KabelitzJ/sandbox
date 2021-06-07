#include <iostream>

#include <core/entry_point.hpp>

void sbx::setup(const std::vector<std::string>& cli_args) {
  std::cout << "Hello from " << cli_args[0] << "!\n";
}
