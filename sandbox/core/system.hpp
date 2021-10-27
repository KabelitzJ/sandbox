#ifndef SBX_ECS_SYSTEM_HPP_
#define SBX_ECS_SYSTEM_HPP_

#include <type_traits>
#include <utility>

#include <types/primitives.hpp>

namespace sbx {

struct system_base {

  virtual ~system_base() = default;

  virtual void initialize() = 0;
  virtual void update(const time delta_time) = 0;
  virtual void finished() = 0;
  virtual void aborted() = 0;

}; // struct system_base

class system : public system_base {

  enum class state : uint8 {
    uninitialized = 0,
    running = 1,
    finished = 2,
    aborted = 3
  }; // enum class state

public:

  system()
  : _current_state{state::uninitialized} { };

  virtual ~system() = default;

  void abort() {
    if (is_alive()) {
      _current_state = state::aborted;
    }
  }

  [[nodiscard]] bool is_finished() const noexcept {
    return _current_state == state::finished || _current_state == state::aborted;
  }

  [[nodiscard]] bool is_alive() const noexcept {
    return _current_state == state::running;
  }

protected:

  void finish() noexcept {
    if (is_alive()) {
      _current_state = state::finished;
    }
  }

private:

  friend class scheduler;

  void _initialize() {
    initialize();

    _current_state = state::running;
  }

  state _current_state{state::uninitialized};

}; // class system


template<typename Function>
class system_adaptor : public system, private Function {

public:

  template<typename... Args>
  system_adaptor(Args&&... args)
  : Function{std::forward<Args>(args)...} { }

  void initialize() override { }

  void update(const time delta_time) override {
    Function::operator()(delta_time, [this](){ this->finish(); });
  }

  void finished() override { }

  void aborted() override { }

}; // class system_adaptor

} // namespace sbx

#endif // SBX_ECS_SYSTEM_HPP_
