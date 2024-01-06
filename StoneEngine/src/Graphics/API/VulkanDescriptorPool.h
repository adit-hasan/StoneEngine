#pragma once

#include "VulkanIncludes.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanDevice;

	// Do I want this to be copy-able/move-able?
	class VulkanDescriptorPool final
	{
	public:
		VulkanDescriptorPool(VulkanDevice* device, U32 poolSizeCount);
		[[nodiscard]] std::vector<vk::raii::DescriptorSet> AllocateDescriptorSets(
			const vk::raii::DescriptorSetLayout& layout
		);
	private:
		vk::raii::DescriptorPool mDescriptorPool = nullptr;
		VulkanDevice* mDevice = nullptr;
	};
}
