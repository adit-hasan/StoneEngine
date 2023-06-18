#pragma once
#include "../IRenderer.h"

#include "Core/Utils.h"
#include "vulkan/vulkan_raii.hpp"

struct GLFWwindow;

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSwapchain;
	class VulkanGraphicsPipeline;

	class VulkanRenderer final : public IRenderer
	{
	public:
		DISALLOW_COPY_AND_MOVE(VulkanRenderer);

		virtual void Initialize() override;
		VulkanRenderer(GLFWwindow* window);
		virtual ~VulkanRenderer();

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
