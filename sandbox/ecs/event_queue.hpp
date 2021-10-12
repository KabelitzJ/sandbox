#ifndef SBX_ECS_EVENT_QUEUE_HPP_
#define SBX_ECS_EVENT_QUEUE_HPP_

#include <functional>
#include <memory>
#include <queue>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <types/primitives.hpp>

#include <util/memory.hpp>
#include <util/type_index.hpp>

namespace sbx {

/**
 * @brief The event_queue is used as a way for two independet systems to communicate with eachother via events.
 * One system can post a message into the event_queue that another system can listen on without needing to know
 * where the event exactly came from.
 * 
 * @warning Currently the event_queue is not thread safe
 */
class event_queue {

  /**
   * @brief A simple wrapper around a generic event instance
   */
  struct event_handle {
    using instance_type = std::unique_ptr<void, void(*)(void*)>;

    instance_type instance;
  }; // struct event_handle

public:

  /**
   * @brief Adds a new listener for the given event type
   * 
   * Listeners are best used as lambdas that capture the needed context:
   * 
   * @code{.cpp}
   * queue.add_listener<my_event>([this](const auto& event){ this->on_my_event(event); });
   * @endcode 
   * 
   * @tparam Event Type of event that should be listened to
   * @tparam Listener Type of the listener 

   * @param listener Instance of the listener
   */
  template<typename Event, typename Listener>
  void add_listener(Listener&& listener) {
    static_assert(std::is_invocable_r_v<void, Listener, const Event&>, "Wrong signature for listener");

    constexpr auto id = type_index<Event>{};

    _listeners[id].emplace_back(
      [&listener](void* event){ std::invoke(listener, *static_cast<Event*>(event)); }
    );
  }


  /**
   * @brief Adds a new event to the queue
   * 
   * @tparam Event Type of the event thats been added
   * @tparam Args Type list of arguments needed to construct a new event instance
   * @param args Arguments for event creation
   */
  template<typename Event, typename... Args>
  void emplace(Args&&... args) {
    static_assert(!std::is_abstract_v<Event>, "An event can not be abstract");
    static_assert(std::is_constructible_v<Event, Args...>, "Can not construct event from given arguments");

    constexpr auto id = type_index<Event>{};

    auto event = typename event_handle::instance_type{new Event{std::forward<Args>(args)...}, [](auto* ptr){ delete static_cast<Event*>(ptr); }};

    auto handle = event_handle{std::move(event)};

    _event_queue.emplace(id, std::move(handle));
  }

  /**
   * @brief If the queue is not empty it will pop all events and notify the appropriate listeners.
   * When no listener for a given event exists, nothing happens for that event.
   */
  void pop_all() {
    while (!_event_queue.empty()) {
      auto [id, handle] = std::move(_event_queue.front());
      _event_queue.pop();

      if (_listeners.find(id) == _listeners.end()) {
        // No listeners for this event type
        return;
      }

      for (auto& listener : _listeners[id]) {
        listener(handle.instance.get());
      }
    }
  }

private:
  std::unordered_map<uint32, std::vector<std::function<void(void*)>>> _listeners{};
  std::queue<std::pair<uint32, event_handle>> _event_queue{};

}; // class event_queue

} // namespace sbx

#endif // SBX_ECS_EVENT_QUEUE_HPP_
