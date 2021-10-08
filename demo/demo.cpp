#include <iostream>

#include <core/core.hpp>

struct velocity {
  float x;
  float y;
  float z;
};

struct position {
  float x;
  float y;
  float z;

  position& operator+=(const velocity& velocity) {
    x += velocity.x;
    y += velocity.y;
    z += velocity.z;

    return *this;
  }
};

position operator+(position position, const velocity& velocity) {
  position += velocity;

  return position;
}

std::ostream& operator<<(std::ostream& os, const position& position) {
  return os << "[" << position.x << ", " << position.y << ", " << position.z << "]";
}

velocity operator*(const velocity& velocity, const float scalar) {
  return {velocity.x * scalar, velocity.y * scalar, velocity.z * scalar};
}

#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class my_module final : public sbx::module {

  class my_system : public sbx::system<my_system> {

  public:
    my_system() = default;
    ~my_system() = default;

    void initialize() {

    }

    void update([[maybe_unused]] const sbx::fast_time delta_time) {

    }

    void finished() {

    }

    void aborted() {

    }

  }; // class my_system

public:
  my_module() {

  }

  ~my_module() {

  }

  void initialize() override {
    _scheduler->attach<my_system>();

    _scheduler->attach([this]([[maybe_unused]] const auto delta_time, [[maybe_unused]] auto finish){

    });
  }

  void terminate() override {

  }

private:

};

void sbx::setup(sbx::engine& engine) {
  engine.add_module<my_module>();
}
