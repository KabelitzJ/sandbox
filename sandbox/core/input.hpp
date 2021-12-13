#ifndef SBX_CORE_INPUT_HPP_
#define SBX_CORE_INPUT_HPP_

#include <unordered_map>

#include <types/vector.hpp>

#include "key.hpp"
#include "event_queue.hpp"

namespace sbx {

class input {

public:

  input(event_queue* event_queue);
  ~input();

  vector2 mouse_position() const;

private:

  void _initialize();

  event_queue* _event_queue{};
  bool _is_first_mouse_movement{};
  vector2 _mouse_position{};

}; // class input

} // namespace sbx

#endif // SBX_CORE_INPUT_HPP_
