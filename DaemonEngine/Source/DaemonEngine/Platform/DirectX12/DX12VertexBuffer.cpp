#include "kepch.h"
#include "DX12VertexBuffer.h"
#include "DX12Context.h"

#include <typeinfo>

namespace Daemon
{

	DX12VertexBuffer::DX12VertexBuffer(uint32_t size)
		: m_Size(size)
	{
		CreateVertexBuffer(size, nullptr);
	}

	DX12VertexBuffer::DX12VertexBuffer(void* vertices, uint32_t size)
		: m_Size(size)
	{
		CreateVertexBuffer(size, vertices);
	}

	DX12VertexBuffer::~DX12VertexBuffer()
	{
	}

	void DX12VertexBuffer::SetData(void* data, uint32_t size)
	{
		memcpy(m_LocalStorage, data, size);
		// Copy the triangle data to the vertex buffer.
		m_VertexBuffer->Map(0, nullptr, &m_LocalStorage);
		memcpy(m_LocalStorage, data, size);
		m_VertexBuffer->Unmap(0, nullptr);
		SetVertexBufferView(size, 40); // size / sizeof(float)
	}

	void DX12VertexBuffer::CreateVertexBuffer(uint32_t size, void* data)
	{
		if (size == 0)
		{
			size = 1;
		}
		m_LocalStorage = malloc(size); // new uint8_t[size];

		D3D12_HEAP_PROPERTIES heapProps;
		ZeroMemory(&heapProps, sizeof(D3D12_HEAP_PROPERTIES));
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(D3D12_RESOURCE_DESC));
		vertexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		vertexBufferDesc.Alignment = 0;
		vertexBufferDesc.Width = static_cast<UINT64>(size);
		vertexBufferDesc.Height = 1;
		vertexBufferDesc.DepthOrArraySize = 1;
		vertexBufferDesc.MipLevels = 1;
		vertexBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		vertexBufferDesc.SampleDesc.Count = 1;
		vertexBufferDesc.SampleDesc.Quality = 0;
		vertexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		vertexBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ID3D12Device8* device = DX12Context::Get().GetDevice();
		HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &vertexBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_VertexBuffer));
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create Vertex Buffer");

		if (data)
		{
			SetData(data, size);
		}
	}

	void DX12VertexBuffer::SetVertexBufferView(uint32_t sizeInBytes, uint32_t strideInBytes)
	{
		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = strideInBytes;
		m_VertexBufferView.SizeInBytes = sizeInBytes;
	}

}