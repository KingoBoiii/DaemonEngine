#include "kepch.h"
#include "Window.h"

#include "DaemonEngine/Renderer/RendererAPI.h"

#include "DaemonEngine/Platform/GLFW/GLFWWindow.h"
#include "DaemonEngine/Platform/Win32/Win32Window.h"

namespace Daemon
{

	void* Window::s_NativeWindowHandle = nullptr;

	Unique<Window> Window::Create(const WindowSpecification& specification)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:		return CreateUnique<GLFWWindow>(specification);
			case RendererAPIType::DirectX11:	
			case RendererAPIType::DirectX12:	return CreateUnique<Win32Window>(specification);
			case RendererAPIType::None:
			default: break;
		}
		KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
	}

}
