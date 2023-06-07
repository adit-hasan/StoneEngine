#include "Application.h"
#include "Logger.h"

namespace StoneEngine
{
	void Application::Initialize()
	{
		// TODO: Register all engine components
		Core::SetLogLevel(Core::Info);
		ClientInitialize();
	}

	void Application::TearDown()
	{
		ClientTearDown();
	}

	void Application::Run()
	{
		// TODO: Manage the lifetime of the application

		// TODO: Call update on all registered components
	}

	void Application::Update(U64 frameTime)
	{
	}
}
