#pragma once

namespace Daemon
{

	class UniformBuffer
	{
	public:
		static Shared<UniformBuffer> Create(uint32_t size, uint32_t binding);
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
	};

}