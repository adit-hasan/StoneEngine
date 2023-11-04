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

namespace StoneEngine::Graphics::API::Vulkan
{
	VulkanRenderer::VulkanRenderer(GLFWwindow* window) :
		mInstance(std::make_unique<VulkanInstance>()),
		mDevice(std::make_unique<VulkanDevice>()),
        mSwapChain(std::make_unique<VulkanSwapchain>()),
		mCommandPool(std::make_unique<VulkanCommandPool>()),
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
        
		mSwapChain->Initialize(&mSurface, mDevice.get(), width, height);

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

		for (VulkanFrameContext& frameContext : mFrameContexts) {
			frameContext = VulkanFrameContext(mDevice.get(), *mCommandPool.get());
		}
	}

	void VulkanRenderer::DrawFrame() const
	{
		// Waiting for Success signal
		// If we want controlled frame rate we want to wait on Timeout
		const auto& frameContext = mFrameContexts[mCurrentFrame];
		frameContext.ResetFences();
		frameContext.WaitForFences();

		const auto [result, imageIndex] = mSwapChain->AcquireNextImage(UINT16_MAX, *frameContext.GetImageAcquiredSemaphore());
		Core::FatalAssert(result == vk::Result::eSuccess, "Failed to acquire image");
		Core::FatalAssert(imageIndex < mSwapChain->GetImages().size(), "Invalid image index returned from VulkanSwapchain::AcquireNextImage");
		
		RecordCommandBuffer([&](const vk::raii::CommandBuffer& commandBuffer, const vk::RenderPass& renderPass)
		{
			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mGraphicsPipeline->GetPipeline());
			const auto& extent = mGraphicsPipeline->GetSwapchainExtent();

			vk::Viewport viewport(0.0f, 0.0f, (float)extent.width, (float)extent.height, 0.0f, 1.0f);
			vk::Rect2D scissor({ 0, 0 }, extent);
			commandBuffer.setViewport(0, viewport);
			commandBuffer.setScissor(0, scissor);

			// Adhoc draw command
			commandBuffer.draw(3, 1, 0, 0);
		}, imageIndex, mFrameContexts[mCurrentFrame].GetCommandBuffer());

		vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
		vk::SubmitInfo         submitInfo(*frameContext.GetImageAcquiredSemaphore(), waitDestinationStageMask, *frameContext.GetCommandBuffer(), *frameContext.GetIsRenderCompleteSemaphore());
		mDevice->GetGraphicsQueue().submit(submitInfo, *frameContext.GetFence());

		vk::PresentInfoKHR presentInfoKHR(*frameContext.GetIsRenderCompleteSemaphore(), *mSwapChain->Get(), imageIndex);
		const auto presentResult = mDevice->GetPresentQueue().presentKHR(presentInfoKHR);
	
		mDevice->GetLogicalDevice().waitIdle();

		mCurrentFrame = (mCurrentFrame + 1) % sFramesInFlight;
	}

	void VulkanRenderer::RecordCommandBuffer(
		const std::function<void(
			const vk::raii::CommandBuffer&, 
			const vk::RenderPass&)>& recordFunction,
		int imageIndex,
		const vk::raii::CommandBuffer& commandBuffer) const
	{
		commandBuffer.reset();
		BeginCommandBuffer(commandBuffer);

		const auto& renderPass = *mGraphicsPipeline->GetRenderPass();

		BeginRenderPass(commandBuffer, renderPass, imageIndex);

		if (recordFunction)
		{
			recordFunction(commandBuffer, renderPass);
		}
		EndRenderPass(commandBuffer);
		EndCommandBuffer(commandBuffer);
	}

	void VulkanRenderer::BeginCommandBuffer(const vk::raii::CommandBuffer& commandBuffer) const
	{
		vk::CommandBufferBeginInfo beginInfo;
		commandBuffer.begin(beginInfo);
	}

	void VulkanRenderer::EndCommandBuffer(const vk::raii::CommandBuffer& commandBuffer) const
	{
		commandBuffer.end();
	}

	void VulkanRenderer::BeginRenderPass(const vk::raii::CommandBuffer& commandBuffer, const vk::RenderPass& renderPass, U32 imageIndex) const
	{
		vk::RenderPassBeginInfo renderPassBeginInfo(renderPass, mFrameBuffers[imageIndex].GetHandle());
		renderPassBeginInfo.renderArea.setOffset({ 0,0 });
		renderPassBeginInfo.renderArea.setExtent(mGraphicsPipeline->GetSwapchainExtent());
		renderPassBeginInfo.clearValueCount = 1;

		vk::ClearValue clearValue = vk::ClearColorValue(0.8f, 0.2f, 0.2f, 0.2f);
		renderPassBeginInfo.pClearValues = &clearValue;

		commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
	}

	void VulkanRenderer::EndRenderPass(const vk::raii::CommandBuffer& commandBuffer) const
	{
		commandBuffer.endRenderPass();
	}
}