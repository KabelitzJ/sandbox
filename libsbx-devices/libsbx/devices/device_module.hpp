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

#include <vector>
#include <memory>
#include <vector>
#include <cstdint>
#include <cinttypes>

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <libsbx/core/module.hpp>
#include <libsbx/core/platform.hpp>
#include <libsbx/core/slot.hpp>

#include <libsbx/devices/monitor.hpp>
#include <libsbx/devices/window.hpp>
#include <libsbx/devices/events.hpp>

namespace sbx::devices {

class device_module : public core::module<device_module> {

  inline static const auto registered = register_module(stage::normal);

public:

  device_module();

  ~device_module() override;

  void update([[maybe_unused]] const core::time& delta_time) override;

  std::vector<const char*> required_extensions() const;

  monitor& current_monitor();

  window& current_window();

private:

  std::unique_ptr<monitor> _monitor{};
  std::unique_ptr<window> _window{};

}; // class device_module

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_DEVICE_MODULE_HPP_
