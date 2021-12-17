#pragma once
#include "DaemonEngine/Renderer/IndexBuffer.h"
#include "DirectX12.h"

namespace Daemon
{

	class DX12IndexBuffer : public IndexBuffer
	{
	public:
		DX12IndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~DX12IndexBuffer();

		virtual uint32_t GetCount() const override { return m_Count; }

		virtual void Bind() const override {}
		virtual void Unbind() const override {}

		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const { return m_IndexBufferView; }
	private:
		void CreateIndexBuffer(uint32_t count, void* data);
		void SetIndexBufferView(uint32_t sizeInBytes, DXGI_FORMAT format);
	private:
		uint32_t m_Count;
		ID3D12Resource* m_IndexBuffer;
		D3D12_INDEX_BUFFER_VIEW  m_IndexBufferView;

		friend class DX12Pipeline;
		friend class DX12Renderer;
	};

}
