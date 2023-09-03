#pragma once

#include "Core/Utils.h"
#include "vulkan/vulkan_raii.hpp"

namespace StoneEngine::Graphics::API::Vulkan
{ 
	class VulkanCommandBuffers final
	{
	public:
		DISALLOW_COPY_AND_MOVE(VulkanCommandBuffers);

		VulkanCommandBuffers(vk::raii::CommandBuffers buffer);
		void Begin(U32 bufferIndex) const;
		void End(U32 bufferIndex) const;
		void BindGraphicsPipeline(U32 bufferIndex, vk::PipelineBindPoint pipelineType, const vk::Pipeline& pipeline) const;
		void BindViewPortAndScissor(U32 bufferIndex, const vk::Viewport& viewport, const vk::Rect2D& scissor) const;

	explicit operator const vk::raii::CommandBuffers& () const
	{
		return mCommandBuffers;
	}

	const vk::raii::CommandBuffer& operator[](size_t index) const
	{
		if (index < mCommandBuffers.size())
		{
			return mCommandBuffers[index];
		}
		else
		{
			return nullptr;
		}
	}

	private:
		// Vector of CommandBuffer
		vk::raii::CommandBuffers mCommandBuffers;
	};
}

