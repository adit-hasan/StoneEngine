#pragma once

#include "API/VulkanRenderer.h"

namespace StoneEngine::Graphics
{
	class GLFWWindow;
	class IRenderer;

	enum class GraphicsAPI
	{
		Vulkan = 0,
		None =1
	};

	class RendererFactory
	{
	public:
		template<GraphicsAPI API>
		static std::shared_ptr<IRenderer> GetInstance(GLFWwindow* window)
		{
			static_assert(API == GraphicsAPI::Vulkan, "Valid and implemented API required!");
			
			static std::shared_ptr<IRenderer> instance = std::make_shared<API::Vulkan::VulkanRenderer>(window);
			if (!instance->isInitialized()) { instance->Initialize(); }
			return instance;
		}
	};

}

