#ifndef THREAD_POOL_HPP_
#define THREAD_POOL_HPP_

#include <condition_variable>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class thread_pool {
public:
  thread_pool(std::size_t worker_count = std::thread::hardware_concurrency());
  ~thread_pool();

  template<class Function, class... Args>
  [[nodiscard]] std::future<std::result_of_t<Function(Args...)>> enqueue(Function&& function, Args&&... args);
  
private:
  std::vector<std::thread> _workers;
  std::queue<std::function<void()>> _tasks;
  std::mutex _queue_mutex;
  std::condition_variable _condition;
  bool _is_running;
};

inline thread_pool::thread_pool(std::size_t worker_count) : _is_running(true) {
  _workers.reserve(worker_count);
  for(std::size_t i(0); i < worker_count; ++i) {
    _workers.emplace_back([this]() {
        while (true) {
          std::function<void()> task;

          {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            _condition.wait(lock, [this](){ return !_is_running || !_tasks.empty(); });

            if(!_is_running && _tasks.empty()) {
              return;
            }

            task = std::move(_tasks.front());
            _tasks.pop();
          }

          task();
        }
      }
    );
  }
}

template<class Function, class... Args>
std::future<std::result_of_t<Function(Args...)>> thread_pool::enqueue(Function&& function, Args&&... args) {
  using return_type = std::result_of_t<Function(Args...)>;

  auto task = std::make_shared<std::packaged_task<return_type()> >(
    std::bind(std::forward<Function>(function), std::forward<Args>(args)...)
  );
      
  std::future<return_type> task_result = task->get_future();
  {
    std::unique_lock<std::mutex> lock(_queue_mutex);

    if(!_is_running) {
      throw std::runtime_error("thread_pool::enqueue called on stopped thread_pool");
    }

    _tasks.emplace([task](){ (*task)(); });
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

  for(std::thread& worker: _workers) {
    worker.join();
  }
}

#endif // THREAD_POOL_HPP_
