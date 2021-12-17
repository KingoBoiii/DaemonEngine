#pragma once
#include "DaemonEngine/Scene/Scene.h"

#include "DaemonEngine/Renderer/Framebuffer.h"

namespace Daemon
{

	struct SceneRendererSpecification
	{

	};

	class SceneRenderer
	{
	public:
		static Shared<SceneRenderer> Create(Shared<Scene> scene, const SceneRendererSpecification& specification = SceneRendererSpecification());
	public:
		SceneRenderer(Shared<Scene> scene, const SceneRendererSpecification& specification);

		void Initialize();

		void BeginScene();
		void EndScene();

		void SetViewportSize(uint32_t width, uint32_t height);

		void* GetFinalImage();
	private:
		Shared<Scene> m_Scene;
		Shared<Framebuffer> m_Framebuffer;
		SceneRendererSpecification m_Specification;

		friend class EditorLayer;
	};

}
