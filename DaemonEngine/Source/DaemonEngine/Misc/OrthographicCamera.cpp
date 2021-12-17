#include "kepch.h"
#include "OrthographicCamera.h"

#include <glm/gtx/transform.hpp>

namespace Daemon
{

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearPlane, float farPlane)
		: m_NearPlane(nearPlane), m_FarPlane(farPlane)
	{
		SetProjection(left, right, bottom, top);
		m_ViewMatrix = glm::mat4(1.0f);
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, m_NearPlane, m_FarPlane);
	}

	void OrthographicCamera::SetPosition(const glm::vec3& position)
	{
		m_ViewMatrix = glm::inverse(glm::translate(position));
	}

}