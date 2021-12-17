#include "kepch.h"
#include "Camera.h"

namespace Daemon
{

	Camera::Camera(const glm::mat4& projectionMatrix)
		: m_ProjectionMatrix(projectionMatrix)
	{
	}

}