#pragma once

#include "Core/Utils.h"

#include "VulkanIncludes.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanDevice;

	class VulkanFrameBuffer final
	{
	public:
		DISALLOW_COPY(VulkanFrameBuffer);

		VulkanFrameBuffer(vk::raii::Framebuffer buffer, VulkanDevice* device) noexcept;
		VulkanFrameBuffer(VulkanFrameBuffer&& other) noexcept;

		explicit operator const vk::raii::Framebuffer& () const 
		{
			return mFrameBuffer;
		}

		[[nodiscard]] VkFramebuffer GetHandle() const;

	private:
		vk::raii::Framebuffer mFrameBuffer;
		VulkanDevice* mDevice;
	};
}


