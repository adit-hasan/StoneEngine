#pragma once
#include "vulkan/vulkan.hpp"
#include "VulkanUtils.h"

namespace StoneEngine::Graphics::API::Vulkan
{
    using InputDescriptions = std::vector<vk::VertexInputAttributeDescription>;

    template <typename T>
    constexpr vk::Format MapTypeToVulkanFormat();

    template <>
    constexpr vk::Format MapTypeToVulkanFormat<float>() {
        return vk::Format::eR32Sfloat;
    }

    template <>
    constexpr vk::Format MapTypeToVulkanFormat<glm::vec2>() {
        return vk::Format::eR32G32Sfloat;
    }

    template <>
    constexpr vk::Format MapTypeToVulkanFormat<glm::vec3>() {
        return vk::Format::eR32G32B32Sfloat;
    }

    template <>
    constexpr vk::Format MapTypeToVulkanFormat<glm::vec4>() {
        return vk::Format::eR32G32B32A32Sfloat;
    }

    template <typename T>
    static vk::VertexInputAttributeDescription GetAttributeDescription(int bindingIndex, size_t location, size_t offset) 
    {
        vk::VertexInputAttributeDescription attributeDescription{};
        attributeDescription.binding = bindingIndex;
        attributeDescription.location = static_cast<uint32_t>(location);
        attributeDescription.format = MapTypeToVulkanFormat<T>();
        attributeDescription.offset = static_cast<uint32_t>(offset);
        return attributeDescription;
    }

    // Recursive case for processing each type in the parameter pack
    template <typename T, typename... Types>
    static InputDescriptions GenerateAttributeDescriptionsHelper(
        int bindingIndex, size_t offset, size_t location) {

        // Generate the attribute description for the current type
        vk::VertexInputAttributeDescription attributeDescription = GetAttributeDescription<T>(bindingIndex, location, offset);
        std::vector<vk::VertexInputAttributeDescription> result;
        result.push_back(attributeDescription);

        if constexpr (sizeof...(Types)) 
        {
            // More than one parameter in the pack
            // Recursively process the remaining types
            InputDescriptions remainingAttributes = GenerateAttributeDescriptionsHelper<Types...>(
                bindingIndex, offset + sizeof(T), location + 1);

            result.insert(result.end(), remainingAttributes.begin(), remainingAttributes.end());
        }
        return result;
    }

    // Wrapper function for user convenience
    template <typename... Types>
    static InputDescriptions GenerateAttributeDescriptions(int bindingIndex) {
        return GenerateAttributeDescriptionsHelper<Types...>(bindingIndex, 0, 0);
    }

#define GENERATE_VULKAN_ATTRIBUTE_DESCRIPTIONS(...) \
    [[nodiscard]] static InputDescriptions GetVertexInputAttributeDescriptions(int bindingIndex) { \
        return StoneEngine::Graphics::API::Vulkan::GenerateAttributeDescriptions<__VA_ARGS__>(bindingIndex); \
    }
}

