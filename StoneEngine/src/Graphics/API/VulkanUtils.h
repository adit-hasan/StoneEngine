#pragma once
#include "vulkan/vulkan.hpp"
#include "../../Core/Logger.h"
#include "Graphics/Models/VertexData.h"

namespace StoneEngine::Graphics::API::Vulkan
{
    inline VKAPI_ATTR VkBool32 VKAPI_CALL
        debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData) {
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            Core::LogDebug("Validation Layer: {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            Core::LogInfo("Validation Layer: {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            Core::LogWarn("Validation Layer: {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            Core::LogError("Validation Layer: {}", pCallbackData->pMessage);
            break;
        default:
            Core::LogDebug("Validation Layer: {}", pCallbackData->pMessage);
            break;
        }

        return VK_FALSE;
    }

    inline PFN_vkCreateDebugUtilsMessengerEXT  pfnVkCreateDebugUtilsMessengerEXT;
    inline PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

    inline VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance                                 instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pMessenger)
    {
        return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
    }

    inline VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, VkAllocationCallbacks const* pAllocator)
    {
        return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
    }

    inline bool checkLayers(std::vector<char const*> const& layers, std::vector<vk::LayerProperties> const& properties)
    {
        // return true if all layers are listed in the properties
        return std::all_of(layers.begin(),
            layers.end(),
            [&properties](char const* name)
            {
                return std::find_if(properties.begin(),
                properties.end(),
                [&name](vk::LayerProperties const& property)
                    { return strcmp(property.layerName, name) == 0; }) != properties.end();
            });
    }



    template<class TVertex>
    constexpr vk::VertexInputBindingDescription GetBindingDescription(int bindingIndex, vk::VertexInputRate inputRate)
    {
        vk::VertexInputBindingDescription bindingDescription;
        bindingDescription.binding = bindingIndex;
        bindingDescription.stride = sizeof(TVertex);
        bindingDescription.inputRate = inputRate;

        return bindingDescription;
    }
}

