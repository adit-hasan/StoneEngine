#include "VulkanCommandPool.h"
#include "VulkanDevice.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	VulkanCommandPool::VulkanCommandPool(CommandPoolPurpose purpose, const VulkanDevice& device)
		: mPurpose(purpose)
	{
		vk::CommandPoolCreateFlags flags = mPurpose == CommandPoolPurpose::PersistentBuffers ?
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer :
			vk::CommandPoolCreateFlagBits::eTransient;

		vk::CommandPoolCreateInfo createInfo{
			flags,
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
