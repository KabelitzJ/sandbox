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

struct initialize_event { };
struct terminate_event { };

class my_module final : public sbx::module {

  class my_system : public sbx::system<my_system> {

  public:
    my_system(sbx::event_queue* event_queue)
    : _event_queue{event_queue},
      _counter{0.0f} { }

    ~my_system() = default;

    void initialize() {
      _event_queue->add_listener<sbx::time>([this](const auto& event){ _updated(event); });

      _event_queue->emplace<initialize_event>();
    }

    void update(const sbx::time delta_time) {
      _counter += delta_time;

      _event_queue->emplace<sbx::time>(_counter);

      if (_counter >= sbx::time{1}) {
        finish();
      }
    }

    void finished() {
      terminate();
    }

    void aborted() {
      terminate();
    }

    void terminate() {
      _event_queue->emplace<terminate_event>();
    }

  private:
    void _updated(const sbx::time& event) {
      std::cout << event << '\n';
    }

    sbx::event_queue* _event_queue{};
    sbx::time _counter{};

  }; // class my_system

public:
  my_module() {

  }

  ~my_module() {

  }

  void initialize() override {
    scheduler->attach<my_system>(event_queue);

    // [NOTE] KAJ 2021-10-12 18:50: Instant finish here, otherwise it will keep the engine alive
    scheduler->attach([this]([[maybe_unused]] const auto delta_time, [[maybe_unused]] auto finish){
      finish();
    });

    event_queue->add_listener<initialize_event>([]([[maybe_unused]] const auto& event){
      std::cout << "initialized\n";
    });

    event_queue->add_listener<terminate_event>([]([[maybe_unused]] const auto& event){
      std::cout << "terminated\n";
    });
  }

  void terminate() override {

  }

private:

};

void sbx::setup(sbx::engine& engine) {
  engine.add_module<my_module>();
}
