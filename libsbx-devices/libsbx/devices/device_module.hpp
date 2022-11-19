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
 * @file libsbx/devices/devices_module.hpp 
 */

#ifndef LIBSBX_DEVICES_DEVICE_MODULE_HPP_
#define LIBSBX_DEVICES_DEVICE_MODULE_HPP_

/**
 * @ingroup libsbx-devices
 */

#include <memory>
#include <cinttypes>

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <libsbx/core/module.hpp>

#include <libsbx/devices/monitor.hpp>
#include <libsbx/devices/window.hpp>

namespace sbx::devices {

class device_module : public core::module<device_module> {

  inline static const auto registered = register_module(stage::normal);

public:

  device_module() {
    glfwSetErrorCallback([](std::int32_t error_code, const char* description){
      sbx::core::logger::error("({}) {}", error_code, description);
    });

    if (!glfwInit()) {
      throw std::runtime_error{"Failed to initialize GLFW"};
    }

    if (!glfwVulkanSupported()) {
      throw std::runtime_error{"Vulkan is not supported"};
    }

    _monitor = std::make_unique<monitor>();
    _window = std::make_unique<window>(window_create_info{"Window", 960, 720});
  }

  ~device_module() override {
    _window.reset();
    _monitor.reset();

    glfwTerminate();
  }

  void update(const core::time& delta_time) override {
    glfwPollEvents();
    _window->set_title(fmt::format("Window [Delta: {:.6f} ms]", delta_time));
  }

  std::vector<const char*> get_required_extentions() const {
    auto extention_count = std::uint32_t{};

    const auto* glfw_extentions = glfwGetRequiredInstanceExtensions(&extention_count);

    auto extentions = std::vector<const char*>{glfw_extentions, glfw_extentions + extention_count};

    extentions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extentions;
  }

  monitor& current_monitor() {
    return *_monitor;
  }

  window& current_window() {
    return *_window;
  }

private:

  std::unique_ptr<monitor> _monitor{};
  std::unique_ptr<window> _window{};

}; // class device_module

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_DEVICE_MODULE_HPP_
