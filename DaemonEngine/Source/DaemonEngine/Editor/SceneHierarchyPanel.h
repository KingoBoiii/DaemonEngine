#pragma once
#include "DaemonEngine/Scene/Scene.h"
#include "DaemonEngine/Scene/Entity.h"

namespace Daemon
{

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Shared<Scene>& scene);

		void SetSceneContext(const Shared<Scene>& scene);
		void OnGUI();

		void SetSelectedEntity(Entity entity);
		Entity GetSelectedEntity() const { return m_SelectedEntity; }
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Shared<Scene> m_SceneContext;
		Entity m_SelectedEntity;
	};

}