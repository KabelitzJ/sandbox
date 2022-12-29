#include <iostream>
#include <cstdlib>
#include <chrono>

#include <libsbx/core/core.hpp>

class foo : public sbx::core::module<foo> {

}; // class foo

int main() {
  sbx::core::logger::info("Hello, World!");

  return EXIT_SUCCESS;
}
