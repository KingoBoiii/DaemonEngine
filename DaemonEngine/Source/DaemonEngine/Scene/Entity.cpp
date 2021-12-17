#include "kepch.h"
#include "Entity.h"

namespace Daemon
{

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

}