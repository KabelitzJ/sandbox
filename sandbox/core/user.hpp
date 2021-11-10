#ifndef SBX_CORE_USER_HPP_
#define SBX_CORE_USER_HPP_

#include <cassert>

#include "event_queue.hpp"
#include "scheduler.hpp"
#include "resource_cache.hpp"
#include "scene.hpp"

namespace sbx {

// [TODO] KAJ 2021-11-10 14:11 - Find a better name than user...

class event_queue_user {

public:

  event_queue_user() = default;
  virtual ~event_queue_user() = default;

  template<typename Event, typename Listener>
  void add_listener(Listener&& listener) {
    assert(_event_queue); // Event queue has not been initialized
    _event_queue->add_listener<Event>(std::move(listener));
  }

  template<typename Event, typename... Args>
  void dispatch_event(Args&&... args) {
    assert(_event_queue); // Event queue has not been initialized
    _event_queue->dispatch_event<Event>(std::forward<Args>(args)...);
  }

  event_queue* get_event_queue() {
    return _event_queue;
  }

private:

  friend class engine;

  inline static event_queue* _event_queue{nullptr};

}; // class event_queue_user


class scheduler_user {

public:

  scheduler_user() = default;
  virtual ~scheduler_user() = default;

  template<typename System, typename... Args>
  void add_system(Args&&... args) {
    assert(_scheduler); // Scheduler has not been initialized
    _scheduler->add_system<System>(std::forward<Args>(args)...);
  }

  template<typename Function>
  void add_system(Function&& function) {
    assert(_scheduler); // Scheduler has not been initialized
    _scheduler->add_system(std::move(function));
  }

  scheduler* get_scheduler() {
    return _scheduler;
  }

private:

  friend class engine;

  inline static scheduler* _scheduler{nullptr};

}; // class scheduler_user


class resource_cache_user {

public:

  resource_cache_user() = default;
  virtual ~resource_cache_user() = default;

  template<typename Resource, typename... Args>
  void load_resource(const std::string& name, Args&&... args) {
    assert(_resource_cache); // Resource cache has not been initialized
    _resource_cache->load<Resource>(name, std::forward<Args>(args)...);
  }

  template<typename Resource>
  std::shared_ptr<Resource> get(const std::string& name) {
    assert(_resource_cache); // Resource cache has not been initialized
    return _resource_cache->get<Resource>(name);
  }

  resource_cache* get_resource_cache() {
    return _resource_cache;
  }

private:

  friend class engine;

  inline static resource_cache* _resource_cache{nullptr};

}; // class resource_cache_user


class scene_user {

public:

  scene_user() = default;
  virtual ~scene_user() = default;

  entity create_entity(const entity parent = null_entity) {
    _scene->create_entity(parent);
  }

  void destroy_entity(const entity entity) {
    _scene->destroy_entity(entity);
  }

  template<typename Component, typename... Args>
  decltype(auto) emplace_component(const entity entity, Args&&... args) {
    return _scene->emplace_component<Component>(entity, std::forward<Args>(args)...);
  }

  template<typename... Components>
  decltype(auto) get_components(const entity entity) const {
    return _scene->get_components<Components...>(entity);
  }

  template<typename... Components>
  decltype(auto) get_components(const entity entity) {
    return _scene->get_components<Components...>(entity);
  }

  template<typename... Components>
  void remove_components(const entity entity) {
    return _scene->remove_components<Components...>(entity);
  }

  template<typename... Components>
  [[nodiscard]] bool has_components(const entity entity) const {
    return _scene->has_components<Components...>(entity);
  }

  template<typename... Components, typename... Excludes>
  [[nodiscard]] decltype(auto) create_view(exclude_t<Excludes...> = {}) const {
    return _scene->create_view<Components..., Excludes...>();
  }

  template<typename... Components, typename... Excludes>
  [[nodiscard]] decltype(auto) create_view(exclude_t<Excludes...> = {}) {
    return _scene->create_view<Components..., Excludes...>();
  }

  scene* get_scene() {
    return _scene;
  }

private:

  friend class engine;

  inline static scene* _scene{nullptr};

};

} // namespace sbx

#endif // SBX_CORE_USER_HPP_
