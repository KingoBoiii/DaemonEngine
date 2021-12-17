#pragma once
#include "Scene.h"

namespace YAML
{
	class Emitter;
}

namespace Daemon
{

	class SceneSerializer
	{
	public:
		SceneSerializer(const Shared<Scene>& scene);

		void Serialize(const std::string& filepath);
		void SerializeRuntime(const std::string& filepath);

		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string& filepath);
	private:
		void SerializeEntity(YAML::Emitter& out, Entity entity);
	private:
		Shared<Scene> m_Scene;
	};

}
