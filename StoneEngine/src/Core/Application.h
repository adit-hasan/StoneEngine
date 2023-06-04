#pragma once

#include "../pch.h"

namespace StoneEngine
{
	class Application
	{
	public:
		virtual void Initialize();
		virtual void TearDown();
		virtual ~Application() = default;
		virtual void Run();
	private:
		virtual void Update(U64 frameTime);
	protected:
		Application() = default;
		virtual void ClientInitialize() = 0;
		virtual void ClientTearDown() = 0;

	private:
		//std::unique_ptr<CoreLoop> mCoreLoop;
	};

	std::unique_ptr<Application> CreateApplication();
}

