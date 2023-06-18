#pragma once

#include "Core/Utils.h"
#include "vulkan/vulkan_raii.hpp"

namespace StoneEngine::Graphics::API::Vulkan
{ 
	class VulkanCommandBuffer
	{
	public:
		DISALLOW_COPY_AND_MOVE(VulkanCommandBuffer);

	explicit operator const vk::raii::CommandBuffer& () const
	{
		return mCommandPool;
	}

	private:
		vk::raii::CommandBuffer mCommandPool;
	};
}

