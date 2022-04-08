#ifndef SBX_ASYNC_THREAD_POOL_HPP_
#define SBX_ASYNC_THREAD_POOL_HPP_

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace sbx {

class thread_pool {

public:

  using size_type = std::size_t;

  thread_pool(const size_type thread_count = std::thread::hardware_concurrency())
  : _is_running{true},
    _condition{},
    _mutex{},
    _tasks{},
    _threads{} {
    _initialite(thread_count);
  }

  thread_pool(const thread_pool&) = delete;

  thread_pool(thread_pool&&) = default;

  ~thread_pool() {
    _terminate();
  }

  thread_pool& operator=(const thread_pool&) = delete;

  thread_pool& operator=(thread_pool&&) = default;

  template<typename Function, typename... Args>
  requires (std::is_invocable_v<Function, Args...>)
  std::future<std::invoke_result_t<Function, Args...>> enqueue(Function&& function, Args&&... args) {
    using return_type = std::invoke_result_t<Function, Args...>;

    auto promise = std::make_shared<std::promise<return_type>>();
    auto future = promise->get_future();

    {
      auto lock = std::scoped_lock{_mutex};

      _tasks.emplace([promise, function = std::forward<Function>(function), ...args = std::forward<Args>(args)](){
        try {
          if constexpr (std::is_void_v<return_type>) {
            std::invoke(function, std::forward<Args>(args)...);
            promise->set_value();
          } else {
            promise->set_value(std::invoke(function, std::forward<Args>(args)...));
          }
        } catch (...) {
          try {
            promise->set_exception(std::current_exception());
          } catch (...) { 
            // [NOTE] KAJ 2022-04-07 20:48 - Something went really really wrong...
          }
        }
      });
    }

    _condition.notify_one();

    return future;
  }

private:

  void _initialite(const size_type thread_count) {
    _threads.reserve(thread_count);

    for (auto i = size_type{0}; i < thread_count; ++i) {
      _threads.emplace_back([this](){ _run(); });
    }
  }

  void _terminate() {
    _is_running = false;

    _condition.notify_all();

    for (auto& thread : _threads) {
      thread.join();
    }
  }

  void _run() {
    while (true) {
      auto lock = std::unique_lock{_mutex};

      _condition.wait(lock, [this](){ return !_tasks.empty() || !_is_running; });

      if (!_is_running) {
        break;
      }

      auto task = std::move(_tasks.front());
      _tasks.pop();

      lock.unlock();

      std::invoke(task);
    }
  }

  std::atomic_bool _is_running{};
  std::condition_variable _condition{};
  std::mutex _mutex{};
  std::queue<std::function<void()>> _tasks{};
  std::vector<std::thread> _threads{};

}; // class thread_pool

} // namespace sbx

#endif // SBX_ASYNC_THREAD_POOL_HPP_
