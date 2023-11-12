#pragma once

#include "VulkanIncludes.h"

#include "Core/Utils.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanDevice;

	class VulkanGraphicsPipeline final
	{
		using ShaderModule = vk::raii::ShaderModule;
		using PipelineLayout = vk::raii::PipelineLayout;
		using RenderPass = vk::raii::RenderPass;
		using GraphicsPipeline = vk::raii::Pipeline;

	public:
		DISALLOW_COPY(VulkanGraphicsPipeline);

		VulkanGraphicsPipeline(
			std::string_view vertexShaderPath,
			std::string_view fragmentShaderPath,
			VulkanDevice* device,
			vk::SurfaceFormatKHR format
		);

		void Recreate();

		[[nodiscard]] const RenderPass& GetRenderPass() const;
		[[nodiscard]] const vk::Pipeline& GetPipeline() const;

	private:
		ShaderModule LoadShader(std::string_view shaderFilePath);

		GraphicsPipeline mPipeline;
		ShaderModule mVertexShaderModule;
		ShaderModule mFragmentShaderModule;
		PipelineLayout mPipelineLayout;
		VulkanDevice* mDevice;
		RenderPass mRenderPass;
	};
}

