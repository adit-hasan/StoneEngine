#pragma once

#include "Core/Utils.h"
#include "VulkanDevice.h"

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanFrameBuffer final
	{
	public:
		DISALLOW_COPY(VulkanFrameBuffer);

		VulkanFrameBuffer() = default;

		explicit operator const vk::raii::Framebuffer& () const 
		{
			return mFrameBuffer;
		}

	private:
		vk::raii::Framebuffer mFrameBuffer;
		VulkanDevice* mDevice;
	};
}


