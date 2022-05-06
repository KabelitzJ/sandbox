#ifndef DEMO_SUBSCRIPTION_HPP_
#define DEMO_SUBSCRIPTION_HPP_

#include <types/primitives.hpp>

namespace demo {

class subscription {

  friend class event_manager;
  
  template<typename Callback>
  friend class callback_container;

public:

  subscription()
  : _is_valid{false},
    _sparse_index{0},
    _dense_index{0},
    _version{0},
    _event_id{0} { }

  ~subscription() = default;

private:

  subscription(const sbx::uint32 sparse_index, const sbx::uint32 dense_index, const sbx::uint32 version, const sbx::uint32 event_id)
  : _is_valid{true},
    _sparse_index{sparse_index},
    _dense_index{dense_index},
    _version{version},
    _event_id{event_id} { }

  bool _is_valid{};

  sbx::uint32 _sparse_index{};
  sbx::uint32 _dense_index{};
  sbx::uint32 _version{};
  sbx::uint32 _event_id{};

}; // class subscription

} // namespace demo

#endif // DEMO_SUBSCRIPTION_HPP_
