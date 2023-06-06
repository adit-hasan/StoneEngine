#pragma once

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"

#include "Core/Utils.h"
#include "VulkanDevice.h"

namespace StoneEngine::Graphics::API::Vulkan
{
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
			vk::Extent2D mExtent,
			vk::SurfaceFormatKHR format
		);

		void Recreate();

		[[nodiscard]] const RenderPass& GetRenderPass() const;

	private:
		ShaderModule LoadShader(std::string_view shaderFilePath);

		GraphicsPipeline mPipeline;
		ShaderModule mVertexShaderModule;
		ShaderModule mFragmentShaderModule;
		PipelineLayout mPipelineLayout;
		VulkanDevice* mDevice;
		vk::Extent2D mExtent;
		RenderPass mRenderPass;
	};
}

