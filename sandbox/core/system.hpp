#ifndef SBX_ECS_SYSTEM_HPP_
#define SBX_ECS_SYSTEM_HPP_

#include <type_traits>
#include <utility>
#include <iostream>

#include <types/primitives.hpp>

#include "user.hpp"

namespace sbx {

class system : public event_queue_user, public resource_cache_user, public scene_user {

public:

  system();
  virtual ~system() = default;

  virtual void initialize() = 0;
  virtual void update(const time delta_time) = 0;
  virtual void terminate() = 0;

  [[nodiscard]] bool is_running() const noexcept;

protected:

  void exit() noexcept {
    _terminate();
  }

private:

  friend class scheduler;
  friend class engine;

  void _initialize();
  void _update(const time delta_time);
  void _terminate();

  bool _is_running{false};

}; // class system


template<typename Function>
class system_adaptor : public system, private Function {

public:

  template<typename... Args>
  system_adaptor(Args&&... args)
  : Function{std::forward<Args>(args)...} { }

  void initialize() override { }

  void update(const time delta_time) override {
    Function::operator()(delta_time, [this](){ exit(); });
  }

  void terminate() override { }

}; // class system_adaptor

} // namespace sbx

#endif // SBX_ECS_SYSTEM_HPP_
