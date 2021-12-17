#pragma once
#include "Camera.h"

namespace Daemon
{

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float nearPlane = -1.0f, float farPlane = 1.0f);
		~OrthographicCamera() = default;
		
		void SetProjection(float left, float right, float bottom, float top);
		void SetPosition(const glm::vec3& position);
	private:
		glm::vec3 m_Position;
		float m_NearPlane;
		float m_FarPlane;
	};

}
