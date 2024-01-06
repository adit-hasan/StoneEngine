#pragma once

#include "VulkanIncludes.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanDevice;
	class VulkanCommandPool;
	class VulkanBuffer;

	class VulkanFrameContext final
	{
	public:
		VulkanFrameContext();
		VulkanFrameContext(
			VulkanDevice* device, 
			const VulkanCommandPool& commandPool, 
			std::unique_ptr<VulkanBuffer> uniformBuffer = nullptr,
			vk::raii::DescriptorSet descriptorSet = nullptr);
		void ResetFences() const;
		void WaitForFences() const;
		void DestroyCommandBuffer();

		[[nodiscard]] const vk::raii::CommandBuffer& GetCommandBuffer() const;
		[[nodiscard]] const vk::raii::Fence& GetFence() const;
		[[nodiscard]] const vk::raii::Semaphore& GetImageAcquiredSemaphore() const;
		[[nodiscard]] const vk::raii::Semaphore& GetIsRenderCompleteSemaphore() const;
		[[nodiscard]] const VulkanBuffer& GetUniformBuffer() const;
	private:
		VulkanDevice* mDevice = nullptr;
		vk::raii::CommandBuffer mCommandBuffer = nullptr;
		vk::raii::Fence mInFlightFence = nullptr;
		vk::raii::Semaphore mImageAcquiredSemaphore = nullptr;
		vk::raii::Semaphore mIsRenderCompleteSemaphore = nullptr;
		std::unique_ptr<VulkanBuffer> mUniformBuffer = nullptr;
		vk::raii::DescriptorSet mDescriptorSet = nullptr;
	};
}