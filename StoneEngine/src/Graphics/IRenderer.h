#pragma once

#include "pch.h"
#include <GLFW/glfw3.h>

namespace StoneEngine::Graphics
{
	class IRenderer
	{
	public:
		virtual void Initialize() = 0;
		bool isInitialized() const { return mIsInitialized; }
		virtual ~IRenderer() {}

	protected:
		bool mIsInitialized = false;
	};
}
