#pragma once
#include "DaemonEngine/Renderer/UniformBuffer.h"
#include "DirectX12.h"

namespace Daemon
{

	class DX12UniformBuffer : public UniformBuffer
	{
	public:
		DX12UniformBuffer(uint32_t size, uint32_t binding);
		virtual ~DX12UniformBuffer();

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

		virtual void Bind() const override {}
		virtual void Unbind() const override {}

		D3D12_CPU_DESCRIPTOR_HANDLE GetD3DX12ConstantBufferHandle() const { return m_ConstantBufferHandle; }
	private:
		void CreateConstantBuffer();
		void CreateConstantBufferHeap();
		void SetConstantBufferView(ID3D12Device8* device, uint32_t sizeInBytes);
	private:
		uint32_t m_Binding;
		uint32_t m_Size;
		void* m_LocalStorage = nullptr;

		//D3DX12 resources
		ID3D12Resource* m_ConstantBuffer;
		ID3D12DescriptorHeap* m_ConstantBufferHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_ConstantBufferHandle;

		friend class DX12Renderer;
	};

}

