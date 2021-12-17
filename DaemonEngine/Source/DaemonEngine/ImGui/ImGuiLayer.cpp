#include "kepch.h"
#include "ImGuiLayer.h"

#include "DaemonEngine/Renderer/RendererAPI.h"
#include "DaemonEngine/Platform/OpenGL/OpenGLImGuiLayer.h"
#include "DaemonEngine/Platform/DirectX11/DX11ImGuiLayer.h"
#include "DaemonEngine/Platform/DirectX12/DX12ImGuiLayer.h"

namespace Daemon
{

	ImGuiLayer* ImGuiLayer::Create()
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:		return new OpenGLImGuiLayer();
			case RendererAPIType::DirectX11:	return new DX11ImGuiLayer();
			case RendererAPIType::DirectX12:	return new DX12ImGuiLayer();
			case RendererAPIType::None:
			default: break;
		}
		KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
	}

}
