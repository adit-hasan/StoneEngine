#pragma once
#include "../IRenderer.h"

#include "Core/Utils.h"
#include "VulkanIncludes.h"

#include "Graphics/API/VulkanFrameBuffer.h"
#include "Graphics/API/VulkanFrameContext.h"
#include <Graphics/Models/VertexData.h>

struct GLFWwindow;

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSwapchain;
	class VulkanGraphicsPipeline;
	class VulkanCommandPool;
	class VulkanBuffer;

	struct CommandBufferSubmitResponse
	{
		vk::raii::CommandBuffer CommandBuffer;
		vk::raii::Fence Fence;
	};

	class VulkanRenderer final : public IRenderer
	{
	public:
		DISALLOW_COPY_AND_MOVE(VulkanRenderer);

		virtual void Initialize() override;
		// TODO: Tear down function,
		// Command buffers need to be explicitly destroyed before the command pool
		VulkanRenderer(GLFWwindow* window);
		virtual ~VulkanRenderer();

		void DrawFrame() override;
		void RecordCommandBuffer(
			const std::function<void(const vk::raii::CommandBuffer&, const vk::RenderPass&)>& recordFunction,
			int imageIndex,
			const vk::raii::CommandBuffer& commandBuffer) const;
		void OnResize() override { mShouldResize = true; }
		std::optional<CommandBufferSubmitResponse> UploadDataToBuffer(
			void* sourceData, 
			U64 size,
			const vk::Buffer& buffer,
			bool waitOnCompletion = true);

	private:
		static constexpr int sFramesInFlight = 2;
		mutable int mCurrentFrame = 0;
		bool mShouldResize = false;

		void BeginRenderPass(const vk::raii::CommandBuffer& commandBuffer, const vk::RenderPass& renderPass, U32 imageIndex) const;
		void EndRenderPass(const vk::raii::CommandBuffer& commandBuffer) const;
		void BeginCommandBuffer(const vk::raii::CommandBuffer& commandBuffer) const;
		void EndCommandBuffer(const vk::raii::CommandBuffer& commandBuffer) const;

		void Recreate();
		void RecreateFramebuffers();

		// TODO: Temp data
		std::array<VertexData, 4> mVertices;
		std::array<U32, 6> mIndices;

		// Persistent Buffer resources
		std::unique_ptr<VulkanCommandPool> mPersistentCommandPool = nullptr;
		std::unique_ptr<VulkanBuffer> mVertexBuffer = nullptr;
		std::unique_ptr<VulkanBuffer> mIndexBuffer = nullptr;

		// Transient buffer resources
		// To be used for one-off tasks such as data transfers
		std::unique_ptr<VulkanCommandPool> mTransientCommandPool = nullptr;

		std::unique_ptr<VulkanInstance> mInstance = nullptr;
		std::unique_ptr<VulkanDevice> mDevice = nullptr;
		vk::raii::SurfaceKHR mSurface = nullptr;
		std::unique_ptr<VulkanSwapchain> mSwapChain;
		std::unique_ptr<VulkanGraphicsPipeline> mGraphicsPipeline;

		std::array<VulkanFrameContext, sFramesInFlight> mFrameContexts;
		std::vector<VulkanFrameBuffer> mFrameBuffers;

		vk::raii::PipelineLayout mPipelineLayout;
		GLFWwindow* mWindow; // non-owning, if Window dies, so does VulkanRenderer
	};
}
