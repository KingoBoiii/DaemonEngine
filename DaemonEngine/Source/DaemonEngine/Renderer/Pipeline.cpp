#include "kepch.h"
#include "Pipeline.h"

#include "DaemonEngine/Renderer/RendererAPI.h"
#include "DaemonEngine/Platform/OpenGL/OpenGLPipeline.h"
#include "DaemonEngine/Platform/DirectX11/DX11Pipeline.h"
#include "DaemonEngine/Platform/DirectX12/DX12Pipeline.h"

namespace Daemon
{

    Shared<Pipeline> Pipeline::Create(const PipelineSpecification& specification)
    {
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:		return CreateShared<OpenGLPipeline>(specification);
			case RendererAPIType::DirectX11:	return CreateShared<DX11Pipeline>(specification);
			case RendererAPIType::DirectX12:	return CreateShared<DX12Pipeline>(specification);
			case RendererAPIType::None:
			default: break;
		}
		KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
    }

}
