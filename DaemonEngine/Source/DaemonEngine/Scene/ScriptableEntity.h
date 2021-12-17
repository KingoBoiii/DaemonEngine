#pragma once
#include "Entity.h"

namespace Daemon
{

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}
		
		template<typename T, typename... Args>
		T& AddComponent()
		{
			return m_Entity.AddComponent<T>();
		}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}
	protected:
		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;
		virtual void OnUpdate(float deltaTime) = 0;
	private:
		Entity m_Entity;
		friend class Scene;
	};

}