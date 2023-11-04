#pragma once

#include "pch.h"

namespace StoneEngine::Graphics
{
	class IRenderer
	{
	public:
		virtual void Initialize() = 0;
		bool isInitialized() const { return mIsInitialized; }
		virtual ~IRenderer() {}

		virtual void DrawFrame() const = 0;
	protected:
		bool mIsInitialized = false;
	};
}
