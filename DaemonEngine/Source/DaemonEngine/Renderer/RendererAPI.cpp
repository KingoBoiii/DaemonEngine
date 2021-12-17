#include "kepch.h"
#include "RendererAPI.h"

#include "RendererAPI.h"
#include "DaemonEngine/Platform/OpenGL/OpenGLRenderer.h"
#include "DaemonEngine/Platform/DirectX11/DX11Renderer.h"
#include "DaemonEngine/Platform/DirectX12/DX12Renderer.h"

namespace Daemon
{

    Shared<RendererAPI> RendererAPI::Create()
    {
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:		return CreateShared<OpenGLRenderer>();
			case RendererAPIType::DirectX11:	return CreateShared<DX11Renderer>();
			case RendererAPIType::DirectX12:	return CreateShared<DX12Renderer>();
			case RendererAPIType::Vulkan:
			case RendererAPIType::None:
			default: break;
		}
		KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
    }

	std::string RendererAPI::GetRendererName()
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:		return "OpenGL";
			case RendererAPIType::DirectX11:	return "DirectX 11";
			case RendererAPIType::DirectX12:	return "DirectX 12";
			case RendererAPIType::Vulkan:		return "Vulkan";
			case RendererAPIType::None:
			default: break;
		}
		return "Unknown";
	}

	void RendererAPI::SetAPI(RendererAPIType api)
	{
		s_CurrentRendererAPI = api;
	}

}
