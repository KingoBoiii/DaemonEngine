#include "kepch.h"
#include "OpenGLRenderer.h"
#include "OpenGLContext.h"

#include "DaemonEngine/Renderer/Pipeline.h"
#include "DaemonEngine/Renderer/VertexBuffer.h"
#include "DaemonEngine/Renderer/IndexBuffer.h"
#include "DaemonEngine/Renderer/UniformBuffer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#include <gl/GL.h>

namespace Daemon
{

	void OpenGLRenderer::Init()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRenderer::Clear() const
	{
		// Obsolete - Using this might result in strange behaviour, might be removed at a later point
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRenderer::SetClearColor(const glm::vec4& color) const
	{
		// Obsolete - Using this might result in strange behaviour, might be removed at a later point
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRenderer::Clear(uint32_t buffer, const glm::vec4& clearColorRGBA) const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(clearColorRGBA.r, clearColorRGBA.g, clearColorRGBA.b, clearColorRGBA.a);
	}

	void OpenGLRenderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRenderer::BeginFrame()
	{
	}

	void OpenGLRenderer::EndFrame()
	{
	}

	void OpenGLRenderer::RenderMesh(Shared<Pipeline> pipeline, Shared<UniformBuffer> uniformBuffer, Shared<Mesh> mesh, const glm::mat4& transform)
	{
	}

	void OpenGLRenderer::RenderGeometry(Shared<Pipeline> pipeline, Shared<UniformBuffer> uniformBuffer, Shared<VertexBuffer> vertexBuffer, Shared<IndexBuffer> indexBuffer, uint32_t indexCount)
	{
		if (indexCount == 0)
		{
			indexCount = indexBuffer->GetCount();
		}

		pipeline->Bind();
		vertexBuffer->Bind();
		indexBuffer->Bind();
		uniformBuffer->Bind();

		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}