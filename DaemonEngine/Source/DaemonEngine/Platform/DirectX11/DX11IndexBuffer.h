#pragma once
#include "DaemonEngine/Renderer/IndexBuffer.h"

#include <d3d11.h>

namespace Daemon
{

	class DX11IndexBuffer : public IndexBuffer
	{
	public:
		DX11IndexBuffer(uint32_t* indices, uint32_t count);
		~DX11IndexBuffer();

		virtual uint32_t GetCount() const override { return m_Count; }

		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		ID3D11Buffer* m_BufferHandle;
		uint32_t m_Count;
	};

}