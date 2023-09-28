#ifndef LIBSBX_DEVICES_WINDOW_HPP_
#define LIBSBX_DEVICES_WINDOW_HPP_

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <functional>
#include <unordered_set>
#include <cmath>

#include <libsbx/core/concepts.hpp>
#include <libsbx/core/delegate.hpp>

#include <libsbx/utility/bitmask.hpp>

#include <libsbx/math/vector2.hpp>

#include <libsbx/signals/signal.hpp>

#include <libsbx/devices/events.hpp>

namespace sbx::devices {

enum class key : std::int32_t {
  unknown = GLFW_KEY_UNKNOWN,
  space = GLFW_KEY_SPACE,
  apostrophe = GLFW_KEY_APOSTROPHE,
  comma = GLFW_KEY_COMMA,
  minus = GLFW_KEY_MINUS,
  period = GLFW_KEY_PERIOD,
  slash = GLFW_KEY_SLASH,
  zero = GLFW_KEY_0,
  one = GLFW_KEY_1,
  two = GLFW_KEY_2,
  three = GLFW_KEY_3,
  four = GLFW_KEY_4,
  five = GLFW_KEY_5,
  six = GLFW_KEY_6,
  seven = GLFW_KEY_7,
  eight = GLFW_KEY_8,
  nine = GLFW_KEY_9,
  semicolon = GLFW_KEY_SEMICOLON,
  equal = GLFW_KEY_EQUAL,
  a = GLFW_KEY_A,
  b = GLFW_KEY_B,
  c = GLFW_KEY_C,
  d = GLFW_KEY_D,
  e = GLFW_KEY_E,
  f = GLFW_KEY_F,
  g = GLFW_KEY_G,
  h = GLFW_KEY_H,
  i = GLFW_KEY_I,
  j = GLFW_KEY_J,
  k = GLFW_KEY_K,
  l = GLFW_KEY_L,
  m = GLFW_KEY_M,
  n = GLFW_KEY_N,
  o = GLFW_KEY_O,
  p = GLFW_KEY_P,
  q = GLFW_KEY_Q,
  r = GLFW_KEY_R,
  s = GLFW_KEY_S,
  t = GLFW_KEY_T,
  u = GLFW_KEY_U,
  v = GLFW_KEY_V,
  w = GLFW_KEY_W,
  x = GLFW_KEY_X,
  y = GLFW_KEY_Y,
  z = GLFW_KEY_Z,
  left_bracket = GLFW_KEY_LEFT_BRACKET,
  backslash = GLFW_KEY_BACKSLASH,
  right_bracket = GLFW_KEY_RIGHT_BRACKET,
  grave_accent = GLFW_KEY_GRAVE_ACCENT,
  world_1 = GLFW_KEY_WORLD_1,
  world_2 = GLFW_KEY_WORLD_2,
  escape = GLFW_KEY_ESCAPE,
  enter = GLFW_KEY_ENTER,
  tab = GLFW_KEY_TAB,
  backspace = GLFW_KEY_BACKSPACE,
  insert = GLFW_KEY_INSERT,
  delete_ = GLFW_KEY_DELETE,
  right = GLFW_KEY_RIGHT,
  left = GLFW_KEY_LEFT,
  down = GLFW_KEY_DOWN,
  up = GLFW_KEY_UP,
  page_up = GLFW_KEY_PAGE_UP,
  page_down = GLFW_KEY_PAGE_DOWN,
  home = GLFW_KEY_HOME,
  end = GLFW_KEY_END,
  caps_lock = GLFW_KEY_CAPS_LOCK,
  scroll_lock = GLFW_KEY_SCROLL_LOCK,
  num_lock = GLFW_KEY_NUM_LOCK,
  print_screen = GLFW_KEY_PRINT_SCREEN,
  pause = GLFW_KEY_PAUSE,
  f1 = GLFW_KEY_F1,
  f2 = GLFW_KEY_F2,
  f3 = GLFW_KEY_F3,
  f4 = GLFW_KEY_F4,
  f5 = GLFW_KEY_F5,
  f6 = GLFW_KEY_F6,
  f7 = GLFW_KEY_F7,
  f8 = GLFW_KEY_F8,
  f9 = GLFW_KEY_F9,
  f10 = GLFW_KEY_F10,
  f11 = GLFW_KEY_F11,
  f12 = GLFW_KEY_F12,
  f13 = GLFW_KEY_F13,
  f14 = GLFW_KEY_F14,
  f15 = GLFW_KEY_F15,
  f16 = GLFW_KEY_F16,
  f17 = GLFW_KEY_F17,
  f18 = GLFW_KEY_F18,
  f19 = GLFW_KEY_F19,
  f20 = GLFW_KEY_F20,
  f21 = GLFW_KEY_F21,
  f22 = GLFW_KEY_F22,
  f23 = GLFW_KEY_F23,
  f24 = GLFW_KEY_F24,
  f25 = GLFW_KEY_F25,
  kp_0 = GLFW_KEY_KP_0,
  kp_1 = GLFW_KEY_KP_1,
  kp_2 = GLFW_KEY_KP_2,
  kp_3 = GLFW_KEY_KP_3,
  kp_4 = GLFW_KEY_KP_4,
  kp_5 = GLFW_KEY_KP_5,
  kp_6 = GLFW_KEY_KP_6,
  kp_7 = GLFW_KEY_KP_7,
  kp_8 = GLFW_KEY_KP_8,
  kp_9 = GLFW_KEY_KP_9,
  kp_decimal = GLFW_KEY_KP_DECIMAL,
  kp_divide = GLFW_KEY_KP_DIVIDE,
  kp_multiply = GLFW_KEY_KP_MULTIPLY,
  kp_subtract = GLFW_KEY_KP_SUBTRACT,
  kp_add = GLFW_KEY_KP_ADD,
  kp_enter = GLFW_KEY_KP_ENTER,
  kp_equal = GLFW_KEY_KP_EQUAL,
  left_shift = GLFW_KEY_LEFT_SHIFT,
  left_control = GLFW_KEY_LEFT_CONTROL,
  left_alt = GLFW_KEY_LEFT_ALT,
  left_super = GLFW_KEY_LEFT_SUPER,
  right_shift = GLFW_KEY_RIGHT_SHIFT,
  right_control = GLFW_KEY_RIGHT_CONTROL,
  right_alt = GLFW_KEY_RIGHT_ALT,
  right_super = GLFW_KEY_RIGHT_SUPER,
  menu = GLFW_KEY_MENU,
}; // enum class key

enum class mouse_button : std::int32_t {
  one = GLFW_MOUSE_BUTTON_1,
  two = GLFW_MOUSE_BUTTON_2,
  three = GLFW_MOUSE_BUTTON_3,
  four = GLFW_MOUSE_BUTTON_4,
  five = GLFW_MOUSE_BUTTON_5,
  six = GLFW_MOUSE_BUTTON_6,
  seven = GLFW_MOUSE_BUTTON_7,
  eight = GLFW_MOUSE_BUTTON_8,
  left = GLFW_MOUSE_BUTTON_LEFT,
  right = GLFW_MOUSE_BUTTON_RIGHT,
  middle = GLFW_MOUSE_BUTTON_MIDDLE,
}; // enum class mouse_button

enum class input_action : std::int32_t {
  release = GLFW_RELEASE,
  press = GLFW_PRESS,
  repeat = GLFW_REPEAT,
}; // enum class input_action

enum class input_mod : std::int32_t {
  shift = GLFW_MOD_SHIFT,
  control = GLFW_MOD_CONTROL,
  alt = GLFW_MOD_ALT,
  super = GLFW_MOD_SUPER,
  caps_lock = GLFW_MOD_CAPS_LOCK,
  num_lock = GLFW_MOD_NUM_LOCK,
}; // enum class input_mod

struct window_create_info {
  std::string title{};
  std::uint32_t width{};
  std::uint32_t height{};
}; // struct window_create_info

class window {

public:

