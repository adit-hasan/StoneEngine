#pragma once

#include "Core/Utils.h"
#include "VulkanIncludes.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanDevice;
	enum class VulkanBufferType
	{
		VertexBuffer = 0,
		IndexBuffer = 1,
		StagingBuffer = 2,
		BufferTypeCount
	};

	enum class VertexInputRate
	{
		eVertex = VK_VERTEX_INPUT_RATE_VERTEX,
		eInstance = VK_VERTEX_INPUT_RATE_INSTANCE
	};

	class VulkanBuffer final {
	public:
		DISALLOW_COPY(VulkanBuffer);

		void Bind();
		void Unbind();
		void UploadData(void* sourceData, size_t size) const;

		[[nodiscard]] size_t GetSize() const { return mSize; }
		[[nodiscard]] const vk::raii::Buffer& GetBuffer() const { return mBuffer; }
		friend class VulkanBufferBuilder;
	private:
		static U32 GetMemoryType(vk::PhysicalDeviceMemoryProperties memoryProperties, U32 typeBits, vk::MemoryPropertyFlags requirementsMask);
		VulkanBuffer() = default;
		VulkanBuffer(vk::BufferUsageFlags usage, size_t size, vk::MemoryPropertyFlags requiredMemoryProperties, const VulkanDevice& device);

		void* mData;
		size_t mSize;
		vk::BufferUsageFlags mUsage;
		vk::raii::Buffer mBuffer = nullptr;
		vk::raii::DeviceMemory mDeviceMemory = nullptr;
		vk::MemoryRequirements mMemoryRequirements;
	};

	class VulkanBufferBuilder
	{
	public:
		[[nodiscard]] static std::unique_ptr<VulkanBuffer> BuildBuffer(VulkanBufferType bufferType,
			size_t size,
			const VulkanDevice& device);
	};
}
