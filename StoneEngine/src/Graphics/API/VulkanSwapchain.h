#pragma once

#include "VulkanIncludes.h"

#include "Core/Utils.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	class VulkanDevice;

	class VulkanSwapchain final
	{
	public:
		DISALLOW_COPY(VulkanSwapchain);

		VulkanSwapchain() : mSwapchain(nullptr), mDevice(nullptr) {}
		void Initialize(
			vk::raii::SurfaceKHR* surface,
			VulkanDevice* device,
			int width,
			int height);

		void Recreate(int width, int height);

		[[nodiscard]] const std::vector<vk::Image>& GetImages() const;
		[[nodiscard]] const std::vector<vk::raii::ImageView>& GetImageViews() const;
		[[nodiscard]] const vk::Extent2D& GetExtent() const;
		[[nodiscard]] const vk::SurfaceFormatKHR& GetFormat() const;
		[[nodiscard]] std::pair<vk::Result, U32> AcquireNextImage(
			U64 timeout,
			vk::Semaphore semaphore = nullptr,
			vk::Fence fence = nullptr) const;
		[[nodiscard]] const vk::raii::SwapchainKHR& Get();

	private:
		VulkanDevice* mDevice;
		vk::raii::SurfaceKHR* mSurface;
		vk::raii::SwapchainKHR mSwapchain;
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

