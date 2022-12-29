#include <iostream>
#include <cstdlib>
#include <chrono>

#include <libsbx/core/core.hpp>

int main() {
  sbx::core::logger::info("Hello, World!");

  return EXIT_SUCCESS;
}
