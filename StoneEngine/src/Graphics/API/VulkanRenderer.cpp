#include "VulkanRenderer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../../pch.h"
#include "Core/Logger.h"

#include "Graphics/API/VulkanInstance.h"
#include "Graphics/API/VulkanDevice.h"
#include "Graphics/API/VulkanSwapchain.h"
#include "Graphics/API/VulkanGraphicsPipeline.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	VulkanRenderer::VulkanRenderer(GLFWwindow* window) :
		mInstance(std::make_unique<VulkanInstance>()),
		mDevice(std::make_unique<VulkanDevice>()),
        mSwapChain(std::make_unique<VulkanSwapchain>()),
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

			mFrameBuffers.emplace_back(mDevice->GetLogicalDevice(), framebufferCreateInfo);
		}
	}
}