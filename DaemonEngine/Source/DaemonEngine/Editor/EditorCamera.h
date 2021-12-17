#pragma once
#include "DaemonEngine/Misc/Camera.h"
#include "DaemonEngine/Events/Event.h"
#include "DaemonEngine/Events/KeyEvent.h"
#include "DaemonEngine/Events/MouseEvent.h"

namespace Daemon
{

	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fieldOfView, float nearClip = 0.1f, float farClip = 100.0f);
		EditorCamera(float fieldOfView, uint32_t width, uint32_t height, float nearClip = 0.1f, float farClip = 100.0f);
		virtual ~EditorCamera() = default;

		void OnUpdate(float deltaTime);

		void SetViewportSize(uint32_t width, uint32_t height);
	private:
		void Move(float deltaTime);
		void Rotate(float deltaTime);

		void UpdateProjection();
	private:
		glm::vec3 m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 m_Position = { 0.0f, 0.0f, 5.0f };

		float m_FieldOfView = glm::radians(45.0f);
		float m_AspectRatio = 1.0f;
		float m_NearClip = 0.1f;
		float m_FarClip = 1.0f;

		float m_MovementSpeed = 2.5f;
		float m_Sensitivity = 1.0f;

		uint32_t m_ViewportWidth = 1600;
		uint32_t m_ViewportHeight = 900;

		bool m_FirstMouse = true;
		float m_Yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
		float m_Pitch = 0.0f;
		float m_LastX = 800.0f / 2.0;
		float m_LastY = 600.0 / 2.0;
	};

}
