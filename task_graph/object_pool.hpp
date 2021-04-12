#ifndef TASK_GRAPH_OBJECT_POOL_HPP_
#define TASK_GRAPH_OBJECT_POOL_HPP_

#include <memory>
#include <functional>
#include <deque>
#include <algorithm>

namespace tg {


template<typename T>
class object_pool {

public:
  explicit object_pool(std::size_t initial_size = 1048u);

  object_pool(const object_pool&) = delete;

  object_pool(object_pool&&) = delete;

  ~object_pool();

  object_pool& operator=(const object_pool&) = delete;
  object_pool& operator=(object_pool&&) = delete;

  template<typename... Args>
  std::unique_ptr<T, std::function<void(T*)>> request(Args&&... args);

private:
  std::deque<T*> _object_pool; // is deque the best container?

};

template<typename T>
object_pool<T>::object_pool(std::size_t initial_size) : _object_pool() {
  for (std::size_t i = 0; i < initial_size; ++i) {
    T* object = reinterpret_cast<T*>(::operator new(sizeof(T)));
    _object_pool.push_back(object);
  }
}

template<typename T>
object_pool<T>::~object_pool() {
  while (!_object_pool.empty()) {
    T* object = _object_pool.front();
    _object_pool.pop_front();
    ::operator delete(object);
  }
}

template<typename T>
template<typename... Args>
std::unique_ptr<T, std::function<void(T*)>> object_pool<T>::request(Args&&... args) {
  T* object = nullptr;

  if (!_object_pool.empty()) {
    object = new(_object_pool.front()) T(std::forward<Args>(args)...);
    _object_pool.pop_front();
  } else {
    object = new T(std::forward<Args>(args)...);
  }

  auto object_pointer = std::unique_ptr<T, std::function<void(T*)>>(object, [this](T* object){ 
    object->~T();
    _object_pool.push_back(object);
  });

  return object_pointer;
}

} // namespace tg

#endif // TASK_GRAPH_OBJECT_POOL_HPP_