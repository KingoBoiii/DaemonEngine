#include "kepch.h"
#include "DaemonEngine/Core/Input/Input.h"
#include "OrthographicCameraController.h"

namespace Daemon
{

	OrthographicCameraController::OrthographicCameraController(float aspectRatio)
		: m_AspectRatio(aspectRatio), m_Camera(OrthographicCamera(-m_AspectRatio, m_AspectRatio, -m_ZoomLevel, m_ZoomLevel))
	{
	}

	void OrthographicCameraController::OnUpdate(float deltaTime)
	{
		if (Input::IsKeyPressed(Key::A)) 
		{
			m_CameraPosition.x -= m_MovementSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(Key::D))
		{
			m_CameraPosition.x += m_MovementSpeed * deltaTime;
		}

		if (Input::IsKeyPressed(Key::S))
		{
			m_CameraPosition.y -= m_MovementSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(Key::W))
		{
			m_CameraPosition.y += m_MovementSpeed * deltaTime;
		}
		m_Camera.SetPosition(m_CameraPosition);

		m_MovementSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(KE_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrollEvent));
	}

	void OrthographicCameraController::Resize(float aspectRatio)
	{
		m_AspectRatio = aspectRatio;
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	bool OrthographicCameraController::OnMouseScrollEvent(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);

		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

}
