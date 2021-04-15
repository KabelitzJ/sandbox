#ifndef SANDBOX_RESOURCE_POOL_HPP_
#define SANDBOX_RESOURCE_POOL_HPP_

#include <functional>
#include <unordered_map>
#include <deque>

#include <resource_handle.hpp>

namespace sbx {

template<typename T>
class resource_pool {

public:
  using pointer = T*;
  using handle = resource_handle<T>;

  resource_pool(std::size_t initial_size = 1024);
  resource_pool(const resource_pool<T>&) = delete;
  resource_pool(resource_pool<T>&&) = delete;
  ~resource_pool();

  resource_pool<T> operator=(const resource_pool<T>&) = delete;
  resource_pool<T> operator=(resource_pool<T>&&) = delete;

  template<typename... Args>
  [[nodiscard]] handle acquire(Args&&... args);

private:
  void _release(pointer resource);

  std::unordered_map<pointer, std::function<void()>> _invalidation_callbacks;
  std::deque<pointer> _resources;

  template<typename>
  friend class resource_handle;

}; // class resource_pool

template<typename T>
resource_pool<T>::resource_pool(std::size_t initial_size) : _invalidation_callbacks(), _resources() {
  for (std::size_t i = 0; i < initial_size; ++i) {
    pointer resource = static_cast<pointer>(::operator new(sizeof(T)));
    _resources.push_back(resource);
  }
}

template<typename T>
resource_pool<T>::~resource_pool() {
  while (!_resources.empty()) {
    pointer resource = _resources.front();
    _resources.pop_front();

    _invalidation_callbacks.erase(resource);

    ::operator delete(static_cast<void*>(resource));
  }

  for (auto& [id, callback] : _invalidation_callbacks) {
    callback();
  }
}

template<typename T>
template<typename... Args>
[[nodiscard]] resource_handle<T> resource_pool<T>::acquire(Args&&... args) {
  pointer resource = nullptr;

  if (!_resources.empty()) {
    resource = _resources.front();
    _resources.pop_front();

    resource = new(resource) T(std::forward<Args>(args)...);
  }

  if (!resource) {
    resource = new T(std::forward<Args>(args)...);
  }

  resource_handle<T> handle(this, resource);

  _invalidation_callbacks[resource] = std::bind(&resource_handle<T>::_invalidate_parent, &handle);

  return handle;
}

template<typename T>
void resource_pool<T>::_release(pointer resource) {
  resource->~T();
  _resources.push_back(resource);

  _invalidation_callbacks.erase(resource);
}

} // namespace sbx

#endif // SANDBOX_RESOURCE_POOL_HPP_
