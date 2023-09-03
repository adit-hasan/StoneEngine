#include "VulkanCommandPool.h"
#include "VulkanDevice.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	VulkanCommandPool::VulkanCommandPool()
		: mCommandPool(nullptr)
	{
	}

	void VulkanCommandPool::Initialize(const VulkanDevice& device)
	{
		vk::CommandPoolCreateInfo createInfo{
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			device.GetQueueFamilyIndices().GraphicsFamily.value()
		};

		mCommandPool = vk::raii::CommandPool(device.GetLogicalDevice(), createInfo);
	}
}
