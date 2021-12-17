#pragma once
#include "DaemonEngine/Renderer/VertexBuffer.h"
#include "DirectX12.h"

namespace Daemon
{

	class DX12VertexBuffer : public VertexBuffer
	{
	public:
		DX12VertexBuffer(uint32_t size);
		DX12VertexBuffer(void* vertices, uint32_t size);
		virtual ~DX12VertexBuffer();

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind() const override {}
		virtual void Unbind() const override {}

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const { return m_VertexBufferView; }
	private:
		void CreateVertexBuffer(uint32_t size, void* data);
		void SetVertexBufferView(uint32_t sizeInBytes, uint32_t strideInBytes);
	private:
		uint32_t m_Size;
		ID3D12Resource* m_VertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW  m_VertexBufferView;

		void* m_LocalStorage = nullptr;

		friend class DX12Pipeline;
	};

}
