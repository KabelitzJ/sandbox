#include <libsbx/graphics/devices/logical_device.hpp>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/target.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/devices/validation_layers.hpp>
#include <libsbx/graphics/devices/extensions.hpp>

namespace sbx::graphics {

// [NOTE] KAJ 2023-02-23 12:19 - Maybe rework the queue creation

static auto _print_queue_families(const VkQueueFamilyProperties& queue_family_properties) -> std::string {
  auto result = std::string{};

  if (queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
    if (!result.empty()) {
      result += "|";
    }
    result += "Graphics";
  }

  if (queue_family_properties.queueFlags & VK_QUEUE_COMPUTE_BIT) {
    if (!result.empty()) {
      result += "|";
    }
    result += "Compute";
  }

  if (queue_family_properties.queueFlags & VK_QUEUE_TRANSFER_BIT) {
    if (!result.empty()) {
      result += "|";
    }
    result += "Transfer";
  }

  if (queue_family_properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
    if (!result.empty()) {
      result += "|";
    }
    result += "Sparse Binding";
  }

  return result;
};

auto queue::handle() const noexcept -> const VkQueue& {
  return _handle;
}

queue::operator const VkQueue&() const noexcept {
  return _handle;
}

auto queue::family() const noexcept -> std::uint32_t {
  return _family;
}

logical_device::logical_device(const physical_device& physical_device) {
  _create_logical_device(physical_device);
}

logical_device::~logical_device() {
  wait_idle();

  vkDestroyDevice(_handle, nullptr);
}

auto logical_device::handle() const noexcept -> const VkDevice& {
  return _handle;
}

logical_device::operator const VkDevice&() const noexcept {
  return _handle;
}

auto logical_device::enables_features() const -> const VkPhysicalDeviceFeatures& {
  return _enabled_features;
}

auto logical_device::graphics_queue() const -> const queue& {
  return _graphics_queue;
}

auto logical_device::compute_queue() const -> const queue& {
  return _compute_queue;
}

auto logical_device::transfer_queue() const -> const queue& {
  return _transfer_queue;
}

auto logical_device::wait_idle() const -> void {
  validate(vkDeviceWaitIdle(_handle));
}

auto logical_device::_get_queue_family_indices(const physical_device& physical_device) const -> queue_family_indices {
  auto result = queue_family_indices{};

  auto device_queue_family_property_count = std::uint32_t{0};
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &device_queue_family_property_count, nullptr);

	auto device_queue_family_properties = std::vector<VkQueueFamilyProperties>{device_queue_family_property_count};
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &device_queue_family_property_count, device_queue_family_properties.data());

  for (auto i = std::uint32_t{0}; i < device_queue_family_property_count; ++i) {
    core::logger::debug("sbx::graphics", "Queue Family {} supports {} queues of type [{}]", i, device_queue_family_properties[i].queueCount, _print_queue_families(device_queue_family_properties[i]));

    // [NOTE] KAJ 2023-03-20 18:57 - Always pick the queue that is the most specialized for the task i.e. has the least flags other than the one we are looking for
		if (device_queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			if (!result.graphics) {
        result.graphics = i;
      } else {
        const auto old_queue = device_queue_family_properties[*result.graphics];

        if (std::popcount(device_queue_family_properties[i].queueFlags) < std::popcount(old_queue.queueFlags)) {
          result.graphics = i;
        }
      }
		}

		if (device_queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			if (!result.compute) {
        result.compute = i;
      } else {
        const auto old_queue = device_queue_family_properties[*result.compute];

        if (std::popcount(device_queue_family_properties[i].queueFlags) < std::popcount(old_queue.queueFlags)) {
          result.compute = i;
        }
      }
		}

		if (device_queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
			if (!result.transfer) {
        result.transfer = i;
      } else {
        const auto old_queue = device_queue_family_properties[*result.transfer];

        if (std::popcount(device_queue_family_properties[i].queueFlags) < std::popcount(old_queue.queueFlags)) {
          result.transfer = i;
        }
      }
		}
	}

	if (!result.graphics) {
		throw std::runtime_error("Failed to find suitable graphics queue family");
  }

  if (!result.compute) {
    throw std::runtime_error("Failed to find suitable compute queue family");
  }

  if (!result.transfer) {
    throw std::runtime_error("Failed to find suitable transfer queue family");
  }

  return result;
}

