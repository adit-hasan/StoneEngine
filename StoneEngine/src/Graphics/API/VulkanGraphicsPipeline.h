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

	public:
		DISALLOW_COPY(VulkanGraphicsPipeline);

		VulkanGraphicsPipeline(
			std::string_view vertexShaderPath,
			std::string_view fragmentShaderPath,
			VulkanDevice* device,
			vk::Extent2D mExtent
		);

		void Recreate();

	private:
		ShaderModule LoadShader(std::string_view shaderFilePath);

		ShaderModule mVertexShaderModule;
		ShaderModule mFragmentShaderModule;
		PipelineLayout mPipelineLayout;
		VulkanDevice* mDevice;
		vk::Extent2D mExtent;
	};
}

