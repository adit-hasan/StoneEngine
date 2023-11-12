#include "VulkanBuffer.h"
#include "VulkanDevice.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	VulkanBuffer::VulkanBuffer(vk::BufferUsageFlags usage, size_t size, vk::MemoryPropertyFlags requiredMemoryProperties, const VulkanDevice& device)
		: mUsage(usage), mSize(size)
	{
		vk::BufferCreateInfo bufferCreateInfo({}, mSize, mUsage);
		bufferCreateInfo.setSharingMode(vk::SharingMode::eExclusive);
		mBuffer = vk::raii::Buffer(device.GetLogicalDevice(), bufferCreateInfo);
		
		mMemoryRequirements = mBuffer.getMemoryRequirements();
		const auto memoryProperties = device.GetMemoryProperties();

		auto memoryType = VulkanBuffer::GetMemoryType(
			memoryProperties,
			mMemoryRequirements.memoryTypeBits,
			requiredMemoryProperties);
		
		vk::MemoryAllocateInfo memoryAllocateInfo(mMemoryRequirements.size, memoryType);
		mDeviceMemory = vk::raii::DeviceMemory(device.GetLogicalDevice(), memoryAllocateInfo);
		mBuffer.bindMemory(*mDeviceMemory, 0);
	}

	void VulkanBuffer::UploadData(void* sourceData, size_t size) const
	{
		U8* pData = static_cast<uint8_t*>(mDeviceMemory.mapMemory(0, mMemoryRequirements.size));
		memcpy(pData, sourceData, size);
		mDeviceMemory.unmapMemory();
	}

	U32 VulkanBuffer::GetMemoryType(vk::PhysicalDeviceMemoryProperties memoryProperties, U32 typeBits, vk::MemoryPropertyFlags requirementsMask)
	{
		U32 typeIndex = U32(~0);
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			if ((typeBits & 1) && ((memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask))
			{
				typeIndex = i;
				break;
			}
			typeBits >>= 1;
		}
		assert(typeIndex != uint32_t(~0));
		return typeIndex;
	}

	std::unique_ptr<VulkanBuffer> VulkanBufferBuilder::BuildBuffer(
		VulkanBufferType bufferType,
		size_t size,
		const VulkanDevice& device)
	{
		vk::BufferUsageFlags bufferUsage;
		vk::MemoryPropertyFlags memoryProperties;
		switch (bufferType)
		{
		case VulkanBufferType::VertexBuffer:
			// Do we really want to have all vertex buffers as transfer dests
			bufferUsage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
			memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
			break;
		case VulkanBufferType::IndexBuffer:
			bufferUsage = vk::BufferUsageFlagBits::eIndexBuffer;
			break;
		case VulkanBufferType::StagingBuffer:
			bufferUsage = vk::BufferUsageFlagBits::eTransferSrc;
			memoryProperties = vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible;
			break;
		default:
			bufferUsage = vk::BufferUsageFlagBits::eVertexBuffer;
			break;
		}
		auto buffer = new VulkanBuffer(bufferUsage, size, memoryProperties, device);
		return std::unique_ptr<VulkanBuffer>(buffer);
	}
}