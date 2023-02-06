#ifndef LIBSBX_ASYNC_LOADER_HPP_
#define LIBSBX_ASYNC_LOADER_HPP_

#include <thread>
#include <future>
#include <functional>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <ranges>

#include <libsbx/utility/noncopyable.hpp>

namespace sbx::async {

class loader : public utility::noncopyable {

public:

  loader()
  : _is_running{true},
    _thread{[this](){ _run(); }} { }

  ~loader() {
    _is_running = false;

    _condition.notify_all();

    _thread.join();
  }

  template<typename Return, typename Callable, typename... Args>
  requires (std::is_invocable_r_v<Return, Callable, Args...>)
  [[nodiscard]] auto enqueue(Callable&& callable, Args&&... args) -> std::future<Return> {
    auto promise = std::make_shared<std::promise<Return>>();
    auto future = promise->get_future();

    {
      auto lock = std::scoped_lock{_mutex};

      _tasks.emplace([promise, callable = std::forward<Callable>(callable), ...args = std::forward<Args>(args)](){
        try {
          if constexpr (std::is_void_v<Return>) {
            std::invoke(callable, std::forward<Args>(args)...);
            promise->set_value();
          } else {
            promise->set_value(std::invoke(callable, std::forward<Args>(args)...));
          }
        } catch (...) {
          promise->set_exception(std::current_exception());
        }
      });
    }

    _condition.notify_one();

    return future;
  }

private:

  auto _run() -> void {
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

  bool _is_running{};
  std::mutex _mutex{};
  std::condition_variable _condition{};
  std::thread _thread{};
  std::queue<std::function<void()>> _tasks{};

}; // class loader

} // namespace sbx::async

#endif // LIBSBX_ASYNC_LOADER_HPP_
