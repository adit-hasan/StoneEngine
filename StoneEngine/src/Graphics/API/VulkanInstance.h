#pragma once

#include "VulkanIncludes.h"

#include "Core/Utils.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	constexpr auto constValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	class VulkanInstance final
	{
		static constexpr std::string_view sApplicationName = "Sandbox";
		static constexpr std::string_view sEngineName = "StoneEngine";

	public:
		DISALLOW_COPY(VulkanInstance)

		VulkanInstance();
		~VulkanInstance() = default;

		const vk::raii::Instance& GetInstance() const
		{
			return mInstance;
		}

	private:
		[[nodiscard]] std::vector<const char*> GetRequiredInstanceExtensions(vk::InstanceCreateInfo& createInfo);
		vk::raii::Instance mInstance;
		vk::raii::DebugUtilsMessengerEXT mDebugUtilsMessenger;
	};
}