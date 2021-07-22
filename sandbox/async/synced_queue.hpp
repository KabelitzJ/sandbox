#ifndef SBX_ASYNC_SYNCED_QUEUE_HPP_
#define SBX_ASYNC_SYNCED_QUEUE_HPP_

#include <queue>
#include <mutex>

namespace sbx {

/**
 * @brief A synchronized wrapper around a std::queue
 * 
 * @tparam T Data type of the queue
 */
template<typename T>
class synced_queue {

public:
  using value_type      = typename std::queue<T>::value_type;
  using reference       = typename std::queue<T>::reference;
  using const_reference = typename std::queue<T>::const_reference;
  using size_type       = typename std::queue<T>::size_type;

  synced_queue()                    = default;
  synced_queue(const synced_queue&) = delete;
  synced_queue(synced_queue&&)      = delete;
  ~synced_queue()                   = default;

  synced_queue& operator= (const synced_queue&) = delete;
  synced_queue& operator= (synced_queue&&)      = delete;

  /**
   * @brief Locks the %synced_queue and checks if it is empty
   * 
   * @return true when the %synced_queue is empty; false otherwise
   */
  [[nodiscard]] bool is_empty() const {
    auto lock = std::lock_guard<std::mutex>{_mutex};
    return _queue.empty();
  }

  /**
   * @brief Locks the %synced_queue and fetches the current size
   * 
   * @return Number of elements in the %synced_queue
   */
  size_type size() const {
    auto lock = std::lock_guard<std::mutex>{_mutex};
    return _queue.size();
  }

  /**
   * @brief Locks the %synced_queue and return a read/write reference to the first element
   * 
   * @return reference First element of the %synced_queue
   */
  reference front() {
    auto lock = std::lock_guard<std::mutex>{_mutex};
    return _queue.front();
  }

  /**
   * @brief Locks the %synced_queue and return a read-only reference to the first element
   * 
   * @return reference First element of the %synced_queue
   */
  const_reference front() const {
    auto lock = std::lock_guard<std::mutex>{_mutex};
    return _queue.front();
  }

  /**
   * @brief Locks the %synced_queue and removes the first element
   */
  void pop() {
    auto lock = std::lock_guard<std::mutex>{_mutex};
    return _queue.pop();
  }

  /**
   * @brief Locks the %synced_queue, creates a new element at the end and copies the value into it 
   * 
   * @param value The value for the new element
   */
  void push(const value_type& value) {
    auto lock = std::lock_guard<std::mutex>{_mutex};
    _queue.push(value);
  }

  /**
   * @brief Locks the %synced_queue, creates a new element at the end and moves the value into it 
   * 
   * @param value The value for the new element
   */
  void push(value_type&& value) {
    auto lock = std::lock_guard<std::mutex>{_mutex};
    _queue.push(value);
  }

  /**
   * @brief Locks the %synced_queue and creates a new element in place at the end
   * 
   * @tparam Args Argument types for constructing a new instance of T
   * @param args List of constructor arguments for T
   * @return Reference to the newly constructed instance
   */
  template<typename... Args>
  reference emplace(Args&&... args) {
    auto lock = std::lock_guard<std::mutex>{_mutex};
    return _queue.emplace(std::forward<Args>(args)...);
  }

private:
  std::queue<value_type> _queue;
  std::mutex             _mutex;

}; // class synced_queue

} // namespace sbx

#endif // SBX_ASYNC_SYNCED_QUEUE_HPP_
