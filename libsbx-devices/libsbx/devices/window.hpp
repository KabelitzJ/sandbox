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
 * @file libsbx/devices/window.hpp 
 */

#ifndef LIBSBX_DEVICES_WINDOW_HPP_
#define LIBSBX_DEVICES_WINDOW_HPP_

/**
 * @ingroup libsbx-devices
 */

#include <string>
#include <cinttypes>

#include <GLFW/glfw3.h>

#include <libsbx/devices/events.hpp>
#include <libsbx/devices/key.hpp>
#include <libsbx/devices/modifiers.hpp>

#include <libsbx/core/signal.hpp>
#include <libsbx/core/slot.hpp>

namespace sbx::devices {

struct window_create_info {
  std::string title{};
  std::uint32_t width{};
  std::uint32_t height{};
};

class window {

public:

  window(const window_create_info& create_info);

  ~window();

  GLFWwindow* handle() const noexcept;

  operator GLFWwindow*() const noexcept;

  bool should_close() const;

  void show();
  
  void hide();

  void close();

  void set_title(const std::string& title);

  void register_on_key_pressed(const core::slot<key_pressed_event>& listener);

  void register_on_key_released(const core::slot<key_released_event>& listener);

private:

  void _setup_callbacks();

  GLFWwindow* _handle{};
  core::signal<key_pressed_event> _on_key_pressed{};
  core::signal<key_released_event> _on_key_released{};

}; // class window

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_WINDOW_HPP_
