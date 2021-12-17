#include "kepch.h"
#include "VertexBuffer.h"

#include "RendererAPI.h"
#include "DaemonEngine/Platform/OpenGL/OpenGLVertexBuffer.h"
#include "DaemonEngine/Platform/DirectX11/DX11VertexBuffer.h"
#include "DaemonEngine/Platform/DirectX12/DX12VertexBuffer.h"

namespace Daemon
{
	Shared<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:		return CreateShared<OpenGLVertexBuffer>(size);
			case RendererAPIType::DirectX11:	return CreateShared<DX11VertexBuffer>(size);
			case RendererAPIType::DirectX12:	return CreateShared<DX12VertexBuffer>(size);
			case RendererAPIType::None:
			default: break;
		}
		KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
	}
	Shared<VertexBuffer> VertexBuffer::Create(void* vertices, uint32_t size)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:		return CreateShared<OpenGLVertexBuffer>(vertices, size);
			case RendererAPIType::DirectX11:	return CreateShared<DX11VertexBuffer>(vertices, size);
			case RendererAPIType::DirectX12:	return CreateShared<DX12VertexBuffer>(vertices, size);
			case RendererAPIType::None:
			default: break;
		}
		KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
	}

}