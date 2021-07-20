#include <iostream>

#include <core/entry_point.hpp>

void sbx::core::setup(const std::vector<std::string_view>& cli_args) {
  std::cout << "Hello from " << cli_args[0] << "!\n";
}
