#pragma once
#include "BufferLayout.h"

namespace Daemon
{

	class VertexBuffer
	{
	public:
		static Shared<VertexBuffer> Create(uint32_t size);
		static Shared<VertexBuffer> Create(void* vertices, uint32_t size);
	public:
		virtual ~VertexBuffer() = default;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
	};

}
