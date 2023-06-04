#include "VulkanSwapchain.h"
#include "Core/Utils.h"
#include "Core/Logger.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	void VulkanSwapchain::Initialize(
        const vk::raii::SurfaceKHR& surface,
        const VulkanDevice& device,
        int width,
        int height)
	{
        // We cache any persistent settings here

        auto& [formats, presentModes, surfaceCapabilities] = device.GetSwapchainSupportDetails();

        // Pick surface format
        const auto format = std::find_if(formats.begin(), formats.end(), [](vk::SurfaceFormatKHR format)
            {
                return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
            });

        Core::FatalAssert(format != formats.end(), "Failed to pick a suitable surface format");

        mFormat = *format;

        // Pick the presentation mode
        const auto presentMode = std::find_if(presentModes.begin(), presentModes.end(), [](vk::PresentModeKHR presentMode)
            {
                return presentMode == vk::PresentModeKHR::eMailbox;
            });

        // Only eFifo is guaranteed to be supported
        mPresentMode = (presentMode != presentModes.end()) ? *presentMode : vk::PresentModeKHR::eFifo;

        // Set extent size
        mCapabilities = surfaceCapabilities;

        if (surfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max())
        {
            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            // If the surface size is undefined, the size is set to the size of the images requested.
            actualExtent.width = std::clamp(actualExtent.width, mCapabilities.minImageExtent.width, mCapabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, mCapabilities.minImageExtent.height, mCapabilities.maxImageExtent.height);
            mExtent = actualExtent;
        }
        else
        {
            // If the surface size is defined, the swap chain size must match
            mExtent = surfaceCapabilities.currentExtent;
        }

        // Create the SwapchainKHR object
        mImageCount = mCapabilities.minImageCount + 1;

        if (mCapabilities.maxImageCount > 0 && mImageCount > mCapabilities.maxImageCount) {
            mImageCount = mCapabilities.maxImageCount;
        }

        auto preTransform = (surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
            ? vk::SurfaceTransformFlagBitsKHR::eIdentity
            : surfaceCapabilities.currentTransform;

        vk::SwapchainCreateInfoKHR swapChainCreateInfo;
        swapChainCreateInfo.setSurface(*surface);
        swapChainCreateInfo.setMinImageCount(mImageCount);
        swapChainCreateInfo.setImageFormat(mFormat.format);
        swapChainCreateInfo.setImageColorSpace(mFormat.colorSpace);
        swapChainCreateInfo.setImageExtent(mExtent);
        swapChainCreateInfo.setImageArrayLayers(1);
        swapChainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
        swapChainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
        
        auto [graphicsFamily, presentFamily] = device.GetQueueFamilyIndices();

        if (graphicsFamily != presentFamily) {
            swapChainCreateInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
            swapChainCreateInfo.setQueueFamilyIndexCount(2);

            U32 queueFamilyIndices[] = { graphicsFamily.value(), presentFamily.value() };
            swapChainCreateInfo.setPQueueFamilyIndices(queueFamilyIndices);
        }
        else {
            swapChainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
            swapChainCreateInfo.setQueueFamilyIndexCount(0);
            swapChainCreateInfo.setPQueueFamilyIndices(nullptr);
        }

        swapChainCreateInfo.setPreTransform(preTransform);

        // TODO: Would be cool to play around with this. A translucent gamemode perhaps???
        swapChainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        swapChainCreateInfo.setPresentMode(mPresentMode);
        swapChainCreateInfo.setClipped(true);
        swapChainCreateInfo.setOldSwapchain(VK_NULL_HANDLE);
        
        mVKSwapchain = vk::raii::SwapchainKHR(device.GetLogicalDevice(), swapChainCreateInfo);
        mSwapchainImages = mVKSwapchain.getImages();

        mImageViews.reserve(mSwapchainImages.size());
        vk::ImageViewCreateInfo imageViewCreateInfo(
            {},
            {},
            vk::ImageViewType::e2D,
            mFormat.format,
            vk::ComponentMapping(),
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
        );

        for (const auto& image : mSwapchainImages)
        {
            imageViewCreateInfo.image = image;
            mImageViews.push_back(vk::raii::ImageView(device.GetLogicalDevice(), imageViewCreateInfo));
        }
	}
    void VulkanSwapchain::OnResize(int width, int height)
    {
        Core::LogInfo("Recreating swapchan because window was resized to width: {} height: {}", width, height);
        return;
    }

    const std::vector<vk::Image>& VulkanSwapchain::GetImages() const
    {
        return mSwapchainImages;
    }

    const std::vector<vk::raii::ImageView>& VulkanSwapchain::GetImageViews() const
    {
        return mImageViews;
    }

    const vk::Extent2D& VulkanSwapchain::GetExtent() const
    {
        return mExtent;
    }

    const vk::SurfaceFormatKHR& VulkanSwapchain::GetFormat() const
    {
        return mFormat;
    }
}

