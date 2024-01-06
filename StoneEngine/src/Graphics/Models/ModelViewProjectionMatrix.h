#pragma once

#include "glm/glm.hpp"

namespace StoneEngine::Graphics
{
	struct ModelViewProjectionMatrix
	{
		glm::mat4 Model;
		glm::mat4 View;
		glm::mat4 Projection;
	};
}