#pragma once

#include "../pch.h"
#include "Application.h"

extern std::unique_ptr<StoneEngine::Application> StoneEngine::CreateApplication();

int main(int argc, char** argv)
{
	auto app = StoneEngine::CreateApplication();

	app->Initialize();
	app->Run();
	app->TearDown();
}
