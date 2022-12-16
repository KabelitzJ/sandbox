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

namespace sbx::devices {

window::window(const window_create_info& create_info) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _window = glfwCreateWindow(create_info.width, create_info.height, create_info.title.c_str(), nullptr, nullptr);

  if (!_window) {
    throw std::runtime_error("Failed to create window");
  }

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

void window::_setup_callbacks() {
  glfwSetKeyCallback(_window, []([[maybe_unused]] GLFWwindow* window, int keycode, [[maybe_unused]] int scancode, int action, int mods){
    auto& dispatcher = core::core_module::get().dispatcher();

    if (action == GLFW_PRESS) {
      dispatcher.enqueue<key_pressed_event>(key{keycode}, modifiers{mods});
    } else if (action == GLFW_RELEASE) {
      dispatcher.enqueue<key_released_event>(key{keycode}, modifiers{mods});
    }
  });
}

} // namespace sbx::devices
