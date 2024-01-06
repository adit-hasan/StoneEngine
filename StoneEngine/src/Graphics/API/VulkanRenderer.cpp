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
#include "Graphics/API/VulkanBuffer.h"
#include "Graphics/API/VulkanDescriptorPool.h"
#include "Graphics/Models/ModelViewProjectionMatrix.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

namespace StoneEngine::Graphics::API::Vulkan
{
	VulkanRenderer::VulkanRenderer(GLFWwindow* window) :
		mInstance(std::make_unique<VulkanInstance>()),
        mSwapChain(std::make_unique<VulkanSwapchain>()),
		mWindow(window),
		mPipelineLayout(nullptr)
	{
		mGraphicsPipeline.reset();
	}

	VulkanRenderer::~VulkanRenderer() 
	{
		for (auto& frameContext : mFrameContexts)
		{
			frameContext.DestroyCommandBuffer();
		}
	}

	void VulkanRenderer::Initialize()
	{
		VkSurfaceKHR surface;

		if (glfwCreateWindowSurface(*mInstance->GetInstance(), mWindow, nullptr, &surface) != VK_SUCCESS)
		{
			Core::LogError("Failed to create GLFW window surface!");
		}

		mSurface = vk::raii::SurfaceKHR(mInstance->GetInstance(), surface);

		mDevice = std::make_unique<VulkanDevice>(mInstance.get(), mSurface);

		int width, height;
		glfwGetFramebufferSize(mWindow, &width, &height);

		mSwapChain->Initialize(&mSurface, mDevice.get(), width, height);

		mGraphicsPipeline.reset(new VulkanGraphicsPipeline(
			"C:\\Users\\adith\\source\\repos\\StoneEngine\\StoneEngine\\Resources\\Shaders\\vert.spv",
			"C:\\Users\\adith\\source\\repos\\StoneEngine\\StoneEngine\\Resources\\Shaders\\frag.spv",
			mDevice.get(),
			mSwapChain->GetFormat()
		));

		// Create framebuffers
		RecreateFramebuffers();

		// Create CommandPools
		mPersistentCommandPool = std::make_unique<VulkanCommandPool>(CommandPoolPurpose::PersistentBuffers, *mDevice.get());
		mTransientCommandPool = std::make_unique<VulkanCommandPool>(CommandPoolPurpose::TransientBuffers, *mDevice.get());

		mDescriptorPool = std::make_unique<VulkanDescriptorPool>(mDevice.get(), sFramesInFlight);
		std::stack < vk::raii::DescriptorSet, std::vector<vk::raii::DescriptorSet>> descriptorSets(
			mDescriptorPool->AllocateDescriptorSets(mGraphicsPipeline->GetDescriptorSetLayout())
		);
		for (VulkanFrameContext& frameContext : mFrameContexts) {
			auto uniformBuffer = VulkanBufferBuilder::BuildBuffer(VulkanBufferType::UniformBuffer, sizeof(ModelViewProjectionMatrix), *mDevice.get());
			auto descriptorSet = std::move(descriptorSets.top());
			frameContext = VulkanFrameContext(
				mDevice.get(), 
				*mPersistentCommandPool.get(), 
				std::move(uniformBuffer),
				std::move(descriptorSet));
			descriptorSets.pop();
		}

		// -- TEMP CODE --
		// Vertices
		mVertices[0] = { glm::vec2(-0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f) };
		mVertices[1] = { glm::vec2(0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f) };
		mVertices[2] = { glm::vec2(0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f) };
		mVertices[3] = { glm::vec2(-0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f) };
		mVertexBuffer = VulkanBufferBuilder::BuildBuffer(VulkanBufferType::VertexBuffer, sizeof(mVertices), *mDevice.get());
		mIndices = { 0, 1, 2, 2, 3, 0 };
		mIndexBuffer = VulkanBufferBuilder::BuildBuffer(VulkanBufferType::IndexBuffer, sizeof(mIndices), *mDevice.get());
		
		// Move timing to engine core
		mClock.Reset();
		
		// -- TEMP CODE --
	}

	void VulkanRenderer::RecreateFramebuffers()
	{
		mFrameBuffers.clear();
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
	}

