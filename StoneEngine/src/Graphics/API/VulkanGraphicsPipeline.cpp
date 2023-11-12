#include "VulkanGraphicsPipeline.h"
#include "VulkanDevice.h"
#include "VulkanUtils.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	VulkanGraphicsPipeline::VulkanGraphicsPipeline(
		std::string_view vertexShaderPath,
		std::string_view fragmentShaderPath,
		VulkanDevice* device,
		vk::SurfaceFormatKHR format
	) :
		mDevice(device),
		mVertexShaderModule(nullptr),
		mFragmentShaderModule(nullptr),
		mPipelineLayout(nullptr),
		mRenderPass(nullptr),
		mPipeline(nullptr)
	{
		Core::LogInfo("Creating VulkanGraphicsPipeline");
		// Maybe this constructor is doing too much???

		// TODO: Extract this out to resource manager, other classes should only have handles
		auto vertexShaderModule = LoadShader(vertexShaderPath);
		auto fragmentShaderModule = LoadShader(fragmentShaderPath);

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

		vk::PipelineShaderStageCreateInfo shaderStageCreateInfo[] = { vertShaderStageCreateInfo, fragmentShaderStageCreateInfo };
		
		// Fixed function stages

		// Dynamic state
		constexpr std::array<vk::DynamicState, 2> dynamicStates = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		vk::PipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.setDynamicStates(dynamicStates);

		vk::VertexInputBindingDescription bindingInfo = GetBindingDescription<VertexData>(0, vk::VertexInputRate::eVertex);
		vk::VertexInputAttributeDescription* attributeInfos = GetAttributeDescription(0).data();
		// vertex input
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo(
			vk::PipelineVertexInputStateCreateFlags(),
			1,
			&bindingInfo,
			2,
			attributeInfos
		);

		// input assembly
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo(
			vk::PipelineInputAssemblyStateCreateFlags(),
			vk::PrimitiveTopology::eTriangleList,
			VK_FALSE,
			nullptr
		);

		// Might not need to pass in the view port valuesb
		vk::PipelineViewportStateCreateInfo viewportStateInfo(
			vk::PipelineViewportStateCreateFlags(),
			1,
			nullptr,
			1,
			nullptr
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

		// Create render pass
		vk::AttachmentDescription attachmentDescription(
			{},
			format.format,
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::ePresentSrcKHR
		);

		vk::AttachmentReference attachmentReference(
			0,
			vk::ImageLayout::eColorAttachmentOptimal
		);

		vk::SubpassDescription subpassDescription;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &attachmentReference;

		vk::RenderPassCreateInfo renderPassInfo(
			{},
			1,
			&attachmentDescription,
			1,
			&subpassDescription
		);

		mRenderPass = vk::raii::RenderPass(mDevice->GetLogicalDevice(), renderPassInfo);

		// Create the graphics pipeline
		vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
		graphicsPipelineCreateInfo.stageCount = 2;
		graphicsPipelineCreateInfo.pStages = shaderStageCreateInfo;
		graphicsPipelineCreateInfo.pVertexInputState = &vertexInputInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
		graphicsPipelineCreateInfo.pTessellationState = nullptr;
		graphicsPipelineCreateInfo.pViewportState = &viewportStateInfo;
		graphicsPipelineCreateInfo.pRasterizationState = &rasterizer;
		graphicsPipelineCreateInfo.pMultisampleState = &multisampling;
		graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
		graphicsPipelineCreateInfo.pColorBlendState = &colorBlending;
		graphicsPipelineCreateInfo.pDynamicState = &dynamicState;
		graphicsPipelineCreateInfo.layout = *mPipelineLayout;
		graphicsPipelineCreateInfo.renderPass = *mRenderPass;

		mPipeline = vk::raii::Pipeline(mDevice->GetLogicalDevice(), nullptr, graphicsPipelineCreateInfo);
	}

	vk::raii::ShaderModule VulkanGraphicsPipeline::LoadShader(std::string_view shaderFilePath)
	{
		auto byteCode = Core::ReadFileAsBinary(shaderFilePath);

		vk::ShaderModuleCreateInfo createInfo({},
			byteCode.size(),
			reinterpret_cast<const uint32_t*>(byteCode.data()),
			nullptr);

		return vk::raii::ShaderModule(mDevice->GetLogicalDevice(), createInfo);
	}

	const vk::raii::RenderPass& VulkanGraphicsPipeline::GetRenderPass() const
	{
		return mRenderPass;
	}

	const vk::Pipeline& VulkanGraphicsPipeline::GetPipeline() const
	{
		return *mPipeline;
	}
}