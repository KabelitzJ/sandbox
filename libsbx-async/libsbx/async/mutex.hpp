#ifndef LIBSBX_ASYNC_MUTEX_HPP_
#define LIBSBX_ASYNC_MUTEX_HPP_

#include <concepts>
#include <mutex>

namespace sbx::async {

template<typename Type>
concept lockable = requires(Type type) {
  { type.lock() } -> std::same_as<void>;
  { type.unlock() } -> std::same_as<void>;
  { type.try_lock() } -> std::same_as<bool>;
}; // concept lockable

class null_mutex {

public:

  null_mutex() = default;

  ~null_mutex() = default;

  void lock() noexcept {}

  void unlock() noexcept {}

  bool try_lock() noexcept { return true; }

}; // class null_mutex

} // namespace sbx::async

#endif // LIBSBX_ASYNC_MUTEX_HPP_
