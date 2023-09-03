#pragma once

#include "Core/Utils.h"
#include "vulkan/vulkan_raii.hpp"

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanDevice;
	class VulkanCommandPool final
	{
	public:
		DISALLOW_COPY_AND_MOVE(VulkanCommandPool);

		VulkanCommandPool();

		void Initialize(const VulkanDevice& device);

		explicit operator const vk::raii::CommandPool& () const
		{
			return mCommandPool;
		}

		const vk::raii::CommandPool& GetInstance() const
		{
			return mCommandPool;
		}
		
	private:
		vk::raii::CommandPool mCommandPool;
	};
}

