#ifndef SBX_ECS_PROCESS_HPP_
#define SBX_ECS_PROCESS_HPP_

#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {
 
template<typename Derived, typename Delta>
class basic_process {

public:

  using delta_type = Delta;

  basic_process() = default;

  basic_process(const basic_process&) = delete;

  basic_process(basic_process&&) = default;

  virtual ~basic_process() {
    static_assert(std::is_base_of_v<basic_process, Derived>, "Incorrect use of the class template");
  }

  basic_process& operator=(const basic_process&) = delete;

  basic_process& operator=(basic_process&&) = default;

  bool is_alive() const noexcept {
    return _current_state == state::running;
  }

  bool is_finished() const noexcept {
    return _current_state == state::succeeded || _current_state == state::failed;
  }

  void abort(const bool immediately = false) {
    if(is_alive()) {
      _current_state = state::aborted;

      if(immediately) {
        tick({});
      }
    }
  }

  void tick(const Delta delta) {
    switch (_current_state) {
      case state::uninitialized: {
        _next(std::integral_constant<state, state::uninitialized>{});
        _current_state = state::running;
        break;
      }
      case state::running: {
        _next(std::integral_constant<state, state::running>{}, delta);
        break;
      }
      default: {
        break;
      }
    }

    switch(_current_state) {
      case state::succeeded: {
        _next(std::integral_constant<state, state::succeeded>{});
        break;
      }
      case state::failed: {
        _next(std::integral_constant<state, state::failed>{});
        break;
      }
      case state::aborted: {
        _next(std::integral_constant<state, state::aborted>{});
        _current_state = state::failed;
        break;
      }
      default: {
          break;
      }
    }
  }

protected:

  void succeed() noexcept {
    if (is_alive()) {
      _current_state = state::succeeded;
    }
  }

  void fail() noexcept {
    if (is_alive()) {
      _current_state = state::failed;
    }
  }

private:

  enum class state : uint32 {
    uninitialized = 0,
    running,
    succeeded,
    failed,
    aborted
  };

  template<typename Target = Derived>
  auto _next(std::integral_constant<state, state::uninitialized>) -> decltype(std::declval<Target>().initialize(), void()) {
    static_cast<Target*>(this)->initialize();
  }

  template<typename Target = Derived>
  auto _next(std::integral_constant<state, state::running>, const Delta delta) -> decltype(std::declval<Target>().update(delta), void()) {
    static_cast<Target*>(this)->update(delta);
  }

  template<typename Target = Derived>
  auto _next(std::integral_constant<state, state::succeeded>) -> decltype(std::declval<Target>().succeeded(), void()) {
    static_cast<Target *>(this)->succeeded();
  }

  template<typename Target = Derived>
  auto _next(std::integral_constant<state, state::failed>) -> decltype(std::declval<Target>().failed(), void()) {
    static_cast<Target *>(this)->failed();
  }

  template<typename Target = Derived>
  auto _next(std::integral_constant<state, state::aborted>) -> decltype(std::declval<Target>().aborted(), void()) {
    static_cast<Target *>(this)->aborted();
  }

  void _next(...) const noexcept {}

  state _current_state{state::uninitialized};

}; // class basic_process


template<typename Function, typename Delta>
class process_adaptor : public basic_process<process_adaptor<Function, Delta>, Delta>, private Function {

public:

  template<typename... Args>
  process_adaptor(Args&&... args) : Function{std::forward<Args>(args)...} {

  }

  void update(const Delta delta) {
    Function::operator()(delta, [this](){ this->succeed(); }, [this](){ this->fail(); });
  }

}; // class process_adaptor

} // namespace sbx

#endif // SBX_ECS_PROCESS_HPP_
