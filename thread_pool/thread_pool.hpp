#ifndef THREAD_POOL_HPP_
#define THREAD_POOL_HPP_

/**
 * @file thread_pool.hpp
 *
 * @brief Defines the class tp::thread_pool
 *
 * @author Jonas Kabelitz <https://www.github.com/Kabelitz/>
 * @date 09.04.2021
 */

#include <condition_variable>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace tp {

/**
 * @class thread_pool
 * 
 * @brief Holds a vector of worker threads that are trying to fetch and execut tasks from a task queue
 */
class thread_pool {
  
public:
  /**
   * @brief Constructs a new thread_pool object
   * 
   * @param worker_count The amount of worker threads (default is the value of 'std::thread::hardware_concurrency()')
   */
  explicit thread_pool(std::size_t worker_count = std::thread::hardware_concurrency());

  /**
   * @brief Construct a new thread pool object
   * 
   */
  thread_pool(const thread_pool&) = delete;

  /**
   * @brief Construct a new thread pool object
   * 
   */
  thread_pool(thread_pool&&) = delete;

  /**
   * @brief Destroys the thread_pool object
   * 
   */
  ~thread_pool();

  /** 
   * @brief 
   * 
   * @return thread_pool& 
   */
  thread_pool& operator=(const thread_pool&) = delete;

  /**
   * @brief 
   * 
   * @return thread_pool& 
   */
  thread_pool& operator=(thread_pool&&) = delete;

  /**
   * @brief Places a task into the task queue to be executed by the next available worker thread
   * 
   * @tparam Function Type of the Callable e.g. function, functor or method
   * @tparam Args List of the parameter types for the function
   * @param function Callable to be executed
   * @param args Parameters that are passed to the function
   * @return std::future<std::result_of_t<Function(Args...)>> Result of the function call
   */
  template<typename Function, typename... Args>
  [[nodiscard]] std::future<std::result_of_t<Function(Args...)>> enqueue(Function&& function, Args&&... args);
  
private:
  using task = std::function<void()>;

  std::vector<std::thread> _workers;
  std::queue<task> _task_queue;
  std::mutex _queue_mutex;
  std::condition_variable _condition;
  bool _is_running;

};

inline thread_pool::thread_pool(std::size_t worker_count) : _is_running(true) {
  _workers.reserve(worker_count);
  for (std::size_t i(0); i < worker_count; ++i) {
    _workers.emplace_back([this]() {
        while (true) {
          task current_task;

          {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            _condition.wait(lock, [this](){ return !_is_running || !_task_queue.empty(); });

            // make sure all task have been handled before exiting
            if (!_is_running && _task_queue.empty()) {
              return;
            }

            current_task = std::move(_task_queue.front());
            _task_queue.pop();
          }

          current_task();
        }
      }
    );
  }
}

template<typename Function, typename... Args>
std::future<std::result_of_t<Function(Args...)>> thread_pool::enqueue(Function&& function, Args&&... args) {
  using return_type = std::result_of_t<Function(Args...)>;

  auto task = std::make_shared<std::packaged_task<return_type()>>(
    std::bind(std::forward<Function>(function), std::forward<Args>(args)...)
  );
      
  std::future<return_type> task_result = task->get_future();
  {
    std::unique_lock<std::mutex> lock(_queue_mutex);

    if (!_is_running) {
      throw std::runtime_error("thread_pool::enqueue called on stopped thread_pool");
    }

    _task_queue.emplace([task](){ (*task)(); });
  }

  _condition.notify_one();

  return task_result;
}

inline thread_pool::~thread_pool() {
  {
    std::unique_lock<std::mutex> lock(_queue_mutex);
    _is_running = false;
  }

  _condition.notify_all();

  for (auto& worker: _workers) {
    worker.join();
  }
}

} // namespace tp

#endif // THREAD_POOL_HPP_
