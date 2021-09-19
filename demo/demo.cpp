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
  }

};

void sbx::setup(sbx::engine& engine) {
  engine.add_module<my_module>();
}
