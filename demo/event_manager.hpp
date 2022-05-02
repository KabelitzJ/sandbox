#ifndef DEMO_EVT_EVENT_MANAGER_HPP_
#define DEMO_EVT_EVENT_MANAGER_HPP_

#include <vector>
#include <memory>

#include <types/primitives.hpp>

#include <utils/type_id.hpp>

#include "callback_container.hpp"
#include "subscription.hpp"

#include "logger.hpp"

namespace demo {

class event_manager {

  using callback_container_type = std::vector<std::unique_ptr<callback_container_base>>;

public:

  event_manager(logger* logger)
  : _logger{logger},
    _callbacks{},
    _current_event_id{} { }

  event_manager(const event_manager& other) = delete;

  event_manager(event_manager&& other) = delete;

  ~event_manager() = default;

  event_manager& operator=(const event_manager& other) = delete;

  event_manager& operator=(event_manager&& other) = delete;

  template<typename Event, typename Function>
  requires (std::is_invocable_r_v<void, Function, const Event&>)
  subscription subscribe(Function&& function) {
    const auto event_id = _event_id<Event>();

    auto& container = *static_cast<callback_container<Event>*>(_callbacks[event_id].get());

    return container.add(std::forward<Function>(function));
  }

  template<typename Event, typename Instance, typename Function>
  requires (std::is_invocable_r_v<void, Function, Instance*, const Event&>)
  subscription subscribe(Instance* instance, Function&& function) {
    return subscribe<Event>([instance, function = std::move(function)](const Event& event) {
      std::invoke(function, instance, event);
    });
  }

  void unsubscribe(const subscription& handle) {
    if (handle._is_valid) {
      _callbacks[handle._event_id]->remove(handle);
    }
  }

  template<typename Event, typename... Args>
  requires (std::is_constructible_v<Event, Args...>)
  void dispatch(Args&&... args) {
    const auto event_id = _event_id<Event>();

    auto& container = *static_cast<callback_container<Event>*>(_callbacks[event_id].get());

    const auto event = Event{std::forward<Args>(args)...};

    for (const auto& callback : container._callbacks) {
      std::invoke(callback, event);
    }
  }

private:


  template<typename Event>
  sbx::uint32 _event_id() {
    static auto event_id = _register_event<Event>();
    return event_id;
  }

  template<typename Event>
  sbx::uint32 _register_event() {
    _callbacks.emplace_back(std::make_unique<callback_container<Event>>(_logger, _current_event_id));
    return _current_event_id++;
  }

  logger* _logger{};

  callback_container_type _callbacks{};
  sbx::uint32 _current_event_id{};

}; // class event_manager

} // namespace demo

#endif // DEMO_EVT_EVENT_MANAGER_HPP_
