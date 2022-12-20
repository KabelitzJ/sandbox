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
 * @file libsbx/devices/window.cpp 
 */

#include <libsbx/devices/window.hpp>

#include <stdexcept>

#include <libsbx/core/logger.hpp>

namespace sbx::devices {

window::window(const window_create_info& create_info) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _window = glfwCreateWindow(create_info.width, create_info.height, create_info.title.c_str(), nullptr, nullptr);

  if (!_window) {
    throw std::runtime_error("Failed to create window");
  }

  glfwSetWindowUserPointer(_window, this);

  _setup_callbacks();
}

window::~window() {
  glfwDestroyWindow(_window);
}

bool window::should_close() const {
  return glfwWindowShouldClose(_window);
}

void window::close() {
  glfwSetWindowShouldClose(_window, true);
}

void window::set_title(const std::string& title) {
  glfwSetWindowTitle(_window, title.c_str());
}

void window::register_on_key_pressed(const core::slot<key_pressed_event>& listener) {
  _on_key_pressed.connect(listener);
}

void window::register_on_key_released(const core::slot<key_released_event>& listener) {
  _on_key_released.connect(listener);
}

void window::_setup_callbacks() {
  glfwSetKeyCallback(_window, [](GLFWwindow* window, std::int32_t keycode, [[maybe_unused]] std::int32_t scancode, std::int32_t action, std::int32_t mods){
    auto* handle = static_cast<sbx::devices::window*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS) {
      handle->_on_key_pressed.emit(sbx::devices::key_pressed_event{key{keycode, scancode}, modifiers{mods}});
    } else if (action == GLFW_RELEASE) {
      handle->_on_key_released.emit(sbx::devices::key_released_event{key{keycode, scancode}, modifiers{mods}});
    }
  });
}

} // namespace sbx::devices
