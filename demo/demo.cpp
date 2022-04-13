#include <iostream>
#include <cstddef>
#include <type_traits>
#include <bitset>
#include <variant>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <filesystem>

#include <math/math.hpp>
#include <types/types.hpp>
#include <container/container.hpp>
#include <memory/memory.hpp>
#include <ecs/ecs.hpp>
#include <utils/utils.hpp>
#include <async/async.hpp>
#include <io/io.hpp>
#include <core/core.hpp>

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "json_node.hpp"
#include "json_tokenizer.hpp"
#include "json_parser.hpp"
#include "json_document.hpp"

struct transform {
  sbx::vector3 position{};
  sbx::vector3 scale{};
  sbx::quaternion rotation{};
};

class application {

public:

  application()
  : _window_handle{nullptr},
    _instance{nullptr},
    _validation_layers{"VK_LAYER_KHRONOS_validation"} {
    
  }

  ~application() {
    
  }

  void run() {
    _initialize_window();
    _initialize_vulkan();
    _loop();
    _terminate_vulkan();
    _terminate_window();
  }

private:

  void _initialize_window() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window_handle = glfwCreateWindow(800, 600, "Sandbox", nullptr, nullptr);
  }

  void _initialize_vulkan() {
    _create_instance();
    _initialize_validation_layers();
  }

  void _create_instance() {
    auto app_info = VkApplicationInfo{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Sandbox";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Sandbox";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    auto create_info = VkInstanceCreateInfo{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    auto glfw_extension_count = sbx::uint32{};

    auto glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;
    create_info.enabledLayerCount = static_cast<sbx::uint32>(_validation_layers.size());
    create_info.ppEnabledLayerNames = _validation_layers.data();

    if (vkCreateInstance(&create_info, nullptr, &_instance) != VK_SUCCESS) {
      throw std::runtime_error("failed to create instance!");
    }

    auto extension_count = sbx::uint32{};

    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

    auto extensions = std::vector<VkExtensionProperties>{extension_count};

    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

    std::cout << "available extensions (" << extension_count << "):\n";

    for (const auto& extension : extensions) {
      std::cout << '\t' << extension.extensionName << '\n';
    }
  }

  void _initialize_validation_layers() {
    auto layer_count = sbx::uint32{};
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    auto available_layers = std::vector<VkLayerProperties>{layer_count};
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (const auto& layer : _validation_layers) {
      auto layer_found = false;

      for (const auto& available_layer : available_layers) {
        if (std::strcmp(layer, available_layer.layerName) == 0) {
          layer_found = true;
          break;
        }
      }

      if (!layer_found) {
        throw std::runtime_error("validation layer requested but not available!");
      }
    }
  }

  void _loop() {
    while (!glfwWindowShouldClose(_window_handle)) {
      glfwPollEvents();
    }
  }

  void _terminate_vulkan() {
    vkDestroyInstance(_instance, nullptr);
  }

  void _terminate_window() {
    glfwDestroyWindow(_window_handle);
    glfwTerminate();
  }

  GLFWwindow* _window_handle{nullptr};
  VkInstance _instance{nullptr};
  std::vector<const char*> _validation_layers{};

}; // class application

int main() {

  std::cout << "Hello, Sandbox!" << std::endl;

  auto app = application{};

  try {
    app.run();
  } catch (const std::exception& exception) {
    std::cerr << exception  .what() << std::endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}