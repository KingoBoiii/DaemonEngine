#include "kepch.h"
#include "GLFWInput.h"
#include "DaemonEngine/Core/Application.h"
#include "GLFWWindow.h"

#include "DaemonEngine/Renderer/RendererAPI.h"

#include <GLFW/glfw3.h>

namespace Daemon
{

	bool GLFWInput::IsKeyPressed(KeyCode key)
	{
		GLFWWindow* glfwWindow = reinterpret_cast<GLFWWindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(glfwWindow->m_Window, static_cast<int32_t>(key));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool GLFWInput::IsMouseButtonPressed(MouseCode button)
	{
		GLFWWindow* glfwWindow = reinterpret_cast<GLFWWindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(glfwWindow->m_Window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 GLFWInput::GetMousePosition()
	{
		GLFWWindow* glfwWindow = reinterpret_cast<GLFWWindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(glfwWindow->m_Window, &xpos, &ypos);

		return { (float)xpos, RendererAPI::Current() == RendererAPIType::OpenGL ? (float)-ypos : (float)ypos };
	}

}
