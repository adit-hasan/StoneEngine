#pragma once

#include "glm/glm.hpp"

namespace StoneEngine::Graphics
{
	// TODO: ECS component
	struct VertexData
	{
		glm::vec2 Position;
		glm::vec3 Colour;
	};
}