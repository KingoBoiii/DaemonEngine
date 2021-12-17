#include "kepch.h"
#include "GLFWWindow.h"

#include "DaemonEngine/Events/ApplicationEvent.h"
#include "DaemonEngine/Events/MouseEvent.h"
#include "DaemonEngine/Events/KeyEvent.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Daemon
{

	GLFWWindow::GLFWWindow(const WindowSpecification& specification)
		: m_Specification(specification)
	{
		s_NativeWindowHandle = this;
	}

	GLFWWindow::~GLFWWindow()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void GLFWWindow::Initialize()
	{
		int success = glfwInit();
		KE_CORE_ASSERT(success, "Could not intialize GLFW!");

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		if (m_Specification.Fullscreen)
		{
			GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
			m_Window = glfwCreateWindow(mode->width, mode->height, m_Specification.Title.c_str(), primaryMonitor, nullptr);
		}
		else
		{
			m_Window = glfwCreateWindow(m_Specification.Width, m_Specification.Height, m_Specification.Title.c_str(), nullptr, nullptr);
		}
		KE_CORE_ASSERT(m_Window, "Failed to create GLFW window!");

		m_RendererContext = RendererContext::Create(m_Window);
		m_RendererContext->Init();

		SwapChainSpecification spec = {};
		spec.Width = m_Specification.Width;
		spec.Height = m_Specification.Height;
		spec.WindowHandle = m_Window;
		m_SwapChain = SwapChain::Create(spec);

		SetVSync(m_Specification.VSync);
		glfwSetWindowUserPointer(m_Window, &m_Data);

		SetupCallbacks();
	}

	void GLFWWindow::Update()
	{
		glfwPollEvents();
		//glfwSwapBuffers(m_Window);
	}

	void GLFWWindow::Present() const
	{
		m_SwapChain->Present();
	}

	void GLFWWindow::Maximize()
	{
		glfwMaximizeWindow(m_Window);
	}

	bool GLFWWindow::GetVSync() const
	{
		return m_Data.VSync;
	}

	void GLFWWindow::SetVSync(bool vsync)
	{
		m_Data.VSync = vsync;
		glfwSwapInterval(vsync ? 1 : 0);
	}

	void GLFWWindow::SetTitle(const std::string& title)
	{
		glfwSetWindowTitle(m_Window, title.c_str());
	}

	void GLFWWindow::SetupCallbacks()
	{
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int32_t width, int32_t height)
		{
			WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowResizeEvent e(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
			data.EventCallback(e);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent e;
			data.EventCallback(e);
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent e((float)xOffset, (float)yOffset);
			data.EventCallback(e);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent e(button);
					data.EventCallback(e);
				}
				break;
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent e(button);
					data.EventCallback(e);
				}
				break;
			}
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent e(key, 0);
					data.EventCallback(e);
				}
				break;
				case GLFW_RELEASE:
				{
					KeyReleasedEvent e(key);
					data.EventCallback(e);
				}
				break;
				case GLFW_REPEAT:
				{
					KeyPressedEvent e(key, 1);
					data.EventCallback(e);
				}
				break;
			}
		});
	}

}
