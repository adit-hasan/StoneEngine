#pragma once
#include "vulkan/vulkan.hpp"
#include "../../Core/Logger.h"

namespace StoneEngine::Graphics::API::Vulkan
{
    static VKAPI_ATTR VkBool32 VKAPI_CALL
        debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData) {
        Core::LogDebug("Validation Layer: {}", pCallbackData->pMessage);

        return VK_FALSE;
    }

    PFN_vkCreateDebugUtilsMessengerEXT  pfnVkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

    VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance                                 instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pMessenger)
    {
        return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
    }

    VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, VkAllocationCallbacks const* pAllocator)
    {
        return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
    }

    bool checkLayers(std::vector<char const*> const& layers, std::vector<vk::LayerProperties> const& properties)
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
}
