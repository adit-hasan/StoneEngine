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
		mGraphicsPipeline(std::make_unique<VulkanGraphicsPipeline>()),
		mSurface(nullptr),
		mWindow(window),
		mPipelineLayout(nullptr){}

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

		// TODO: Extract this out to resource manager, other classes should only have handles
		auto vertexShaderModule = LoadShader("C:\\Users\\adith\\source\\repos\\StoneEngine\\StoneEngine\\Resources\\Shaders\\vert.spv");
		auto fragmentShaderModule = LoadShader("C:\\Users\\adith\\source\\repos\\StoneEngine\\StoneEngine\\Resources\\Shaders\\frag.spv");

		auto vertShaderStageCreateInfo = vk::PipelineShaderStageCreateInfo(
											vk::PipelineShaderStageCreateFlags(),
											vk::ShaderStageFlagBits::eVertex,
											*vertexShaderModule,
											"main"
											);

		auto fragmentShaderStageCreateInfo = vk::PipelineShaderStageCreateInfo(
			vk::PipelineShaderStageCreateFlags(),
			vk::ShaderStageFlagBits::eFragment,
			*fragmentShaderModule,
			"main"
		);

		vk::PipelineShaderStageCreateInfo shaderStageCreateInfo[] = { vertShaderStageCreateInfo, fragmentShaderStageCreateInfo};
	
		// Fixed function stages

		// Dynamic state
		constexpr std::array<vk::DynamicState, 2> dynamicStates = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		vk::PipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.setDynamicStates(dynamicStates);

		// vertext input
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo(
			vk::PipelineVertexInputStateCreateFlags(),
			0,
			nullptr,
			0,
			nullptr
		);

		// input assembly
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo(
			vk::PipelineInputAssemblyStateCreateFlags(),
			vk::PrimitiveTopology::eTriangleList,
			VK_FALSE,
			nullptr
		);

		// viewports and scissors
		vk::Viewport viewport(
			0.0f,
			0.0f,
			(float)mSwapChain->GetExtent().width,
			(float)mSwapChain->GetExtent().height,
			0.0f,
			1.0f
		);

		vk::Rect2D scissor({ 0,0 }, mSwapChain->GetExtent());

		vk::PipelineViewportStateCreateInfo viewportStateInfo(
			vk::PipelineViewportStateCreateFlags(),
			1,
			&viewport,
			1,
			&scissor
		);

		// rasterizer
		vk::PipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		rasterizer.frontFace = vk::FrontFace::eClockwise;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		// multi-sampling
		// TODO: enable this 
		vk::PipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		// colour blending

		vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
		colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
		colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
		colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
		colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
		colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

		vk::PipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = vk::LogicOp::eCopy;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		// pipeline layout creation
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		mPipelineLayout = mDevice->GetLogicalDevice().createPipelineLayout(pipelineLayoutInfo);
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