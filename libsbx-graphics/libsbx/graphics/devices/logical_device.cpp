#include <libsbx/graphics/devices/logical_device.hpp>

#include <optional>

#include <libsbx/core/logger.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/devices/validation_layers.hpp>
#include <libsbx/graphics/devices/extensions.hpp>

namespace sbx::graphics {

logical_device::logical_device(const physical_device& physical_device) {
  _create_queue_indices(physical_device);
  _create_logical_logical_device(physical_device);
}

logical_device::~logical_device() {
  wait_for_idle();

  vkDestroyDevice(_handle, nullptr);
}

auto logical_device::handle() const noexcept -> VkDevice {
  return _handle;
}

logical_device::operator VkDevice() const noexcept {
  return _handle;
}

auto logical_device::enables_features() const -> const VkPhysicalDeviceFeatures& {
  return _enabled_features;
}

auto logical_device::graphics_queue() const -> const queue& {
  return _graphics_queue;
}
  
auto logical_device::present_queue() const -> const queue& {
  return _present_queue;
}

auto logical_device::compute_queue() const -> const queue& {
  return _compute_queue;
}

auto logical_device::transfer_queue() const -> const queue& {
  return _transfer_queue;
}

auto logical_device::wait_for_idle() const -> void {
  graphics_module::validate(vkDeviceWaitIdle(_handle));
}

auto logical_device::_create_queue_indices(const physical_device& physical_device) -> void {
  auto device_queue_family_property_count = std::uint32_t{0};
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &device_queue_family_property_count, nullptr);

	auto device_queue_family_properties = std::vector<VkQueueFamilyProperties>{device_queue_family_property_count};
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &device_queue_family_property_count, device_queue_family_properties.data());

  auto graphics_family = std::optional<std::uint32_t>{};
  auto present_family = std::optional<std::uint32_t>{};
  auto compute_family = std::optional<std::uint32_t>{};
  auto transfer_family = std::optional<std::uint32_t>{};

  for (auto i = std::uint32_t{0}; i < device_queue_family_property_count; ++i) {
		if (device_queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			graphics_family = i;
			_graphics_queue.family = i;
			_supported_queues |= VK_QUEUE_GRAPHICS_BIT;
		}

		if (device_queue_family_properties[i].queueCount > 0) {
			present_family = i;
			_present_queue.family = i;
		}

		if (device_queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			compute_family = i;
			_compute_queue.family = i;
			_supported_queues |= VK_QUEUE_COMPUTE_BIT;
		}

		if (device_queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
			transfer_family = i;
			_transfer_queue.family = i;
			_supported_queues |= VK_QUEUE_TRANSFER_BIT;
		}

		if (graphics_family && present_family && compute_family && transfer_family) {
			break;
		}
	}

	if (!graphics_family) {
		throw std::runtime_error("Failed to find queue family supporting VK_QUEUE_GRAPHICS_BIT");
  }
}

auto logical_device::_create_logical_logical_device(const physical_device& physical_device) -> void {
  auto queue_create_infos = std::vector<VkDeviceQueueCreateInfo>{};
	float queue_priorities = 0.0f;

	if (_supported_queues & VK_QUEUE_GRAPHICS_BIT) {
		auto graphics_queue_create_info = VkDeviceQueueCreateInfo{};
		graphics_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		graphics_queue_create_info.queueFamilyIndex = _graphics_queue.family;
		graphics_queue_create_info.queueCount = 1;
		graphics_queue_create_info.pQueuePriorities = &queue_priorities;

		queue_create_infos.emplace_back(graphics_queue_create_info);
	} else {
		_graphics_queue.family = 0;
	}

	if (_supported_queues & VK_QUEUE_COMPUTE_BIT && _compute_queue.family != _graphics_queue.family) {
		auto compute_queue_create_info = VkDeviceQueueCreateInfo{};
		compute_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		compute_queue_create_info.queueFamilyIndex = _compute_queue.family;
		compute_queue_create_info.queueCount = 1;
		compute_queue_create_info.pQueuePriorities = &queue_priorities;

		queue_create_infos.emplace_back(compute_queue_create_info);
	} else {
		_compute_queue.family = _graphics_queue.family;
	}

	if (_supported_queues & VK_QUEUE_TRANSFER_BIT && _transfer_queue.family != _graphics_queue.family && _transfer_queue.family != _compute_queue.family) {
		auto transfer_queue_create_info = VkDeviceQueueCreateInfo{};
		transfer_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		transfer_queue_create_info.queueFamilyIndex = _transfer_queue.family;
		transfer_queue_create_info.queueCount = 1;
		transfer_queue_create_info.pQueuePriorities = &queue_priorities;

		queue_create_infos.emplace_back(transfer_queue_create_info);
	} else {
		_transfer_queue.family = _graphics_queue.family;
	}

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
		core::logger::warn("Selected GPU does not support wireframe pipelines");
	}

	if (physical_device_features.samplerAnisotropy) {
		enabled_features.samplerAnisotropy = true;
  } else {
		core::logger::warn("Selected GPU does not support sampler anisotropy");
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
		core::logger::warn("Selected GPU does not support vertex pipeline stores and atomics");
  }

	if (physical_device_features.fragmentStoresAndAtomics) {
		enabled_features.fragmentStoresAndAtomics = true;
  } else {
		core::logger::warn("Selected GPU does not support fragment stores and atomics");
  }

	if (physical_device_features.shaderStorageImageExtendedFormats) {
		enabled_features.shaderStorageImageExtendedFormats = true;
  } else {
		core::logger::warn("Selected GPU does not support shader storage extended formats");
  }

	if (physical_device_features.shaderStorageImageWriteWithoutFormat) {
		enabled_features.shaderStorageImageWriteWithoutFormat = true;
  } else {
		core::logger::warn("Selected GPU does not support shader storage write without format");
  }

  if (physical_device_features.shaderClipDistance) {
		enabled_features.shaderClipDistance = true;
  } else {
		core::logger::warn("Selected GPU does not support shader clip distance");
  }

  if (physical_device_features.shaderCullDistance) {
		enabled_features.shaderCullDistance = true;
  } else {
		core::logger::warn("Selected GPU does not support shader cull distance");
  }

	if (physical_device_features.geometryShader) {
		enabled_features.geometryShader = true;
  } else {
		core::logger::warn("Selected GPU does not support geometry shaders");
  }

	if (physical_device_features.tessellationShader) {
		enabled_features.tessellationShader = true;
  } else {
		core::logger::warn("Selected GPU does not support tessellation shaders");
  }

	if (physical_device_features.multiViewport) {
		enabled_features.multiViewport = true;
  } else {
		core::logger::warn("Selected GPU does not support multi viewports");
  }

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
	device_create_info.pEnabledFeatures = &enabled_features;

	graphics_module::validate(vkCreateDevice(physical_device, &device_create_info, nullptr, &_handle));

	vkGetDeviceQueue(_handle, _graphics_queue.family, 0, &_graphics_queue.handle);
	vkGetDeviceQueue(_handle, _present_queue.family, 0, &_present_queue.handle);
	vkGetDeviceQueue(_handle, _compute_queue.family, 0, &_compute_queue.handle);
	vkGetDeviceQueue(_handle, _transfer_queue.family, 0, &_transfer_queue.handle);
}

} // namespace sbx::graphics
