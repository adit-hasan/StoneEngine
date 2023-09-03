#include "VulkanFrameBuffer.h"
#include "VulkanDevice.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	VulkanFrameBuffer::VulkanFrameBuffer(vk::raii::Framebuffer buffer, VulkanDevice* device)  noexcept
		: mFrameBuffer(std::move(buffer)), mDevice(device)
	{
	}

	VulkanFrameBuffer::VulkanFrameBuffer(VulkanFrameBuffer&& other) noexcept
		: mFrameBuffer(std::move(other.mFrameBuffer)), mDevice(other.mDevice)
	{
	}

	VkFramebuffer VulkanFrameBuffer::GetHandle() const
	{
		return *mFrameBuffer;
	}
}
