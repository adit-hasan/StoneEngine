#include "VulkanSwapchain.h"
#include "Core/Utils.h"
#include "Core/Logger.h"
#include "VulkanDevice.h"

namespace StoneEngine::Graphics::API::Vulkan
{
	void VulkanSwapchain::Initialize(
        vk::raii::SurfaceKHR* surface,
        VulkanDevice* device,
        int width,
        int height)
	{
        Core::LogInfo("Initializing VulkanSwapchain");
        // We cache any persistent settings here
        mDevice = device;
        mSurface = surface;
        Recreate(width, height);

	}
    void VulkanSwapchain::Recreate(int width, int height)
    {
        Core::LogInfo("Recreating swapchan with width: {} height: {}", width, height);
        
        mDevice->SetSwapchainSupportDetails(*mSurface);
        auto& [formats, presentModes, surfaceCapabilities] = mDevice->GetSwapchainSupportDetails();

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
        swapChainCreateInfo.setSurface(*(*mSurface));
        swapChainCreateInfo.setMinImageCount(mImageCount);
        swapChainCreateInfo.setImageFormat(mFormat.format);
        swapChainCreateInfo.setImageColorSpace(mFormat.colorSpace);
        swapChainCreateInfo.setImageExtent(mExtent);
        swapChainCreateInfo.setImageArrayLayers(1);
        swapChainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
        swapChainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);

        auto [graphicsFamily, presentFamily] = mDevice->GetQueueFamilyIndices();

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
        auto previousSwapChain = *mSwapchain;
        // TODO: Would be cool to play around with this. A translucent gamemode perhaps???
        swapChainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        swapChainCreateInfo.setPresentMode(mPresentMode);
        swapChainCreateInfo.setClipped(true);
        swapChainCreateInfo.setOldSwapchain(previousSwapChain);

        mSwapchain = vk::raii::SwapchainKHR(mDevice->GetLogicalDevice(), swapChainCreateInfo);
        mSwapchainImages = mSwapchain.getImages();

        mImageViews.clear();
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
            mImageViews.emplace_back(mDevice->GetLogicalDevice(), imageViewCreateInfo);
        }
        return;
    }

    [[nodiscard]] const std::vector<vk::Image>& VulkanSwapchain::GetImages() const
    {
        return mSwapchainImages;
    }

    [[nodiscard]] const std::vector<vk::raii::ImageView>& VulkanSwapchain::GetImageViews() const
    {
        return mImageViews;
    }

    [[nodiscard]] const vk::Extent2D& VulkanSwapchain::GetExtent() const
    {
        return mExtent;
    }

    [[nodiscard]] const vk::SurfaceFormatKHR& VulkanSwapchain::GetFormat() const
    {
        return mFormat;
    }

    [[nodiscard]] std::pair<vk::Result, U32> VulkanSwapchain::AcquireNextImage(U64 timeout, vk::Semaphore semaphore, vk::Fence fence) const
    {
        return mSwapchain.acquireNextImage(timeout, semaphore, fence);
    }
    [[nodiscard]] const vk::raii::SwapchainKHR& VulkanSwapchain::Get()
    {
        return mSwapchain;
    }
}