	void VulkanRenderer::DrawFrame()
	{
		// Waiting for Success signal
		// If we want controlled frame rate we want to wait on Timeout
		Core::LogDebug("Drawing frame: {}", mCurrentFrame);
		const auto& frameContext = mFrameContexts[mCurrentFrame];
		frameContext.WaitForFences();

		unsigned int imageIndex;
		try
		{
			const auto [result, index] = mSwapChain->AcquireNextImage(UINT16_MAX, *frameContext.GetImageAcquiredSemaphore());
			imageIndex = index;
		}
		catch (...)
		{
			Recreate();
			return;
		}

		Core::FatalAssert(imageIndex < mSwapChain->GetImages().size(), "Invalid image index returned from VulkanSwapchain::AcquireNextImage");
		
		UploadToDeviceBuffer(mVertices.data(), sizeof(mVertices), *mVertexBuffer->GetBuffer());
		UploadToDeviceBuffer(mIndices.data(), sizeof(mIndices), *mIndexBuffer->GetBuffer());
		
		// Update uniform buffer
		const float time = mClock.GetElapsedTimeInSeconds();
		auto* uniformBufferMemory = frameContext.GetUniformBuffer().GetMappedMemory();
		ModelViewProjectionMatrix mvp{};
		mvp.Model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		mvp.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		mvp.Projection = glm::perspective(glm::radians(45.0f), mSwapChain->GetExtent().width / (float)mSwapChain->GetExtent().height, 0.1f, 10.0f);
		mvp.Projection[1][1] *= -1;
		memcpy(uniformBufferMemory, &mvp, sizeof(mvp));

		frameContext.ResetFences();
		RecordCommandBuffer([&](const vk::raii::CommandBuffer& commandBuffer, const vk::RenderPass& renderPass)
		{
			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mGraphicsPipeline->GetPipeline());
			const auto& extent = mSwapChain->GetExtent();

			vk::Viewport viewport(0.0f, 0.0f, (float)extent.width, (float)extent.height, 0.0f, 1.0f);
			vk::Rect2D scissor({ 0, 0 }, extent);
			commandBuffer.setViewport(0, viewport);
			commandBuffer.setScissor(0, scissor);

			// Bind buffers
			commandBuffer.bindVertexBuffers(0, { *mVertexBuffer->GetBuffer()}, {0});
			commandBuffer.bindIndexBuffer(*mIndexBuffer->GetBuffer(), 0, vk::IndexType::eUint32);

			// Adhoc draw command
			commandBuffer.drawIndexed(static_cast<U32>(mIndices.size()), 1, 0, 0, 0);
		}, imageIndex, frameContext.GetCommandBuffer());

		vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
		vk::SubmitInfo         submitInfo(*frameContext.GetImageAcquiredSemaphore(), waitDestinationStageMask, *frameContext.GetCommandBuffer(), *frameContext.GetIsRenderCompleteSemaphore());
		mDevice->GetGraphicsQueue().submit(submitInfo, *frameContext.GetFence());

		vk::PresentInfoKHR presentInfoKHR(*frameContext.GetIsRenderCompleteSemaphore(), *mSwapChain->Get(), imageIndex);
		
		try
		{
			const auto presentResult = mDevice->GetPresentQueue().presentKHR(presentInfoKHR);
			if (mShouldResize)
			{
				Recreate();
				mShouldResize = false;
			}
		}
		catch (...)
		{
			Recreate();
		}

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
		renderPassBeginInfo.renderArea.setExtent(mSwapChain->GetExtent());
		renderPassBeginInfo.clearValueCount = 1;

		vk::ClearValue clearValue = vk::ClearColorValue(0.7f, 0.2f, 0.2f, 0.2f);
		renderPassBeginInfo.pClearValues = &clearValue;

		commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
	}

	void VulkanRenderer::EndRenderPass(const vk::raii::CommandBuffer& commandBuffer) const
	{
		commandBuffer.endRenderPass();
	}

	void VulkanRenderer::Recreate()
	{
		int width, height;
		glfwGetFramebufferSize(mWindow, &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(mWindow, &width, &height);
			glfwWaitEvents();
		}
		mDevice->GetLogicalDevice().waitIdle();
		mSwapChain->Recreate(width, height);
		RecreateFramebuffers();
	}

	std::optional<CommandBufferSubmitResponse> VulkanRenderer::UploadToDeviceBuffer(void* data, U64 size, const vk::Buffer& buffer, bool waitOnCompletion)
	{
		auto commandBuffer = mTransientCommandPool->CreateCommandBuffer(*mDevice.get());
		auto stagingBuffer = VulkanBufferBuilder::BuildBuffer(VulkanBufferType::StagingBuffer, size, *mDevice.get());

		stagingBuffer->UploadData(static_cast<U8*>(data), size);
		
		// Submit command to transfer
		vk::CommandBufferBeginInfo beginInfo;
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
		commandBuffer.begin(beginInfo);
		const vk::BufferCopy bufferCopy(0, 0, size);
		commandBuffer.copyBuffer(*stagingBuffer->GetBuffer(), buffer, bufferCopy);
		commandBuffer.end();

		vk::SubmitInfo submitInfo;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &(*commandBuffer);
		submitInfo.pWaitDstStageMask = nullptr;

		vk::raii::Fence fence(mDevice->GetLogicalDevice(), vk::FenceCreateInfo());
		mDevice->GetLogicalDevice().resetFences(*fence);
		mDevice->GetGraphicsQueue().submit(submitInfo, *fence);

		if (waitOnCompletion)
		{
			while (vk::Result::eSuccess != mDevice->GetLogicalDevice().waitForFences(*fence, VK_TRUE, UINT16_MAX))
				;
			return std::nullopt;
		}
		else
		{
			return std::make_optional<CommandBufferSubmitResponse>(std::move(commandBuffer), std::move(fence));
		}
	}
}