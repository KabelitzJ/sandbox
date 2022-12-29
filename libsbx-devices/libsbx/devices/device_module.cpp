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
 * @file libsbx/devices/devices_module.cpp
 */

#include <libsbx/devices/device_module.hpp>

#include <cinttypes>

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <libsbx/core/logger.hpp>

namespace sbx::devices {

device_module::device_module() {
  glfwSetErrorCallback([](std::int32_t error_code, const char* description){
    sbx::core::logger::error("({}) {}", error_code, description);
  });

  if (!glfwInit()) {
    throw std::runtime_error{"Failed to initialize GLFW"};
  }

  if (!glfwVulkanSupported()) {
    throw std::runtime_error{"Vulkan is not supported"};
  }

  _monitor = std::make_unique<devices::monitor>();
  _window = std::make_unique<devices::window>(window_create_info{"Window", 960, 720});
}

device_module::~device_module() {
  _window.reset();
  _monitor.reset();

  glfwTerminate();
}

void device_module::update([[maybe_unused]] const core::time& delta_time) {
  glfwPollEvents();
}

std::vector<const char*> device_module::required_instance_extensions() const {
  auto extention_count = std::uint32_t{};

  const auto* glfw_extensions = glfwGetRequiredInstanceExtensions(&extention_count);

  auto extensions = std::vector<const char*>{glfw_extensions, glfw_extensions + extention_count};

  return extensions;
}

monitor& device_module::monitor() {
  return *_monitor;
}

window& device_module::window() {
  return *_window;
}

} // namespace sbx::devices
