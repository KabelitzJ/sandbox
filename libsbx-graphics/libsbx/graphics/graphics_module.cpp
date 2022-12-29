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
 * @file libsbx/graphics/graphics_module.cpp
 */

#include <libsbx/core/logger.hpp>

#include <libsbx/devices/device_module.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

graphics_module::graphics_module() {
// : _instance{std::make_unique<graphics::instance>()},
//   _physical_device{std::make_unique<graphics::physical_device>(*_instance)},
//   _logical_device{std::make_unique<graphics::logical_device>(*_physical_device)} {
//     _surface = std::make_unique<graphics::surface>(*_instance, *_physical_device, *_logical_device);
  core::logger::info("{}", devices::device_module::get().answer());
  core::logger::info("{}", devices::device_module::get().greet("Jonas"));
  // core::logger::info("{}", devices::device_module::get().window().should_close());
}

graphics_module::~graphics_module() {
  // validate(_logical_device->graphics_queue().wait_idle());
}

void graphics_module::validate(VkResult result) {
  if (result >= 0) {
    return;
  }

  throw std::runtime_error{fmt::format("Vulkan error: {}", _stringify_result(result))};
}

void graphics_module::update([[maybe_unused]] const core::time& delta_time) {

}

instance& graphics_module::instance() noexcept {
  // return *_instance;
}

physical_device& graphics_module::physical_device() noexcept {
  // return *_physical_device;
}

logical_device& graphics_module::logical_device() noexcept {
  // return *_logical_device;
} 

surface& graphics_module::surface() noexcept {
  // return *_surface;
} 

std::string graphics_module::_stringify_result(VkResult result) {
  switch (result) {
    case VK_SUCCESS:
      return "Success";
    case VK_NOT_READY:
      return "A fence or query has not yet completed";
    case VK_TIMEOUT:
      return "A wait operation has not completed in the specified time";
    case VK_EVENT_SET:
      return "An event is signaled";
    case VK_EVENT_RESET:
      return "An event is unsignaled";
    case VK_INCOMPLETE:
      return "A return array was too small for the result";
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      return "A host memory allocation has failed";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      return "A device memory allocation has failed";
    case VK_ERROR_INITIALIZATION_FAILED:
      return "Initialization of an object could not be completed for implementation-specific reasons";
    case VK_ERROR_DEVICE_LOST:
      return "The logical or physical device has been lost";
    case VK_ERROR_MEMORY_MAP_FAILED:
      return "Mapping of a memory object has failed";
    case VK_ERROR_LAYER_NOT_PRESENT:
      return "A requested layer is not present or could not be loaded";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      return "A requested extension is not supported";
    case VK_ERROR_FEATURE_NOT_PRESENT:
      return "A requested feature is not supported";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
      return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible";
    case VK_ERROR_TOO_MANY_OBJECTS:
      return "Too many objects of the type have already been created";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
      return "A requested format is not supported on this device";
    case VK_ERROR_SURFACE_LOST_KHR:
      return "A surface is no longer available";
    case VK_ERROR_OUT_OF_POOL_MEMORY:
      return "A allocation failed due to having no more space in the descriptor pool";
    case VK_SUBOPTIMAL_KHR:
      return "A swapchain no longer matches the surface properties exactly, but can still be used";
    case VK_ERROR_OUT_OF_DATE_KHR:
      return "A surface has changed in such a way that it is no longer compatible with the swapchain";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
      return "The display used by a swapchain does not use the same presentable image layout";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
      return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API";
    case VK_ERROR_VALIDATION_FAILED_EXT:
      return "A validation layer found an error";
    default:
      return "Unknown Vulkan error";
	}
}

} // namespace sbx::graphics
