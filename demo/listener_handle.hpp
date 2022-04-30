#ifndef DEMO_EVT_LISTENER_HANDLE_HPP_
#define DEMO_EVT_LISTENER_HANDLE_HPP_

#include <types/primitives.hpp>

namespace demo {

class listener_handle {

  friend class event_manager;
  template<typename Callback>
  friend class callback_container;

public:

  ~listener_handle() = default;

private:

  listener_handle(const sbx::uint32 sparse_index, const sbx::uint32 dense_index, const sbx::uint32 version, const sbx::uint32 event_id)
  : _sparse_index{sparse_index},
    _dense_index{dense_index},
    _version{version},
    _event_id{event_id} { }

  sbx::uint32 _sparse_index{};
  sbx::uint32 _dense_index{};
  sbx::uint32 _version{};
  sbx::uint32 _event_id{};

}; // class listener_handle

} // namespace demo

#endif // DEMO_EVT_LISTENER_HANDLE_HPP_
