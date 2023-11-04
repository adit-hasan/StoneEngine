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

	vk::raii::CommandBuffer VulkanCommandPool::CreateCommandBuffer(const VulkanDevice& device) const
	{
		vk::CommandBufferAllocateInfo commandBufferCreateInfo(
			*mCommandPool,
			vk::CommandBufferLevel::ePrimary,
			1
		);

		return std::move(vk::raii::CommandBuffers(device.GetLogicalDevice(), commandBufferCreateInfo).front());
	}
}
