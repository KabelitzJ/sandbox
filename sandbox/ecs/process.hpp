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

  virtual ~basic_process() {
    static_assert(std::is_base_of_v<basic_process, Derived>, "Incorrect use of the class template");
  }

  basic_process& operator=(const basic_process&) = delete;

  bool is_alive() const noexcept {
    return _current_state == state::running;
  }

  bool is_finished() const noexcept {
    return _current_state == state::succeeded || _current_state == state::failed;
  }

  void abort(const bool immediately = false);

  void tick(const Delta delta);

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
  decltype(std::declval<Target>().initialize(), void()) _next(std::integral_constant<state, state::uninitialized>) {
    static_cast<Target*>(this)->initialize();
  }

  template<typename Target = Derived>
  decltype(std::declval<Target>().update(Delta), void()) _next(std::integral_constant<state, state::running>, const Delta delta) {
    static_cast<Target*>(this)->update(delta);
  }

  template<typename Target = Derived>
  decltype(std::declval<Target>().succeeded(), void()) _next(std::integral_constant<state, state::succeeded>) {
    static_cast<Target *>(this)->succeeded();
  }

  template<typename Target = Derived>
  decltype(std::declval<Target>().failed(), void()) _next(std::integral_constant<state, state::failed>) {
    static_cast<Target *>(this)->failed();
  }

  template<typename Target = Derived>
  decltype(std::declval<Target>().aborted(), void()) _next(std::integral_constant<state, state::aborted>) {
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

#include "process.inl"

#endif // SBX_ECS_PROCESS_HPP_
