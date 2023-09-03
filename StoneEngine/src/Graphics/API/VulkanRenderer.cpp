#include "VulkanRenderer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../../pch.h"
#include "Core/Logger.h"

#include "Graphics/API/VulkanInstance.h"
#include "Graphics/API/VulkanDevice.h"
#include "Graphics/API/VulkanSwapchain.h"
#include "Graphics/API/VulkanGraphicsPipeline.h"
#include "Graphics/API/VulkanCommandPool.h"
#include "Graphics/API/VulkanCommandBuffers.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	VulkanRenderer::VulkanRenderer(GLFWwindow* window) :
		mInstance(std::make_unique<VulkanInstance>()),
		mDevice(std::make_unique<VulkanDevice>()),
        mSwapChain(std::make_unique<VulkanSwapchain>()),
		mCommandPool(std::make_unique<VulkanCommandPool>()),
		mCommandBuffers(nullptr),
		mSurface(nullptr),
		mWindow(window),
		mPipelineLayout(nullptr)
	{
		mGraphicsPipeline.reset();
	}

	VulkanRenderer::~VulkanRenderer() {}

	void VulkanRenderer::Initialize()
	{
		mInstance->Initialize();

		VkSurfaceKHR surface;

		if (glfwCreateWindowSurface(*mInstance->GetInstance(), mWindow, nullptr, &surface) != VK_SUCCESS)
		{
			Core::LogError("Failed to create GLFW window surface!");
		}

		mSurface = vk::raii::SurfaceKHR(mInstance->GetInstance(), surface);

		mDevice->Initialize(mInstance.get(),mSurface);

		int width, height;
		glfwGetFramebufferSize(mWindow, &width, &height);
        
		mSwapChain->Initialize(mSurface, *mDevice.get(), width, height);

		mGraphicsPipeline.reset(new VulkanGraphicsPipeline(
			"C:\\Users\\adith\\source\\repos\\StoneEngine\\StoneEngine\\Resources\\Shaders\\vert.spv",
			"C:\\Users\\adith\\source\\repos\\StoneEngine\\StoneEngine\\Resources\\Shaders\\frag.spv",
			mDevice.get(),
			mSwapChain->GetExtent(),
			mSwapChain->GetFormat()
		));

		// Create framebuffers
		const auto& imageViews = mSwapChain->GetImageViews();
		auto [eWidth, eHeight] = mSwapChain->GetExtent();
		const auto& renderPass = mGraphicsPipeline->GetRenderPass();

		mFrameBuffers.reserve(imageViews.size());

		std::array<vk::ImageView, 1> attachments;

		for (const auto& imageView : imageViews)
		{
			attachments[0] = *imageView;

			vk::FramebufferCreateInfo framebufferCreateInfo({}, *renderPass, attachments, eWidth, eHeight, 1);
			vk::raii::Framebuffer frameBuffer(mDevice->GetLogicalDevice(), framebufferCreateInfo);
			mFrameBuffers.emplace_back(std::move(frameBuffer), mDevice.get());
		}

		// Create CommandPool
		mCommandPool->Initialize(*mDevice.get());

		// Create command buffers
		vk::CommandBufferAllocateInfo commandBufferCreateInfo(
			*mCommandPool->GetInstance(),
			vk::CommandBufferLevel::ePrimary,
			1
		);

		vk::raii::CommandBuffers commandBuffers(mDevice->GetLogicalDevice(), commandBufferCreateInfo);

		mCommandBuffers.reset(new VulkanCommandBuffers(std::move(commandBuffers)));
	}

	void VulkanRenderer::DrawFrame() const
	{
		U32 imageIndex = 0;
		const auto& renderPass = *mGraphicsPipeline->GetRenderPass();

		BeginCommandBuffer();
		BeginRenderPass(renderPass, imageIndex);

		mCommandBuffers->BindGraphicsPipeline(imageIndex, vk::PipelineBindPoint::eGraphics, mGraphicsPipeline->GetPipeline());
		const auto& extent = mGraphicsPipeline->GetSwapchainExtent();

		vk::Viewport viewport(0.0f, 0.0f, (float)extent.width, (float)extent.height, 0.0f, 1.0f);
		vk::Rect2D scissor({ 0, 0 }, extent);
		mCommandBuffers->BindViewPortAndScissor(imageIndex, viewport, scissor);

		// Adhoc draw command
		(*mCommandBuffers)[imageIndex].draw(3, 1, 0, 0);

		EndRenderPass();
		EndCommandBuffer();
	}

	void VulkanRenderer::BeginCommandBuffer() const
	{
		mCommandBuffers->Begin(0);
	}

	void VulkanRenderer::EndCommandBuffer() const
	{
		mCommandBuffers->End(0);
	}

	void VulkanRenderer::BeginRenderPass(const vk::RenderPass& renderPass, U32 imageIndex) const
	{
		vk::RenderPassBeginInfo renderPassBeginInfo(renderPass, mFrameBuffers[imageIndex].GetHandle());
		renderPassBeginInfo.renderArea.setOffset({ 0,0 });
		renderPassBeginInfo.renderArea.setExtent(mGraphicsPipeline->GetSwapchainExtent());
		renderPassBeginInfo.clearValueCount = 1;

		vk::ClearValue clearValue = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassBeginInfo.pClearValues = &clearValue;

		// Just use the first buffer for now
		(*mCommandBuffers)[0].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
	}

	void VulkanRenderer::EndRenderPass() const
	{
		(*mCommandBuffers)[0].endRenderPass();
	}
}