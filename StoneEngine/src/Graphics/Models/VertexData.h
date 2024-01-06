#pragma once

#include "glm/glm.hpp"
#include "Graphics/API/VulkanAttributeDescriptions.h"

namespace StoneEngine::Graphics
{
	// TODO: ECS component
	struct VertexData
	{
		glm::vec2 Position;
		glm::vec3 Colour;
	
		static std::vector<vk::VertexInputAttributeDescription> GetVertexInputAttributeDescriptions(int bindingIndex)
		{
			return StoneEngine::Graphics::API::Vulkan::GenerateAttributeDescriptions<glm::vec2, glm::vec3>(bindingIndex);
		}
	};

}