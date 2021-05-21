#ifndef SBX_CORE_EVENT_QUEUE_HPP_
#define SBX_CORE_EVENT_QUEUE_HPP_

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <queue>
#include <memory>

#include <GLFW/glfw3.h>

#include "event.hpp"

namespace sbx {

class event_queue {

public:
  event_queue(GLFWwindow* context);
  ~event_queue();

  void poll();

  template<typename EventType, typename Callback>
  void subscribe(Callback&& subscriber);

private:

  template<typename EventType, typename... Args>
  void _push(Args&&... args);

  void _bind_callbacks();
  void _unbind_callbacks();

  GLFWwindow* _context;
  std::queue<std::pair<std::type_index, std::unique_ptr<event>>> _queue;
  std::unordered_map<std::type_index, std::vector<std::function<void(event&)>>> _subscribers;

}; // class event_queue

template<typename EventType, typename Callback>
inline void event_queue::subscribe(Callback&& subscriber) {
  std::type_index type = std::type_index(typeid(EventType));
  _subscribers[type].push_back([subscriber](event& e){
    std::invoke(subscriber, static_cast<EventType&>(e));
  });
}

template<typename EventType, typename... Args>
inline void event_queue::_push(Args&&... args) {
  std::type_index type = std::type_index(typeid(EventType));
  auto event = std::make_unique<EventType>(std::forward<Args>(args)...);

  _queue.emplace(type, std::move(event));
}

} // namespace sbx

#endif // SBX_CORE_EVENT_QUEUE_HPP_