auto logical_device::_get_enabled_features(const physical_device& physical_device) const -> VkPhysicalDeviceFeatures {
  const auto& physical_device_features = physical_device.features();
	auto enabled_features = VkPhysicalDeviceFeatures{};

	if (physical_device_features.sampleRateShading) {
		enabled_features.sampleRateShading = true;
  }

	if (physical_device_features.fillModeNonSolid) {
		enabled_features.fillModeNonSolid = true;

		if (physical_device_features.wideLines) {
		  enabled_features.wideLines = true;
    }
	} else {
		core::logger::warn("sbx::graphics", "Selected GPU does not support wireframe pipelines");
	}

	if (physical_device_features.samplerAnisotropy) {
		enabled_features.samplerAnisotropy = true;
  } else {
		core::logger::warn("sbx::graphics", "Selected GPU does not support sampler anisotropy");
  }

	if (physical_device_features.textureCompressionBC) {
		enabled_features.textureCompressionBC = true;
  } else if (physical_device_features.textureCompressionASTC_LDR) {
		enabled_features.textureCompressionASTC_LDR = true;
  } else if (physical_device_features.textureCompressionETC2) {
		enabled_features.textureCompressionETC2 = true;
  }

	if (physical_device_features.vertexPipelineStoresAndAtomics) {
		enabled_features.vertexPipelineStoresAndAtomics = true;
  } else {
		core::logger::warn("sbx::graphics", "sbx::graphics", "Selected GPU does not support vertex pipeline stores and atomics");
  }

	if (physical_device_features.fragmentStoresAndAtomics) {
		enabled_features.fragmentStoresAndAtomics = true;
  } else {
		core::logger::warn("sbx::graphics", "Selected GPU does not support fragment stores and atomics");
  }

	if (physical_device_features.shaderStorageImageExtendedFormats) {
		enabled_features.shaderStorageImageExtendedFormats = true;
  } else {
		core::logger::warn("sbx::graphics", "Selected GPU does not support shader storage extended formats");
  }

	if (physical_device_features.shaderStorageImageWriteWithoutFormat) {
		enabled_features.shaderStorageImageWriteWithoutFormat = true;
  } else {
		core::logger::warn("sbx::graphics", "Selected GPU does not support shader storage write without format");
  }

  if (physical_device_features.shaderClipDistance) {
		enabled_features.shaderClipDistance = true;
  } else {
		core::logger::warn("sbx::graphics", "Selected GPU does not support shader clip distance");
  }

  if (physical_device_features.shaderCullDistance) {
		enabled_features.shaderCullDistance = true;
  } else {
		core::logger::warn("sbx::graphics", "Selected GPU does not support shader cull distance");
  }

	if (physical_device_features.geometryShader) {
		enabled_features.geometryShader = true;
  } else {
		core::logger::warn("sbx::graphics", "Selected GPU does not support geometry shaders");
  }

	if (physical_device_features.tessellationShader) {
		enabled_features.tessellationShader = true;
  } else {
		core::logger::warn("sbx::graphics", "Selected GPU does not support tessellation shaders");
  }

	if (physical_device_features.multiViewport) {
		enabled_features.multiViewport = true;
  } else {
		core::logger::warn("sbx::graphics", "Selected GPU does not support multi viewports");
  }

  return enabled_features;
}

auto logical_device::_create_logical_device(const physical_device& physical_device) -> void {
  const auto queue_family_indices = _get_queue_family_indices(physical_device);

  _graphics_queue._family = queue_family_indices.graphics.value();
  _compute_queue._family = queue_family_indices.compute.value();
  _transfer_queue._family = queue_family_indices.transfer.value();

  auto queue_create_infos = std::vector<VkDeviceQueueCreateInfo>{};
	auto queue_priorities = 0.0f;

  auto graphics_queue_create_info = VkDeviceQueueCreateInfo{};
  graphics_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  graphics_queue_create_info.queueFamilyIndex = queue_family_indices.graphics.value();
  graphics_queue_create_info.queueCount = 1;
  graphics_queue_create_info.pQueuePriorities = &queue_priorities;

  queue_create_infos.emplace_back(graphics_queue_create_info);

  if (_compute_queue.family() != _graphics_queue.family()) {
    auto compute_queue_create_info = VkDeviceQueueCreateInfo{};
    compute_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    compute_queue_create_info.queueFamilyIndex = queue_family_indices.compute.value();
    compute_queue_create_info.queueCount = 1;
    compute_queue_create_info.pQueuePriorities = &queue_priorities;

    queue_create_infos.emplace_back(compute_queue_create_info);
  }

  if (_transfer_queue.family() != _graphics_queue.family() && _transfer_queue.family() != _compute_queue.family()) {
    auto transfer_queue_create_info = VkDeviceQueueCreateInfo{};
    transfer_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    transfer_queue_create_info.queueFamilyIndex = queue_family_indices.transfer.value();
    transfer_queue_create_info.queueCount = 1;
    transfer_queue_create_info.pQueuePriorities = &queue_priorities;

    queue_create_infos.emplace_back(transfer_queue_create_info);
  }

  _enabled_features = _get_enabled_features(physical_device);

  const auto instance_validation_layers = validation_layers::instance();
  const auto device_extensions = extensions::device();

	auto device_create_info = VkDeviceCreateInfo{};
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.queueCreateInfoCount = static_cast<std::uint32_t>(queue_create_infos.size());
	device_create_info.pQueueCreateInfos = queue_create_infos.data();
  device_create_info.enabledLayerCount = static_cast<std::uint32_t>(instance_validation_layers.size());
  device_create_info.ppEnabledLayerNames = instance_validation_layers.data();
	device_create_info.enabledExtensionCount = static_cast<std::uint32_t>(device_extensions.size());
	device_create_info.ppEnabledExtensionNames = device_extensions.data();
	device_create_info.pEnabledFeatures = &_enabled_features;

	validate(vkCreateDevice(physical_device, &device_create_info, nullptr, &_handle));

	vkGetDeviceQueue(_handle, _graphics_queue._family, 0, &_graphics_queue._handle);
	vkGetDeviceQueue(_handle, _compute_queue._family, 0, &_compute_queue._handle);
	vkGetDeviceQueue(_handle, _transfer_queue._family, 0, &_transfer_queue._handle);
}

} // namespace sbx::graphics
