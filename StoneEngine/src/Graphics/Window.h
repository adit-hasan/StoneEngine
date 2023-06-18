#pragma once

struct GLFWwindow;

namespace StoneEngine::Graphics
{
	class IRenderer;

	struct DestroyGLFWWindow {

		void operator()(GLFWwindow* ptr) {
			glfwDestroyWindow(ptr);
		}

	};
	typedef std::unique_ptr<GLFWwindow, DestroyGLFWWindow> WindowAPIPtr;

	class Window
	{
	public:
		Window(const std::string& windowName, const uint32_t width, const uint32_t height);

		void Initialize();
		void InitializeWindow();
		void TearDown();

		virtual ~Window() {}

	private:
		const uint32_t mWidth, mHeight;
		const std::string mWindowName;
		WindowAPIPtr mAPIWindow;
		std::shared_ptr<IRenderer> mRenderer;
	};

	// Basically the window
	[[nodiscard]] std::unique_ptr<Window> CreateRenderSubstrate(const std::string& appName, const uint32_t width = 800, const uint32_t height = 600);
}

