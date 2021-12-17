#include "kepch.h"
#include "RendererContext.h"

#include "RendererAPI.h"
#include "DaemonEngine/Platform/OpenGL/OpenGLContext.h"
#include "DaemonEngine/Platform/DirectX11/DX11Context.h"
#include "DaemonEngine/Platform/DirectX12/DX12Context.h"

namespace Daemon
{

	Shared<RendererContext> RendererContext::Create(void* windowHandle)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::OpenGL:		return CreateShared<OpenGLContext>((GLFWwindow*)windowHandle);
		case RendererAPIType::DirectX11:	return CreateShared<DX11Context>((HWND*)windowHandle);
		case RendererAPIType::DirectX12:	return CreateShared<DX12Context>((HWND*)windowHandle);
		case RendererAPIType::Vulkan:
		case RendererAPIType::None:
		default: break;
		}
		KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
	}

}
