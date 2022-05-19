#ifndef DEMO_CALLBACK_CONTAINER_HPP_
#define DEMO_CALLBACK_CONTAINER_HPP_

#include <vector>
#include <queue>
#include <functional>
#include <type_traits>
#include <typeindex>

#include <types/primitives.hpp>

#include <utils/type_name.hpp>

#include "subscription.hpp"

#include "logger.hpp"

namespace demo {

struct callback_container_base {

  virtual ~callback_container_base() = default;

  virtual void remove(const subscription& handle) = 0;

}; // struct callback_container_base

template<typename Type>
class callback_container : public callback_container_base {

  friend class event_manager;

public:

  using value_type = Type;
  using callback_type = std::function<void(const value_type&)>;

  callback_container(logger* logger, const sbx::uint32 event_id)
  : _logger{logger},
    _event_id{event_id},
    _callbacks{},
    _sparse{},
    _dense{},
    _free_indices{} { }

  callback_container(const callback_container&) = delete;

  callback_container(callback_container&&) = delete;

  ~callback_container() {
    if (!_callbacks.empty()) {
      const auto type_name = sbx::type_name<Type>::value();
      _logger->warn("{} callback(s) still registered for event type '{}'.", _callbacks.size(), type_name);
    }
  }

  callback_container& operator=(const callback_container&) = delete;

  callback_container& operator=(callback_container&&) = delete;

  template<typename Callback>
  requires (std::is_invocable_r_v<void, Callback, const value_type&>)
  subscription add(Callback&& callback) {
    auto sparse_index = sbx::uint32{0};

    if (_free_indices.empty()) {
      sparse_index = static_cast<sbx::uint32>(_callbacks.size());
      _sparse.push_back(subscription{sparse_index, sparse_index, sbx::uint32{0}, _event_id});
    } else {
      sparse_index = _free_indices.front();
      _free_indices.pop();
    }

    _dense.push_back(sparse_index);
    _callbacks.emplace_back(std::forward<Callback>(callback));

    return _sparse[sparse_index];
  }

  void remove(const subscription& handle) override {
    if (handle._version != _sparse[handle._sparse_index]._version) {
      _logger->warn("Attempted to remove a listener that has already been removed.");
      return;
    }

    _sparse[handle._sparse_index]._version++;

    const auto remove_index = _sparse[handle._sparse_index]._dense_index;
    const auto last_index = _callbacks.size() - sbx::uint32{1};

    _dense[remove_index] = _dense[last_index];
    _sparse[_dense[remove_index]]._dense_index = remove_index;

    std::swap(_callbacks[remove_index], _callbacks[last_index]);

    _free_indices.push(handle._sparse_index);
    _callbacks.pop_back();
    _dense.pop_back();
  }

private:

  logger* _logger{};

  sbx::uint32 _event_id{};

  std::vector<callback_type> _callbacks{};

  std::vector<subscription> _sparse{};
  std::vector<sbx::uint32> _dense{};
  std::queue<sbx::uint32> _free_indices{};
  
}; // class callback_container

} // namespace demo

#endif // DEMO_CALLBACK_CONTAINER_HPP_
