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
  using value_type = T;
  using pointer = value_type*;
  using const_pointer = const pointer;
  using size_type = std::size_t;
  using handle = resource_handle<value_type>;

  resource_pool(size_type initial_size = 1024);
  resource_pool(const resource_pool<T>&) = delete;
  resource_pool(resource_pool<T>&&) = delete;
  ~resource_pool();

  resource_pool<T> operator=(const resource_pool<T>&) = delete;
  resource_pool<T> operator=(resource_pool<T>&&) = delete;

  template<typename... Args>
  [[nodiscard]] handle acquire_handle(Args&&... args);

  template <typename... Args>
  [[nodiscard]] pointer create(Args&&... args);

  void destroy(pointer resource);

private:
  pointer _pop_front();
  
  void _allocate_n(size_type n);

  void _release(pointer resource);

  size_type _capacity;
  std::unordered_map<pointer, std::function<void()>> _invalidation_callbacks;
  std::deque<pointer> _resources;

  template<typename>
  friend class resource_handle;

}; // class resource_pool

template<typename T>
resource_pool<T>::resource_pool(size_type initial_size) : _capacity(initial_size), _invalidation_callbacks(), _resources() {
  _allocate_n(initial_size);
}

template<typename T>
resource_pool<T>::~resource_pool() {
  for (auto* resource : _resources) {
    _invalidation_callbacks.erase(resource);

    ::operator delete(static_cast<void*>(resource));
    resource = nullptr;
  }

  _resources.clear();

  for (auto& [id, callback] : _invalidation_callbacks) {
    callback();
  }
}

template<typename T>
template<typename... Args>
resource_handle<T> resource_pool<T>::acquire_handle(Args&&... args) {
  pointer resource = create(std::forward<Args>(args)...);

  resource_handle<T> handle(this, resource);

  _invalidation_callbacks[resource] = std::bind(&resource_handle<T>::_invalidate_parent, &handle);

  return handle;
}

template <typename T>
template <typename... Args>
typename resource_pool<T>::pointer resource_pool<T>::create(Args&&... args) {
  pointer resource = _pop_front();

  return new(resource) T(std::forward<Args>(args)...);
}

template <typename T>
void resource_pool<T>::destroy(pointer resource) {
  resource->~T();
  _resources.push_back(resource);
}

template <typename T>
typename resource_pool<T>::pointer resource_pool<T>::_pop_front() {
  if (_resources.empty()) {
    _allocate_n(_capacity);
    _capacity *= 2;
  }

  pointer resource = _resources.front();
  _resources.pop_front();

  return resource;
}

template <typename T>
void resource_pool<T>::_allocate_n(size_type n) {
  for (size_type i = 0; i < n; ++i) {
    pointer resource = static_cast<pointer>(::operator new(sizeof(T)));
    _resources.push_back(resource);
  }
}

template<typename T>
void resource_pool<T>::_release(pointer resource) {
  destroy(resource);

  _invalidation_callbacks.erase(resource);
}

} // namespace sbx

#endif // SANDBOX_RESOURCE_POOL_HPP_
