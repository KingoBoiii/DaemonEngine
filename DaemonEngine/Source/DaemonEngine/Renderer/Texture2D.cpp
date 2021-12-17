#include "kepch.h"
#include "Texture2D.h"

#include "RendererAPI.h"
#include "DaemonEngine/Platform/OpenGL/OpenGLTexture2D.h"
#include "DaemonEngine/Platform/DirectX11/DX11Texture2D.h"
#include "DaemonEngine/Platform/DirectX12/DX12Texture2D.h"

namespace Daemon
{

	Shared<Texture2D> Texture2D::Create(const std::string& filepath, const TextureSpecification& specification)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:		return CreateShared<OpenGLTexture2D>(filepath, specification);
			case RendererAPIType::DirectX11:	return CreateShared<DX11Texture2D>(filepath, specification);
			case RendererAPIType::DirectX12:	return CreateShared<DX12Texture2D>(filepath, specification);
			case RendererAPIType::None:
			default: break;
		}
		KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
	}

	Shared<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, const TextureSpecification& specification)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:		return CreateShared<OpenGLTexture2D>(width, height, specification);
			case RendererAPIType::DirectX11:	return CreateShared<DX11Texture2D>(width, height, specification);
			case RendererAPIType::DirectX12:	return CreateShared<DX12Texture2D>(width, height, specification);
			case RendererAPIType::None:
			default: break;
		}
		KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
	}

	Shared<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, void* data, const TextureSpecification& specification)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::OpenGL:		return CreateShared<OpenGLTexture2D>(width, height, data, specification);
		case RendererAPIType::DirectX11:	return CreateShared<DX11Texture2D>(width, height, data, specification);
		case RendererAPIType::DirectX12:	return CreateShared<DX12Texture2D>(width, height, data, specification);
		case RendererAPIType::None:
		default: break;
		}
		KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
	}

}