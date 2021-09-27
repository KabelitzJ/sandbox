#include <iostream>

#include <core/core.hpp>
#include <ecs/process.hpp>

struct position {
  float x;
  float y;
  float z;
};

struct velocity {
  float x;
  float y;
  float z;
};

class my_process final : public sbx::basic_process<my_process, sbx::fast_time> {

public:
  my_process() = default;
  ~my_process() = default;

  void initialize() {

  }

  void update(const sbx::fast_time delta) {
    std::cout << "update(" << delta << ")\n";
  }

  void succeeded() {

  }

  void failed() {

  }

};

class my_system final : public sbx::system {

public:
  my_system() {

  }

  ~my_system() {

  }

  void initialize() override {
    _scheduler->attach<my_process>();

    const auto player = _registry->create_entity();

    (void) player;
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

    const auto e = _registry->create_entity();
    (void)e;
  }

};

void sbx::setup(sbx::engine& engine) {
  engine.add_module<my_module>();
}
