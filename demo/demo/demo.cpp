#include <iostream>

#include <libsbx/core/core.hpp>
#include <libsbx/ecs/ecs.hpp>

class test_module : public sbx::core::module<test_module> {
  
  inline static const auto registered = register_module(stage::normal);

public:

  test_module() = default;
  ~test_module() override = default;

  void update() override {
    std::cout << "test_module::update()\n";
  }

private:

}; // class test_module

int main() {
  std::cout << "libsbx-core: " << LIBSBX_CORE_VERSION_STR << "\n";
  std::cout << "libsbx-ecs: " << LIBSBX_ECS_VERSION_STR << "\n";

  sbx::core::module_manager::create_all();

  sbx::core::module_manager::update_stages();

  sbx::core::module_manager::destroy_all();

  return 0;
}
