#include <libsbx/graphics/devices/logical_device.hpp>

#include <optional>

#include <libsbx/core/logger.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/devices/extentions.hpp>
#include <libsbx/graphics/devices/layers.hpp>

namespace sbx::graphics {

logical_device::logical_device(const physical_device& physical_device) {
  _create_queues(physical_device);
  _create_logical_device(physical_device);
}

logical_device::~logical_device() {
  graphics_module::validate(wait_idle());

  vkDestroyDevice(_handle, nullptr);
}

VkDevice logical_device::handle() const noexcept {
  return _handle;
}

logical_device::operator VkDevice() const noexcept {
  return _handle;
}

VkPhysicalDeviceFeatures logical_device::physical_device_features() const noexcept {
  return _physical_device_features;
}

const logical_device::queue& logical_device::graphics_queue() const noexcept {
  return _queues.graphics_queue;
}

const logical_device::queue& logical_device::present_queue() const noexcept {
  return _queues.present_queue;
}

const logical_device::queue& logical_device::compute_queue() const noexcept {
  return _queues.compute_queue;
}

const logical_device::queue& logical_device::transfer_queue() const noexcept {
  return _queues.transfer_queue;
}

VkResult logical_device::wait_idle() {
  return vkDeviceWaitIdle(_handle);
}

void logical_device::_create_queues(const physical_device& physical_device) {
  auto device_queue_family_property_count = std::uint32_t{0};
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &device_queue_family_property_count, nullptr);
	auto device_queue_family_properties = std::vector<VkQueueFamilyProperties>{device_queue_family_property_count};
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &device_queue_family_property_count, device_queue_family_properties.data());

  auto graphics_family = std::optional<std::uint32_t>{};
  auto present_family = std::optional<std::uint32_t>{};
  auto compute_family = std::optional<std::uint32_t>{};
  auto transfer_family = std::optional<std::uint32_t>{};

  auto index = std::uint32_t{0};

  for (const auto& property : device_queue_family_properties) {
    if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphics_family = index;
      _queues.graphics_queue.family = index;
      _queues.supported_queues |= VK_QUEUE_GRAPHICS_BIT;
    }

    if (property.queueCount > 0) {
      present_family = index;
      _queues.present_queue.family = index;
    }

    if (property.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      compute_family = index;
      _queues.compute_queue.family = index;
      _queues.supported_queues |= VK_QUEUE_COMPUTE_BIT;
    }

    if (property.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      compute_family = index;
      _queues.compute_queue.family = index;
      _queues.supported_queues |= VK_QUEUE_TRANSFER_BIT;
    }

    if (graphics_family && present_family && compute_family && transfer_family) {
      break;
    }

    ++index;
  }

  if (!graphics_family) {
    throw std::runtime_error{"Failed to find queue family supporting VK_QUEUE_GRAPHICS_BIT"};
  }
}

void logical_device::_create_logical_device(const physical_device& physical_device) {
  auto queue_create_infos = std::vector<VkDeviceQueueCreateInfo>{};
	auto queue_priority = 0.0f;

	if (_queues.supported_queues & VK_QUEUE_GRAPHICS_BIT) {
		auto graphics_queue_create_info = VkDeviceQueueCreateInfo{};
		graphics_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		graphics_queue_create_info.queueFamilyIndex = _queues.graphics_queue.family;
		graphics_queue_create_info.queueCount = 1;
		graphics_queue_create_info.pQueuePriorities = &queue_priority;

		queue_create_infos.emplace_back(graphics_queue_create_info);
	} else {
		_queues.graphics_queue.family = 0;
	}

	if (_queues.supported_queues & VK_QUEUE_COMPUTE_BIT && _queues.compute_queue.family != _queues.graphics_queue.family) {
		auto compute_queue_create_info = VkDeviceQueueCreateInfo{};
		compute_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		compute_queue_create_info.queueFamilyIndex = _queues.compute_queue.family;
		compute_queue_create_info.queueCount = 1;
		compute_queue_create_info.pQueuePriorities = &queue_priority;

		queue_create_infos.emplace_back(compute_queue_create_info);
	} else {
		_queues.compute_queue.family = _queues.graphics_queue.family;
	}

	if (_queues.supported_queues & VK_QUEUE_TRANSFER_BIT && _queues.transfer_queue.family != _queues.graphics_queue.family && _queues.transfer_queue.family != _queues.compute_queue.family) {
		VkDeviceQueueCreateInfo transfer_queue_create_info = {};
		transfer_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		transfer_queue_create_info.queueFamilyIndex = _queues.transfer_queue.family;
		transfer_queue_create_info.queueCount = 1;
		transfer_queue_create_info.pQueuePriorities = &queue_priority;

		queue_create_infos.emplace_back(transfer_queue_create_info);
	} else {
		_queues.transfer_queue.family = _queues.graphics_queue.family;
	}

	auto physical_device_features = physical_device.features();

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

  const auto extensions = extentions::device();
  const auto layers = layers::validation();

	auto device_create_info = VkDeviceCreateInfo{};
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.queueCreateInfoCount = static_cast<std::uint32_t>(queue_create_infos.size());
	device_create_info.pQueueCreateInfos = queue_create_infos.data();
  device_create_info.enabledLayerCount = static_cast<std::uint32_t>(layers.size());
  device_create_info.ppEnabledLayerNames = layers.data();
	device_create_info.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
	device_create_info.ppEnabledExtensionNames = extensions.data();
	device_create_info.pEnabledFeatures = &enabled_features;

	graphics_module::validate(vkCreateDevice(physical_device, &device_create_info, nullptr, &_handle));

	vkGetDeviceQueue(_handle, _queues.graphics_queue.family, 0, &_queues.graphics_queue.handle);
	vkGetDeviceQueue(_handle, _queues.present_queue.family, 0, &_queues.present_queue.handle);
	vkGetDeviceQueue(_handle, _queues.compute_queue.family, 0, &_queues.compute_queue.handle);
	vkGetDeviceQueue(_handle, _queues.transfer_queue.family, 0, &_queues.transfer_queue.handle);
}

} // namespace sbx::graphics
