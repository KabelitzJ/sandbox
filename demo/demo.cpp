#include <iostream>

#include <core/core.hpp>

struct prosition {
  float x;
  float y;
  float z;
};

struct velocity {
  float x;
  float y;
  float z;
};

struct model {};

class my_system final : public sbx::system {

public:
  my_system() {

  }

  ~my_system() {

  }

  void initialize() override {

  }

};

class my_module final : public sbx::module {

public:
  my_module() {

  }

  ~my_module() {

  }

  void initialize() override {
    add_system<my_system>();

    const auto player = _registry->create_entity();

    _registry->add_component<prosition>(player, 0.0f, 0.0f, 0.0f);
    _registry->add_component<velocity>(player, 1.0f, 0.0f, 0.0f);

    std::cout << std::boolalpha << _registry->has_component<model>(player) << '\n';
  }

};

void sbx::setup(sbx::engine& engine) {
  engine.add_module<my_module>();
}
