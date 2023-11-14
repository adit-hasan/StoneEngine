#include "../../pch.h"
#include "VulkanInstance.h"
#include <GLFW/glfw3.h>


#include "VulkanUtils.h"
#include "../../Core/Logger.h"

namespace StoneEngine::Graphics::API::Vulkan
{
    VulkanInstance::VulkanInstance()
        :mInstance(nullptr),
        mDebugUtilsMessenger(nullptr)
    {
        vk::raii::Context context;
        vk::ApplicationInfo applicationInfo(sApplicationName.data(), 1, sEngineName.data(), 1, VK_API_VERSION_1_1);

        // initialize the vk::InstanceCreateInfo
        vk::InstanceCreateInfo instanceCreateInfo({}, &applicationInfo);

        auto extensions = GetRequiredInstanceExtensions(instanceCreateInfo);

#ifndef NDEBUG
        instanceCreateInfo.setPEnabledLayerNames(constValidationLayers);
#endif
        instanceCreateInfo.setEnabledExtensionCount(extensions.size());
        instanceCreateInfo.setPEnabledExtensionNames(extensions);

        // Assign validation layers
        std::vector<vk::LayerProperties> instanceLayerProperties = context.enumerateInstanceLayerProperties();

        std::vector<char const*> instanceLayerNames;
        instanceLayerNames.push_back("VK_LAYER_KHRONOS_validation");
        if (!checkLayers(instanceLayerNames, instanceLayerProperties))
        {
            Core::LogError("{}", "Set the environment variable VK_LAYER_PATH to point to the location of your layers");
            exit(1);
        }

        instanceCreateInfo.setPEnabledLayerNames(instanceLayerNames);

        // create an Instance
        mInstance = vk::raii::Instance(context, instanceCreateInfo);

#ifndef NDEBUG
        pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(mInstance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
        if (!pfnVkCreateDebugUtilsMessengerEXT)
        {
            Core::LogError("{}", "GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function.");
            exit(1);
        }

        pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(mInstance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
        if (!pfnVkDestroyDebugUtilsMessengerEXT)
        {
            Core::LogError("{}", "GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function.");
            exit(1);
        }

        vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;

        debugUtilsMessengerCreateInfo.setMessageSeverity(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
        debugUtilsMessengerCreateInfo.setMessageType(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
        debugUtilsMessengerCreateInfo.setPfnUserCallback(debugCallback);

        mDebugUtilsMessenger = vk::raii::DebugUtilsMessengerEXT(mInstance, debugUtilsMessengerCreateInfo);
#endif
    }

    [[nodiscard]] std::vector<const char*>  VulkanInstance::GetRequiredInstanceExtensions(vk::InstanceCreateInfo& createInfo)
    {
        uint32_t glfwExtensionCount = 0;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        auto extensions = std::vector<const char*>{ glfwExtensions, glfwExtensions + glfwExtensionCount };
        
#ifndef NDEBUG
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        return extensions;
    }

}