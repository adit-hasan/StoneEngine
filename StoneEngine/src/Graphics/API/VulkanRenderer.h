#pragma once
#include "../IRenderer.h"
#include "Graphics/API/VulkanInstance.h"
#include "Graphics/API/VulkanDevice.h"
#include "Graphics/API/VulkanSwapchain.h"
#include "Graphics/API/VulkanGraphicsPipeline.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanRenderer final : public IRenderer
	{
	public:
		DISALLOW_COPY_AND_MOVE(VulkanRenderer);

		virtual void Initialize() override;
		VulkanRenderer(GLFWwindow* window);
		virtual ~VulkanRenderer() {}

	private:
		std::unique_ptr<VulkanInstance> mInstance;
		std::unique_ptr<VulkanDevice> mDevice;
		vk::raii::SurfaceKHR mSurface;
		std::unique_ptr<VulkanSwapchain> mSwapChain;
		std::unique_ptr<VulkanGraphicsPipeline> mGraphicsPipeline;
		
		std::vector<vk::raii::Framebuffer> mFrameBuffers;

		vk::raii::PipelineLayout mPipelineLayout;
		GLFWwindow* mWindow; // non-owning, if Window dies, so does VulkanRenderer
	};
}

