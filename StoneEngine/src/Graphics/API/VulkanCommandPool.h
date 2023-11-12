#pragma once

#include "Core/Utils.h"
#include "VulkanIncludes.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanDevice;

	enum class CommandPoolPurpose
	{
		PersistentBuffers = 0,
		TransientBuffers = 1
	};

	class VulkanCommandPool final
	{
	public:
		DISALLOW_COPY_AND_MOVE(VulkanCommandPool);

		VulkanCommandPool(CommandPoolPurpose purpose, const VulkanDevice& device);

		explicit operator const vk::raii::CommandPool& () const
		{
			return mCommandPool;
		}

		[[nodiscard]] const CommandPoolPurpose GetPurpose() const
		{
			return mPurpose;
		}

		[[nodiscard]] const vk::raii::CommandPool& GetInstance() const
		{
			return mCommandPool;
		}

		vk::raii::CommandBuffer CreateCommandBuffer(const VulkanDevice& device) const;
		
	private:
		CommandPoolPurpose mPurpose;
		vk::raii::CommandPool mCommandPool = nullptr;
	};
}

