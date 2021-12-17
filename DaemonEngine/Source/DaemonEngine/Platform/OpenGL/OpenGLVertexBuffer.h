#pragma once
#include "DaemonEngine/Renderer/VertexBuffer.h"

namespace Daemon
{

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(void* vertices, uint32_t size);
		~OpenGLVertexBuffer();

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		uint32_t m_RendererID;
	};

}
