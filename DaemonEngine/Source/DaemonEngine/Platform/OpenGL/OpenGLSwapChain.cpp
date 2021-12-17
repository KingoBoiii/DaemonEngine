#include "kepch.h"
#include "OpenGLSwapChain.h"

#include <GLFW/glfw3.h>

namespace Daemon
{

	OpenGLSwapChain::OpenGLSwapChain(const SwapChainSpecification& specification)
		: m_Specification(specification)
	{
	}

	void OpenGLSwapChain::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.Width = width;
		m_Specification.Height= height;
	}

	void OpenGLSwapChain::Present() const
	{
		GLFWwindow* windowHandle = reinterpret_cast<GLFWwindow*>(m_Specification.WindowHandle);
		glfwSwapBuffers(windowHandle);
	}

}