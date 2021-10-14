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

class my_module final : public sbx::module {

  class my_other_system : public sbx::system<my_other_system> {

    public:
      my_other_system(sbx::event_queue* event_queue)
      : _event_queue{event_queue},
        _counter{0.0f} { }

      ~my_other_system() = default;

      void initialize() {
        _event_queue->add_listener<sbx::time>([this](const auto& event){
          _counter += event;

          if (_counter >= sbx::time{1}) {
            finish();
          }
        });
      }

      void update([[maybe_unused]] const sbx::time delta_time) {
        
      }

      void finished() {
        
      }

      void aborted() {
        
      }

    private:
      sbx::event_queue* _event_queue{};
      sbx::time _counter{};
  };

  class my_system : public sbx::system<my_system> {

  public:
    my_system(sbx::event_queue* event_queue)
    : _event_queue{event_queue},
      _counter{0.0f} { }

    ~my_system() = default;

    void initialize() {

    }

    void update(const sbx::time delta_time) {
      _counter += delta_time;

      _event_queue->emplace<sbx::time>(_counter);

      if (_counter >= sbx::time{1}) {
        finish();
      }
    }

    void finished() {
      
    }

    void aborted() {
      
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
    _scheduler->attach<my_system>(_event_queue);
    _scheduler->attach<my_other_system>(_event_queue);
  }

  void terminate() override {

  }

private:

};

void sbx::setup(sbx::engine& engine) {
  engine.add_module<my_module>();
}
