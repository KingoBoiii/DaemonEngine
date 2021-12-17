#pragma once
#include "DaemonEngine/Scene/ScriptableEntity.h"

namespace Daemon
{

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* scriptComponent)
			{
				delete scriptComponent->Instance; 
				scriptComponent->Instance = nullptr;
			};
		}
	};

}
