#pragma once

#include "VulkanIncludes.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanDevice;
	class VulkanCommandPool;

	class VulkanFrameContext final
	{
	public:
		VulkanFrameContext();
		VulkanFrameContext(VulkanDevice* device, const VulkanCommandPool& commandPool);
		void ResetFences() const;
		void WaitForFences() const;

		[[nodiscard]] const vk::raii::CommandBuffer& GetCommandBuffer() const;
		[[nodiscard]] const vk::raii::Fence& GetFence() const;
		[[nodiscard]] const vk::raii::Semaphore& GetImageAcquiredSemaphore() const;
		[[nodiscard]] const vk::raii::Semaphore& GetIsRenderCompleteSemaphore() const;
	private:
		VulkanDevice* mDevice;
		vk::raii::CommandBuffer mCommandBuffer;
		vk::raii::Fence mInFlightFence;
		vk::raii::Semaphore mImageAcquiredSemaphore;
		vk::raii::Semaphore mIsRenderCompleteSemaphore;
	};
}