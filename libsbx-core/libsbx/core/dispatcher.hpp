/* 
 * Copyright (c) 2022 Jonas Kabelitz
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * You should have received a copy of the MIT License along with this program.
 * If not, see <https://opensource.org/licenses/MIT/>.
 */

/**
 * @file libsbx/core/dispatcher.hpp
 */

#ifndef LIBSBX_CORE_DISPATCHER_HPP_
#define LIBSBX_CORE_DISPATCHER_HPP_

/**
 * @ingroup libsbx-core
 */

#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <libsbx/core/signal.hpp>
#include <libsbx/core/slot.hpp>

namespace sbx::core {

namespace detail {

struct dispatcher_handler_base {
  virtual ~dispatcher_handler_base() = default;
  virtual void emit_all() = 0;
  virtual void clear_events() = 0;
  virtual void clear_connections() = 0;
  virtual void clear() = 0;
}; // struct dispatcher_handler_base

template<typename Type>
class dispatcher_handler : public dispatcher_handler_base {

public:

  dispatcher_handler() = default;
  
  ~dispatcher_handler() override = default;

  void connect(slot<Type>& slot) {
    _signal.connect(slot);
  }

  void disconnect(slot<Type>& slot) {
    _signal.disconnect(slot);
  }

  template<typename... Args>
  void enqueue(Args&&... args) {
    if constexpr (std::is_aggregate_v<Type>) {
      _events.push_back(Type{std::forward<Args>(args)...});
    } else {
      _events.emplace_back(std::forward<Args>(args)...);
    }
  }

  void emit_all() override {
    if (_signal.is_empty()) {
      // We dont have any listeners anyway.
      _events.clear();
      return;
    }

    for (const auto& event: _events) {
      _signal.emit(event);
    }

    _events.clear();
  }

  void clear_events() {
    _events.clear();
  }

  void clear_connections() {
    _signal.clear();
  }

  void clear() {
    clear_events();
    clear_connections();
  }

private:

  signal<Type> _signal{};
  std::vector<Type> _events{};

}; // class dispatcher_handler

} // namespace detail

class dispatcher {

  template<typename Type>
  using handler_type = detail::dispatcher_handler<Type>;

public:

  template<typename Type, typename... Args>
  void enqueue(Args&&... args) {
    _assure<Type>().enqueue(std::forward<Args>(args)...);
  }

  template<typename Type>
  void connect(slot<Type>& slot) {
    _assure<Type>().connect(slot);
  }

  template<typename Type>
  void disconnect(slot<Type>& slot) {
    _assure<Type>().disconnect(slot);
  }

  void emit_all() {
    for (auto& [type, handler] : _handlers) {
      handler->emit_all();
    }
  }

private:

  template<typename Type>
  handler_type<Type>& _assure() {
    const auto type = std::type_index{typeid(Type)};

    auto entry = _handlers.find(type);

    if (entry == _handlers.cend()) {
      entry = _handlers.insert({type, std::make_unique<handler_type<Type>>()}).first;
    }

    return *static_cast<handler_type<Type>*>(entry->second.get());
  }

  std::unordered_map<std::type_index, std::unique_ptr<detail::dispatcher_handler_base>> _handlers{};

}; // class dispatcher

template<typename Event>
struct basic_listener {
  
  template<typename Callable>
  basic_listener(Callable&& callable) : _slot{std::forward<Callable>(callable)} { }

  sbx::core::slot<Event>& slot() {
    return _slot;
  }

private:

  sbx::core::slot<Event> _slot{};

}; // struct basic_listener

} // namespace sbx::core

#endif // LIBSBX_CORE_DISPATCHER_HPP_
