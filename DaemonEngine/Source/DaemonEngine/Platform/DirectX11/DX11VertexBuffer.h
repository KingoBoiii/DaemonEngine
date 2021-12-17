#pragma once
#include "DaemonEngine/Renderer/VertexBuffer.h"

#include <d3d11.h>
#include <Windows.h>

namespace Daemon
{

	class DX11VertexBuffer : public VertexBuffer
	{
	public:
		DX11VertexBuffer(uint32_t size);
		DX11VertexBuffer(void* vertices, uint32_t size);
		~DX11VertexBuffer();

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		ID3D11Buffer* m_BufferHandle;
		D3D11_BUFFER_DESC m_BufferDesc;
		D3D11_MAPPED_SUBRESOURCE m_MappedSubresource;
		uint32_t m_Size;
	};

}