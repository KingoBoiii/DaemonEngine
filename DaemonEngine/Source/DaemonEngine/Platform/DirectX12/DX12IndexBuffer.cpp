#include "kepch.h"
#include "DX12IndexBuffer.h"
#include "DX12Context.h"

namespace Daemon
{

	DX12IndexBuffer::DX12IndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		CreateIndexBuffer(count, reinterpret_cast<void*>(indices));
	}

	DX12IndexBuffer::~DX12IndexBuffer()
	{
	}

	void DX12IndexBuffer::CreateIndexBuffer(uint32_t count, void* data)
	{
		if (count == 0)
		{
			count = 1;
		}

		D3D12_HEAP_PROPERTIES heapProps;
		ZeroMemory(&heapProps, sizeof(D3D12_HEAP_PROPERTIES));
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		uint32_t size = count * sizeof(uint32_t);
		D3D12_RESOURCE_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(D3D12_RESOURCE_DESC));
		indexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		indexBufferDesc.Alignment = 0;
		indexBufferDesc.Width = static_cast<UINT64>(size);
		indexBufferDesc.Height = 1;
		indexBufferDesc.DepthOrArraySize = 1;
		indexBufferDesc.MipLevels = 1;
		indexBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		indexBufferDesc.SampleDesc.Count = 1;
		indexBufferDesc.SampleDesc.Quality = 0;
		indexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		indexBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ID3D12Device8* device = DX12Context::Get().GetDevice();
		HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &indexBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_IndexBuffer));
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create Index Buffer");

		// Copy the data to the index buffer.
		void* pIndexDataBegin;
		D3D12_RANGE range{ 0, 0 };

		m_IndexBuffer->Map(0, &range, &pIndexDataBegin);
		memcpy(pIndexDataBegin, data, size);
		m_IndexBuffer->Unmap(0, nullptr);
		SetIndexBufferView(size, DXGI_FORMAT_R32_UINT);
	}

	void DX12IndexBuffer::SetIndexBufferView(uint32_t sizeInBytes, DXGI_FORMAT format)
	{
		ZeroMemory(&m_IndexBufferView, sizeof(D3D12_INDEX_BUFFER_VIEW));
		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = format;
		m_IndexBufferView.SizeInBytes = sizeInBytes;
	}

}