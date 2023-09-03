#pragma once
#include "../IRenderer.h"

#include "Core/Utils.h"
#include "vulkan/vulkan_raii.hpp"
#include "Graphics/API/VulkanFrameBuffer.h"

struct GLFWwindow;

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSwapchain;
	class VulkanGraphicsPipeline;
	class VulkanCommandPool;
	class VulkanCommandBuffer;

	class VulkanRenderer final : public IRenderer
	{
	public:
		DISALLOW_COPY_AND_MOVE(VulkanRenderer);

		virtual void Initialize() override;
		// TODO: Tear down function,
		// Command buffers need to be explicitly destroyed before the command pool
		VulkanRenderer(GLFWwindow* window);
		virtual ~VulkanRenderer();

		void BeginRenderPass(const vk::RenderPass& renderPass, U32 imageIndex) const;
		void EndRenderPass() const;
		void BeginCommandBuffer() const;
		void EndCommandBuffer() const;

		void DrawFrame() const;
	private:
		std::unique_ptr<VulkanInstance> mInstance;
		std::unique_ptr<VulkanDevice> mDevice;
		vk::raii::SurfaceKHR mSurface;
		std::unique_ptr<VulkanSwapchain> mSwapChain;
		std::unique_ptr<VulkanCommandPool> mCommandPool;
		std::unique_ptr<VulkanCommandBuffers> mCommandBuffers;
		std::unique_ptr<VulkanGraphicsPipeline> mGraphicsPipeline;

		std::vector<VulkanFrameBuffer> mFrameBuffers;

		vk::raii::PipelineLayout mPipelineLayout;
		GLFWwindow* mWindow; // non-owning, if Window dies, so does VulkanRenderer
	};
}
