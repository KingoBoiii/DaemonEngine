#include "kepch.h"
#include "IndexBuffer.h"

#include "RendererAPI.h"
#include "DaemonEngine/Platform/OpenGL/OpenGLIndexBuffer.h"
#include "DaemonEngine/Platform/DirectX11/DX11IndexBuffer.h"
#include "DaemonEngine/Platform/DirectX12/DX12IndexBuffer.h"

namespace Daemon
{

	Shared<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:		return CreateShared<OpenGLIndexBuffer>(indices, count);
			case RendererAPIType::DirectX11:	return CreateShared<DX11IndexBuffer>(indices, count);
			case RendererAPIType::DirectX12:	return CreateShared<DX12IndexBuffer>(indices, count);
			case RendererAPIType::None:
			default: break;
		}
		KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
	}

}

