#pragma once
#include "../IRenderer.h"

#include "Core/Utils.h"
#include "vulkan/vulkan_raii.hpp"
#include "Graphics/API/VulkanFrameBuffer.h"
#include "Graphics/API/VulkanFrameContext.h"

struct GLFWwindow;

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSwapchain;
	class VulkanGraphicsPipeline;
	class VulkanCommandPool;

	class VulkanRenderer final : public IRenderer
	{
	public:
		DISALLOW_COPY_AND_MOVE(VulkanRenderer);

		virtual void Initialize() override;
		// TODO: Tear down function,
		// Command buffers need to be explicitly destroyed before the command pool
		VulkanRenderer(GLFWwindow* window);
		virtual ~VulkanRenderer();


		void DrawFrame() const override;
		void RecordCommandBuffer(
			const std::function<void(const vk::raii::CommandBuffer&, const vk::RenderPass&)>& recordFunction,
			int imageIndex,
			const vk::raii::CommandBuffer& commandBuffer) const;

	private:
		static constexpr int sFramesInFlight = 2;
		mutable int mCurrentFrame = 0;
		void BeginRenderPass(const vk::raii::CommandBuffer& commandBuffer, const vk::RenderPass& renderPass, U32 imageIndex) const;
		void EndRenderPass(const vk::raii::CommandBuffer& commandBuffer) const;
		void BeginCommandBuffer(const vk::raii::CommandBuffer& commandBuffer) const;
		void EndCommandBuffer(const vk::raii::CommandBuffer& commandBuffer) const;

		void RecreateSwapchain() const;

		std::unique_ptr<VulkanInstance> mInstance;
		std::unique_ptr<VulkanDevice> mDevice;
		vk::raii::SurfaceKHR mSurface;
		std::unique_ptr<VulkanSwapchain> mSwapChain;
		std::unique_ptr<VulkanCommandPool> mCommandPool;
		std::unique_ptr<VulkanGraphicsPipeline> mGraphicsPipeline;

		std::array<VulkanFrameContext, sFramesInFlight> mFrameContexts;
		std::vector<VulkanFrameBuffer> mFrameBuffers;

		vk::raii::PipelineLayout mPipelineLayout;
		GLFWwindow* mWindow; // non-owning, if Window dies, so does VulkanRenderer
	};
}
