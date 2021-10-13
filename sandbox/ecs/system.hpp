#ifndef SBX_ECS_SYSTEM_HPP_
#define SBX_ECS_SYSTEM_HPP_

#include <type_traits>
#include <utility>

#include <types/primitives.hpp>

namespace sbx {

template<typename Derived, typename Delta>
class basic_system;

template<typename Derived>
using system = basic_system<Derived, fast_time>;

template<typename Derived, typename Delta>
class basic_system {

  enum class state : uint8 {
    uninitialized = 0,
    running = 1,
    finished = 2,
    aborted = 3
  }; // enum class state

public:

  using delta_type = Delta;

  basic_system()
  : _current_state{state::uninitialized} { };

  virtual ~basic_system() {
    static_assert(std::is_base_of_v<basic_system, Derived>, "Incorrect use of class template");
  }

  [[nodiscard]] bool is_alive() const noexcept {
    return _current_state == state::running;
  }

  [[nodiscard]] bool is_finished() const noexcept {
    return _current_state == state::finished;
  }

  void abort(const bool immediately = false) {
    if (is_alive()) {
      _current_state = state::aborted;

      if (immediately) {
        tick({});
      }
    }
  }

  void tick(const delta_type delta_time) {
    switch (_current_state) {
      case state::uninitialized: {
        _next(std::integral_constant<state, state::uninitialized>{});
        _current_state = state::running;
        break;
      }
      case state::running: {
        _next(std::integral_constant<state, state::running>{}, delta_time);
      }
      default: {
        break;
      }
    }

    switch (_current_state) {
      case state::aborted: {
        _next(std::integral_constant<state, state::aborted>{});
        _current_state = state::finished;
        break;
      }
      case state::finished: {
        _next(std::integral_constant<state, state::finished>{});
        break;
      }
      default: {
        break;
      }
    }
  }

protected:
  void finish() noexcept {
    if (is_alive()) {
      _current_state = state::finished;
    }
  }

private:
  template<typename Target = Derived>
  auto _next(std::integral_constant<state, state::uninitialized>) -> decltype(std::declval<Target>().initialize(), void()) {
      static_cast<Target*>(this)->initialize();
  }

  template<typename Target = Derived>
  auto _next(std::integral_constant<state, state::running>, Delta delta_time) -> decltype(std::declval<Target>().update(delta_time), void()) {
    static_cast<Target*>(this)->update(delta_time);
  }

  template<typename Target = Derived>
  auto _next(std::integral_constant<state, state::finished>) -> decltype(std::declval<Target>().finished(), void()) {
    static_cast<Target*>(this)->finished();
  }

  template<typename Target = Derived>
  auto _next(std::integral_constant<state, state::aborted>) -> decltype(std::declval<Target>().aborted(), void()) {
    static_cast<Target*>(this)->aborted();
  }

  void _next(...) const noexcept {}


  state _current_state{state::uninitialized};

}; // class basic_system


template<typename Function, typename Delta>
class system_adaptor : public basic_system<system_adaptor<Function, Delta>, Delta>, private Function {

public:
  using delta_type = Delta;

  template<typename... Args>
  system_adaptor(Args&&... args)
  : Function{std::forward<Args>(args)...} { }

  void update(const delta_type delta_time) {
    Function::operator()(delta_time, [this](){ this->finish(); });
  } 

}; // class system_adaptor

} // namespace sbx

#endif // SBX_ECS_SYSTEM_HPP_
