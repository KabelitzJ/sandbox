#ifndef SANDBOX_OBJECT_POOL_HPP_
#define SANDBOX_OBJECT_POOL_HPP_

#include <memory>
#include <functional>
#include <deque>
#include <algorithm>

namespace sbx {

/**
 * @class object_pool 
 *
 * @brief Caches objects to avoid frequent allocations and deallocations
 * 
 * @tparam T Type of objects this pool holds
 */
template<typename T>
class object_pool {

public:
  using pointer = std::unique_ptr<T, std::function<void(T*)>>;

  /**
   * @brief Construct a new object pool object
   * 
   * @param initial_size Amount of instances that should be allocated on creation
   */
  explicit object_pool(std::size_t initial_size = 1048u);

  /**
   * @brief Construct a new object pool object
   * 
   */
  object_pool(const object_pool&) = delete;

  /**
   * @brief Construct a new object pool object
   * 
   */
  object_pool(object_pool&&) = delete;

  /**
   * @brief Destroy the object pool object
   * 
   */
  ~object_pool();

  /**
   * @brief 
   * 
   * @return object_pool& 
   */
  object_pool& operator=(const object_pool&) = delete;

  /**
   * @brief 
   * 
   * @return object_pool& 
   */
  object_pool& operator=(object_pool&&) = delete;

  /**
   * @brief Provides a std::unique_ptr to a instance of T with a custom 
   * deleter that puts the memory back in the pool on destruction
   * 
   * @tparam Args 
   * 
   * @param args Arguments that are needed to create an instance of @c T
   * @return std::unique_ptr<T, std::function<void(T*)>> 
   */
  template<typename... Args>
  pointer request(Args&&... args);

private:
  std::deque<T*> _object_pool; // is deque the best container?

};

template<typename T>
object_pool<T>::object_pool(std::size_t initial_size) : _object_pool() {
  for (std::size_t i = 0; i < initial_size; ++i) {
    T* object = static_cast<T*>(::operator new(sizeof(T)));
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
typename object_pool<T>::pointer object_pool<T>::request(Args&&... args) {
  T* object = nullptr;

  if (!_object_pool.empty()) {
    T* pool_object = _object_pool.front();
    _object_pool.pop_front();

    object = new(pool_object) T(std::forward<Args>(args)...);
  } else {
    object = new T(std::forward<Args>(args)...);
  }

  auto object_pointer = std::unique_ptr<T, std::function<void(T*)>>(object, [this](T* object){ 
    object->~T();
    _object_pool.push_back(object);
  });

  return object_pointer;
}

} // namespace sbx

#endif // SANDBOX_OBJECT_POOL_HPP_