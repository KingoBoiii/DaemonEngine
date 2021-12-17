#pragma once
#include "OrthographicCamera.h"

#include "DaemonEngine/Events/ApplicationEvent.h"
#include "DaemonEngine/Events/MouseEvent.h"

namespace Daemon 
{

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio);
		~OrthographicCameraController() = default;

		void OnUpdate(float deltaTime);
		void OnEvent(Event& e);

		void Resize(float aspectRatio);

		// Setters
		inline void SetZoomLevel(float zoomLevel) { m_ZoomLevel = zoomLevel; m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel); }

		// Getters
		const OrthographicCamera& GetCamera() const { return m_Camera; }
	private:
		bool OnMouseScrollEvent(MouseScrolledEvent& e);
	private:
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_MovementSpeed = 1.0f;

		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;
	};

}
