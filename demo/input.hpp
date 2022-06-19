#ifndef DEMO_INPUT_HPP_
#define DEMO_INPUT_HPP_

#include <unordered_map>
#include <vector>
#include <iostream>

#include <utils/noncopyable.hpp>
#include <utils/nonmovable.hpp>

#include "events.hpp"
#include "event_manager.hpp"
#include "key.hpp"
#include "button.hpp"

namespace demo {

class input : public sbx::noncopyable, public sbx::nonmovable {

  friend class application;

public:

  input(event_manager* event_manager)
  : _event_manager{event_manager},
    _subscriptions{},
    _last_keys{},
    _current_keys{},
    _last_buttons{},
    _current_buttons{} {
    _initialize();
  }

  ~input() {
    for (const auto& subscription : _subscriptions) {
      _event_manager->unsubscribe(subscription);
    }
  }

  [[nodiscard]] bool is_key_pressed(const key& key) const noexcept {
    const auto current = _current_keys.find(key);
    const auto last = _last_keys.find(key);

    const auto has_current = current != _current_keys.cend() ? current->second : false;
    const auto has_last = last != _last_keys.cend() ? last->second : false;

    return has_current && !has_last;
  }

  [[nodiscard]] bool is_key_released(const key& key) const noexcept {
    const auto current = _current_keys.find(key);
    const auto last = _last_keys.find(key);

    const auto has_current = current != _current_keys.cend() ? current->second : false;
    const auto has_last = last != _last_keys.cend() ? last->second : false;

    return !has_current && has_last;
  }

  [[nodiscard]] bool is_button_pressed(const button& button) const noexcept {
    const auto current = _current_buttons.find(button);
    const auto last = _last_buttons.find(button);

    const auto has_current = current != _current_buttons.cend() ? current->second : false;
    const auto has_last = last != _last_buttons.cend() ? last->second : false;

    return has_current && !has_last;
  }

  [[nodiscard]] bool is_button_released(const button& button) const noexcept {
    const auto current = _current_buttons.find(button);
    const auto last = _last_buttons.find(button);

    const auto has_current = current != _current_buttons.cend() ? current->second : false;
    const auto has_last = last != _last_buttons.cend() ? last->second : false;

    return !has_current && has_last;
  }

private:

  void _update() {
    _last_keys = std::move(_current_keys);
    _current_keys.clear();
    _last_buttons = std::move(_current_buttons);
    _current_buttons.clear();
  }

  void _initialize() {
    _subscriptions.emplace_back(_event_manager->subscribe<key_pressed_event>([this](const auto& event) {
      _current_keys[event.key] = true;
    }));

    _subscriptions.emplace_back(_event_manager->subscribe<key_released_event>([this](const auto& event) {
      _current_keys[event.key] = false;
    }));

    _subscriptions.emplace_back(_event_manager->subscribe<button_pressed_event>([this](const auto& event) {
      _current_buttons[event.button] = true;
    }));

    _subscriptions.emplace_back(_event_manager->subscribe<button_released_event>([this](const auto& event) {
      _current_buttons[event.button] = false;
    }));
  }

  event_manager* _event_manager{};

  std::vector<subscription> _subscriptions{};
  std::unordered_map<key, bool> _last_keys{};
  std::unordered_map<key, bool> _current_keys{};
  std::unordered_map<button, bool> _last_buttons{};
  std::unordered_map<button, bool> _current_buttons{};

};

} // namespace demo

#endif // DEMO_INPUT_HPP_
