#ifndef SBX_CORE_INPUT_HPP_
#define SBX_CORE_INPUT_HPP_

#include <unordered_map>

#include <types/vector.hpp>

#include "key.hpp"
#include "mouse_button.hpp"
#include "event_queue.hpp"

namespace sbx {

class input {

public:

  input(event_queue* event_queue);
  ~input();

  vector2 mouse_offset() const;

  vector2 mouse_position() const;

  bool is_key_down(const key key) const;
  bool is_key_up(const key key) const;

  bool is_mouse_button_down(const mouse_button button) const;
  bool is_mouse_button_up(const mouse_button button) const;

private:

  void _initialize();

  event_queue* _event_queue{};
  bool _is_first_mouse_movement{};
  vector2 _mouse_offset{};
  vector2 _mouse_position{};
  std::unordered_map<key, bool> _key_states{};
  std::unordered_map<mouse_button, bool> _mouse_button_states{};
  
  
}; // class input

} // namespace sbx

#endif // SBX_CORE_INPUT_HPP_
