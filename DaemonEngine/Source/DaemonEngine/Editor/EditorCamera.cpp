#include "kepch.h"
#include "EditorCamera.h"

#include "DaemonEngine/Core/Application.h"
#include "DaemonEngine/Core/Input/Input.h"
#include "DaemonEngine/Core/KeyCodes.h"
#include "DaemonEngine/Core/MouseCodes.h"

#include <glm/gtx/quaternion.hpp>

namespace Daemon
{

	EditorCamera::EditorCamera(float fieldOfView, float nearClip, float farClip)
		: m_FieldOfView(fieldOfView), m_NearClip(nearClip), m_FarClip(farClip)
	{
		Application& app = Application::Get();
		m_ViewportWidth = app.GetWindow().GetWidth();
		m_ViewportHeight = app.GetWindow().GetHeight();
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_CameraFront, m_CameraUp);
		//m_ViewMatrix = glm::inverse(m_ViewMatrix);
		UpdateProjection();
	}

	EditorCamera::EditorCamera(float fieldOfView, uint32_t width, uint32_t height, float nearClip, float farClip)
		: m_FieldOfView(fieldOfView), m_ViewportWidth(width), m_ViewportHeight(height), m_NearClip(nearClip), m_FarClip(farClip)
	{
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_CameraFront, m_CameraUp);
		//m_ViewMatrix = glm::inverse(m_ViewMatrix);
		UpdateProjection();
	}

	void EditorCamera::OnUpdate(float deltaTime)
	{
		if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
		{
			Move(deltaTime);
			Rotate(deltaTime);
			m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_CameraFront, m_CameraUp);
			//m_ViewMatrix = glm::inverse(m_ViewMatrix);
		}
		else
		{
			m_FirstMouse = true;
		}
	}

	void EditorCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
		UpdateProjection();
	}

	void EditorCamera::Move(float deltaTime)
	{
		if (Input::IsKeyPressed(Key::Q))
		{
			m_Position += m_CameraUp * m_MovementSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(Key::E))
		{
			m_Position -= m_CameraUp * m_MovementSpeed * deltaTime;
		}

		if (Input::IsKeyPressed(Key::W))
		{
			m_Position += m_CameraFront * m_MovementSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(Key::S))
		{
			m_Position -= m_CameraFront * m_MovementSpeed * deltaTime;
		}

		if (Input::IsKeyPressed(Key::A))
		{
			m_Position -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_MovementSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(Key::D))
		{
			m_Position += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_MovementSpeed * deltaTime;
		}
	}

	void EditorCamera::Rotate(float deltaTime)
	{
		glm::vec2 mp = { Input::GetMouseX(), Input::GetMouseY() };

		if (m_FirstMouse)
		{
			m_LastX = mp.x;
			m_LastY = mp.y;
			m_FirstMouse = false;
		}

		float xoffset = mp.x - m_LastX;
		float yoffset = m_LastY - mp.y; // reversed since y-coordinates go from bottom to top
		m_LastX = mp.x;
		m_LastY = mp.y;

		xoffset *= (m_Sensitivity / 10.0f);
		yoffset *= (m_Sensitivity / 10.0f);

		m_Yaw += xoffset;
		m_Pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_CameraFront = glm::normalize(front);
	}

	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = (float)m_ViewportWidth / (float)m_ViewportHeight;
		m_ProjectionMatrix = glm::perspective(m_FieldOfView, m_AspectRatio, m_NearClip, m_FarClip);
	}

}