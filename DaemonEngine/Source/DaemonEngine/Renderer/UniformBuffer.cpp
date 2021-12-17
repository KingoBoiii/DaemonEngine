#include "kepch.h"
#include "UniformBuffer.h"

#include "RendererAPI.h"
#include "DaemonEngine/Platform/OpenGL/OpenGLUniformBuffer.h"
#include "DaemonEngine/Platform/DirectX11/DX11UniformBuffer.h"
#include "DaemonEngine/Platform/DirectX12/DX12UniformBuffer.h"

namespace Daemon
{

	Shared<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:		return CreateShared<OpenGLUniformBuffer>(size, binding);
			case RendererAPIType::DirectX11:	return CreateShared<DX11UniformBuffer>(size, binding);
			case RendererAPIType::DirectX12:	return CreateShared<DX12UniformBuffer>(size, binding);
			case RendererAPIType::Vulkan:
			case RendererAPIType::None:
			default: break;
		}
		KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
	}

}
