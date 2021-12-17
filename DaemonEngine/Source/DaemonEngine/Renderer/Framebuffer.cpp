#include "kepch.h"
#include "Framebuffer.h"

#include "RendererAPI.h"
#include "DaemonEngine/Platform/OpenGL/OpenGLFramebuffer.h"
#include "DaemonEngine/Platform/DirectX11/DX11Framebuffer.h"

namespace Daemon
{

    Shared<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specification)
    {
		switch (RendererAPI::Current())
		{
		case RendererAPIType::DirectX11:	return CreateShared<DX11Framebuffer>(specification);
		case RendererAPIType::DirectX12:	break;
		case RendererAPIType::OpenGL:		return CreateShared<OpenGLFramebuffer>(specification);
		case RendererAPIType::Vulkan:		break;
		case RendererAPIType::None:
		default: break;
		}
		KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
    }

}
