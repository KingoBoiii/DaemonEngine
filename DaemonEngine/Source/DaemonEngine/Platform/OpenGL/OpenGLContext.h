#pragma once
#include "DaemonEngine/Renderer/RendererContext.h"

struct GLFWwindow;

namespace Daemon
{

	class OpenGLContext : public RendererContext
	{
	public:
		OpenGLContext(GLFWwindow* window);
		~OpenGLContext() = default;

		virtual void Init() override;

		inline GLFWwindow* GetWindowHandle() { return m_WindowHandle; }
		static inline OpenGLContext& Get() { return *s_Instance; }
	private:
		GLFWwindow* m_WindowHandle;
	private:
		static OpenGLContext* s_Instance;
	};

}
