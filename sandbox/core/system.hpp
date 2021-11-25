#ifndef SBX_ECS_SYSTEM_HPP_
#define SBX_ECS_SYSTEM_HPP_

#include <type_traits>
#include <utility>
#include <iostream>
#include <memory>
#include <string>

#include <types/primitives.hpp>
#include <types/transform.hpp>

#include "resource_cache.hpp"
#include "scene.hpp"
#include "event_queue.hpp"

namespace sbx {

class system {

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

  entity create_entity(const transform& transform = transform{}, const entity parent = null_entity);

  void destroy_entity(const entity entity);

  template<typename Component, typename... Args>
  decltype(auto) add_component(const entity entity, Args&&... args) {
    assert(_scene); // Scene is uninitialized
    return _scene->add_component<Component>(entity, std::forward<Args>(args)...);
  }

  template<typename... Components>
  decltype(auto) get_components(const entity entity) const {
    assert(_scene); // Scene is uninitialized
    return _scene->get_components<Components...>(entity);
  }

  template<typename... Components>
  decltype(auto) get_components(const entity entity) {
    assert(_scene); // Scene is uninitialized
    return _scene->get_components<Components...>(entity);
  }

  template<typename... Components>
  void remove_components(const entity entity) {
    assert(_scene); // Scene is uninitialized
    return _scene->remove_components<Components...>(entity);
  }

  template<typename... Components>
  [[nodiscard]] bool has_components(const entity entity) const {
    assert(_scene); // Scene is uninitialized
    return _scene->has_components<Components...>(entity);
  }

  template<typename... Components, typename... Excludes>
  [[nodiscard]] decltype(auto) create_view(exclude_t<Excludes...> = {}) const {
    assert(_scene); // Scene is uninitialized
    return _scene->create_view<Components..., Excludes...>();
  }

  template<typename... Components, typename... Excludes>
  [[nodiscard]] decltype(auto) create_view(exclude_t<Excludes...> = {}) {
    assert(_scene); // Scene is uninitialized
    return _scene->create_view<Components..., Excludes...>();
  }

  template<typename Event, typename Listener>
  void add_listener(Listener&& listener) {
    assert(_event_queue); // Event queue is uninitialized
    _event_queue->add_listener<Event>(std::move(listener));
  }

  template<typename Event, typename... Args>
  void dispatch_event(Args&&... args) {
    assert(_event_queue); // Event queue is uninitialized
    _event_queue->dispatch_event<Event>(std::forward<Args>(args)...);
  }

  template<typename Resource, typename... Args>
  void load_resource(const std::string& name, Args&&... args) {
    assert(_resource_cache); // Resource cache is uninitialized
    _resource_cache->load<Resource>(name, std::forward<Args>(args)...);
  }

  template<typename Resource>
  std::shared_ptr<Resource> get_resource(const std::string& name) {
    assert(_resource_cache); // Resource cache is uninitialized
    return _resource_cache->get<Resource>(name);
  }

  scene* get_scene() noexcept {
    assert(_scene); // Scene is uninitialized
    return _scene;
  }

  event_queue* get_event_queue() noexcept {
    assert(_event_queue); // Event queue is uninitialized
    return _event_queue;
  }

  resource_cache* get_resource_cache() noexcept {
    assert(_resource_cache); // Resource cache is uninitialized
    return _resource_cache;
  }

private:

  friend class scheduler;
  friend class engine;

  inline static scene* _scene{nullptr};
  inline static event_queue* _event_queue{nullptr};
  inline static resource_cache* _resource_cache{nullptr};

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
