#pragma once
#include <DaemonEngine.h>
#include <DaemonEngine/Editor/SceneHierarchyPanel.h>

#include "Panels/ContentBrowserPanel.h"

namespace Daemon
{

	class EditorLayer : public Layer
	{
	public:
		EditorLayer() : Layer("EditorLayer") {}
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float deltaTime) override;
		virtual void OnGUI() override;
		virtual void OnEvent(Event& e) override;
	private:
		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();

		void ShowGameViewport();
		void ShowEditorViewport();
		void ShowTopMenu();

		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
	private:
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		int m_GizmoType = -1;
		glm::vec2 m_ViewportSize;
		glm::vec2 m_ViewportBounds[2];
		Entity m_HoveredEntity;

		EditorCamera m_EditorCamera;
		Shared<Scene> m_Scene;
		Shared<SceneRenderer> m_SceneRenderer;
		Shared<Framebuffer> m_GameViewport;
	};

}
