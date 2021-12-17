#include "kepch.h"
#include "SceneRenderer.h"

#include "DaemonEngine/Core/Application.h"

namespace Daemon
{
	Shared<SceneRenderer> SceneRenderer::Create(Shared<Scene> scene, const SceneRendererSpecification& specification)
	{
		return CreateShared<SceneRenderer>(scene, specification);
	}

	SceneRenderer::SceneRenderer(Shared<Scene> scene, const SceneRendererSpecification& specification)
		: m_Specification(specification), m_Scene(scene)
	{
		Initialize();
	}

	void SceneRenderer::Initialize()
	{
		Application& app = Application::Get();

		FramebufferSpecification specification;
		specification.Width = app.GetWindow().GetWidth();
		specification.Height = app.GetWindow().GetHeight();
		specification.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		specification.ClearColor = { 0.2f, 0.2f, 0.2f, 0.2f };
		m_Framebuffer = Framebuffer::Create(specification);
	}

	void SceneRenderer::BeginScene()
	{
		m_Framebuffer->Bind();
		m_Framebuffer->Clear();
	}

	void SceneRenderer::EndScene()
	{
		m_Framebuffer->Unbind();
	}

	void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_Framebuffer->Resize(width, height);
	}

	void* SceneRenderer::GetFinalImage()
	{
		return m_Framebuffer->GetColorAttachment(0);
	}

}