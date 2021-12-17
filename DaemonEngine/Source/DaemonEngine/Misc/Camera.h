#pragma once
#include "DaemonEngine/Renderer/RendererAPI.h"
#include <glm/glm.hpp>

namespace Daemon
{

	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projectionMatrix);
		virtual ~Camera() = default;

		glm::mat4 GetProjView() 
		{ 
			return GetInternalProjection() * GetView();
		}

		glm::mat4 GetProjView() const
		{
			return GetInternalProjection() * GetView();
		}

		void SetProjectionMatrix(const glm::mat4& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }

		const glm::mat4& GetProjection() const { return GetInternalProjection(); }
		const glm::mat4& GetView() const { return m_ViewMatrix; }
	private:
		glm::mat4 GetInternalProjection() const
		{
			glm::mat4 proj = m_ProjectionMatrix;
			if (RendererAPI::Current() == RendererAPIType::OpenGL)
			{
				proj[1][1] = -proj[1][1];
			}
			return proj;
		}
	protected:
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
	};

}
