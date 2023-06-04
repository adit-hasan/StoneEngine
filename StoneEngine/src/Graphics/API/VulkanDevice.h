#pragma once

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include "VulkanInstance.h"
#include "pch.h"

#include "Core/Utils.h"


namespace StoneEngine::Graphics::API::Vulkan
{
	static const std::vector<const char*> _DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	class VulkanDevice final
	{
		using Surface			= vk::raii::SurfaceKHR;
		using PhysicalDevice	= vk::raii::PhysicalDevice;
		using PhysicalDevices	= vk::raii::PhysicalDevices;
		using Device			= vk::raii::Device;
		using Queue				= vk::raii::Queue;

	public:
		DISALLOW_COPY(VulkanDevice);

		struct QueueFamilyIndices
		{
			std::optional<U32> GraphicsFamily;
			std::optional<U32> PresentFamily;
			bool isSuitable() const 
			{ 
				return GraphicsFamily.has_value() && PresentFamily.has_value(); 
			}
		};

		struct SwapchainSupportDetails
		{
			std::vector<vk::SurfaceFormatKHR> SurfaceFormats;
			std::vector<vk::PresentModeKHR> PresentModes;
			vk::SurfaceCapabilitiesKHR SurfaceCapabilities;

			bool isSuitable() const
			{
				return !SurfaceFormats.empty() && !PresentModes.empty();
			}
		};

	public:
		VulkanDevice();
		void Initialize(const VulkanInstance* const instance, const vk::raii::SurfaceKHR& surface);
		void SetSwapchainSupportDetails(const Surface& surface);
		inline const SwapchainSupportDetails& GetSwapchainSupportDetails() const
		{
			return mSwapchainSupportDetails;
		}
		inline const Device& GetLogicalDevice() const
		{
			return mDevice;
		}
		inline const QueueFamilyIndices& GetQueueFamilyIndices() const
		{
			return mQueueFamilyIndices;
		}
		inline auto GetDispatcher() const
		{
			return mPhysicalDevice.getDispatcher();
		}

	private:
		PhysicalDevice mPhysicalDevice;
		QueueFamilyIndices mQueueFamilyIndices;
		SwapchainSupportDetails mSwapchainSupportDetails;
		Device mDevice; // logical device
		Queue mGraphicsQueue;
		Queue mPresentQueue;


		bool SelectPhysicalDevice(const PhysicalDevices& devices);
		int RankPhysicalDevice(const PhysicalDevice& device);
	};
}

