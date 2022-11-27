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
 * @file libsbx/graphics/graphics_module.hpp 
 */

#ifndef LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_
#define LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_

/**
 * @ingroup libsbx-graphics
 */


#include <memory>

#include <libsbx/core/module.hpp>

#include <libsbx/devices/device_module.hpp>

#include <libsbx/graphics/instance.hpp>

namespace sbx::graphics {

class graphics_module : public core::module<graphics_module> {

  inline static const auto registered = register_module(stage::normal, core::dependencies<devices::device_module>{});

public:

  graphics_module()
  : _instance{std::make_unique<graphics::instance>()} { }

  ~graphics_module() override = default;

  void update([[maybe_unused]] const core::time& delta_time) override {

  }

  instance& instance() {
    return *_instance;
  }

private:

  std::unique_ptr<graphics::instance> _instance{};

}; // class graphics_module

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_
