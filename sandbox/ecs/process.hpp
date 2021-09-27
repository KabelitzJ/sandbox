#ifndef SBX_ECS_PROCESS_HPP_
#define SBX_ECS_PROCESS_HPP_

#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {
 
template<typename Derived, typename Delta>
class process {

public:

  using delta_type = Delta;

  process() = default;

  virtual ~process() {
    static_assert(std::is_base_of_v<process, Derived>, "Incorrect use of the class template");
  }

  bool is_alive() const noexcept {
    return current_state == state::RUNNING;
  }

  bool is_finished() const noexcept {
    return current_state == state::SUCCEEDED || current_state == state::FAILED;
  }

  void abort(const bool immediately = false) {
    if(is_alive()) {
      current_state = state::ABORTED;

      if(immediately) {
          tick({});
      }
    }
  }

  void tick(const Delta delta) {
    switch (current_state) {
    case state::UNINITIALIZED:
      next(std::integral_constant<state, state::UNINITIALIZED>{});
      current_state = state::RUNNING;
      break;
    case state::RUNNING:
      next(std::integral_constant<state, state::RUNNING>{}, delta);
      break;
    default:
      // suppress warnings
      break;
    }

    switch(current_state) {
    case state::SUCCEEDED:
      next(std::integral_constant<state, state::SUCCEEDED>{});
      break;
    case state::FAILED:
      next(std::integral_constant<state, state::FAILED>{});
      break;
    case state::ABORTED:
      next(std::integral_constant<state, state::ABORTED>{});
      current_state = state::FAILED;
      break;
    default:
      // suppress warnings
      break;
    }
  }

protected:

  void succeed() noexcept {
    if (is_alive()) {
      current_state = state::SUCCEEDED;
    }
  }

  void fail() noexcept {
    if (is_alive()) {
      current_state = state::FAILED;
    }
  }

private:

  enum class state : uint32 {
    UNINITIALIZED = 0,
    RUNNING,
    SUCCEEDED,
    FAILED,
    ABORTED
  };

  template<typename Target = Derived>
  auto next(std::integral_constant<state, state::UNINITIALIZED>)
  -> decltype(std::declval<Target>().init(), void()) {
    static_cast<Target*>(this)->init();
  }

  template<typename Target = Derived>
  auto next(std::integral_constant<state, state::RUNNING>, const Delta delta)
  -> decltype(std::declval<Target>().update(delta), void()) {
    static_cast<Target*>(this)->update(delta);
  }

  template<typename Target = Derived>
  auto next(std::integral_constant<state, state::SUCCEEDED>)
  -> decltype(std::declval<Target>().succeeded(), void()) {
    static_cast<Target *>(this)->succeeded();
  }

  template<typename Target = Derived>
  auto next(std::integral_constant<state, state::FAILED>)
  -> decltype(std::declval<Target>().failed(), void()) {
    static_cast<Target *>(this)->failed();
  }

  template<typename Target = Derived>
  auto next(std::integral_constant<state, state::ABORTED>)
  -> decltype(std::declval<Target>().aborted(), void()) {
    static_cast<Target *>(this)->aborted();
  }

  void next(...) const noexcept {}

  state current_state{state::UNINITIALIZED};

}; // class process


template<typename Function, typename Delta>
class process_adaptor : process<process_adaptor<Function, Delta>, Delta>, private Function {

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
