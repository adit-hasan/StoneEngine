#pragma once

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"

#include "Core/Utils.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanDevice;

	class VulkanSwapchain final
	{
	public:
		DISALLOW_COPY(VulkanSwapchain);

		VulkanSwapchain() : mVKSwapchain(nullptr) {}
		void Initialize(
			const vk::raii::SurfaceKHR& surface,
			const VulkanDevice& device,
			int width,
			int height);

		void OnResize(int width, int height);

		[[nodiscard]] const std::vector<vk::Image>& GetImages() const;
		[[nodiscard]] const std::vector<vk::raii::ImageView>& GetImageViews() const;
		[[nodiscard]] const vk::Extent2D& GetExtent() const;
		[[nodiscard]] const vk::SurfaceFormatKHR& GetFormat() const;

	private:
		vk::raii::SwapchainKHR mVKSwapchain;
		U32 mImageCount;
		std::vector<vk::Image> mSwapchainImages;
		std::vector<vk::raii::ImageView> mImageViews;

		// Swapchain support details
		vk::SurfaceCapabilitiesKHR mCapabilities;
		vk::SurfaceFormatKHR mFormat;
		vk::PresentModeKHR mPresentMode;
		vk::Extent2D mExtent;
	};

}

