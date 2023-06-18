#pragma once

#include "Core/Utils.h"
#include "vulkan/vulkan_raii.hpp"

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanCommandPool
	{
	public:
		DISALLOW_COPY_AND_MOVE(VulkanCommandPool);

		explicit operator const vk::raii::CommandPool& () const
		{
			return mCommandPool;
		}
		
	private:
		vk::raii::CommandPool mCommandPool;
	};
}

