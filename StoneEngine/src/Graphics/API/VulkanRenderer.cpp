#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "../../pch.h"
#include "VulkanRenderer.h"
#include "Core/Logger.h"

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
	}

	vk::raii::ShaderModule VulkanRenderer::LoadShader(const std::string& shaderFilePath)
	{
		auto byteCode = Core::ReadFileAsBinary(shaderFilePath);

		vk::ShaderModuleCreateInfo createInfo({}, 
			byteCode.size(), 
			reinterpret_cast<const uint32_t*>(byteCode.data()), 
			nullptr);

		return vk::raii::ShaderModule(mDevice->GetLogicalDevice(), createInfo);
	}
}