  window(const window_create_info& create_info)
  : _last_mouse_position{-1.0f, -1.0f} {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // [NOTE] KAJ 2023-08-15 : Currently there seems to be a bug in the Vukan SDK version 1.3.250.1 that causes the validation layers to crash when resizing the window.
    // glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_VISIBLE, false);

    _title = create_info.title;
    _width = create_info.width;
    _height = create_info.height;

    _handle = glfwCreateWindow(static_cast<std::int32_t>(_width), static_cast<std::int32_t>(_height), _title.c_str(), nullptr, nullptr);

    if (!_handle) {
      throw std::runtime_error{"Could not create glfw window"};
    }

    glfwSetWindowUserPointer(_handle, this);

    glfwFocusWindow(_handle);

    if (glfwRawMouseMotionSupported()) {
      glfwSetInputMode(_handle, GLFW_RAW_MOUSE_MOTION, true);
    }

    // glfwSetInputMode(_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    _set_callbacks();
  }

  ~window() {
    glfwDestroyWindow(_handle);
  }

  auto handle() -> GLFWwindow* {
    return _handle;
  }

  operator GLFWwindow*() {
    return _handle;
  }

  auto title() const -> const std::string& {
    return _title;
  }

  auto set_title(const std::string& title) -> void {
    _title = title;
    glfwSetWindowTitle(_handle, _title.c_str());
  }

  auto width() const -> std::uint32_t {
    return _width;
  }

  auto height() const -> std::uint32_t {
    return _height;
  }

