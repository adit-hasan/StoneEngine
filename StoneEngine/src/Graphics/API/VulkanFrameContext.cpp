#include "VulkanFrameContext.h"

#include "Graphics/API/VulkanDevice.h"
#include "Graphics/API/VulkanCommandPool.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	VulkanFrameContext::VulkanFrameContext() :
		mDevice(nullptr),
		mCommandBuffer(nullptr),
		mInFlightFence(nullptr),
		mImageAcquiredSemaphore(nullptr),
		mIsRenderCompleteSemaphore(nullptr)
	{}

	VulkanFrameContext::VulkanFrameContext(VulkanDevice* device, const VulkanCommandPool& commandPool) : 
		mDevice(device),
		mCommandBuffer(commandPool.CreateCommandBuffer(*device)),
		mInFlightFence(vk::raii::Fence(device->GetLogicalDevice(), vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled))),
		mImageAcquiredSemaphore(vk::raii::Semaphore(device->GetLogicalDevice(), vk::SemaphoreCreateInfo())),
		mIsRenderCompleteSemaphore(vk::raii::Semaphore(device->GetLogicalDevice(), vk::SemaphoreCreateInfo()))
	{
	}

	void VulkanFrameContext::ResetFences() const
	{
		mDevice->GetLogicalDevice().resetFences(*mInFlightFence);
	}

	const vk::raii::CommandBuffer& VulkanFrameContext::GetCommandBuffer() const
	{
		return mCommandBuffer;;
	}

	void VulkanFrameContext::DestroyCommandBuffer()
	{
		mCommandBuffer.clear();
	}

	void VulkanFrameContext::WaitForFences() const
	{
		while (vk::Result::eSuccess != mDevice->GetLogicalDevice().waitForFences(*mInFlightFence, VK_TRUE, UINT16_MAX))
			;
	}

	const vk::raii::Fence& VulkanFrameContext::GetFence() const
	{
		return mInFlightFence;
	}

	const vk::raii::Semaphore& VulkanFrameContext::GetImageAcquiredSemaphore() const
	{
		return mImageAcquiredSemaphore;
	}

	const vk::raii::Semaphore& VulkanFrameContext::GetIsRenderCompleteSemaphore() const
	{
		return mIsRenderCompleteSemaphore;
	}
}