#include "kepch.h"
#include "DX12UniformBuffer.h"
#include "DX12Context.h"

namespace Daemon
{

	DX12UniformBuffer::DX12UniformBuffer(uint32_t size, uint32_t binding)
		: m_Size(size), m_Binding(binding)
	{
		m_LocalStorage = malloc(m_Size);
		CreateConstantBufferHeap();
		CreateConstantBuffer();
	}

	DX12UniformBuffer::~DX12UniformBuffer()
	{
		//delete m_LocalStorage; 
		//m_LocalStorage = nullptr;
	}

	void DX12UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		void* pData;
		memcpy(m_LocalStorage, data, size);

		HRESULT hr = m_ConstantBuffer->Map(m_Binding, nullptr, &pData);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to map Constant buffer");

		memcpy(pData, (const uint8_t*)data + offset, size);
		m_ConstantBuffer->Unmap(m_Binding, nullptr);
	}

	void DX12UniformBuffer::CreateConstantBuffer()
	{
		D3D12_HEAP_PROPERTIES heapProps;
		ZeroMemory(&heapProps, sizeof(D3D12_HEAP_PROPERTIES));
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(D3D12_RESOURCE_DESC));
		constantBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		constantBufferDesc.Alignment = 0;
		constantBufferDesc.Width = static_cast<UINT64>(m_Size);
		constantBufferDesc.Height = 1;
		constantBufferDesc.DepthOrArraySize = 1;
		constantBufferDesc.MipLevels = 1;
		constantBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		constantBufferDesc.SampleDesc.Count = 1;
		constantBufferDesc.SampleDesc.Quality = 0;
		constantBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		constantBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ID3D12Device8* device = DX12Context::Get().GetDevice();
		device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &constantBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_ConstantBuffer));

		SetConstantBufferView(device, m_Size);
	}

	void DX12UniformBuffer::CreateConstantBufferHeap()
	{
		// Describe and create a constant buffer view (CBV) descriptor heap.
		// Flags indicate that this descriptor heap can be bound to the pipeline 
		// and that descriptors contained in it can be referenced by a root table.
		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		ID3D12Device8* device = DX12Context::Get().GetDevice();
		HRESULT hr = device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_ConstantBufferHeap));
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create Descriptor Heap for Constant Buffer");
	}

	void DX12UniformBuffer::SetConstantBufferView(ID3D12Device8* device, uint32_t sizeInBytes)
	{
		// Describe and create a constant buffer view.
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_ConstantBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = m_Size;
		device->CreateConstantBufferView(&cbvDesc, m_ConstantBufferHeap->GetCPUDescriptorHandleForHeapStart());
	}

}