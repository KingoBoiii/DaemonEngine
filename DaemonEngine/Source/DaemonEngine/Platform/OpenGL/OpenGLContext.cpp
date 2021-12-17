#include "kepch.h"
#include "OpenGLContext.h"
#include "DaemonEngine/Core/Application.h"
#include "DaemonEngine/Core/Window.h"

#undef NOGDI
#include <Windows.h>
#define NOGDI

#pragma comment(lib,"opengl32.lib")

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Daemon
{

	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:         KE_CORE_CRITICAL(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:       KE_CORE_ERROR(message); return;
			case GL_DEBUG_SEVERITY_LOW:          KE_CORE_WARN(message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: KE_CORE_TRACE(message); return;
		}

		KE_CORE_ASSERT(false, "Unknown severity level!");
	}

	OpenGLContext* OpenGLContext::s_Instance = nullptr;

	OpenGLContext::OpenGLContext(GLFWwindow* window)
		: m_WindowHandle(window)
	{
		s_Instance = this;
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		KE_CORE_ASSERT(status, "Failed to initialize Glad!");

		KE_CORE_TRACE("----------------------------------------------");
		KE_CORE_TRACE("OpenGL Info:");
		KE_CORE_TRACE("| Vendor: {0}", glGetString(GL_VENDOR));
		KE_CORE_TRACE("| Renderer: {0}", glGetString(GL_RENDERER));
		KE_CORE_TRACE("| Version: {0}", glGetString(GL_VERSION));
		KE_CORE_TRACE("----------------------------------------------");

#ifdef KE_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

#ifdef KE_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		KE_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "DaemonEngine requires at least OpenGL version 4.5!");
#endif
	}

}