// StoneEngineSandbox.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "StoneEngine.h"

class Sandbox : public StoneEngine::Application
{
public:
	Sandbox(): Application()
	{
	}

	~Sandbox()
	{
	}

	void ClientInitialize() override
	{
		StoneEngine::Core::LogInfo("Initializing client application");
		mWindow = StoneEngine::Graphics::CreateRenderSubstrate("Sandbox Application");
		mWindow->Initialize();
	}

	void ClientTearDown() override
	{
		StoneEngine::Core::LogInfo("Tearing down client application");

	}

	void Run() override
	{
		StoneEngine::Core::LogInfo("Running client application");
	}

private:
	std::unique_ptr<StoneEngine::Graphics::Window> mWindow;

};

std::unique_ptr<StoneEngine::Application> StoneEngine::CreateApplication()
{
	return std::make_unique<Sandbox>();
}