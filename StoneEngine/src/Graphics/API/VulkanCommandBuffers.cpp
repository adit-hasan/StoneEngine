#include "VulkanCommandBuffers.h"

#include "vulkan/vulkan.hpp"

namespace StoneEngine::Graphics::API::Vulkan
{
	VulkanCommandBuffers::VulkanCommandBuffers(vk::raii::CommandBuffers buffer)
		: mCommandBuffers(std::move(buffer))
	{
	}

	void VulkanCommandBuffers::Begin(U32 bufferIndex) const
	{
		vk::CommandBufferBeginInfo beginInfo;
		mCommandBuffers[bufferIndex].begin(beginInfo);
	}

	void VulkanCommandBuffers::End(U32 bufferIndex) const
	{
		mCommandBuffers[bufferIndex].end();
	}

	void VulkanCommandBuffers::BindGraphicsPipeline(U32 bufferIndex, vk::PipelineBindPoint pipelineType, const vk::Pipeline& pipeline) const
	{
		mCommandBuffers[bufferIndex].bindPipeline(pipelineType,	pipeline);
	}

	void VulkanCommandBuffers::BindViewPortAndScissor(U32 bufferIndex, const vk::Viewport& viewport, const vk::Rect2D& scissor) const
	{
		mCommandBuffers[bufferIndex].setViewport(0, viewport);
		mCommandBuffers[bufferIndex].setScissor(0, scissor);
	}
}
