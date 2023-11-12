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

    constexpr vk::Format MapTypeToVulkanFormat() {
        return vk::Format::eUndefined;
    }

    template <typename T>
    constexpr vk::Format MapTypeToVulkanFormat() {
        if constexpr (std::is_same_v<T, float>) {
            return vk::Format::eR32Sfloat;
        }
        else if constexpr (std::is_same_v<T, glm::vec2>) {
            return vk::Format::eR32G32Sfloat;
        }
        else if constexpr (std::is_same_v<T, glm::vec3>) {
            return vk::Format::eR32G32B32Sfloat;
        }
        else if constexpr (std::is_same_v<T, glm::vec4>) {
            return vk::Format::eR32G32B32A32Sfloat;
        }
        else {
            // Handle other types or add more specializations as needed
            return vk::Format::eUndefined;
        }
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

    inline std::array<vk::VertexInputAttributeDescription, 2> GetAttributeDescription(int bindingIndex)
    {
        std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = bindingIndex;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = MapTypeToVulkanFormat<glm::vec2>();
        attributeDescriptions[0].offset = offsetof(VertexData, Position);

        attributeDescriptions[1].binding = bindingIndex;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = MapTypeToVulkanFormat<glm::vec3>();
        attributeDescriptions[1].offset = offsetof(VertexData, Colour);

        return attributeDescriptions;
    }
}
