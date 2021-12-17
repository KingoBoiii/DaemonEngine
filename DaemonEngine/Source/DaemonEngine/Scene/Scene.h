#pragma once
#include "DaemonEngine/Editor/EditorCamera.h"

#include <entt.hpp>

namespace Daemon
{

	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name);
		void DestroyEntity(Entity entity);

		void SetViewportSize(uint32_t width, uint32_t height);

		void OnRuntimeUpdate(float deltaTime);
		void OnEditorUpdate(float deltaTime, EditorCamera& editorCamera);

		// Runtime
		void OnRuntimeStart();
		void OnRuntimeStop();

		Entity GetPrimaryCameraEntity();
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
		template<typename T>
		void OnComponentRemoved(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		Entity* m_Physics2DBodyEntityBuffer = nullptr;

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}
