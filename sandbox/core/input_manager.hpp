#ifndef SBX_CORE_INPUT_MANAGER_HPP_
#define SBX_CORE_INPUT_MANAGER_HPP_

#include <unordered_map>

#include <glm/vec2.hpp>

#include <evtsys/event_queue.hpp>
#include <evtsys/key_codes.hpp>
#include <evtsys/mouse_buttons.hpp>
#include <evtsys/key_event.hpp>
#include <evtsys/mouse_event.hpp>

namespace sbx {

class input_manager {

public:
  input_manager(event_queue& event_queue);
  ~input_manager();

  bool is_key_pressed(key_code key) const;
  bool is_mouse_button_pressed(mouse_button button) const;
  const glm::vec2& mouse_position() const;

private:
  void _bind_callbacks();

  void _on_key_pressed_event(key_pressed_event& event);
  void _on_key_repeated_event(key_repeated_event& event);
  void _on_key_released_event(key_released_event& event);
  void _on_mouse_moved_event(mouse_moved_event& event);
  void _on_mouse_button_pressed_event(mouse_button_pressed_event& event);
  void _on_mouse_button_released_event(mouse_button_released_event& event);

  event_queue& _event_queue;
  std::unordered_map<key_code, bool> _key_states;
  std::unordered_map<mouse_button, bool> _button_states;
  glm::vec2 _mouse_position;

}; // class input_manager

} // namespace sbx

#endif // SBX_CORE_INPUT_MANAGER_HPP_