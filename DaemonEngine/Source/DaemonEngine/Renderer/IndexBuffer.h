#pragma once

namespace Daemon
{

	class IndexBuffer
	{
	public:
		static Shared<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	public:
		virtual ~IndexBuffer() = default;

		virtual uint32_t GetCount() const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
	};

}
