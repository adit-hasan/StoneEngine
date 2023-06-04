#include "VulkanDevice.h"
#include "vulkan/vulkan_enums.hpp"
#include <Core/Logger.h>

namespace StoneEngine::Graphics::API::Vulkan
{
	VulkanDevice::VulkanDevice()
		: mPhysicalDevice(nullptr),
		mDevice(nullptr),
		mGraphicsQueue(nullptr),
		mPresentQueue(nullptr)
	{

	}

	void VulkanDevice::Initialize(const VulkanInstance* const instance, const vk::raii::SurfaceKHR& surface)
	{
		Core::LogInfo("Initializing VulkanDevice.");

		// initialize
		auto physicalDevices = PhysicalDevices(instance->GetInstance());

		Core::FatalAssert(SelectPhysicalDevice(physicalDevices), "VulkanDevice::Initialize -> Error while selecting devices");

		// Find queue families
		auto queueFamilyProperties = mPhysicalDevice.getQueueFamilyProperties();
		int i = 0;
		bool isQueueSuitable = false;
		for (const auto& queueFamily : queueFamilyProperties)
		{
			if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
			{
				mQueueFamilyIndices.GraphicsFamily = i;
			}

			if (mPhysicalDevice.getSurfaceSupportKHR(i, *surface))
			{
				mQueueFamilyIndices.PresentFamily = i;
			}

			if (mQueueFamilyIndices.isSuitable()) 
			{
				isQueueSuitable = true;
				break; 
			}

			i++;
		}

		Core::FatalAssert(isQueueSuitable, "VulkanDevice::Initialize -> No suitable queue family found on the device");

		SetSwapchainSupportDetails(surface);
		
		Core::FatalAssert(mSwapchainSupportDetails.isSuitable(), "VulkanDevice::Initialize -> Required swapchain features not available");

		// Create Logical Device
		float                     queuePriority = 0.0f;
		vk::DeviceQueueCreateInfo graphicsQCreateInfo({}, mQueueFamilyIndices.GraphicsFamily.value(), 1, &queuePriority);
		vk::DeviceQueueCreateInfo presentQCreateInfo({}, mQueueFamilyIndices.PresentFamily.value(), 1, &queuePriority);
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{graphicsQCreateInfo, presentQCreateInfo};
	
		vk::DeviceCreateInfo      deviceCreateInfo({}, queueCreateInfos);

		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(_DeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = _DeviceExtensions.data();

		mDevice = Device(mPhysicalDevice, deviceCreateInfo);

		mGraphicsQueue = mDevice.getQueue(mQueueFamilyIndices.GraphicsFamily.value(), 0);
		mPresentQueue = mDevice.getQueue(mQueueFamilyIndices.PresentFamily.value(), 0);
	}

	void VulkanDevice::SetSwapchainSupportDetails(const Surface& surface)
	{
		mSwapchainSupportDetails.SurfaceFormats = mPhysicalDevice.getSurfaceFormatsKHR(*surface);
		mSwapchainSupportDetails.SurfaceCapabilities = mPhysicalDevice.getSurfaceCapabilitiesKHR(*surface);

		mSwapchainSupportDetails.PresentModes = mPhysicalDevice.getSurfacePresentModesKHR(*surface);
	}

	bool VulkanDevice::SelectPhysicalDevice(const PhysicalDevices& devices)
	{
		Core::LogInfo("VulkanDevice::SelectPhysicalDevice -> Selecting a physical device");
		if (devices.size() <= 0)
		{
			Core::LogError("VulkanDevice::SelectPhysicalDevice -> The list of physical devices is empty");
			return false;
		}

		std::multimap<int, PhysicalDevice> candidates;

		for (const auto& device : devices)
		{
			candidates.emplace(RankPhysicalDevice(device), device);
		}

		if (candidates.rbegin()->first > 0)
		{
			mPhysicalDevice = candidates.rbegin()->second;
			return true;
		}
		else
		{
			Core::LogError("VulkanDevice::SelectPhysicalDevice -> No suitable physical device was found");
			return false;
		}
	}

	int VulkanDevice::RankPhysicalDevice(const PhysicalDevice& device)
	{
		// Check for deal breakers first

		// Application can't function without geometry shaders
		const auto& features = device.getFeatures();
		if (!features.geometryShader) {
			return 0;
		}

		const auto& supportedDeviceExtensions = device.enumerateDeviceExtensionProperties();

		std::set<std::string_view> requiredExtensions(_DeviceExtensions.begin(), _DeviceExtensions.end());
		
		for (const auto& extension : supportedDeviceExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}
		if (!requiredExtensions.empty())
		{
			Core::LogError("VulkanDevice::RankPhysicalDevice -> The physical device is missing {} required extensions", requiredExtensions.size());
			return 0;
		}

		int score = 0;

		// Discrete GPUs have a significant performance advantage
		const auto& properties = device.getProperties();
		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			score += 1000;
		}

		// Maximum possible size of textures affects graphics quality
		score += properties.limits.maxImageDimension2D;

		return score;
	}

}
