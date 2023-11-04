#include "../pch.h"

#include "Window.h"
#include "../Core/Logger.h"
#include "RendererFactory.h"


namespace StoneEngine::Graphics
{
	// TODO: Remove hard references to GLFW and make it agnostic

	Window::Window(const std::string& windowName, const uint32_t width, const uint32_t height)
		: mWidth(width), mHeight(height), mWindowName(windowName)
	{

	}

	void Window::Initialize()
	{
		// Initialize Window
		InitializeWindow();

		// Initialize Renderer
		mRenderer = RendererFactory::GetInstance<GraphicsAPI::Vulkan>(mAPIWindow.get());
	}

	void Window::InitializeWindow()
	{
		Core::LogInfo("Creating window with size: {},{} and name: {}", mWidth, mHeight, mWindowName);
		
		if (!glfwInit())
		{
			Core::LogError("Window::InitializeWindow() -> glfwInit() failed");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		// TODO: implement window resizing
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		auto window = glfwCreateWindow(mWidth, mHeight, mWindowName.c_str(), nullptr, nullptr);
		mAPIWindow = WindowAPIPtr(window);
	}

	void Window::TearDown()
	{
		glfwDestroyWindow(mAPIWindow.get());
		glfwTerminate();
	}

	void Window::DrawFrame() const
	{
		glfwPollEvents();
		mRenderer->DrawFrame();
	}

	bool Window::shouldClose() const
	{
		return glfwWindowShouldClose(mAPIWindow.get());
	}

	std::shared_ptr<IRenderer> Window::GetRendererInstance() const
	{
		return mRenderer;
	}

	std::unique_ptr<Window> CreateRenderSubstrate(const std::string& appName, const uint32_t width, const uint32_t height)
	{
		auto window = std::make_unique<Window>(appName, width, height);
		return window;
	}
}
