#include "VulkanDescriptorPool.h"

#include "VulkanDevice.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice* device, U32 poolSizeCount)
	{
		vk::DescriptorPoolSize poolSize(vk::DescriptorType::eUniformBuffer, poolSizeCount);
		vk::DescriptorPoolCreateInfo createInfo(
			vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, poolSizeCount, poolSize
		);

		mDescriptorPool = vk::raii::DescriptorPool(device->GetLogicalDevice(), createInfo);
	}

	std::vector<vk::raii::DescriptorSet> VulkanDescriptorPool::AllocateDescriptorSets(const vk::raii::DescriptorSetLayout& layout)
	{
		vk::DescriptorSetAllocateInfo allocateInfo(*mDescriptorPool, *layout);
		vk::raii::DescriptorSets descriptorSets(mDevice->GetLogicalDevice(), allocateInfo);

		return descriptorSets;
	}
}