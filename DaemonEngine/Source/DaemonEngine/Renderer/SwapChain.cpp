#include "kepch.h"
#include "SwapChain.h"

#include "RendererAPI.h"
#include "DaemonEngine/Platform/DirectX11/DX11SwapChain.h"
#include "DaemonEngine/Platform/OpenGL/OpenGLSwapChain.h"

namespace Daemon
{

    Shared<SwapChain> SwapChain::Create(const SwapChainSpecification& specification)
    {
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:		return CreateShared<OpenGLSwapChain>(specification);
			case RendererAPIType::DirectX11:	return CreateShared<DX11SwapChain>(specification);
			case RendererAPIType::DirectX12:
			case RendererAPIType::Vulkan:
			case RendererAPIType::None:
			default: break;
		}
		KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
    }

}