  auto aspect_ratio() const -> std::float_t {
    return static_cast<std::float_t>(_width) / static_cast<std::float_t>(_height);
  }

  /**
   * @brief Determins if the window should be closed
   * @return true if the window should be closed, false otherwise
   */
  auto should_close() -> bool {
    return glfwWindowShouldClose(_handle);
  }

  /**
   * @brief Makes the window visible
   */
  auto show() -> void {
    glfwShowWindow(_handle);
  }

  /**
   * @brief Hides the window
   */
  auto hide() -> void {
    glfwHideWindow(_handle);
  }

  auto is_iconified() const noexcept -> bool {
    return glfwGetWindowAttrib(_handle, GLFW_ICONIFIED);
  }

  auto is_focused() const noexcept -> bool {
    return glfwGetWindowAttrib(_handle, GLFW_FOCUSED);
  }

  auto is_visible() const noexcept -> bool {
    return glfwGetWindowAttrib(_handle, GLFW_VISIBLE);
  }

  auto on_window_closed_signal() -> signals::signal<window_closed_event>& {
    return _on_window_closed_signal;
  }

  auto on_window_moved_signal() -> signals::signal<window_moved_event>& {
    return _on_window_moved_signal;
  }

  auto on_window_resized_signal() -> signals::signal<window_resized_event>& {
    return _on_window_resized_signal;
  }

  auto on_framebuffer_resized() -> signals::signal<framebuffer_resized_event>& {
    return _on_framebuffer_resized;
  }

  auto on_key_pressed() -> signals::signal<key_pressed_event>& {
    return _on_key_pressed;
  }

  auto on_key_released() -> signals::signal<key_released_event>& {
    return _on_key_released;
  }

  auto on_mouse_moved() -> signals::signal<mouse_moved_event>& {
    return _on_mouse_moved;
  }

private:

  void _set_callbacks() {
    glfwSetWindowUserPointer(_handle, this);

    glfwSetWindowCloseCallback(_handle, [](GLFWwindow* window){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      self._on_window_closed_signal(window_closed_event{});
    });

    glfwSetWindowPosCallback(_handle, [](GLFWwindow* window, std::int32_t x, std::int32_t y){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      self._on_window_moved_signal(window_moved_event{x, y});
    });

    glfwSetWindowSizeCallback(_handle, [](GLFWwindow* window, std::int32_t width, std::int32_t height){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      self._on_window_resized_signal(window_resized_event{width, height});
    });

    glfwSetFramebufferSizeCallback(_handle, [](GLFWwindow* window, std::int32_t width, std::int32_t height){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      self._on_framebuffer_resized(framebuffer_resized_event{width, height});

      self._width = static_cast<std::uint32_t>(width);
      self._height = static_cast<std::uint32_t>(height);
    });

    glfwSetKeyCallback(_handle, [](GLFWwindow* window, std::int32_t key, std::int32_t scancode, std::int32_t action, std::int32_t mods){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      if (action == GLFW_PRESS) {
        self._on_key_pressed(key_pressed_event{key, scancode, action, mods});
      } else if (action == GLFW_RELEASE) {
        self._on_key_released(key_released_event{key, scancode, action, mods});
      }
    });

    glfwSetCursorPosCallback(_handle, [](auto* window, auto x, auto y){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      auto mouse_position = math::vector2{static_cast<std::float_t>(x), static_cast<std::float_t>(y)};

      if (self._last_mouse_position.x < 0.0f || self._last_mouse_position.y < 0.0f) {
        self._on_mouse_moved(mouse_moved_event{mouse_position.x, mouse_position.y});
      } else {
        self._on_mouse_moved(mouse_moved_event{mouse_position.x - self._last_mouse_position.x, mouse_position.y - self._last_mouse_position.y});
        self._last_mouse_position = mouse_position;
      }
    });
  }

  std::string _title{};
  std::uint32_t _width{};
  std::uint32_t _height{};

  GLFWwindow* _handle{};

  math::vector2 _last_mouse_position;

  signals::signal<window_closed_event> _on_window_closed_signal;
  signals::signal<window_moved_event> _on_window_moved_signal;
  signals::signal<window_resized_event> _on_window_resized_signal;
  signals::signal<framebuffer_resized_event> _on_framebuffer_resized;
  signals::signal<key_pressed_event> _on_key_pressed;
  signals::signal<key_released_event> _on_key_released;
  signals::signal<mouse_moved_event> _on_mouse_moved;

}; // class window

} // namespace sbx::devices

template<>
struct sbx::utility::enable_bitmask_operators<sbx::devices::input_action> : std::true_type { };

template<>
struct sbx::utility::enable_bitmask_operators<sbx::devices::input_mod> : std::true_type { };

#endif // LIBSBX_DEVICES_WINDOW_HPP_
