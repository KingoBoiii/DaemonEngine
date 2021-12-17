#pragma once
#include "DaemonEngine/Renderer/UniformBuffer.h"

#include <d3d11.h>

namespace Daemon
{

	class DX11UniformBuffer : public UniformBuffer
	{
	public:
		DX11UniformBuffer(uint32_t size, uint32_t binding);
		virtual ~DX11UniformBuffer();

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		ID3D11Buffer* m_CBuffer;
		uint32_t m_Binding;
	};

}